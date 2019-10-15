// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "DemonSlayerCharacter.h"
#include "DemonSlayerProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Engine/Engine.h"
#include "ConstructorHelpers.h"
#include "Engine/TextureCube.h"
#include "EngineUtils.h"
#include "DemonController.h"
#include "SlayerModeDecal.h"
#include "SlayerModeObject.h"
#include "TimerManager.h"
#include "DemonSlayer2GameMode.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ADemonSlayerCharacter

ADemonSlayerCharacter::ADemonSlayerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	// Create a gun and attach it to the right-hand VR controller.
	// Create a gun mesh component
	VR_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("VR_Gun"));
	VR_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	VR_Gun->bCastDynamicShadow = false;
	VR_Gun->CastShadow = false;
	VR_Gun->SetupAttachment(R_MotionController);
	VR_Gun->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	VR_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("VR_MuzzleLocation"));
	VR_MuzzleLocation->SetupAttachment(VR_Gun);
	VR_MuzzleLocation->SetRelativeLocation(FVector(0.000004, 53.999992, 10.000000));
	VR_MuzzleLocation->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));		// Counteract the rotation of the VR gun model.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	// Set demon slayer is activated to false
	isDemonSlayerActivated = false;
	// Retrieve cubemap 
	static ConstructorHelpers::FObjectFinder<UTextureCube> CubemapAsset(TEXT("TextureCube'/Game/StarterContent/HDRI/HDRI_Epic_Courtyard_Daylight.HDRI_Epic_Courtyard_Daylight'"));
	if (CubemapAsset.Object != NULL)
	{
		CubeMap = CubemapAsset.Object;
	}

	// Initialise sounds
	/*static ConstructorHelpers::FObjectFinder<USoundCue> footstepCue(TEXT("SoundCue'/Game/Audio/footstep_gravel_Cue.footstep_gravel_Cue'"));
	footstepAudioCue = footstepCue.Object;
	footstepAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstepAudioComp"));
	footstepAudioComponent->bAutoActivate = false;
	footstepAudioComponent->AutoAttachParent;
	footstepAudioComponent->SetSound(footstepAudioCue);*/
}

void ADemonSlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		VR_Gun->SetHiddenInGame(false, true);
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		VR_Gun->SetHiddenInGame(true, true);
		Mesh1P->SetHiddenInGame(false, true);
	}
	
	// Initiialise base stats
	hasDemonSlayer = false;
	demonSlayerMeter = 1.0f;
	health = 100.0f;
	cooldownRate = 0.001f;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	isAttacking = false;
	currentAttackCooldown = ATTACK_COOLDOWN;
	if (Cast<ADemonSlayer2GameMode>(GetWorld()->GetAuthGameMode()))
	{
		hasDemonSlayer = true;
	}

}

//////////////////////////////////////////////////////////////////////////
// Input

void ADemonSlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	// Bind other actions
	PlayerInputComponent->BindAction("ToggleDemonSlayerMode", IE_Pressed, this, &ADemonSlayerCharacter::ToggleDemonSlayer);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ADemonSlayerCharacter::OnInteract);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &ADemonSlayerCharacter::OnStopInteract);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &ADemonSlayerCharacter::Attack);
	PlayerInputComponent->BindAction("Hint", IE_Pressed, this, &ADemonSlayerCharacter::Hint);
	PlayerInputComponent->BindAction("Stun", IE_Pressed, this, &ADemonSlayerCharacter::Stun);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ADemonSlayerCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ADemonSlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ADemonSlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ADemonSlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ADemonSlayerCharacter::LookUpAtRate);
}

void ADemonSlayerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ADemonSlayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	/*if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}*/
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ADemonSlayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ADemonSlayerCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ADemonSlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ADemonSlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ADemonSlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ADemonSlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ADemonSlayerCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ADemonSlayerCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ADemonSlayerCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ADemonSlayerCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void ADemonSlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CallMyTrace();
	// If active object is being interacted with
	if (ObjectInteractingWith != NULL && ObjectInteractingWith->GetIsActive() == true)
	{
		// Add progress as current progress plus time since last tick
		ObjectInteractingWith->SetProgress(ObjectInteractingWith->GetProgress() + DeltaTime);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), ObjectInteractingWith->GetProgress()));
		// If progress is at max
		if (ObjectInteractingWith->GetProgress() >= holdSecond)
		{
			// Make demon slayer available if mask was interacted with
			if (ObjectInteractingWith->GetObjectiveID() == 101)
			{
				hasDemonSlayer = true;
			}
			// Complete objective
			ObjectInteractingWith->SetInteractedWith(true);
			if (ObjectInteractingWith->GetDisappearsAfterInteract())
			{
				ObjectInteractingWith->SetActorHiddenInGame(true);
				ObjectInteractingWith->SetActorEnableCollision(false);
			}
			ObjectInteractingWith = NULL;
		}
	}

	// If Demon Slayer mode is activated
	if (isDemonSlayerActivated && demonSlayerMeter > 0.0f)
	{
		// Reduce meter by cooldown constant
		demonSlayerMeter -= SLAYERDECREASERATE;
		if (demonSlayerMeter < 0.0f)
		{
			demonSlayerMeter = 0.0f;
		}
		if (demonSlayerMeter == 0.0f)
		{
			// Deactivate Demon Slayer mode
			DemonSlayerOff();
		}
	}

	// If Demon Slayer mode is deactivated and meter is not full
	if (!isDemonSlayerActivated && demonSlayerMeter < 1.0f)
	{
		// Increase meter by cooldown constant
		demonSlayerMeter += cooldownRate;
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), demonSlayerMeter));
		if (demonSlayerMeter > 1.0f)
		{
			demonSlayerMeter = 1.0f;
		}
	}

	if(currentAttackCooldown < ATTACK_COOLDOWN)
	{
		currentAttackCooldown += DeltaTime;
	}
}

void ADemonSlayerCharacter::ToggleDemonSlayer()
{
	// If player can activate Demon Slayer mode
	if (hasDemonSlayer)
	{
		// If Demon Slayer mode is off and the meter is full
		if (isDemonSlayerActivated == false && demonSlayerMeter == 1.0f)
		{
			// Turn on Demon Slayer mode
			DemonSlayerOn();
		}
		// If Demon Slayer mode is on
		else if (isDemonSlayerActivated == true)
		{
			// Turn off Demon Slayer mode
			DemonSlayerOff();
		}
	}
}

void ADemonSlayerCharacter::DemonSlayerOn()
{
	// Activate Demon Slayer mode
	isDemonSlayerActivated = true;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Demon Slayer mode activated"));
	FPostProcessSettings PostProDemonSlayer;
	// Apply red tint to camera
	PostProDemonSlayer.AmbientCubemap = CubeMap;
	PostProDemonSlayer.bOverride_AmbientCubemapTint = true;
	PostProDemonSlayer.AmbientCubemapTint = FLinearColor::Red;
	PostProDemonSlayer.bOverride_AmbientCubemapIntensity = true;
	PostProDemonSlayer.AmbientCubemapIntensity = 4.0f;
	/*PostProDemonSlayer.bOverride_VignetteIntensity;
	PostProDemonSlayer.VignetteIntensity = 1.0f;*/
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), PostProDemonSlayer.VignetteIntensity));
	FirstPersonCameraComponent->PostProcessSettings = PostProDemonSlayer;
	// Set visibility of all objects to true
	/*for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AInteractableObject* object = Cast<AInteractableObject>(*ActorItr);
		if (object)
		{
			if (FVector::Dist(this->GetActorLocation(), object->GetActorLocation()) < 500.0f)
			{
				object->SetActorHiddenInGame(false);
			}
		}
	}*/
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	cooldownRate = cooldownRate / 1.25;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ASlayerModeDecal* slayerDecal = Cast<ASlayerModeDecal>(*ActorItr);
		if (slayerDecal)
		{
			slayerDecal->SetActorHiddenInGame(false);
		}
	}

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ASlayerModeObject* slayerObject = Cast<ASlayerModeObject>(*ActorItr);
		if (slayerObject)
		{
			slayerObject->SetActorHiddenInGame(false);
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f"), cooldownRate));
}

