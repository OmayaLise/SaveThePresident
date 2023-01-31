// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyShoot.h"

#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include <AIController.h>
#include "MyBasicEnemy.h"
#include "Kismet/KismetMathLibrary.h" 

EBTNodeResult::Type UMyEnemyShoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyBasicEnemy* enemy = Cast< AMyBasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	
	APawn* player = GetWorld()->GetFirstPlayerController()->GetPawn();
	FRotator currentRot = enemy->GetActorRotation();
	FRotator NewRot = FMath::RInterpTo(currentRot, UKismetMathLibrary::FindLookAtRotation(enemy->GetActorLocation(), player->GetActorLocation()), FApp::GetDeltaTime(), 5);
	enemy->SetActorRotation(NewRot);

	enemy->ComputeAimDirection();
	enemy->Shoot();

	return EBTNodeResult::Succeeded;
}