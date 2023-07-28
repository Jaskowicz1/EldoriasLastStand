// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DoAttack.h"

#include "AIController.h"
#include "AttackComponent.h"
#include "InventoryComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_DoAttack::UBTTask_DoAttack()
{
	bNotifyTick = 0;
	bCreateNodeInstance = true;
	NodeName = "Do Attack";
}

EBTNodeResult::Type UBTTask_DoAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(TargetActor.IsNone())
		return EBTNodeResult::Failed;

	AAIController* EnemyController = Cast<AAIController>(OwnerComp.GetAIOwner());
	ACharacter* EnemyCharacter = Cast<ACharacter>(EnemyController->GetPawn());
	UBlackboardComponent* Blackboard = EnemyController->GetBlackboardComponent();

	if(!EnemyController || !EnemyCharacter || !Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	// Declare Target here rather than above with the rest as we need to validate Blackboard.

	ACharacter* Target = Cast<ACharacter>(Blackboard->GetValueAsObject(TargetActor.SelectedKeyName));

	if(!Target)
		return EBTNodeResult::Failed;

	UAttackComponent* AttackComp = Cast<UAttackComponent>(EnemyCharacter->GetComponentByClass(
		UAttackComponent::StaticClass()));
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(EnemyCharacter->GetComponentByClass(
		UInventoryComponent::StaticClass()));

	if(!AttackComp || !InventoryComp || !InventoryComp->CurrentWeapon)
		return EBTNodeResult::Failed;
	
	if(!AttackComp->WeaponEquipped)
	{
		EnemyCharacter->PlayAnimMontage(AttackComp->EquipSword);
		AttackComp->WeaponEquipped = true;
	}

	// Well aware that this could be a one liner but it's hardly readable doing that so im not going to.
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(
		EnemyCharacter->GetActorLocation(),Target->GetActorLocation()).GetNormalized();
	EnemyCharacter->SetActorRotation(FRotator(LookRot.Pitch, EnemyCharacter->GetActorRotation().Yaw, LookRot.Roll));
	
	if(!EnemyCharacter->GetMesh()->GetAnimInstance()->Montage_IsActive(nullptr) &&
			!EnemyCharacter->GetMesh()->GetAnimInstance()->Montage_IsActive(AttackComp->Swing1))
	{
		EnemyCharacter->PlayAnimMontage(AttackComp->Swing1);
	}

	return EBTNodeResult::Succeeded;
}
