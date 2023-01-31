// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBasicEnemy.h"

#include "MyDebug.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h" 
#include "Kismet/GameplayStatics.h"
#include "MyGameStateBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/AIModule/Classes/BrainComponent.h>


AMyBasicEnemy::AMyBasicEnemy()
    : AMyCharacter()
    , minDistanceAwareness(75.f)
    , indexPatrol(0)
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    PawnSensor = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensor"));
    PawnSensor->SetPeripheralVisionAngle(50.f);
    PawnSensor->HearingThreshold = 2000.f;
    PawnSensor->bOnlySensePlayers = false;
    PawnSensor->SightRadius = 2000.f;
    PawnSensor->SensingInterval = 0.01f;
    PawnSensor->HearingMaxSoundAge = 0.01f;

    maxAwarenessLevel = 1.f;
    currentAwarenessLevel = 0.f;

    PlayerHasBeenDetectedLastCheck = false;
    playerInsight = false;
    corpseFound = false,
    timerSetAlarm = false;
    wasAlreadyFound = false;
    wasHurt = false;
    hasAlreadyPlayedAlertSound = false;
    activateAlarmWidget = false;

    runSpeed = 800.f;
    walkSpeed = 150.f;
    AwarenessWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("AwarenessBar"));
}

void AMyBasicEnemy::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    if (AwarenessWidget)
    {
        AwarenessWidget->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
        AwarenessWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        AwarenessWidget->SetDrawSize(FVector2D(15.f, 50.f));
    }
}

void AMyBasicEnemy::BeginPlay()
{
    Super::BeginPlay();

    EnemyAIController = Cast<AMyAIController>(GetController());
    state = State::Patrol;

    GameState = GetWorld()->GetGameState<AMyGameStateBase>();   

    awarenessLevelWidget = Cast<UAwarenessLevelWidget>(AwarenessWidget->GetUserWidgetObject());
    AwarenessWidget->SetVisibility(false);

    PlayerSeen = Cast< AMyPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());

}

void AMyBasicEnemy::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensor->OnSeePawn.AddDynamic(this, &AMyBasicEnemy::OnSeePawn);
    PawnSensor->OnHearNoise.AddDynamic(this, &AMyBasicEnemy::OnHearNoise);
}

void AMyBasicEnemy::ComputeAimDirection()
{
    state = State::Aim;

    float roll = FMath::FRandRange(0.f, 1.f);
    float precision = roll >= 0.9f ? 50.f : 100.f;
    FVector2D circularRoll = FMath::RandPointInCircle(precision);
    FVector sphericalRoll = FVector(circularRoll.X, circularRoll.Y, FMath::FRandRange(-1.f, 1.f) * precision);

    FVector MyCharacterPosition = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
    startShoot = GetHeadLocation();

    if(roll >= 0.9f)
        aimDirection = (Cast<AMyCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn())->GetHeadLocation() + sphericalRoll) - startShoot;
    else
        aimDirection = (MyCharacterPosition + sphericalRoll) - startShoot;
}

void AMyBasicEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (life == 0)
        OnDeath();
    else if (life < 0)
        return;

    if (life > 0 && life < 2 && !wasHurt)
    {
        wasHurt = true;
        currentAwarenessLevel = 1.f;
    }

    if (AlarmSpot != nullptr && AlarmSpot->wasActivated)
    {
        GameState->OnLossDelegate.Broadcast();
    }

    if (corpseFound && !timerSetAlarm)
    {
        timerSetAlarm = true;
        GetWorld()->GetTimerManager().SetTimer(TimerHandleDeath, this, &AMyBasicEnemy::StopCorpseAlert, 5, true);
    }

    ManageState();

    if (AwarenessWidget->IsVisible() && awarenessLevelWidget->AwarenessBar)
    {
        awarenessLevelWidget->AwarenessBar->SetPercent(currentAwarenessLevel / maxAwarenessLevel);

        FVector eneCam = PlayerSeen->GetCameraLocation() - GetActorLocation();
        FVector2D EnemyToCameraVec = FVector2D(eneCam.X, eneCam.Y);
        EnemyToCameraVec.Normalize();
    }

    if (PlayerHasBeenDetectedLastCheck)
    {
        PlayerHasBeenDetectedLastCheck = false;
        return;
    }

    if (currentAwarenessLevel > 0.f)
    {
        currentAwarenessLevel -= DeltaTime * 0.3f;
    }
    else if (AwarenessWidget->IsVisible() && currentAwarenessLevel < 0.f)
    {
        currentAwarenessLevel = 0.f;
        AwarenessWidget->SetVisibility(false);
    }

    if (currentAwarenessLevel > maxAwarenessLevel)
        currentAwarenessLevel = maxAwarenessLevel;

    if (playerInsight)
        playerInsight = false;

    if (GetCharacterMovement()->Velocity.SizeSquared() > 0.f && UGameplayStatics::GetTimeSeconds(GetWorld()) - lastTimePlayed > StepSound->Duration)
    {
        lastTimePlayed = UGameplayStatics::GetTimeSeconds(GetWorld());
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), StepSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);
    }
}

