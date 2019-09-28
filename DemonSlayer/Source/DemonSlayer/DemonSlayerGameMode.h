// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Objective.h"
#include "EnemySpawner.h"
#include "InteractableObject.h"
#include "DemonSlayerGameMode.generated.h"

UCLASS(minimalapi)
class ADemonSlayerGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	TArray<Objective*>* objectiveList;

	int objectiveNo;

	Objective* currentObjective;

	class UUserWidget* currentWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	TSubclassOf<class ADemon> enemy;

public:
	ADemonSlayerGameMode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;	

	// Set object connected to objective
	void SetObjectToObjective(AInteractableObject* thisObject);

	UPROPERTY(BlueprintReadWrite)
	FString objectiveDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	TSubclassOf<class UUserWidget> mainHUDWidget;

	void SetDescription(FString newDescription) { objectiveDescription = newDescription; }

	Objective* GetCurrentObjective() { return currentObjective; }

};



