// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayer.h"

#include "StealthLightLevelBar.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Components/CapsuleComponent.h"

#include "MyCrosshair.h"
#include "MyBasicEnemy.h"
#include "MyGameStateBase.h"
#include "CorpseDisposalActor.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

AMyPlayer::AMyPlayer()
	: AMyCharacter()
	, BaseTurnRate(45.f)
	, BaseLookUpRate(45.f)
	, GrabDistance(400.f)
	, colorCrosshair(FColor::White)
	, RUN_SPEED(1200.f)
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	characterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...    
	characterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	characterMovement->JumpZVelocity = 600.f;
	characterMovement->AirControl = 0.2f;
	characterMovement->MaxWalkSpeedCrouched = 150.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 250.0f; // The camera follows at this distance behind the character    
	CameraArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraArm->SocketOffset = FVector(0.f, 60.f, 75.f);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Others objects / variables
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitter"));

	// AimSpotLight
	aimSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Aim Spot Light"));
	aimSpotLight->SetLightColor(FLinearColor::Red);
	aimSpotLight->SetInnerConeAngle(0.5f);
	aimSpotLight->SetOuterConeAngle(0.5f);
	aimSpotLight->SetIntensity(100000.f);
	aimSpotLight->SetupAttachment(FollowCamera);
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

void AMyPlayer::BeginPlay()
{
	Super::BeginPlay();

	crosshairInstance = CreateWidget(Cast<APlayerController>(GetController()), crosshairClass, TEXT("Crosshair"));
	crosshairInstance->AddToViewport();
	Cast<UMyCrosshair>(crosshairInstance)->Init(this);


	UStealthLightLevelBar* StealthLightLevelBar =
		Cast<UStealthLightLevelBar>(CreateWidget(Cast<APlayerController>(GetController()), StealthLightLevelBarClass, TEXT("StealthLightLevelBar")));
	
	if (StealthLightLevelBar)
	{
		StealthLightLevelBar->SetOwningPlayer(Cast<APlayerController>(GetController()));
		StealthLightLevelBar->AddToViewport();
	}

	LaserBeamInstance = UGameplayStatics::SpawnEmitterAttached(LaserBeamParticleSystem, GetMesh(), "laserSocket");

	AMyGameStateBase* gameState = GetWorld()->GetGameState<AMyGameStateBase>();
	gameState->PauseGameDelegate.AddUniqueDynamic(gameState, &AMyGameStateBase::PauseGame);
}

void AMyPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsCrouched)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}

	if (state == State::Death)
	{
		OnDeath();
		FTimerHandle _;
		GetWorld()->GetTimerManager().SetTimer(_, this, &AMyPlayer::PlayerLose, 1, true);
		return;
	}

	float currSpeedSquared = GetCharacterMovement()->Velocity.SizeSquared();
	if(currSpeedSquared > 0.f)
		bIsCrouched ? MakeSound(0.25f, 0.01f) : state == State::Run ? MakeSound(1.f, 0.01f) : MakeSound(0.5f, 0.01f);

	if (enemyCorpse)
		PhysicsHandle->SetTargetLocation(UKismetMathLibrary::TransformLocation(FollowCamera->GetComponentTransform(), enemyCorpseLocation));

	ComputeAimDirection();
	UpdateLaserBeam();

	if (!bIsCrouched && GetCharacterMovement()->Velocity.SizeSquared() > 0.f && UGameplayStatics::GetTimeSeconds(GetWorld()) - lastTimePlayed > StepSound->Duration)
	{
		lastTimePlayed = UGameplayStatics::GetTimeSeconds(GetWorld());
		UGameplayStatics::PlaySound2D(GetWorld(), StepSound, 0.5f);
	}
}

void AMyPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Forward", this, &AMyPlayer::MoveForward);
	PlayerInputComponent->BindAxis("Right", this, &AMyPlayer::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Grab", IE_Pressed, this, &AMyPlayer::GrabEnemy);
	PlayerInputComponent->BindAction("Grab", IE_Released, this, &AMyPlayer::ReleaseEnemy);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AMyPlayer::ToggleCrouch);

	PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &AMyPlayer::PauseGame).bExecuteWhenPaused = true;
}

FVector AMyPlayer::GetCameraLocation()
{
	return FollowCamera->GetComponentLocation();
}

void AMyPlayer::ComputeAimDirection()
{
	startShoot = FollowCamera->GetComponentLocation() + (FollowCamera->GetForwardVector() * CameraArm->TargetArmLength);
	aimDirection = FollowCamera->GetForwardVector();

	FHitResult outHit;
	FCollisionQueryParams collisionParams;
	FVector end = startShoot + (aimDirection * 1000.f);
	collisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(outHit, startShoot, end, ECC_Pawn, collisionParams))
	{
		if (Cast<AMyBasicEnemy>(outHit.GetActor()))
		{
			if (colorCrosshair != FColor::Red)
			{
				colorCrosshair = FColor::Red;
				OnChangePawnAimDelegate.Broadcast(colorCrosshair);
			}
		}
		else if (colorCrosshair != FColor::White)
		{
			colorCrosshair = FColor::White;
			OnChangePawnAimDelegate.Broadcast(colorCrosshair);
		}

		aimSpotLight->SetWorldRotation((outHit.ImpactPoint - FollowCamera->GetComponentLocation()).Rotation());
	}
	else if(colorCrosshair != FColor::White)
	{
		colorCrosshair = FColor::White;
		OnChangePawnAimDelegate.Broadcast(colorCrosshair);
	}
}

