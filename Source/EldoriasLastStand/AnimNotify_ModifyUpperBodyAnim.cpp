// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_ModifyUpperBodyAnim.h"

#include "AnimationComponent.h"
#include "EldoriasLastStandCharacter.h"

void UAnimNotify_ModifyUpperBodyAnim::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UAnimationComponent* AnimationComponent = Cast<UAnimationComponent>(MeshComp->GetOwner()->GetComponentByClass(UAnimationComponent::StaticClass()));

	if(!AnimationComponent)
		return;

	AnimationComponent->CurrentlyUpperBodyAnim = DoingUpperBody;
}
