// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPatrolPositionDef.h"

#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include <AIController.h>
#include "MyBasicEnemy.h"

EBTNodeResult::Type UMyPatrolPositionDef::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyBasicEnemy* enemy = Cast< AMyBasicEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy)
	{
		if (enemy->WayPoints.Num() > 0)
		{
			AActor* NextPos = Cast<AActor>(enemy->WayPoints[enemy->indexPatrol]);
			if (NextPos)
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("RandomPatrolPosition"), NextPos);

				enemy->indexPatrol++;
				if (enemy->indexPatrol == enemy->WayPoints.Num())
					enemy->indexPatrol = 0;

				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
