// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "StealthLightBase.generated.h"

UCLASS()
class SAVETHEPRESIDENT_API AStealthLightBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AStealthLightBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY() USphereComponent*  SphereBoundary;
	UPROPERTY() FTimerHandle TimerHandle;

protected:
	virtual void BeginPlay() override;

	UFUNCTION() void OnSphereBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() void OnSphereEndOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION() void ComputeAvgProbesIntensity();

	UPROPERTY() bool bBoundsOverlapped;
	UPROPERTY() TSet<float> ProbesIntensity;
	UPROPERTY() ACharacter* PlayerCharacter;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void InitSphereBoundaryRadius() { };
	virtual void UpdateALI() { };

	UPROPERTY(EditAnywhere, BlueprintReadOnly) bool DebugOutput;

	// Probes socket name array
	UPROPERTY(EditAnywhere) TArray<FName> Probes;
	
	// Custom upper limit of the outputted detection value. 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MaxDetection = 1.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxUpdatePlayerRange = 2000.f;
	
	// Average lighting intensity
	UPROPERTY(BlueprintReadOnly) float ALI;
};
