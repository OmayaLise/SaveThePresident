// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthPointLight.h"

#include "MyPlayer.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AStealthPointLight::AStealthPointLight(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>("PointLight");

	if (SphereBoundary && PointLightComponent)
	{
		SetRootComponent(SphereBoundary);
        SphereBoundary->OnComponentBeginOverlap.AddDynamic(this, &AStealthPointLight::OnSphereBeginOverlapped);
	    SphereBoundary->OnComponentEndOverlap.AddDynamic(this, &AStealthPointLight::OnSphereEndOverlapped);
		PointLightComponent->AttachToComponent(SphereBoundary, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void AStealthPointLight::InitSphereBoundaryRadius()
{
	SphereBoundary->SetSphereRadius(PointLightComponent->AttenuationRadius, true);
}


void AStealthPointLight::BeginPlay()
{
	Super::BeginPlay();

	if (SphereBoundary)
	{
		SphereBoundary->SetSphereRadius(0.f, true);
		GetWorldTimerManager().SetTimer(TimerHandle,this, &AStealthPointLight::InitSphereBoundaryRadius, 0.1f, false);
	}
}

void AStealthPointLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (PlayerCharacter && bBoundsOverlapped)
	{
		UpdateALI();
	}
}

void AStealthPointLight::UpdateALI()
{
	for (const FName Probe : Probes)
	{
		const UMeshComponent* PlayerMesh = PlayerCharacter->GetMesh();
		if (PlayerMesh)
		{
			const FVector PlayerProbeLocation    = PlayerMesh->GetSocketLocation(Probe);
			const FVector SphereBoundaryLocation = SphereBoundary->GetComponentTransform().GetLocation();

			TArray<FHitResult> OutHits;
			const bool IsProbeShadowed = UKismetSystemLibrary::LineTraceMulti(
				GetWorld(),PlayerProbeLocation, SphereBoundaryLocation,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				DebugOutput, { this }, DebugOutput ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				OutHits, true, FLinearColor::Red, FLinearColor::Green, 0.f);

			if (!IsProbeShadowed)
			{
				const float ProbeDist = FVector2D::Distance(
					static_cast<FVector2D>(PlayerProbeLocation),
					static_cast<FVector2D>(SphereBoundaryLocation));
				ProbesIntensity.Add(FMath::GetMappedRangeValueClamped({ 0.f, PointLightComponent->AttenuationRadius }, { MaxDetection, 0.f }, ProbeDist));
			}
		}
	}
	ComputeAvgProbesIntensity();
}

