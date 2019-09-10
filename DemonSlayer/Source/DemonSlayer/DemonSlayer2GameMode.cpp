// Fill out your copyright notice in the Description page of Project Settings.

#include "DemonSlayer2GameMode.h"
#include "UObject/ConstructorHelpers.h"

ADemonSlayer2GameMode::ADemonSlayer2GameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// Initialise objectives
	objectiveList = new TArray<Objective*>;
	objectiveList->Add(new Objective(201, "Investigate the blood trail"));
	objectiveList->Add(new Objective(202, "Go to the bathroom and find the key"));
	objectiveList->Add(new Objective(203, "Find the door that the key unlocks"));
	objectiveList->Add(new Objective(204, "Find another way into the ballroom"));
	objectiveList->Add(new Objective(205, "Cut down the ballroom door"));

	// Set current objective to first objective in list
	objectiveNo = 0;
	currentObjective = (*objectiveList)[objectiveNo];
	objectiveDescription = currentObjective->GetDescription();
}

void ADemonSlayer2GameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ADemonSlayer2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}