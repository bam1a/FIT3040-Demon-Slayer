// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractableObject.h"
/**
 * 
 */
class DEMONSLAYER_API Objective
{
	/* Objective details */
	int objectiveID;
	FString description;
	FString secondaryDescription;
	bool objectiveCompleted;
	AInteractableObject* objectConnectedTo;
public:
	Objective(int ID, FString objectiveDescription, FString objectiveSecondary);
	~Objective();
	// Accessors 
	FString GetDescription() { return description; }
	FString GetSecondary() { return secondaryDescription; }
	int GetID() { return objectiveID; }
	AInteractableObject* GetObject() { return objectConnectedTo; }
	// Mutators
	void SetObject(AInteractableObject* connectedObject) { objectConnectedTo = connectedObject; }
	
};
