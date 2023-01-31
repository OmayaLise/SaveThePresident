// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "MyAIController.h"
#include "Engine/TargetPoint.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "AwarenessLevelWidget.h"
#include "MyPlayer.h"
#include "MyAlarm.h"

#include "MyBasicEnemy.generated.h"

class AMyGameStateBase;

UCLASS()
class SAVETHEPRESIDENT_API AMyBasicEnemy : public AMyCharacter
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness, meta = (AllowPrivateAccess = "true"))
		float maxAwarenessLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness, meta = (AllowPrivateAccess = "true"))
		bool PlayerHasBeenDetectedLastCheck;

	AMyGameStateBase* GameState;

	TArray<AMyBasicEnemy*> EnemiesInRange;
	AMyPlayer* PlayerSeen;
	bool playerInsight;
	bool corpseFound;
	bool timerSetAlarm;
	bool wasAlreadyFound;
	bool hasAlreadyPlayedAlertSound;
	FTimerHandle TimerHandleDeath;
	FTimerHandle TimerHandleCorpseAlarm;
	FTimerHandle TimerHandleGameOver;
	float walkSpeed;
	float runSpeed;
	bool wasHurt;
	bool activateAlarmWidget;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USoundBase* AlertedSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Alarm)
		AMyAlarm* AlarmSpot;

	class AMyAIController* EnemyAIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness)
		UPawnSensingComponent* PawnSensor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Awareness, meta = (AllowPrivateAccess = "true"))
		float currentAwarenessLevel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Awareness)
		float minDistanceAwareness;

	unsigned int indexPatrol;
	UAwarenessLevelWidget* awarenessLevelWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Awareness)
		UWidgetComponent* AwarenessWidget;

public:
	AMyBasicEnemy();

	UFUNCTION()
		void OnSeePawn(APawn* Pawn);
	UFUNCTION()
		void OnHearNoise(APawn* Pawn, const FVector& Location, float Volume);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WayPoints)
		TArray<class ATargetPoint * > WayPoints;

	virtual void ComputeAimDirection() override;

	void ManageState();
	void StopCorpseAlert();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Shoot() override;

protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnDeath() override;
};
