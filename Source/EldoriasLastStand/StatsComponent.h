// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHealthChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FManaChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDied);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDORIASLASTSTAND_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

#pragma region Delegates

	UPROPERTY(BlueprintAssignable)
        FHealthChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
		FManaChanged OnManaChanged;
		
	UPROPERTY(BlueprintAssignable)
		FPlayerDied OnCharacterDeath;

#pragma endregion

#pragma region Functions

	UFUNCTION(BlueprintCallable, Category="Stats")
		void ModifyHealth(float HealthChange, AController* Damager = nullptr);

	UFUNCTION(BlueprintCallable, Category="Stats")
		void ModifyMana(float ManaChange);

	UFUNCTION(BlueprintCallable, Category="Stats")
		void SetHealth(float _Health);

	UFUNCTION(BlueprintCallable, Category="Stats")
		void SetMana(float _Mana);

#pragma endregion

#pragma region Variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float Health = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float MaxHealth = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float Mana = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float MaxMana = 50;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	bool Dead;

#pragma endregion 
		
};
