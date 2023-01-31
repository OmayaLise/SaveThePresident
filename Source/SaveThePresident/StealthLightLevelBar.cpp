// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthLightLevelBar.h"

#include "MyDebug.h"

#include "MyPlayer.h"
#include "Components/ProgressBar.h"

void UStealthLightLevelBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const AMyPlayer* Player = Cast<AMyPlayer>(GetOwningPlayerPawn());
	if (Player && StealthLightLevelProgressBar)
	{
		StealthLightLevelProgressBar->SetPercent(Player->StealthLevel);
		StealthLightLevelProgressBar->SetRenderOpacity(Player->StealthLevel);
	}
}


