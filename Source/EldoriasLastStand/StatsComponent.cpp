// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"

#include "EnemyCharacter.h"
#include "QuestComponent.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UStatsComponent::ModifyHealth(float HealthChange, AController* Damager)
{

	// Don't allow modification of health if dead.
	if(Dead)
		return;

	if(HealthChange > 0)
	{
		Health += HealthChange;
	} else
	{
		// HealthChange will be minus here, but we can't do - -x because that becomes + x
		// So we make HealthChange into a positive number and then minus health by that.
		Health -= HealthChange * -1;
	}

	Health = Health > MaxHealth ? MaxHealth : Health;

	// We still allow this to be broadcast
	OnHealthChanged.Broadcast();

	if(Health <= 0)
	{
		Dead = true;

		// Is the owner of the Stats Component an enemy
		AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());

		// Make sure enemy character and damager are valid.
		if(EnemyCharacter && Damager)
		{
			// Get QuestComponent from Damager's character.
			UQuestComponent* QuestComponent = Cast<UQuestComponent>(Damager->GetCharacter()->GetComponentByClass(UQuestComponent::StaticClass()));

			// Make sure its valid, damager may not have the QuestComponent
			// and assuming they do will cause a crash if they don't.
			if(QuestComponent)
			{
				QuestComponent->AddEnemyKilled(EnemyCharacter->DisplayName);
			}
		}
		
		OnCharacterDeath.Broadcast();
	}
}

void UStatsComponent::ModifyMana(float ManaChange)
{
	if(ManaChange > 0)
	{
		Mana += ManaChange;
	} else
	{
		Mana -= ManaChange * 1;
	}

	Mana = Mana > MaxMana ? MaxMana : Mana;

	OnManaChanged.Broadcast();
}

void UStatsComponent::SetHealth(float _Health)
{

	Health = _Health;
	
	// Clamp to max health.
	// Yes, I'm aware I could use "FMath::Clamp" but like, why use other libraries?
	// This asks if the health is above max health (after we've set it).
	// If it is, we instantly set to max health as our max value, otherwise we keep it as it is.
	// No point clamping to 0 if below because we're dead at that point anyways.
	Health = Health > MaxHealth ? MaxHealth : Health;

	OnHealthChanged.Broadcast();

	// Still allow death from setting in-case we want an instant kill box or something.
	if(Health <= 0)
    {
    	Dead = true;
    	OnCharacterDeath.Broadcast();
    }
}

void UStatsComponent::SetMana(float _Mana)
{
	Mana = _Mana;

	// Clamping mana to MaxMana so we don't go over the limit.
	Mana = Mana > MaxMana ? MaxMana : Mana;

	OnManaChanged.Broadcast();
}

