// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthSpotLight.h"

#include "MyPlayer.h"

#include "Components/SpotLightComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AStealthSpotLight::AStealthSpotLight(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>("SpotLight");
	
	if (SphereBoundary && SpotLightComponent)
	{
		SetRootComponent(SphereBoundary);

        SphereBoundary->OnComponentBeginOverlap.AddDynamic(this, &AStealthSpotLight::OnSphereBeginOverlapped);
	    SphereBoundary->OnComponentEndOverlap.AddDynamic(this, &AStealthSpotLight::OnSphereEndOverlapped);
		
		SpotLightComponent->AttachToComponent(SphereBoundary, FAttachmentTransformRules::KeepRelativeTransform);
		SpotLightComponent->SetRelativeRotation(FRotator::MakeFromEuler({ 0.f, -90.f, 0.f}));
		SpotLightComponent->SetIntensity(100000.f);
	}
}

void AStealthSpotLight::InitSphereBoundaryRadius()
{
	SphereBoundary->SetSphereRadius(SpotLightComponent->AttenuationRadius, true);
}

void AStealthSpotLight::BeginPlay()
{
	Super::BeginPlay();
}

void AStealthSpotLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SphereBoundary)
	{
		SphereBoundary->SetSphereRadius(SpotLightComponent->AttenuationRadius, true);
		GetWorldTimerManager().SetTimer(TimerHandle,this, &AStealthSpotLight::InitSphereBoundaryRadius, 0.1f, false);
	}
	
	if (PlayerCharacter && bBoundsOverlapped)
	{
		UpdateALI();
	}
}

bool AStealthSpotLight::IsProbeWithinConeAngle(const FVector ProbeLocation, float& ProbeAngle) const
{
	ProbeAngle = FVector::DotProduct(SpotLightComponent->GetForwardVector(),
		UKismetMathLibrary::FindLookAtRotation(SpotLightComponent->GetComponentLocation(), ProbeLocation).Vector());
	const float OuterConeAngle = 1.f - SpotLightComponent->OuterConeAngle / 180.f;
	return ProbeAngle >= OuterConeAngle;
}

void AStealthSpotLight::UpdateALI()
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
				GetWorld(), PlayerProbeLocation, SphereBoundaryLocation,
				UEngineTypes::ConvertToTraceType(ECC_Visibility),
				DebugOutput, { this }, DebugOutput ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
				OutHits, true, FLinearColor::Red, FLinearColor::Green, 0.f);
;

			float ProbeAngle = 0.f;
			if (!IsProbeShadowed)
			{
				if (IsProbeWithinConeAngle(PlayerProbeLocation, ProbeAngle))
				{
					const float DistanceFactor =FMath::GetMappedRangeValueClamped(
						{0.f, SphereBoundary->GetScaledSphereRadius()}, { 1.f, 0.f},
						FVector2D::Distance(static_cast<FVector2D>(PlayerProbeLocation), static_cast<FVector2D>(SphereBoundaryLocation)));
				
					ProbesIntensity.Add(FMath::Clamp(ProbeAngle * DistanceFactor,0.f, MaxDetection));
				}
				else
				{
					ProbesIntensity.Add(0.f);
				}
			}
		}
	}
	ComputeAvgProbesIntensity();
}

