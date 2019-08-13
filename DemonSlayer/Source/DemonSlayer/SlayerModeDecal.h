// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DecalActor.h"
#include "SlayerModeDecal.generated.h"

/**
 * 
 */
UCLASS()
class DEMONSLAYER_API ASlayerModeDecal : public ADecalActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
};
