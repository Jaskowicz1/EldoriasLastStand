// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class ELDORIASLASTSTAND_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Attack, meta = (AllowPrivateAccess = "true"))
	class UAttackComponent* AttackComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stats, meta = (AllowPrivateAccess = "true"))
	class UStatsComponent* StatsComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animations, meta = (AllowPrivateAccess = "true"))
	class UAnimationComponent* AnimationComponent;

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	

	/** Returns WidgetComp subobject **/
	FORCEINLINE class UWidgetComponent* GetWidgetComponent() const { return WidgetComponent; }

#pragma region Movement

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Movement)
	MovementState currentMovementState = MovementState::Walking;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Movement)
	bool CanMove() {
		return currentMovementState != MovementState::Idle;
	}

#pragma endregion

#pragma region CharacterStats

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	FString DisplayName;

#pragma endregion

#pragma region Animations

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation|Stats")
	bool CurrentlyUpperBodyAnim;

#pragma endregion

#pragma region MiscFunctions

	// Is the player currently on the ground?
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Misc)
	bool IsGrounded()
	{
		FHitResult Hit;
		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation();
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Development)
	bool ShowDebugLines;

#pragma endregion

#pragma region Sounds

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Sounds)
	USoundWave* HitSound;

#pragma endregion 

};
