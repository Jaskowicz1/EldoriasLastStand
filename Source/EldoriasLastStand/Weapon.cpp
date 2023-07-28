// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "AttackComponent.h"
#include "AttackDamageType.h"
#include "EnemyCharacter.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// Probably a better way to test if they are two enemy classes (doing StaticClass() returns AActor)
	// but this works for now.
	const AEnemyCharacter* OtherEnemy = Cast<AEnemyCharacter>(OtherActor);
	const AEnemyCharacter* SelfEnemy = Cast<AEnemyCharacter>(GetOwner());

	// Make sure that weapons can't hurt their owners nor their friends (actors of same class).
	if(OtherActor == GetOwner() || (OtherEnemy && SelfEnemy))
	{
		return;
	}

	// Make sure the owner is valid otherwise we can cause a crash.
	if(!GetOwner())
		return;

	UAttackComponent* AttackComp = Cast<UAttackComponent>(GetOwner()->GetComponentByClass(UAttackComponent::StaticClass()));

	// Should never happen but in-case AttackComp is null, kill this.
	if(!AttackComp)
	{
		return;
	}

	if(AttackComp->AllowWeaponDamage)
	{
		// Would ideally implement some for of calculator here to allow third-party control
		// to damage but due to time constraints I won't do this.

		// Damage here is dependent on Combos, Because CombosDone starts at 0, we + 1 so we don't * by 0.
		// This can lead to some major scaling but that's a future me problem, not a now me problem.
		OtherActor->TakeDamage(Damage * (AttackComp->CombosDone + 1),
			FDamageEvent(UAttackDamageType::StaticClass()),
			GetOwner()->GetInstigatorController(),
			GetOwner());
	}
}

