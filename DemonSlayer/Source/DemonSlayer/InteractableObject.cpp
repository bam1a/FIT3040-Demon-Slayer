// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableObject.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "DemonSlayerCharacter.h"
#include "DemonSlayerGameMode.h"
#include "Engine/Engine.h"

// Sets default values
AInteractableObject::AInteractableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Initialise base stats
	progress = 0.0f;
	interactedWith = false;
	isActive = false;

	/*for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ADemonSlayerCharacter* Player = Cast<ADemonSlayerCharacter>(*ActorItr);
		if (Player)
		{
			player = Player;
		}
	}*/
}

// Called when the game starts or when spawned
void AInteractableObject::BeginPlay()
{
	Super::BeginPlay();
	
	//this->SetActorHiddenInGame(true);

	// Referenced https://answers.unrealengine.com/questions/25153/how-to-get-a-reference-to-my-gamemode-class-instan.html 
	// Connect object to its objective
	ADemonSlayerGameMode* thisGameMode = Cast<ADemonSlayerGameMode>(GetWorld()->GetAuthGameMode());
	thisGameMode->SetObjectToObjective(this);
}

// Called every frame
void AInteractableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (player)
	{
		if (player->GetDemonSlayerActivated() == false)
		{
			this->SetActorHiddenInGame(true);
		}
		else
		{
			this->SetActorHiddenInGame(false);
		}
	}*/
}

