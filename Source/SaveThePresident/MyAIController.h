// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MyAIController.generated.h"

/**
 * 
 */
UCLASS()
class SAVETHEPRESIDENT_API AMyAIController : public AAIController
{
	GENERATED_BODY()
public:

	AMyAIController();

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		UBehaviorTree* BehaviorTree;

};
