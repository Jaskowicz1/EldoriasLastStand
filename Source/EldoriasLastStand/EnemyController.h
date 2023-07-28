// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class ELDORIASLASTSTAND_API AEnemyController : public AAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	/** AI Perception Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* AIPerceptionComponent;

public:

	AEnemyController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=AI)
	UBehaviorTree* AI_BT;

	UFUNCTION()
	void TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
};
