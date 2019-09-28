// Fill out your copyright notice in the Description page of Project Settings.

#include "Objective.h"
#include "EngineUtils.h"

Objective::Objective(int ID, FString objectiveDescription, FString objectiveSecondary)
{
	// Initialise objective
	objectiveID = ID;
	description = objectiveDescription;
	secondaryDescription = objectiveSecondary;
}

Objective::~Objective()
{
}
