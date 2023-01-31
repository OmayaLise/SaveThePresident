// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "NavigationSystem.h"
#include "MyPatrolPositionDef.generated.h"

UCLASS()
class SAVETHEPRESIDENT_API UMyPatrolPositionDef : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	class UNavigationSystemV1* NavArea;

	FVector RandomLocation;
	int next;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

};
