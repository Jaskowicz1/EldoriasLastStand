// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "Weapon.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAdded, const FInventoryItemStruct, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemRemoved, const FInventoryItemStruct, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemUpdated, const FInventoryItemStruct, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponAdded, const AWeapon*, Weapon);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDORIASLASTSTAND_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

#pragma region InventoryDelegates

	UPROPERTY(BlueprintAssignable)
	FItemAdded OnItemAdded;

	UPROPERTY(BlueprintAssignable)
	FItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable)
	FItemUpdated OnItemUpdated;

	UPROPERTY(BlueprintAssignable)
	FWeaponAdded OnWeaponAdded;

#pragma endregion

#pragma region Inventory

	/**
	 * An array of all the player's items.
	 * You can add items here within the detail panel for the owner to spawn with the items.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Inventory)
	TArray<FInventoryItemStruct> InventoryItems;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Inventory)
	AWeapon* CurrentWeapon;

	// This is the currently equipped item (Consumable)
	// Ideally, this would be a direct reference to an item in InventoryItems.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Inventory)
	FInventoryItemStruct EquippedItem;

	// Is the inventory currently open?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Inventory)
	bool InventoryOpen;

#pragma endregion

#pragma region InventoryFunctions
	
	/**
	 * This function adds an item to the inventory.
	 * @param item The item to add. If the item already exists, the "AmountOfItem" inside this param will be added to the item in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void AddItemToInventory(FInventoryItemStruct item);

	/**
	 * This function removes an item from the inventory.
	 * @param item The item to remove. <b>The item will only be removed if the "AmountOfItem" inside this param is bigger or equal to the amount in the inventory</b>.
	 */
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void RemoveItemFromInventory(FInventoryItemStruct item);

	UFUNCTION(BlueprintCallable, Category=Inventory)
	void SwitchConsumableByIndex(bool IncreaseIndex);

	/**
	 * This function uses the currently equipped item. <b>There is currently no implementation for multiple stats being affected.</b>
	 */
	UFUNCTION(BlueprintCallable, Category=Inventory)
	void UseEquippedItem();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Inventory)
	TArray<FInventoryItemStruct> GetQuestItemStructs();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Inventory)
	FInventoryItemStruct GetFirstConsumableItem();

#pragma endregion

private:

#pragma region Tags

	FGameplayTag ConsumableTag;
	FGameplayTag WeaponTag;
	FGameplayTag QuestItemTag;

	FGameplayTag HealthTag;
	FGameplayTag ManaTag;

#pragma endregion 
	
};
