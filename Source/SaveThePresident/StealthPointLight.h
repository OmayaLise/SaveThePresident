// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StealthLightBase.h"
#include "Components/PointLightComponent.h"
#include "StealthPointLight.generated.h"

UCLASS()
class SAVETHEPRESIDENT_API AStealthPointLight : public AStealthLightBase
{
	GENERATED_BODY()
	
public:	
	AStealthPointLight(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UPointLightComponent* PointLightComponent;
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void InitSphereBoundaryRadius() override;
	virtual void UpdateALI() override;
};
