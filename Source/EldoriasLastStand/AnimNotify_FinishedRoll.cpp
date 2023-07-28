// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_FinishedRoll.h"

#include "EldoriasLastStandCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UAnimNotify_FinishedRoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AEldoriasLastStandCharacter* EldoriasLastStandCharacter = Cast<AEldoriasLastStandCharacter>(MeshComp->GetOwner());

	// We don't rely on the character casted to above so that AI can roll.
	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(MeshComp->GetOwner()->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
	
	if(CharacterMovementComponent == nullptr)
	{
		return;
	}

	CharacterMovementComponent->GroundFriction = 8;
	
	if(EldoriasLastStandCharacter != nullptr)
	{
		EldoriasLastStandCharacter->currentMovementState = MovementState::Walking;
	}
}
