// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"

#include "AnimationComponent.h"
#include "AttackComponent.h"
#include "BrainComponent.h"
#include "EnemyController.h"
#include "InventoryComponent.h"
#include "StatsComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Don't think we need to tick?
	PrimaryActorTick.bCanEverTick = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Widget"));
	WidgetComponent->SetupAttachment(GetMesh());
	
	// Create an Attack Component
	AttackComp = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComp"));
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComp"));
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComp"));
	AnimationComponent = CreateDefaultSubobject<UAnimationComponent>(TEXT("AnimationComp"));
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

float AEnemyCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	// We do "-<value>" so that it actually deals damage.
	StatsComp->ModifyHealth(-Damage, EventInstigator);

	UGameplayStatics::PlaySound2D(DamageCauser, HitSound, 1, 1);

	if(StatsComp->Dead == true)
	{
		AEnemyController* AIController = Cast<AEnemyController>(GetController());

		if(!AIController)
			return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

		AIController->BrainComponent->StopLogic("Death");
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

		WidgetComponent->SetVisibility(false);

		//FTimerHandle TimerHandle;
		//GetWorldTimerManager().SetTimer(TimerHandle, this, &AActor::Destroy, 1.0f, true, 2.0f);
	}
	
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

