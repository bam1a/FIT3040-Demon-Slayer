// Fill out your copyright notice in the Description page of Project Settings.

#include "AttackPlayer.h"
#include "DemonController.h"

EBTNodeResult::Type UAttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Derived from the FIT3094 Behavior Tree supplementary
	UBehaviorTreeComponent* BTComp = &OwnerComp;
	ADemonController* DemonController = BTComp ? Cast<ADemonController>(BTComp->GetOwner()) : NULL;
	if (DemonController == NULL) {
		return EBTNodeResult::Failed;
	}
	else {
		// Find a location
		DemonController->Attack();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}