// Fill out your copyright notice in the Description page of Project Settings.

#include "DemonSlayer2GameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ADemonSlayer2GameMode::ADemonSlayer2GameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// Initialise objectives
	objectiveList = new TArray<Objective*>;
	objectiveList->Add(new Objective(201, "Investigate the blood trail", "Read the blood writing at the end of the blood trail"));
	objectiveList->Add(new Objective(202, "Go to the bathroom and find the key", "Pick up the key in the bathroom's sink"));
	objectiveList->Add(new Objective(203, "Find the door that the key unlocks", "Unlock the ballroom door in the foyer"));
	objectiveList->Add(new Objective(204, "Find another way into the ballroom", "Grab the axe in the living room"));
	objectiveList->Add(new Objective(205, "Cut down the ballroom door", "Cut down the ballroom door"));

	// Set current objective to first objective in list
	objectiveNo = 0;
	currentObjective = (*objectiveList)[objectiveNo];
	objectiveDescription = currentObjective->GetDescription();

	TArray<AActor*> enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADemon::StaticClass(), enemies);
	for (int i = 0; i < enemies.Num(); i++)
	{
		enemies[i]->SetActorHiddenInGame(true);
		enemies[i]->SetActorTickEnabled(false);
	}
}

void ADemonSlayer2GameMode::BeginPlay()
{
	Super::BeginPlay();
	if (mainHUDWidget) {
		// Create HUD widget
		currentWidget = CreateWidget<UUserWidget>(GetWorld(), mainHUDWidget);
		if (currentWidget)
		{
			// Add HUD widget to viewport
			currentWidget->AddToViewport();
		}
	}
}

void ADemonSlayer2GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}