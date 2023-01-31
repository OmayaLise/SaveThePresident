// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StealthLightBase.h"
#include "Components/SpotlightComponent.h"
#include "StealthSpotLight.generated.h"

UCLASS()
class SAVETHEPRESIDENT_API AStealthSpotLight : public AStealthLightBase
{
	GENERATED_BODY()
	
public:	
	AStealthSpotLight(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USpotLightComponent* SpotLightComponent;
	
protected:
	virtual void BeginPlay() override;


public:	
	virtual void Tick(float DeltaTime) override;
	virtual void InitSphereBoundaryRadius() override;
	virtual void UpdateALI() override;

	bool IsProbeWithinConeAngle(const FVector ProbeLocation, float& ProbeAngle) const;
};
