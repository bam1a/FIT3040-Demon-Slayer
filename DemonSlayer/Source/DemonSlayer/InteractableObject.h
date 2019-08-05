// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableObject.generated.h"

//class ADemonSlayer;

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	IT_Pickup UMETA(DisplayName = "Pick up"),
	IT_Examine UMETA(DisplayName = "Examine"),
	IT_Use UMETA(DisplayName = "Use")
};

UCLASS()
class DEMONSLAYER_API AInteractableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

	// The name of the object
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Details")
	FString Name;

	// The type of interaction
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Details")
	EInteractionType Type;

	// The ID of the objective the object is connected to
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Details")
	int ObjectiveID;

	// The ID of the objective the object is connected to
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Details")
	bool disappearsAfterInteract;

	// The progress of the interaction with the object
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Details")
	float progress=0.f;

	// Whether the object has been fully interacted with
	bool interactedWith;

	// Whether the object is currently active (it is connected to the current objective)
	bool isActive;

	// Temporary member variable to store player for purposes of temporary Demon Slayer mode functionality 
	//ADemonSlayerCharacter* player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Get the current progress of the item interaction
	float GetProgress() { return progress; }

	UFUNCTION(BlueprintCallable, Category = "Object details")
	bool GetIsActive() { return isActive; }

	// Set the current progress of the item interation
	void SetProgress(float newProgress) { progress = newProgress; }

	// Get objective ID
	int GetObjectiveID() { return ObjectiveID; }

	bool GetDisappearsAfterInteract() { return disappearsAfterInteract; }

	// Get object name
	UFUNCTION(BlueprintCallable, Category = "Object details")
	FString GetName() { return Name; }

	// Get object interaction type
	UFUNCTION(BlueprintCallable, Category = "Object details")
	EInteractionType GetInteractionType() { return Type; }

	// Get if object has been fully interacted with
	bool GetInteractedWith() { return interactedWith; }

	// Set if object is active
	UFUNCTION(BlueprintCallable, Category = "Object details")
	void SetIsActive(bool newActive) { isActive = newActive; }

	// Set if object has been fully interacted with
	void SetInteractedWith(bool newInteractedWith) { interactedWith = newInteractedWith; }

};
