// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"

AMyAIController::AMyAIController()
{
}

void AMyAIController::BeginPlay()
{
    Super::BeginPlay();
    RunBehaviorTree(BehaviorTree);
}
