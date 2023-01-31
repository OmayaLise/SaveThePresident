// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAlarm.h"
#include "MyBasicEnemy.h"

AMyAlarm::AMyAlarm()
{
	PrimaryActorTick.bCanEverTick = true;

	DetectionAlarm = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionAlarm"));
	if (DetectionAlarm)
	{
		DetectionAlarm->InitSphereRadius(50.f);
		DetectionAlarm->SetCollisionProfileName(TEXT("OverlapAll"));
	}
}

void AMyAlarm::BeginPlay()
{
	Super::BeginPlay();

	if (DetectionAlarm)
	{
		DetectionAlarm->OnComponentBeginOverlap.AddDynamic(this, &AMyAlarm::OnBeginOverlap);
	}
}

void AMyAlarm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyAlarm::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const AMyBasicEnemy* Enemy = Cast<AMyBasicEnemy>(OtherActor);
	if (Enemy)
	{
		wasActivated = Enemy->state == Alarm;
	}
}
