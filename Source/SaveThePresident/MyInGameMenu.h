// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyInGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class SAVETHEPRESIDENT_API UMyInGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FName MainMenuName;

	UFUNCTION(BlueprintCallable) void Resume();
	UFUNCTION(BlueprintCallable) void Retry();
	UFUNCTION(BlueprintCallable) void Quit();
	UFUNCTION(BlueprintCallable) void ReturnToMainMenu();

};
