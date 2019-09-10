// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DemonSlayerGameMode.h"
#include "DemonSlayer2GameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEMONSLAYER_API ADemonSlayer2GameMode : public ADemonSlayerGameMode
{
	GENERATED_BODY()
	
public:
	ADemonSlayer2GameMode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
