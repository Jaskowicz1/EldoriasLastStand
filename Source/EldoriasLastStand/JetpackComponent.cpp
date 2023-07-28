// Fill out your copyright notice in the Description page of Project Settings.


#include "JetpackComponent.h"

#include "AttackComponent.h"
#include "EldoriasLastStandCharacter.h"

// Sets default values for this component's properties
UJetpackComponent::UJetpackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UJetpackComponent::StartJetpack()
{
	if(!GetOwner() || !GetOwner()->GetWorld() || !AllowedToJetpack || JetpackCurrentFuel <= 0)
	{
		return;
	}

	GetOwner()->GetWorld()->GetTimerManager().ClearTimer(GroundedTimerHandle);
	GroundedTimerHandle.Invalidate();
	
	GetOwner()->GetWorld()->GetTimerManager().SetTimer(JetpackTimerHandle, this,
		&UJetpackComponent::DoJetpack, JetpackThrustInterval, true, DelayForJetpackStart);
}

void UJetpackComponent::EndJetpack()
{
	GetOwner()->GetWorld()->GetTimerManager().ClearTimer(JetpackTimerHandle);
	JetpackTimerHandle.Invalidate();

	GetWorld()->GetTimerManager().SetTimer(GroundedTimerHandle, this, &UJetpackComponent::CheckGround, .01f, true);

	if(JetpackStarted)
	{
		OnJetpackEnded.Broadcast();
		JetpackStarted = false;
	}
}

void UJetpackComponent::ChangeJetpackStatus(bool Disallow)
{
	AllowedToJetpack = !Disallow;
	OnJetpackStatusChanged.Broadcast();

	if(JetpackStarted)
	{
		OnJetpackEnded.Broadcast();
		JetpackStarted = false;

		GetOwner()->GetWorld()->GetTimerManager().ClearTimer(JetpackTimerHandle);
		JetpackTimerHandle.Invalidate();

		GetOwner()->GetWorld()->GetTimerManager().ClearTimer(GroundedTimerHandle);
		GroundedTimerHandle.Invalidate();
	}
}

void UJetpackComponent::DoJetpack()
{
	AEldoriasLastStandCharacter* EldoriasLastStandCharacter = Cast<AEldoriasLastStandCharacter>(GetOwner());

	if(!EldoriasLastStandCharacter || EldoriasLastStandCharacter->GetAttackComponent()->WeaponEquipped
		|| !AllowedToJetpack)
		return;

	EldoriasLastStandCharacter->currentMovementState = MovementState::Jetpack;

	if(!JetpackStarted)
	{
		OnJetpackStarted.Broadcast();
		JetpackStarted = true;
	}

	/**
	 * This makes it so launching is NOT frame dependent, otherwise we'd end up with the Jetpack
	 * being worse if you have less frames as it takes more ms to reach "JetpackThrustInterval".
	 * This means that, even though it takes longer to reach this, it launches the player more
	 * as it's multiplied by Delta Seconds (Delta increases for less FPS).
	 */
	if(JetpackCurrentFuel > 0)
		EldoriasLastStandCharacter->LaunchCharacter(LaunchVector * LaunchMultiplier * GetOwner()->GetWorld()->GetDeltaSeconds(),
			false, false);


	// Effectively clamping.
	// If JetpackCurrentFuel is bigger than zero, take off the drop step.
	// Otherwise, clamp to 0.
	JetpackCurrentFuel = JetpackCurrentFuel > 0 ? JetpackCurrentFuel - JetpackFuelDropStep : 0;

	OnJetpackFuelUpdated.Broadcast();
}

void UJetpackComponent::CheckGround()
{

	AEldoriasLastStandCharacter* Character = Cast<AEldoriasLastStandCharacter>(GetOwner());

	if(!Character)
	{
		GetOwner()->GetWorld()->GetTimerManager().ClearTimer(GroundedTimerHandle);
		GroundedTimerHandle.Invalidate();
		return;
	}

	if(Character->IsGrounded() && Character->currentMovementState != MovementState::Jetpack)
	{
		JetpackCurrentFuel = JetpackCurrentFuel < JetpackMaxFuel ? JetpackCurrentFuel + JetpackFuelDropStep : JetpackMaxFuel;

		OnJetpackFuelUpdated.Broadcast();
		
		if(JetpackCurrentFuel >= JetpackMaxFuel)
		{
			GetOwner()->GetWorld()->GetTimerManager().ClearTimer(GroundedTimerHandle);
			GroundedTimerHandle.Invalidate();
			return;
		}
	}
}


