// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CheckState.h"

#include "AIState.h"
#include "EnemyCharacter.h"
#include "EnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"

UBTService_CheckState::UBTService_CheckState()
{
	bNotifyTick = 1;
	bCreateNodeInstance = true;
	NodeName = "Check Enemy State";
}

void UBTService_CheckState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if(State.IsNone() || TargetActor.IsNone() || DistanceToTarget.IsNone())
		return;

	AAIController* EnemyController = Cast<AAIController>(OwnerComp.GetAIOwner());
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(EnemyController->GetPawn());
	UBlackboardComponent* Blackboard = EnemyController->GetBlackboardComponent();
	UAnimInstance* AnimInstance = EnemyCharacter->GetMesh()->GetAnimInstance();

	if(!EnemyController || !EnemyCharacter || !EnemyController->GetBlackboardComponent() || !EnemyCharacter->GetMesh()
		|| !EnemyCharacter->GetMesh()->GetAnimInstance())
		return;

	ACharacter* Player = Cast<ACharacter>(Blackboard->GetValueAsObject(TargetActor.SelectedKeyName));

	if(!Player)
	{
		Blackboard->SetValueAsEnum(State.SelectedKeyName, static_cast<uint8>(AIState::Idle));
		EnemyCharacter->GetWidgetComponent()->SetVisibility(false);
		return;
	}

	if(!EnemyCharacter->GetWidgetComponent()->IsVisible())
		EnemyCharacter->GetWidgetComponent()->SetVisibility(true);
	
	if(Blackboard->GetValueAsObject(TargetActor.SelectedKeyName))
	{
		// Distance of AI to player.
		Blackboard->SetValueAsFloat(DistanceToTarget.SelectedKeyName,
			static_cast<float>(sqrt(pow((Player->GetActorLocation().X - EnemyCharacter->GetActorLocation().X), 2) +
				pow((Player->GetActorLocation().Y - EnemyCharacter->GetActorLocation().Y), 2) +
				pow((Player->GetActorLocation().Z - EnemyCharacter->GetActorLocation().Z), 2))));
	}

	if(Blackboard->GetValueAsFloat(DistanceToTarget.SelectedKeyName) <= 350 || AnimInstance->Montage_IsActive(nullptr))
	{
		Blackboard->SetValueAsEnum(State.SelectedKeyName, static_cast<uint8>(AIState::Combat));
	} else
	{
		Blackboard->SetValueAsEnum(State.SelectedKeyName, static_cast<uint8>(AIState::Moving));
	}
}