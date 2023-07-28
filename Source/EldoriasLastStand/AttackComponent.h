// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponObtained, AWeapon*, Weapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDORIASLASTSTAND_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

	/** Weapon Child Actor */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* WeaponChild;

public:	
	// Sets default values for this component's properties
	UAttackComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

#pragma region Delegates
	
	UPROPERTY(BlueprintAssignable)
	FWeaponObtained OnWeaponObtained;

#pragma endregion

#pragma region Variables

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon")
	TSubclassOf<AActor> StarterWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Stats")
	bool AllowWeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Weapon Stats")
	bool WeaponEquipped;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	bool ComboWindowOpen;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int AmountOfCombosAllowed = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	int CombosDone = 0;

#pragma endregion

#pragma region Functions

	UFUNCTION(BlueprintCallable, Category=Weapon)
	void SetNewWeapon(TSubclassOf<AActor> weapon);

	UFUNCTION(BlueprintCallable, Category=Weapon)
	void AttachWeaponToPlayer();

	UFUNCTION(BlueprintCallable, Category=Weapon)
	void AttachWeaponToHand();

#pragma endregion

#pragma region Montages

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UAnimMontage* Swing1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UAnimMontage* Swing2;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UAnimMontage* EquipSword;
    	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	UAnimMontage* WithdrawSword;

#pragma endregion 
		
};
