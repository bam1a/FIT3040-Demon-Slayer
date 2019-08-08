// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "IncreaseTimeSeen.generated.h"

/**
 * 
 */
UCLASS()
class DEMONSLAYER_API UIncreaseTimeSeen : public UBTTaskNode
{
	GENERATED_BODY()

	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory) override;
};
