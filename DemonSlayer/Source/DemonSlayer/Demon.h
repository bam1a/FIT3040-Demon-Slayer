// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Demon.generated.h"

UCLASS()
class DEMONSLAYER_API ADemon : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADemon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The demon's health
	float demonHealth;
	float maxHealth=100.f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Get health
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	float GetHealth() { return demonHealth; }

	//get max health
	UFUNCTION(BlueprintPure, Category = Gameplay)
		float GetMaxHealth() { return maxHealth; }

	// Set health
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetHealth(float newHealth) { demonHealth = newHealth; }

	// Get damage bounds
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	float GetDamageLowerBound() { return DAMAGE_LOWERBOUND; }
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	float GetDamageUpperBound() { return DAMAGE_UPPERBOUND; }

	// Lower and upper bound for damage
	const float DAMAGE_LOWERBOUND = 10.0f;
	const float DAMAGE_UPPERBOUND = 25.0f; 

	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	bool isDead;

};
