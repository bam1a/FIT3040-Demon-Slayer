// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DemonSlayerGameMode.h"
#include "DemonSlayerHUD.h"
#include "DemonSlayerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "DemonSlayerCharacter.h"

ADemonSlayerGameMode::ADemonSlayerGameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	//HUDClass = mainHUD->StaticClass();

	// Initialise objectives
	objectiveList = new TArray<Objective*>;
	objectiveList->Add(new Objective(101, "Find the source of the supernatural occurences"));
	objectiveList->Add(new Objective(102, "Investigate the smoke"));
	objectiveList->Add(new Objective(103, "Search for a key into the mansion"));
	objectiveList->Add(new Objective(104, "Enter the mansion"));

	// Set current objective to first objective in list
	objectiveNo = 0;
	currentObjective = (*objectiveList)[objectiveNo];
	objectiveDescription = currentObjective->GetDescription();
}

void ADemonSlayerGameMode::BeginPlay()
{
	// Derived from https://forums.unrealengine.com/development-discussion/c-gameplay-programming/57711-c-gamemode-with-blueprint-hud (Gerrard, 2014)
	if (mainHUDWidget) {
		// Create HUD widget
		currentWidget = CreateWidget<UUserWidget>(GetWorld(), mainHUDWidget);
		if (currentWidget)
		{
			// Add HUD widget to viewport
			currentWidget->AddToViewport();
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d"), (*objectiveList)[objectiveNo]));
}

void ADemonSlayerGameMode::Tick(float DeltaTime)
{
	/*ADemonSlayerCharacter* player = Cast<ADemonSlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (player->GetHealth() <= 0)
	{
		ResetLevel();
	}*/
	// If object of current objective has been fully interacted with
	if (currentObjective->GetObject()->GetInteractedWith())
	{
		// Set object of current objective to inactive
		currentObjective->GetObject()->SetIsActive(false);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Objective completed"));
		if (currentObjective != objectiveList->Last())
		{
			// Increment to next objective
			objectiveNo++;
			currentObjective = (*objectiveList)[objectiveNo];
			currentObjective->GetObject()->SetIsActive(true);
			objectiveDescription = currentObjective->GetDescription();
		}
		else
		{
			// Restart level
			GetWorld()->Exec(GetWorld(), TEXT("RestartLevel"));
		}
	}
}

void ADemonSlayerGameMode::SetObjectToObjective(AInteractableObject* thisObject)
{
	// For each objective 
	for (auto const &objective : *objectiveList)
	{
		// If objective IDs are the same
		if (objective->GetID() == thisObject->GetObjectiveID())
		{
			// Set object of this objective
			objective->SetObject(thisObject);
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%i"), objective->GetObject()->GetObjectiveID()));
		}
	}
}
