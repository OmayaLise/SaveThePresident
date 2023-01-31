// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "AwarenessLevelWidget.generated.h"

UCLASS()
class SAVETHEPRESIDENT_API UAwarenessLevelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
		UProgressBar* AwarenessBar;	
};
