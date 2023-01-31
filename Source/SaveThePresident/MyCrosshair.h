// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MyCrosshair.generated.h"

/**
 * 
 */
UCLASS()
class SAVETHEPRESIDENT_API UMyCrosshair : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UImage* crosshair;

	void Init(APawn* owningPawn);

	UFUNCTION()
	void ChangeColor(const FColor newColor);
};
