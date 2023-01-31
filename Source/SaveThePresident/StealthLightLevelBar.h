// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StealthLightLevelBar.generated.h"


UCLASS()
class SAVETHEPRESIDENT_API UStealthLightLevelBar : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* StealthLightLevelProgressBar;
};