void AMyBasicEnemy::OnSeePawn(APawn* Pawn)
{
    if(life < 0 || Cast< AMyAlarm>(Pawn))
        return;

    FVector enemyLocation = GetActorLocation(), playerLocation = Pawn->GetActorLocation();
    if (Cast< AMyPlayer>(Pawn))
    {
        PlayerHasBeenDetectedLastCheck = true;
        PlayerSeen = Cast< AMyPlayer>(Pawn);

        playerInsight = true;
        if (currentAwarenessLevel < maxAwarenessLevel)
        {
            if (Pawn->GetDistanceTo(this) < 300)
                currentAwarenessLevel += (1.f - (enemyLocation - playerLocation).Size() / PawnSensor->SightRadius) * (1.f - (enemyLocation - playerLocation).Size() / PawnSensor->SightRadius);
            else
                currentAwarenessLevel += (1.f - (enemyLocation - playerLocation).Size() / PawnSensor->SightRadius) * (1.f - (enemyLocation - playerLocation).Size() / PawnSensor->SightRadius)* PlayerSeen->StealthLevel * 0.05f;
        }
    }
    
    if (Cast< AMyBasicEnemy>(Pawn) && Cast< AMyBasicEnemy>(Pawn) != this && FVector::Distance(enemyLocation, playerLocation) < 1000)
    {
        if (Cast<AMyBasicEnemy>(Pawn)->life < 1 && !Cast<AMyBasicEnemy>(Pawn)->wasAlreadyFound)
        {
            currentAwarenessLevel = 1.f;
            playerInsight = true;
            corpseFound = true;
            Cast<AMyBasicEnemy>(Pawn)->wasAlreadyFound = true;
        }
        else if (Cast<AMyBasicEnemy>(Pawn)->state == Alert)
        {
            currentAwarenessLevel = Cast<AMyBasicEnemy>(Pawn)->currentAwarenessLevel;
            this->state = Alert;
        }
    }

    if (!AwarenessWidget->IsVisible() && currentAwarenessLevel > 0.f)
        AwarenessWidget->SetVisibility(true);
}

void AMyBasicEnemy::OnHearNoise(APawn* Pawn, const FVector& Location, float Volume)
{
    if (life < 0)
        return;

    if (Cast< AMyBasicEnemy>(Pawn) || Cast< AMyAlarm>(Pawn))
        return;

    PlayerHasBeenDetectedLastCheck = true;

    FVector enemyLocation = GetActorLocation();

    currentAwarenessLevel += (1.f - (enemyLocation - Location).Size() / PawnSensor->HearingThreshold) * Volume * 0.05f;

    if (currentAwarenessLevel > maxAwarenessLevel)
        currentAwarenessLevel = maxAwarenessLevel;

    if (!AwarenessWidget->IsVisible() && currentAwarenessLevel > 0.f)
        AwarenessWidget->SetVisibility(true);
}

void AMyBasicEnemy::OnDeath()
{
    if (AwarenessWidget->IsVisible())
        AwarenessWidget->SetVisibility(false);

    if (activateAlarmWidget)
        GameState->RemoveAlarmWidget();

    EnemyAIController->BrainComponent->StopLogic("EnemyDeath");
    
    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    CapsuleComp->SetCollisionProfileName("NoCollision");
    DeactivateHeadSphereCollider();
    
    USkeletalMeshComponent* skeletalMesh = GetMesh();
    skeletalMesh->SetCollisionProfileName(TEXT("Ragdoll"));
    skeletalMesh->SetSimulatePhysics(true);

    life = -1;
}

void AMyBasicEnemy::Shoot()
{
    PlaySoundShoot();
    Super::Shoot();
}

void AMyBasicEnemy::ManageState()
{
    EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("PlayerSeenPosition"), PlayerSeen);
    EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("InSight"), playerInsight);
    if (state != Alarm && currentMun < 1)
    {
        if (!activateAlarmWidget)
        {
            activateAlarmWidget = true;
            GameState->OnEnemyGoingToAlarm.Broadcast();
        }
        state = Alarm;
        corpseFound = false;
        characterMovement->MaxWalkSpeed = runSpeed;
        EnemyAIController->GetBlackboardComponent()->SetValueAsObject(FName("NearestAlarm"), AlarmSpot);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsGoingToTheAlarm"), true);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsAlerted"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsSuspicious"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsOnPatrol"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("HasFoundCorpse"), false);
    }
    else if (state != Alert && currentAwarenessLevel >= 0.9f)
    {
        state = Alert;
        if (!hasAlreadyPlayedAlertSound)
        {
            UGameplayStatics::PlaySoundAtLocation(GetWorld(), AlertedSound, GetActorLocation(), 1.f, 1.f, 0.f, SoundAttenuation);
            hasAlreadyPlayedAlertSound = true;
        }
        corpseFound = false;
        characterMovement->MaxWalkSpeed = runSpeed;
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsAlerted"), true);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsSuspicious"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsOnPatrol"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("HasFoundCorpse"), false);
    }
    else if(corpseFound)
    {
        characterMovement->MaxWalkSpeed = runSpeed;
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("HasFoundCorpse"), true);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsSuspicious"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsAlerted"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsOnPatrol"), false);
    }
    else if (state != State::Suspicious && currentAwarenessLevel > 0.1f)
    {
        characterMovement->MaxWalkSpeed = walkSpeed;
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsSuspicious"), true);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsAlerted"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsOnPatrol"), false);
    }
    else if (state != State::Patrol)
    {
        if (hasAlreadyPlayedAlertSound)
            hasAlreadyPlayedAlertSound = false;
        GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
        state = Patrol;
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsOnPatrol"), true);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsAlerted"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("IsSuspicious"), false);
        EnemyAIController->GetBlackboardComponent()->SetValueAsBool(FName("HasFoundCorpse"), false);
    }
}

void AMyBasicEnemy::StopCorpseAlert()
{
    corpseFound = false;
}

