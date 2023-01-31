// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameStateBase.h"
#include "MyBasicEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include <SaveThePresident/MyPlayer.h>
#include "MyDebug.h"

// Sets default values
AMyGameStateBase::AMyGameStateBase()
{
}

void AMyGameStateBase::HandleBeginPlay()
{
	Super::HandleBeginPlay();
	WinGameDelegate.AddUniqueDynamic(this, &AMyGameStateBase::WinGame);
	OnLossDelegate.AddUniqueDynamic(this, &AMyGameStateBase::LoseGame);
	OnEnemyGoingToAlarm.AddUniqueDynamic(this, &AMyGameStateBase::EnemyGoingToAlarm);
	PrimaryActorTick.bCanEverTick = true;
	GetEntity();
	playerController = Cast<APlayerController>(player->GetController());
}

void AMyGameStateBase::GetEntity()
{
	TArray<AActor*> actors;
	TSubclassOf<AMyCharacter> classToFind = AMyCharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, actors);

	for (AActor* actor : actors)
	{
		AMyCharacter* actorEntity = Cast<AMyCharacter>(actor);

		if (Cast<AMyBasicEnemy>(actor))
		{
			enemies.Add(actorEntity);
			continue;
		}

		if (Cast<AMyPlayer>(actor))
		{
			player = actorEntity;
			continue;
		}
	}
}

void AMyGameStateBase::WinGame()
{
	if (!winMenuClass)
		return;

	winMenuInstance = CreateWidget(playerController, winMenuClass);

	playerController->bShowMouseCursor = true;
	playerController->bEnableClickEvents = true;
	playerController->bEnableMouseOverEvents = true;

	winMenuInstance->AddToViewport();
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	UGameplayStatics::PlaySound2D(GetWorld(), WinMusic, 0.5f, 1.f, 0.f, nullptr, nullptr, true);

}

void AMyGameStateBase::PauseGame()
{
	if (!pauseMenuClass)
		return;

	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		Cast<UMyInGameMenu>(pauseMenuInstance)->Resume();
		return;
	}

	pauseMenuInstance = CreateWidget(playerController, pauseMenuClass);

	playerController->bShowMouseCursor = true;
	playerController->bEnableClickEvents = true;
	playerController->bEnableMouseOverEvents = true;

	pauseMenuInstance->AddToViewport();
	UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AMyGameStateBase::LoseGame()
{
	if (!loseMenuClass)
		return;

	loseMenuInstance = CreateWidget(playerController, loseMenuClass);

	playerController->bShowMouseCursor = true;
	playerController->bEnableClickEvents = true;
	playerController->bEnableMouseOverEvents = true;

	loseMenuInstance->AddToViewport();
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	UGameplayStatics::PlaySound2D(GetWorld(), LoseMusic, 0.25f, 1.f, 0.f, nullptr, nullptr, true);
}

void AMyGameStateBase::EnemyGoingToAlarm()
{
	if (!AlertMenu)
		return;
	AlertInstance = CreateWidget<UUserWidget>(GetWorld(), AlertMenu);
	AlertInstance->AddToViewport();
}

void AMyGameStateBase::RemoveAlarmWidget()
{
	if (AlertInstance && !AlertInstance->IsInViewport())
		return;

	for (AMyCharacter* enemy : enemies)
	{
		if (!enemy)
			continue; 
		if (enemy->GetLife() > 0 && enemy->state == State::Alarm)
			return;
	}

	AlertInstance->RemoveFromParent();
	AlertInstance = nullptr;
}
