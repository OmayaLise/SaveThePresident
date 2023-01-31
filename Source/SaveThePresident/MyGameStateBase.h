// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include <SaveThePresident/MyBasicEnemy.h>
#include "MyInGameMenu.h"
#include "MyAlarm.h"

#include "MyGameStateBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWinGameSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPauseGameSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAlarmActivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoss);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyGoingToAlarm);


UCLASS()
class SAVETHEPRESIDENT_API AMyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMyGameStateBase();

	TArray< AMyCharacter*> enemies;
	AMyCharacter* player;
	APlayerController* playerController;

	void HandleBeginPlay();
	void GetEntity();

	UPROPERTY(BlueprintAssignable)
	FOnLoss OnLossDelegate;

	UFUNCTION()
	void LoseGame();

	// Win
	UPROPERTY(BlueprintAssignable)
	FWinGameSignature WinGameDelegate;

	UFUNCTION()
		void WinGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Menu)
		TSubclassOf<UMyInGameMenu> winMenuClass;

	UUserWidget* winMenuInstance;

	// Pause
	UPROPERTY(BlueprintAssignable)
		FPauseGameSignature PauseGameDelegate;

	UFUNCTION()
		void PauseGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Menu)
		TSubclassOf<UMyInGameMenu> pauseMenuClass;

	UUserWidget* pauseMenuInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Menu)
	TSubclassOf<UMyInGameMenu> loseMenuClass;

	UUserWidget* loseMenuInstance;

	UPROPERTY(BlueprintAssignable)
	FOnAlarmActivated OnAlarmActivatedDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		USoundBase* WinMusic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Music)
		USoundBase* LoseMusic;
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemyGoingToAlarm OnEnemyGoingToAlarm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Alert)
	TSubclassOf<UUserWidget> AlertMenu;

	UUserWidget* AlertInstance;

	UFUNCTION()
	void EnemyGoingToAlarm();
	
	void RemoveAlarmWidget();

};
