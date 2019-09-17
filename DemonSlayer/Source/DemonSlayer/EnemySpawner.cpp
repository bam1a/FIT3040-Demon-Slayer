// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "Engine/Engine.h"
#include "ConstructorHelpers.h"
#include "InteractableObject.h"

void AEnemySpawner::SpawnEnemy()
{
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Spawning enemies"));
	ADemon* newDemon = GetWorld()->SpawnActor<ADemon>(enemy, this->GetActorLocation(), this->GetActorRotation(), spawnParams);
	if (newDemon)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), newDemon->GetActorLocation().Z));
	}
}

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	/*static ConstructorHelpers::FObjectFinder<UBlueprint> EnemyBlueprint(TEXT("Blueprint'/Game/Blueprints/Enemy/Demon_BP.Demon_BP'"));
	if (EnemyBlueprint.Object != NULL)
	{
		enemy = (UClass*)EnemyBlueprint.Object->GeneratedClass;
	}*/
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

