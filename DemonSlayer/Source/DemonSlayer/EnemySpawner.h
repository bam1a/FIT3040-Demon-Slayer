// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Demon.h"
#include "EnemySpawner.generated.h"

UCLASS()
class DEMONSLAYER_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

/*Obsolete spawner class*/
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Enemy")
	UClass* enemy;

	void SpawnEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
