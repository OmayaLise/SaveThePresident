// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInGameMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

void UMyInGameMenu::Resume()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (playerController)
	{
		playerController->bShowMouseCursor = false;
		playerController->bEnableClickEvents = false;
		playerController->bEnableMouseOverEvents = false;

		RemoveFromViewport();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(playerController);
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}
}

void UMyInGameMenu::Retry()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController)
	{
		FString nameCurrentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld());
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(playerController);
		UGameplayStatics::OpenLevel(GetWorld(), FName(nameCurrentLevel));
	}
}

void UMyInGameMenu::Quit()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), playerController, EQuitPreference::Quit, true);
	}
}

void UMyInGameMenu::ReturnToMainMenu()
{
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (playerController)
	{
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(playerController);
		UGameplayStatics::OpenLevel(GetWorld(), MainMenuName);
	}
}