// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCrosshair.h"
#include "MyPlayer.h"

void UMyCrosshair::Init(APawn* owningPawn)
{
	Cast<AMyPlayer>(owningPawn)->OnChangePawnAimDelegate.AddUniqueDynamic(this, &UMyCrosshair::ChangeColor);
}

void UMyCrosshair::ChangeColor(const FColor newColor)
{
	crosshair->SetColorAndOpacity(FLinearColor(newColor));
}