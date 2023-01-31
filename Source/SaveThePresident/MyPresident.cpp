// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPresident.h"
#include "MyGameStateBase.h"

// Sets default values
AMyPresident::AMyPresident()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	winZone = CreateDefaultSubobject<UBoxComponent>(TEXT("Win Zone"));
	winZone->SetGenerateOverlapEvents(true);
	winZone->SetCollisionProfileName(TEXT("OverlapAll"));
}

// Called when the game starts or when spawned
void AMyPresident::BeginPlay()
{
	Super::BeginPlay();
	winZone->OnComponentBeginOverlap.AddDynamic(this, &AMyPresident::OnWinZoneBeginOverlap);
}

// Called every frame
void AMyPresident::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyPresident::OnWinZoneBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<AMyPlayer>(OtherActor))
	{
		GetWorld()->GetGameState<AMyGameStateBase>()->WinGameDelegate.Broadcast();
	}
}

