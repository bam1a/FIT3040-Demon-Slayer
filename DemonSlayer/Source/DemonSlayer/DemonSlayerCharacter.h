// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InteractableObject.h"
#include "Demon.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "DemonSlayerCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ADemonSlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UMotionControllerComponent* L_MotionController;


	bool isDemonSlayerActivated;
	bool hasDemonSlayer;

	const float DAMAGE_LOWERBOUND = 20.0f;
	const float DAMAGE_UPPERBOUND = 60.0f;

	// The player's health
	float health;

	const float SLAYERDECREASERATE = 0.001f;
	float cooldownRate;

	float const ATTACK_COOLDOWN = 2.633;
	float currentAttackCooldown;

	UTextureCube* CubeMap;

	// Whether an actor has been raytraced 
	bool actorRaytraced;

	// The item the player is currently looking at
	AInteractableObject* currentActor;

	bool hasBeenAttacked;

public:
	ADemonSlayerCharacter();

	// Obsolete footstep variables
	USoundCue* footstepAudioCue;
	UAudioComponent* footstepAudioComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Enemy")
	bool isAttacking;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Turn Demon Slayer mode on or off 
	void ToggleDemonSlayer();
	void DemonSlayerOn();
	UFUNCTION(BlueprintCallable, Category = SlayerMode)
	void DemonSlayerOff();

	// The current demon the player is attacking
	UPROPERTY(BlueprintReadWrite, Category = Gameplay)
	ADemon* attackTarget;

	// The demon the player is currently looking at
	UPROPERTY(BlueprintReadWrite, Category = Enemy)
	ADemon* currentDemon;

	/** Accessors */
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool GetDemonSlayerActivated() { return isDemonSlayerActivated; }

	//added for checking if the mask is already obtained
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool GetHasDemonSlayer() { return hasDemonSlayer; }

	// Get health
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	float GetHealth() { return health; }

	// Get Demon Slayer meter
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	float GetDemonSlayerMeter() { return demonSlayerMeter; }

	// Get object currently being looked at
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	AInteractableObject* GetCurrentActor() { return currentActor; }

	// Get demon currently being looked at 
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	ADemon* GetCurrentDemon() { return currentDemon; }

	// Get Demon currently attacking
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	ADemon* GetAttackTarget() { return attackTarget; }

	// Determine if player has been attacked
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	bool GetHasBeenAttacked() { return hasBeenAttacked; }

	/** Mutators */
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetHealth(float newHealth) { health = newHealth; }

	// The progress to use Demon Slayer mode (1 is full, 0 is empty)
	UPROPERTY(BlueprintReadWrite, Category = SlayerMode)
	float demonSlayerMeter;

	void OnInteract();
	void OnStopInteract();

	/** Raytracing functions */
	void CallMyTrace();
	bool Trace(UWorld * World, TArray<AActor*>& ActorsToIgnore, const FVector & Start, const FVector & End, FHitResult & HitOut, ECollisionChannel CollisionChannel, bool ReturnPhysMat);
	void ProcessTraceHit(FHitResult & HitOut);

	/** Attack functions */
	void FindAttackTarget();
	void CanAttack(FHitResult & HitOut);
	void Attack();
	void AttackingToFalse() { isAttacking = false; }
	void Stun(); 

	UFUNCTION(BlueprintCallable, Category = Attack)
	float GetDamageLowerBound() { return DAMAGE_LOWERBOUND; }
	UFUNCTION(BlueprintCallable, Category = Attack)
	float GetDamageUpperBound() { return DAMAGE_UPPERBOUND; }

	UFUNCTION(BlueprintCallable, Category = Attack)
	void HasBeenAttacked();
	void HasBeenAttackedToFalse() { hasBeenAttacked = false; }

	void Hint();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ADemonSlayerProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;

	//** holdSecond: a process time for player to interact object.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float holdSecond=2.0f;

	// The item the player is currently interacting with (can be called by blueprint but can't edit it)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	AInteractableObject* ObjectInteractingWith;

protected:

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	//maximium health of player (added by Bon)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
		float maxHealth = 100.f;

	//maximium meter(added by Bon)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
		float maxdemonSlayerMeter = 1.f;

};

