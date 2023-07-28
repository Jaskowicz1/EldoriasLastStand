// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "JetpackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJetpackStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJetpackEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJetpackStatusChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJetpackFuelUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDORIASLASTSTAND_API UJetpackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UJetpackComponent();

#pragma region Delegates

	UPROPERTY(BlueprintAssignable)
	FJetpackStarted OnJetpackStarted;

	UPROPERTY(BlueprintAssignable)
	FJetpackEnded OnJetpackEnded;

	UPROPERTY(BlueprintAssignable)
	FJetpackStatusChanged OnJetpackStatusChanged;

	UPROPERTY(BlueprintAssignable)
	FJetpackFuelUpdated OnJetpackFuelUpdated;

#pragma endregion

#pragma region Functions

	/**
	 * This starts the timer for the jetpack.
	 * <b>The higher the value, the less often the jetpack "thrusts" the player upwards.</b>
	 */
	UFUNCTION(BlueprintCallable, Category=Jetpack)
	void StartJetpack();

	/**
	 * This ends the jetpack.
	 */
	UFUNCTION(BlueprintCallable, Category=Jetpack)
	void EndJetpack();

	/**
	 * @brief This updates AllowedToJetpack and calls OnJetpackStatusChanged.
	 * @param Disallow Don't allow the player to Jetpack? <b>If False, player can jetpack.</b>
	 */
	UFUNCTION(BlueprintCallable, Category=Jetpack)
	void ChangeJetpackStatus(bool Disallow);

#pragma endregion

#pragma region Variables

	/**
	 * The time it takes for the jetpack to start.
	 * <b>The higher the value, the more time it takes for the jetpack to start.</b>
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	float DelayForJetpackStart = .1f;

	/**
	 * The time between each jetpack thrust.
	 * <b>The higher the value, the less often the jetpack "thrusts" the player upwards.</b>
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	float JetpackThrustInterval = .01f;

	/**
	 * The direction the Jetpack launches the player.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	FVector LaunchVector = FVector(0,0,40);

	/**
	 * The multiplier for the Jetpack launch.
	 * <b>This equates to LaunchVector * LaunchMultiplier.</b>
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	float LaunchMultiplier = 1;

	/**
	 * Is the owner allowed to use the jetpack?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	bool AllowedToJetpack;

	/**
	 * The amount max of fuel the Jetpack has.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	float JetpackMaxFuel = 1;

	/**
	 * The amount of fuel the jetpack should drop by.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	float JetpackFuelDropStep = 1;

	/**
	 * The amount of fuel the jetpack has.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Jetpack|Config")
	float JetpackCurrentFuel = 1;

#pragma endregion 
	
private:

	void DoJetpack();

	void CheckGround();
	
	FTimerHandle JetpackTimerHandle;
	FTimerHandle GroundedTimerHandle;

	bool JetpackStarted;
};
