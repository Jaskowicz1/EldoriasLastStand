// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_LaunchPlayer.h"
#include "GameFramework/Character.h"

void UAnimNotify_LaunchPlayer::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                      const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(!DoCppLaunch)
	{
		return;
	}

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());

	if(Character == nullptr)
	{
		return;
	}

	Character->LaunchCharacter(LaunchVector, XYOverride, ZOverride);
}
