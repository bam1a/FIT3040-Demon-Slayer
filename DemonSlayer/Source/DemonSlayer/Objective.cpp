// Fill out your copyright notice in the Description page of Project Settings.

#include "Objective.h"
#include "EngineUtils.h"

Objective::Objective(int ID, FString objectiveDescription)
{
	// Initialise objective
	objectiveID = ID;
	description = objectiveDescription;
}

Objective::~Objective()
{
}
