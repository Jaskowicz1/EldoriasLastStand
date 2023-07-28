// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ComboWindow.generated.h"

/**
 * 
 */
UCLASS()
class ELDORIASLASTSTAND_API UAnimNotify_ComboWindow : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool OpenWindow;

};
