	// Fill out your copyright notice in the Description page of Project Settings.

#include "Demon.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADemon::ADemon()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	demonHealth = 100.0f;
	
}

// Called when the game starts or when spawned
void ADemon::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Demon's speed to walking speed
	GetCharacterMovement()->MaxWalkSpeed = 150.0f;
}

// Called every frame
void ADemon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADemon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

