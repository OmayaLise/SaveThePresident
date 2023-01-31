// Fill out your copyright notice in the Description page of Project Settings.


#include "MyRandomAlertCalc.h"


#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include <AIController.h>
#include "MyBasicEnemy.h"

EBTNodeResult::Type UMyRandomAlertCalc::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (NavArea)
	{
		NavArea->K2_GetRandomReachablePointInRadius(GetWorld(), OwnerComp.GetAIOwner()->GetPawn()->ActorToWorld().GetLocation(),RandomLocation, 800.f);
	}
	else
	{
		return EBTNodeResult::Failed;
	}
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(FName("RandomAlertPosition"), RandomLocation);

	return EBTNodeResult::Succeeded;
}
