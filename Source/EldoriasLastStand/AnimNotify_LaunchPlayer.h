// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_LaunchPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ELDORIASLASTSTAND_API UAnimNotify_LaunchPlayer : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LaunchVector = FVector(0,0,0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool XYOverride;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ZOverride;

	// I wanted to make an overridable function but yeahhhhhhhh.....
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool DoCppLaunch;
};