void ADemonSlayerCharacter::DemonSlayerOff()
{
	// Deactivate Demon Slayer
	isDemonSlayerActivated = false;
	// Return post process settings to normal
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Demon Slayer mode deactivated"));
	FPostProcessSettings PostProNormal;
	FirstPersonCameraComponent->PostProcessSettings = PostProNormal;
	/*for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AInteractableObject* object = Cast<AInteractableObject>(*ActorItr);
		if (object)
		{
			object->SetActorHiddenInGame(true);
		}
	}*/
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ASlayerModeDecal* slayerDecal = Cast<ASlayerModeDecal>(*ActorItr);
		if (slayerDecal)
		{
			slayerDecal->SetActorHiddenInGame(true);
		}
	}
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ASlayerModeObject* slayerObject = Cast<ASlayerModeObject>(*ActorItr);
		if (slayerObject)
		{
			slayerObject->SetActorHiddenInGame(true);
		}
	}
}

void ADemonSlayerCharacter::OnInteract()
{	// If Demon Slayer is deactivated
	if (!isDemonSlayerActivated)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Entered interact"));
		if (currentActor)
		{
			// Set object interacting with to object currently looking at
			ObjectInteractingWith = currentActor;
			//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%d"), ObjectInteractingWith));
		}
	}
}

void ADemonSlayerCharacter::OnStopInteract()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Exited interact"));
	if (currentActor)
	{
		// Reset interaction
		ObjectInteractingWith = NULL;
	}
}

/* Trace code derived from FIT2097 tutorials code*/
void ADemonSlayerCharacter::CallMyTrace()
{
	// Get the location of the camera (where we are looking from) and the direction we are looking in
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();

	// How for in front of our character do we want our trace to extend?
	// ForwardVector is a unit vector, so we multiply by the desired distance
	const FVector End = Start + ForwardVector * 256;

	// Force clear the HitData which contains our results
	FHitResult HitData(ForceInit);

	// What Actors do we want our trace to Ignore?
	TArray<AActor*> ActorsToIgnore;

	//Ignore the player character - so you don't hit yourself!
	ActorsToIgnore.Add(this);

	// Call our Trace() function with the paramaters we have set up
	// If it Hits anything
	if (Trace(GetWorld(), ActorsToIgnore, Start, End, HitData, ECC_Visibility, false))
	{
		// Process our HitData
		if (HitData.GetActor())
		{

			//UE+
			UE_LOG(LogClass, Warning, TEXT("%s was hit."), *HitData.GetActor()->GetName());
			ProcessTraceHit(HitData);

		}
		else
		{
			UE_LOG(LogClass, Warning, TEXT("Nothing was hit."));
		}
	}
	else
	{

		actorRaytraced = false;
		currentActor = NULL;

	}

}

bool ADemonSlayerCharacter::Trace(
	UWorld* World,
	TArray<AActor*>& ActorsToIgnore,
	const FVector& Start,
	const FVector& End,
	FHitResult& HitOut,
	ECollisionChannel CollisionChannel = ECC_Pawn,
	bool ReturnPhysMat = false
) {

	// The World parameter refers to our game world (map/level) 
	// If there is no World, abort
	if (!World)
	{
		return false;
	}

	// Set up our TraceParams object
	FCollisionQueryParams TraceParams(FName(TEXT("My Trace")), true, ActorsToIgnore[0]);

	// Should we simple or complex collision?
	TraceParams.bTraceComplex = true;

	// We don't need Physics materials 
	TraceParams.bReturnPhysicalMaterial = ReturnPhysMat;

	// Add our ActorsToIgnore
	TraceParams.AddIgnoredActors(ActorsToIgnore);

	// When we're debugging it is really useful to see where our trace is in the world
	// We can use World->DebugDrawTraceTag to tell Unreal to draw debug lines for our trace
	// (remove these lines to remove the debug - or better create a debug switch!)
	/*const FName TraceTag("MyTraceTag");
	World->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;*/


	// Force clear the HitData which contains our results
	HitOut = FHitResult(ForceInit);

	// Perform our trace
	World->LineTraceSingleByChannel
	(
		HitOut,		//result
		Start,	//start
		End, //end
		CollisionChannel, //collision channel
		TraceParams
	);

	// If we hit an actor, return true
	return (HitOut.GetActor() != NULL);
}

