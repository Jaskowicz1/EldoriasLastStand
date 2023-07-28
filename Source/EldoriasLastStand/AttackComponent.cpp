// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackComponent.h"

#include "EldoriasLastStandCharacter.h"
#include "InventoryComponent.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if(Character == nullptr)
		return;
	
	WeaponChild = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponActor"));
	WeaponChild->SetupAttachment(Character->GetMesh());
}

// Called when the game starts
void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	SetNewWeapon(StarterWeapon);
}

void UAttackComponent::SetNewWeapon(TSubclassOf<AActor> weapon)
{

	// If the weapon specified is already the current weapon then we should just return.
	if(WeaponChild->GetChildActorClass() == weapon || !weapon)
		return;
	
	// Set the ChildActor Class to the weapon specified.
	// Unreal handles all the other logic as to how it creates the actor etc
	WeaponChild->SetChildActorClass(weapon);
	WeaponChild->GetChildActor()->SetOwner(GetOwner());

	WeaponEquipped = false;

	AEldoriasLastStandCharacter* Character = Cast<AEldoriasLastStandCharacter>(GetOwner());

	if(Character)
	{
		GetWorld()->GetTimerManager().ClearTimer(Character->UnequipTimerHandle);
	}
	
	UInventoryComponent* InventoryComp = Cast<UInventoryComponent>(GetOwner()->
		GetComponentByClass(UInventoryComponent::StaticClass()));

	// Attach the weapon to the actor itself.
	AttachWeaponToPlayer();
	
	AWeapon* weaponCasted = Cast<AWeapon>(WeaponChild->GetChildActor());
	
	if(InventoryComp != nullptr)
		InventoryComp->CurrentWeapon = weaponCasted;
	
	OnWeaponObtained.Broadcast(weaponCasted);
}

void UAttackComponent::AttachWeaponToPlayer()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if(!Character)
		return;

	FAttachmentTransformRules TransformRules =
		FAttachmentTransformRules( EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);

	// Could have each weapon specify their own location&rotation offset.
	// Would help if weapons are different from their pivot point but they won't be (for this project).
	WeaponChild->GetChildActor()->AttachToComponent(Character->GetMesh(), TransformRules, "pelvis");
	WeaponChild->GetChildActor()->SetActorRelativeLocation(FVector(.0f, -6.1f, -16.9f));
	// FRotator not being XYZ and being Pitch, Yaw, Roll makes me want to cry.
	// 1 hour of confusion just because of that.
	WeaponChild->GetChildActor()->SetActorRelativeRotation(FRotator(-70.0f, -90.0f, 330.0f));
}

void UAttackComponent::AttachWeaponToHand()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());

	if(!Character)
		return;

	FAttachmentTransformRules TransformRules =
		FAttachmentTransformRules( EAttachmentRule::SnapToTarget,
			EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, true);

	WeaponChild->GetChildActor()->AttachToComponent(Character->GetMesh(), TransformRules, "hand_r");
	WeaponChild->GetChildActor()->SetActorRelativeLocation(FVector(-10.0f, 2.0f, 8.0f));
	WeaponChild->GetChildActor()->SetActorRelativeRotation(FRotator(.0f, 90.0f, 30.0f));
}

