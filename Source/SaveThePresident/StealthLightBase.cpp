// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthLightBase.h"

#include "MyDebug.h"
#include "MyPlayer.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AStealthLightBase::AStealthLightBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	Probes = { "head", "hand_l", "hand_r", "foot_l", "foot_r" };
	SphereBoundary = CreateDefaultSubobject<USphereComponent>("SphereBoundary");
}

void AStealthLightBase::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AStealthLightBase::OnSphereBeginOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		bBoundsOverlapped = true;
	}
}

void AStealthLightBase::OnSphereEndOverlapped(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		bBoundsOverlapped = false;
	}
}

void AStealthLightBase::ComputeAvgProbesIntensity()
{
	if (ProbesIntensity.Num() != 0)
	{
		const TArray<float> ProbesIntensityArray = ProbesIntensity.Array();
		ALI = 0.f;

		for (const float ProbeIntensity : ProbesIntensityArray)
		{
			ALI += ProbeIntensity;
		}

		ALI /= static_cast<float>(Probes.Num()); // Average according to total sockets number.
		ProbesIntensity.Empty();

		// Transfer ALI to player.
		AMyPlayer* Player = Cast<AMyPlayer>(Cast<AMyCharacter>(PlayerCharacter));
		if (Player)
		{
			const bool bInUpdateRange = FVector::Distance(Player->GetActorLocation(), GetActorLocation()) <= MaxUpdatePlayerRange;
			if (bInUpdateRange)
			{
				Player->UpdateStealthLevel(ALI);
			}
		}

		if (DebugOutput)
		{
			Print(GetName() +  " intensity: " + FString::SanitizeFloat(ALI));
		}
	}
}

void AStealthLightBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