void ADemonSlayerCharacter::ProcessTraceHit(FHitResult& HitOut)
{

	// Cast the actor to ABaseActor
	AInteractableObject* const ActorFound = Cast<AInteractableObject>(HitOut.GetActor());

	if (ActorFound)
	{
		// set object looking to object hit by ray trace 
		actorRaytraced = true;
		currentActor = ActorFound;
	}
	else
	{
		//UE_LOG(LogClass, Warning, TEXT("TestPickup is NOT a Pickup!"));
		// Reset ray trace information
		actorRaytraced = false;
		currentActor = NULL;
		ObjectInteractingWith = NULL;
	}
}

void ADemonSlayerCharacter::Attack()
{
	if (currentAttackCooldown >= ATTACK_COOLDOWN)
	{
		if (isDemonSlayerActivated)
		{
			// Check with ray trace for enemy 
			FindAttackTarget();
			if (attackTarget != NULL)
			{
				// Attack enemy
				isAttacking = true;
				attackTarget->SetHealth(attackTarget->GetHealth() - FMath::RandRange(DAMAGE_LOWERBOUND, DAMAGE_UPPERBOUND));
				ADemonController* attackTargetController = Cast<ADemonController>(attackTarget->GetController());
				// Set enemy's focus to player when attacked
				attackTargetController->SetFocusToPlayer(this);
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Enemy health: %f"), attackTarget->GetHealth()));
				// If enemy's health is zero 
				if (attackTarget->GetHealth() <= 0)
				{
					// Destroy enemy 
					attackTarget->Destroy();
					// Turn off Demon Slayer
					DemonSlayerOff();
					// Reset meter to value based on current meter value
					if (demonSlayerMeter > 0.5f)
					{
						demonSlayerMeter = 0.5f;
					}
					else
					{
						demonSlayerMeter = 0.0f;
					}
				}
				FTimerHandle UnusedHandle;
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &ADemonSlayerCharacter::AttackingToFalse, 2.633f, false);
				currentAttackCooldown = 0.0f;
			}
		}
	}
}

void ADemonSlayerCharacter::Stun()
{	
	// Check with ray trace for enemy 
	FindAttackTarget();
	if (attackTarget != NULL)
	{
		ADemonController* attackTargetController = Cast<ADemonController>(attackTarget->GetController());
		// Call enemy's stun function
		attackTargetController->Stun();
	}
}

void ADemonSlayerCharacter::Hint()
{
	ADemonSlayerGameMode* gameMode = Cast<ADemonSlayerGameMode>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{
		gameMode->SetDescription(gameMode->GetCurrentObjective()->GetSecondary());
	}
	demonSlayerMeter = 0.0f;
}

void ADemonSlayerCharacter::FindAttackTarget()
{
	// Get the location of the camera (where we are looking from) and the direction we are looking in
	const FVector Start = FirstPersonCameraComponent->GetComponentLocation();
	const FVector ForwardVector = FirstPersonCameraComponent->GetForwardVector();

	// How for in front of our character do we want our trace to extend?
	// ForwardVector is a unit vector, so we multiply by the desired distance
	const FVector End = Start + ForwardVector * 256;

	// Force clear the HitData which contains our results
	FHitResult HitData(ForceInit);

	// What Actors do we want our trace to Ignore?
	TArray<AActor*> ActorsToIgnore;

	//Ignore the player character - so you don't hit yourself!
	ActorsToIgnore.Add(this);


	// Call our Trace() function with the paramaters we have set up
	// If it Hits anything
	if (Trace(GetWorld(), ActorsToIgnore, Start, End, HitData, ECC_Visibility, false))
	{
		// Process our HitData
		if (HitData.GetActor())
		{

			//UE+
			UE_LOG(LogClass, Warning, TEXT("%s was hit."), *HitData.GetActor()->GetName());
			CanAttack(HitData);

		}
		else
		{
			UE_LOG(LogClass, Warning, TEXT("Nothing was hit."));
		}
	}
	else
	{
		attackTarget = NULL;
	}

}

void ADemonSlayerCharacter::CanAttack(FHitResult & HitOut)
{
	// Cast to demon
	ADemon* const EnemyFound = Cast<ADemon>(HitOut.GetActor());

	if (EnemyFound)
	{
		// Set attack target to enemy found by ray tracing 
		attackTarget = EnemyFound;
	}

	else 
	{
		// Reset attack target
		attackTarget = NULL;
	}
}




