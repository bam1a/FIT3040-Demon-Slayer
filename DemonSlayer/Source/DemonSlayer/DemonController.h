// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AiPerceptionComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DemonSlayerCharacter.h"
#include "DemonController.generated.h"

/**
 * 
 */
UCLASS()
class DEMONSLAYER_API ADemonController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	UBlackboardComponent* BB;

	FVector initialPosition;

public:
	// The player the demon is attacking
	UPROPERTY(BlueprintReadWrite, Category = "Enemy")
	ADemonSlayerCharacter* attackTarget;

	// Behaviour tree/blackboard components
	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	UBlackboardData* blackboardData;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	UBehaviorTree* behaviorTree;

	UPROPERTY()
	UAIPerceptionComponent* AIPerception;

	UPROPERTY(EditDefaultsOnly, Category = "Blackboard")
	TSubclassOf<UAISense> sight;
	
	void GetRoamLocation();
	void GetPlayerLocation();

	void Senses(AActor* updatedActor, FAIStimulus stimulus);

	/** Trace function */
	bool Trace(UWorld * World, TArray<AActor*>& ActorsToIgnore, const FVector & Start, const FVector & End, FHitResult & HitOut, ECollisionChannel CollisionChannel, bool ReturnPhysMat);
	
	/** Attack functions */
	void FindAttackTarget();
	void CanAttack(FHitResult & HitOut);
	void Attack(); 

	// Set focus to player
	void SetFocusToPlayer(ADemonSlayerCharacter* focus) { SetFocus(focus); }

	void IncreaseTimeSeen();
	
	void ResetTimeSeen();

};