void AMyPlayer::Shoot()
{
	if (state != State::Aim)
		return;

	if (currentMun < 1)
	{
		Reload();
		return;
	}

	Super::Shoot();
	MakeSound(1.f, 0.01f);
}

void AMyPlayer::OnDeath()
{
	// Ragdoll
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	headCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	headCollider->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;
}

void AMyPlayer::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyPlayer::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyPlayer::ToggleCrouch()
{
	bIsCrouched = !bIsCrouched;

	if (bIsCrouched)
	{
		laser->SetRelativeLocation(FVector(30.f, 0.f, 45.f));
	}
	else
	{
		laser->SetRelativeLocation(FVector(30.f, 0.f, 70.f));
	}
}

void AMyPlayer::GrabEnemy()
{
	FHitResult hit;
	FVector camLocation = FollowCamera->GetComponentLocation();
	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);
	if (!GetWorld()->LineTraceSingleByObjectType(hit, camLocation, camLocation + FollowCamera->GetForwardVector() * GrabDistance, ECC_PhysicsBody, collisionParams))
		return;
	enemyCorpseLocation = hit.ImpactPoint;
	enemyCorpse = hit.Component.Get();
	PhysicsHandle->GrabComponentAtLocation(enemyCorpse, hit.BoneName, enemyCorpseLocation);
	enemyCorpseLocation = UKismetMathLibrary::InverseTransformLocation(FollowCamera->GetComponentTransform(), enemyCorpseLocation);
	hit.GetActor()->SetActorLocation(FVector(0.f, 0.f, -100.0f));
}

void AMyPlayer::ReleaseEnemy()
{
	if (!enemyCorpse)
		return;

	PhysicsHandle->ReleaseComponent();

	enemyCorpse->GetOwner()->SetActorLocation(enemyCorpse->GetComponentLocation());

	TArray<AActor*> bins;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACorpseDisposalActor::StaticClass(), bins);
	for (AActor* bin : bins)
	{
		if ((bin->GetActorLocation() - GetActorLocation()).SizeSquared() <= 200.f * 200.f )
		{
			enemyCorpse->GetOwner()->Destroy();
			break;
		}
	}

	enemyCorpse = nullptr;
}

void AMyPlayer::UpdateLaserBeam()
{
	if (state == State::Aim && LaserBeamInstance)
	{
		const USkeletalMeshComponent* PlayerMesh = GetMesh();
		if (PlayerMesh)
		{
			LaserBeamInstance->SetVisibility(true);
			
			const FVector LaserSource      = PlayerMesh->GetSocketLocation("headSocket");
			const FVector LaserDestination = LaserSource + FollowCamera->GetForwardVector() * 5000.f;

			LaserBeamInstance->SetBeamSourcePoint(0, LaserSource, 0);
			LaserBeamInstance->SetBeamTargetPoint(0, LaserDestination, 0);

			FHitResult Result;
			const bool bColliding = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
				LaserSource, LaserDestination,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				false, { this }, EDrawDebugTrace::None, Result, true);

			LaserBeamInstance->SetBeamTargetPoint(0, bColliding ? Result.Location : Result.TraceEnd, 0);
		}
	}
	else
	{
		LaserBeamInstance->SetVisibility(false);
	}
}

void AMyPlayer::StartAim()
{
	if (state != State::None)
		return;

	AMyCharacter::StartAim();
	state = State::Aim;
	bUseControllerRotationYaw = true;
	FollowCamera->FieldOfView = FIELD_OF_VIEW_AIM;
}

void AMyPlayer::EndAim()
{
	if (state != State::Aim)
		return;

	AMyCharacter::EndAim();
	state = State::None;
	bUseControllerRotationYaw = false;
	FollowCamera->FieldOfView = FIELD_OF_VIEW_NONE;

}

void AMyPlayer::PauseGame()
{
	GetWorld()->GetGameState<AMyGameStateBase>()->PauseGameDelegate.Broadcast();
}

void AMyPlayer::MakeSound(float loudness, float noiseLifeTime)
{
	PawnNoiseEmitter->NoiseLifetime = noiseLifeTime;
	loudness = FMath::Clamp<float>(loudness, 0.0f, 1.0f);
	PawnNoiseEmitter->MakeNoise(this, loudness, GetActorLocation());
}

void AMyPlayer::UpdateStealthLevel(float InStealthLevel)
{
	const float BufferSize = StealthALIBuffer.Num();
	if (BufferSize < 5)
	{
		StealthALIBuffer.Add(FMath::IsNearlyZero(InStealthLevel, 0.1f) ? 0.f : InStealthLevel);
	}
	else
	{
		StealthALIBuffer.Sort([](const float& a, const float& b) { return a > b; });
		StealthLevel = StealthALIBuffer[0];
		StealthALIBuffer.Empty();
	}
}

void AMyPlayer::PlayerLose()
{
	GetWorld()->GetGameState<AMyGameStateBase>()->OnLossDelegate.Broadcast();
}
