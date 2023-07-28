// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

AEnemyController::AEnemyController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyController::TargetPerceptionUpdated);

	// Set variables for AI Perception Component within the Blueprint child.

	AAIController::SetGenericTeamId(FGenericTeamId(1));
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	// Don't allow an invalid AI_BT.
	if(!AI_BT)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("AI_BT is invalid."), false);
		return;
	}
	
	RunBehaviorTree(AI_BT);
}

void AEnemyController::TargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if(!Blackboard)
		return;

	if(Stimulus.WasSuccessfullySensed())
	{
		Blackboard->SetValueAsObject("TargetActor", Actor);
	} else
	{
		Blackboard->SetValueAsObject("TargetActor", nullptr);
	}
}

ETeamAttitude::Type AEnemyController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn* OtherPawn = Cast<APawn>(&Other);
	const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());

	if(!OtherPawn || !TeamAgent)
		return ETeamAttitude::Neutral;
	
	FGenericTeamId OtherTeamID = TeamAgent->GetGenericTeamId();
	if (OtherTeamID == GetGenericTeamId()) {
		return ETeamAttitude::Friendly;
	}

	return ETeamAttitude::Hostile;
}
