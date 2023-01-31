#include "MyCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AMyCharacter::AMyCharacter()
	: state(State::None)
	, DAMAGE_BODY(1)
	, DAMAGE_HEAD(2)
	, MUN_MAX(3)
	, currentMun(MUN_MAX)
	, SPEED_WALK(600.f)
	, MAX_LIFE(2)
	, AIM_SPEED_WALK(200.f)
	, AIM_CROUCH_SPEED_WALK(50.f)
	, SPEED_WALK_CROUCH(150.f)
{
	life = MAX_LIFE;
	PrimaryActorTick.bCanEverTick = true;
	characterMovement = GetCharacterMovement();

	headCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Head Collider"));
	headCollider->SetupAttachment(GetMesh(), TEXT("headSocket"));
	headCollider->InitSphereRadius(40.f);
	headCollider->SetCollisionProfileName(TEXT("Pawn"));
	headCollider->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	laser = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraLaser"));
	laser->SetupAttachment(GetRootComponent());

	laserImpact = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraLaserImpact"));
	laserImpact->SetupAttachment(GetRootComponent());

	lastTimePlayed = 0.f;
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	headCollider->bHiddenInGame = true;
	headCollider->SetVisibility(false);

	laser->SetRelativeLocation(FVector(10.f, 0.f, 70.f));
	laser->SetAsset(NS_Laser);
	laser->SetAutoActivate(false);
	laser->SetActive(false);

	laserImpact->SetAsset(NS_LaserImpact);
	laserImpact->SetAutoActivate(false);
	laserImpact->SetActive(false);
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ComputeAimDirection();
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AMyCharacter::StartAim);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AMyCharacter::EndAim);

	PlayerInputComponent->BindAction("Shoot", IE_Pressed, this, &AMyCharacter::Shoot);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AMyCharacter::Reload);
}

void AMyCharacter::MyTakeDamage(const int damage)
{
	PlaySoundHurt();
	life -= damage;
	if (life < 0)
		life = 0;

	if (life < 1)
	{
		state = State::Death;
		OnTakeDamageDelegate.Broadcast();
	}
}

void AMyCharacter::StartAim()
{
	characterMovement->MaxWalkSpeedCrouched = AIM_CROUCH_SPEED_WALK;
	characterMovement->MaxWalkSpeed = AIM_SPEED_WALK;
}

void AMyCharacter::EndAim()
{
	characterMovement->MaxWalkSpeedCrouched = SPEED_WALK_CROUCH;
	characterMovement->MaxWalkSpeed = SPEED_WALK;
}

void AMyCharacter::Shoot()
{
	if (state != State::Aim)
		return;

	PlaySoundShoot();
	currentMun--;

	FVector end = startShoot + (aimDirection * 1000.f);

	FCollisionQueryParams collisionParams;
	collisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(outHitShoot, startShoot, end, ECC_Pawn, collisionParams))
	{
		AMyCharacter* opponentHit = Cast<AMyCharacter>(outHitShoot.GetActor());
		
		if (opponentHit)
		{
			bIsHit = true;

			if (outHitShoot.GetComponent()->GetFName() == "Head Collider")
				opponentHit->MyTakeDamage(DAMAGE_HEAD);
			else
				opponentHit->MyTakeDamage(DAMAGE_BODY);

			recoil = end - opponentHit->GetActorLocation();
			recoil.Z = 0;
		}
		else
		{
			bIsHit = false;
		}
	}
	else
	{
		bIsHit = false;

		startShoot = GetMesh()->GetSocketLocation("headSocket");
		end = startShoot + (aimDirection * 100000.f);
		GetWorld()->LineTraceSingleByChannel(outHitShoot, startShoot, end, ECC_Pawn, collisionParams);
	}

	FVector laserEnd =
		outHitShoot.ImpactPoint.IsNearlyZero() ? end : outHitShoot.ImpactPoint - GetMesh()->GetSocketLocation("headSocket");
	laserEnd.Normalize();

	laser->SetVectorParameter("LaserDirection", laserEnd);
	laser->SetFloatParameter("LaserSpeed", 5000.f);

	laser->SetActive(true);

	FTimerHandle _;
	GetWorld()->GetTimerManager().SetTimer(_, this, &AMyCharacter::StopShootLaser, 0.017f, false);
}

void AMyCharacter::Reload()
{
	currentMun = MUN_MAX;
}

void AMyCharacter::SetLife(const int p_life)
{
	life = p_life;
}

void AMyCharacter::ComputeAimDirection()
{
	startShoot = GetActorLocation() + FVector(0.f, 0.f, 40.f);
	aimDirection = GetActorForwardVector();
}

FVector AMyCharacter::GetHeadLocation()
{
	return headCollider->GetComponentLocation();
}

void AMyCharacter::OnDeath() { }

void AMyCharacter::DeactivateHeadSphereCollider()
{
	headCollider->SetCollisionProfileName("NoCollision");
	headCollider->ToggleVisibility();
}
void AMyCharacter::PlaySoundShoot()
{
	if(shootSound)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), shootSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);
}

void AMyCharacter::PlaySoundHurt()
{
	if (HurtSouds.Num() > 0)
	{
		randSound = FMath::RandRange(0, HurtSouds.Num() - 1);
			UGameplayStatics::PlaySound2D(GetWorld(), HurtSouds[randSound], 0.5f);
	}
}

void AMyCharacter::StopShootLaser()
{
	laser->SetActive(false);
	laserImpact->SetActive(false);
}