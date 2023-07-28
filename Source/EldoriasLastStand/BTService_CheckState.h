// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckState.generated.h"

/**
 * 
 */
UCLASS()
class ELDORIASLASTSTAND_API UBTService_CheckState : public UBTService
{
	GENERATED_BODY()

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:

	UBTService_CheckState();

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector DistanceToTarget;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActor;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector State;
};
