// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "MovementState.h"
#include "QuestData.h"
#include "GameFramework/Character.h"
#include "EldoriasLastStandCharacter.generated.h"

UCLASS(config=Game)
class AEldoriasLastStandCharacter : public ACharacter
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** Weapon Child Actor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* WeaponChild;

	/** Attack Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UAttackComponent* AttackComp;

	/** Statistic Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"))
	class UStatsComponent* StatsComp;

	/** Quest Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Quests, meta = (AllowPrivateAccess = "true"))
	class UQuestComponent* QuestComp;

	/** Inventory Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComp;

	/** Jetpack Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Jetpack, meta = (AllowPrivateAccess = "true"))
	class UJetpackComponent* JetpackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animations, meta = (AllowPrivateAccess = "true"))
	class UAnimationComponent* AnimationComponent;
	
public:
	AEldoriasLastStandCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate.
	 * Can edit this is blueprint child (and should for faster iteration)
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Input)
	float TurnRateGamepad;

	/** The time it takes to un-equip the current weapon from the most recent action. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon|Config")
	float TimeForUnequip = 20.0f;

#pragma region Movement

	/**
	 * The time it takes for the player to hold down the Sprint button in order to start sprinting.
	 * <b>If the player lets go before this threshold, they will dodge instead.</b>
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Config")
	float SprintThreshold = .25f;

	/** The speed at which the player will be at whilst sprinting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Config")
	float SprintSpeed;

	/** The player's current movement state. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Movement)
	MovementState currentMovementState = MovementState::Walking;

	/**
	 * Can the player move?
	 * @returns True if currentMovementState is not equal to Idle.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Movement)
	bool CanMove() {
		return currentMovementState != MovementState::Idle; 
	}

#pragma endregion

#pragma region Animations

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UAnimMontage* StandingRoll;

#pragma endregion

#pragma region Sounds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sounds)
	USoundWave* ComboSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sounds)
	USoundWave* HitSound;

#pragma endregion 

#pragma region MiscFunctions

	/**
	 * Is the player currently on the ground?
	 * @returns True if it detects a valid actor/object 100cm (100 unreal units) below the player mesh.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Misc)
	bool IsGrounded()
	{
		FHitResult Hit;
		FVector TraceStart = GetMesh()->GetComponentLocation();
		FVector TraceEnd = GetMesh()->GetComponentLocation();
		TraceEnd.Z -= 100;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

		if(ShowDebugLines)
			DrawDebugLine(GetWorld(), TraceStart, TraceEnd, Hit.bBlockingHit ? FColor::Green : FColor::Red, false, 5.0f, 0, 5.0f);

		return Hit.bBlockingHit && IsValid(Hit.GetActor());
	}

#pragma endregion

#pragma region Development

	/**
	 * Show line traces that the player does?
	 * <b>This is automatically turned off on BeginPlay() if the project is packaged for distribution.</b>
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Development)
	bool ShowDebugLines;

#pragma endregion
	
protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

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

	void DoJump();

	void CheckGround();
	
	void StopJump();

	/**
	 * This starts a timer to wait .25 seconds for sprinting.
	 */
	void WaitForSprint();

	/**
	 * This starts the sprint.
	 */
	void DoSprint();
	
	/**
	 * Do a dodge.
	 */
	void DoDodge();

	void DoLightAttack();
	
	void DoUnequip();
	
	void ResetTimeDilation();
	
	void UseDownQuickslot();

	float normalSpeed;

public:
	
	FTimerHandle SprintTimerHandle;
	FTimerHandle UnequipTimerHandle;
	FTimerHandle GroundedTimerHandle;
	FTimerHandle ResetTimeDilationHandle;
	
private:
	bool WaitingForSprint;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns AttackComponent **/
	FORCEINLINE class UAttackComponent* GetAttackComponent() const { return AttackComp; }
	/** Returns QuestComponent **/
	FORCEINLINE class UQuestComponent* GetQuestComponent() const { return QuestComp; }
};