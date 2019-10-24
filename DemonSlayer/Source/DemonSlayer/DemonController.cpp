// Fill out your copyright notice in the Description page of Project Settings.

#include "DemonController.h"
#include "NavigationSystem.h"
#include "Engine/Engine.h"
#include "DemonSlayerCharacter.h"
#include "DemonSlayer2GameMode.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Demon.h"
#include "TimerManager.h"

void ADemonController::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;
	
	// Check blackboard and behaviour tree
	if (!blackboardData) { return; }
	UseBlackboard(blackboardData, BB);
	if (!Cast<ADemonSlayer2GameMode>(GetWorld()->GetAuthGameMode()))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
		if (!ensure(behaviorTree)) { return; }
		RunBehaviorTree(behaviorTree);
	}

	// Derived from the FIT3094 Behavior Tree supplementary 
	AIPerception = ADemonController::FindComponentByClass<UAIPerceptionComponent>();
	if (!ensure(AIPerception)) { return; }
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ADemonController::Senses);
	TArray <AActor*> actorsSensed;
	AIPerception->GetPerceivedActors(sight, actorsSensed);

	BB->SetValueAsObject(TEXT("Self"), this->GetOwner());
	BB->SetValueAsBool(TEXT("CanSeePlayer"), false);

	initialPosition = GetPawn()->GetActorLocation();

	isAttacking = false;
}

void ADemonController::Senses(AActor* updatedActor, FAIStimulus stimulus)
{
	// Derived from the FIT3094 Behavior Tree supplementary
	if (stimulus.WasSuccessfullySensed())
	{
		ADemonSlayerCharacter* player = Cast<ADemonSlayerCharacter>(updatedActor);
		if (player)
		{
			BB->SetValueAsBool("CanSeePlayer", true);
		}
		else 
		{
			BB->SetValueAsBool("CanSeePlayer", false);
		}
	}
	else 
	{
		BB->SetValueAsBool("CanSeePlayer", false);
	}
}

void ADemonController::ActivateAI()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
	if (!ensure(behaviorTree)) { return; }
	RunBehaviorTree(behaviorTree);
}

void ADemonController::GetRoamLocation()
{
	// Get navigation system
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (navSys)
	{
		// Get random postion within navigation mesh limits
		FNavLocation roamLoc;
		navSys->GetRandomReachablePointInRadius(initialPosition, 2500.0f, roamLoc);
		BB->SetValueAsVector(TEXT("TargetLocation"), roamLoc.Location);
	}
	// Set movement speed to walk speed
	ADemon* thisDemon = Cast<ADemon>(GetPawn());
	thisDemon->GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

void ADemonController::GetPlayerLocation()
{
	for (TActorIterator<ADemonSlayerCharacter> PlayerItr(GetWorld()); PlayerItr; ++PlayerItr)
	{
		BB->SetValueAsVector(TEXT("PlayerLocation"), PlayerItr->GetActorLocation());
	}
}

void ADemonController::Attack()
{
	if (attackTarget != NULL)
	{
		// Attack player
		//ADemon* thisDemon = Cast<ADemon>(GetPawn());
		//attackTarget->SetHealth(attackTarget->GetHealth() - FMath::RandRange(thisDemon->DAMAGE_LOWERBOUND, thisDemon->DAMAGE_UPPERBOUND));
		isAttacking = true;
		//Cast<ADemonSlayerCharacter>(attackTarget)->HasBeenAttacked();
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Player health: %f"), attackTarget->GetHealth()));
		/*if (attackTarget->GetHealth() <= 0)
		{
			attackTarget->Destroy();
		}*/
	}
}

void ADemonController::IncreaseTimeSeen()
{
	BB->SetValueAsFloat(TEXT("DetectionMeter"), BB->GetValueAsFloat(TEXT("DetectionMeter")) + GetWorld()->GetDeltaSeconds());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Detection meter: %f"), BB->GetValueAsFloat(TEXT("DetectionMeter"))));
	if (BB->GetValueAsFloat(TEXT("DetectionMeter")) >= 2.0f)
	{
		BB->SetValueAsBool("HasDetectedPlayer", true);
	}
}

void ADemonController::IncreaseTimeSeenMediumRange()
{
	BB->SetValueAsFloat(TEXT("DetectionMeter"), BB->GetValueAsFloat(TEXT("DetectionMeter")) + (GetWorld()->GetDeltaSeconds() * 2));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Detection meter(medium): %f"), BB->GetValueAsFloat(TEXT("DetectionMeter"))));
	if (BB->GetValueAsFloat(TEXT("DetectionMeter")) >= 2.0f)
	{
		BB->SetValueAsBool("HasDetectedPlayer", true);
	}
}

void ADemonController::ResetTimeSeen()
{
	BB->SetValueAsFloat(TEXT("TimeSeen"), 0.0f);
	BB->SetValueAsBool("HasDetectedPlayer", false);
}

void ADemonController::Stun()
{
	BB->SetValueAsBool("IsStunned", true);
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ADemonController::Unstun, 2.0f, false);
}

void ADemonController::Unstun()
{
	BB->SetValueAsBool("IsStunned", false);
}

