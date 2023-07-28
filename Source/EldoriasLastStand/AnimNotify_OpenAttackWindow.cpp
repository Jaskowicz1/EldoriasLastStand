// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_OpenAttackWindow.h"

#include "AttackComponent.h"

void UAnimNotify_OpenAttackWindow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UAttackComponent* AttackComp = Cast<UAttackComponent>(MeshComp->GetOwner()->GetComponentByClass(UAttackComponent::StaticClass()));

	// This shouldn't happen but just in-case.
	if(AttackComp == nullptr)
	{
		return;
	}
	
	AttackComp->AllowWeaponDamage = AllowAttack;
}
