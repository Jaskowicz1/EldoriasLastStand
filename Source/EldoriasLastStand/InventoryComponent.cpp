// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include "QuestComponent.h"
#include "StatsComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	ConsumableTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Items.Consumable"));
	WeaponTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Items.Weapons"));
	QuestItemTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Items.Quest"));

	HealthTag = FGameplayTag::RequestGameplayTag(FName("Stats.Player.Health"));
	ManaTag = FGameplayTag::RequestGameplayTag(FName("Stats.Player.Mana"));

	if(!InventoryItems.IsEmpty())
	{
		for(FInventoryItemStruct item : InventoryItems) {
			OnItemAdded.Broadcast(item);
		}

		EquippedItem = GetFirstConsumableItem();

		if(EquippedItem.IsValid())
			OnItemUpdated.Broadcast(EquippedItem);
	}
}

void UInventoryComponent::AddItemToInventory(FInventoryItemStruct item)
{
	// Don't allow a null item.
	if(!IsValid(item.ItemImage))
		return;
	
	// If the inventory is empty, just add anyways and then update quickslot.
	if(InventoryItems.IsEmpty())
	{
		InventoryItems.Add(item);
		
		if(item.TypeOfItem == ConsumableTag)
		{
			EquippedItem = InventoryItems[0];
		} else if(item.TypeOfItem == QuestItemTag)
		{
			UQuestComponent* QuestComponent = Cast<UQuestComponent>(GetOwner()->GetComponentByClass(UQuestComponent::StaticClass()));

			if(QuestComponent)
				QuestComponent->CheckQuests();
		}

		// If it was a quest item and then it was removed because it matched a quest, we need to make sure that
		// we test for that, otherwise it will crash.
		if(InventoryItems.IsEmpty() || !InventoryItems.IsValidIndex(0))
			return;
		
		OnItemAdded.Broadcast(InventoryItems[0]);
		
		return;
	}

	if(InventoryItems.Contains(item))
	{
		// Because the item exists in the inventory, we should set the amount of the item instead of just adding an item.
		
		int itemIndex = InventoryItems.Find(item);
		InventoryItems[itemIndex].AmountOfItem = InventoryItems[itemIndex].AmountOfItem + item.AmountOfItem;

		if(item.TypeOfItem == QuestItemTag)
		{
			UQuestComponent* QuestComponent = Cast<UQuestComponent>(GetOwner()->GetComponentByClass(UQuestComponent::StaticClass()));

			if(QuestComponent)
				QuestComponent->CheckQuests();
		} else
		{
			// If EquippedItem is null or the EquippedItem is the same as the current item, we update the current item.
			if(EquippedItem.ItemImage == nullptr || EquippedItem == item)
			{
				EquippedItem = InventoryItems[itemIndex];
			}

			// Yes, we do check this above, but we need this check here so we can call this delegate.
			if(EquippedItem == item)
				OnItemUpdated.Broadcast(item);
		}

		OnItemAdded.Broadcast(item);
	}
	else // Because the item doesn't exist in the inventory, then we just out-right add it.
	{
		InventoryItems.Add(item);

		// Just avoid the checks below if it's a quest item.
		if(item.TypeOfItem == QuestItemTag)
		{
			UQuestComponent* QuestComponent = Cast<UQuestComponent>(GetOwner()->GetComponentByClass(UQuestComponent::StaticClass()));

			if(QuestComponent)
				QuestComponent->CheckQuests();
		} else
		{
			// Sets new Equipped Item.
			if(EquippedItem.ItemImage == nullptr && item.TypeOfItem.MatchesTag(ConsumableTag))
			{
				EquippedItem = item;
			}
		}

		OnItemAdded.Broadcast(item);
	}
}

void UInventoryComponent::RemoveItemFromInventory(FInventoryItemStruct item)
{
	// Don't allow a null item.
	if(!IsValid(item.ItemImage))
		return;
	
	// If the inventory is empty then don't allow anything.
	if(InventoryItems.IsEmpty())
		return;

	// Can't remove an item if we don't have it.
	if(!InventoryItems.Contains(item))
		return;

	// We can just do this because we know that the item is in the Inventory.
	int itemIndex = InventoryItems.Find(item);
	FInventoryItemStruct ItemStruct = InventoryItems[itemIndex];

	if((ItemStruct.AmountOfItem - item.AmountOfItem) <= 0)
	{
		InventoryItems.Remove(item);
		
		if(item.TypeOfItem.MatchesTag(ConsumableTag) && EquippedItem == item)
		{
			// Set new equipped item.
			EquippedItem = GetFirstConsumableItem();

			// If equipped item hasn't changed then we invalidate it.
			if(EquippedItem == item)
			{
				EquippedItem.Invalidate();
			}

			// Do this regardless.
			OnItemUpdated.Broadcast(EquippedItem);
		}

		OnItemRemoved.Broadcast(item);

		return;
	}

	InventoryItems[itemIndex].AmountOfItem = InventoryItems[itemIndex].AmountOfItem - item.AmountOfItem;
			
	// Update equipped item
	if(EquippedItem == item)
	{
		EquippedItem = InventoryItems[itemIndex];
	}

	OnItemUpdated.Broadcast(EquippedItem);
}

void UInventoryComponent::SwitchConsumableByIndex(bool IncreaseIndex)
{
	if(InventoryItems.IsEmpty() || InventoryItems.Num() == 1)
		return;

	// Make array of only consumable items.
	TArray<FInventoryItemStruct> Consumables;
	for(FInventoryItemStruct ItemStruct : InventoryItems)
	{
		if(ItemStruct.TypeOfItem.MatchesTag(ConsumableTag))
			Consumables.Add(ItemStruct);
	}

	// We don't want to try to switch to anything if there is no consumables or only 1 consumable item in the inventory. 
	if(Consumables.IsEmpty() || Consumables.Num() == 1)
		return;

	// Current index in the Consumables array of our EquippedItem.
	const int indexOfEquippedItem = Consumables.Find(EquippedItem);

	// We create this variable just so that we can make sure we don't have any misleading names because using
	// "indexOfEquippedItem" would be misleading later on.
	// If "IncreaseIndex" is true, increase the current index by 1, otherwise decrease by 1.
	// This means we can simply check if the index is now valid.
	const int indexOfNextItem = IncreaseIndex ? indexOfEquippedItem + 1 : indexOfEquippedItem - 1;

	// Check if the increased/decreased index value is a valid index in the InventoryItems array.
	// If it is then we set the Equipped item to that item in the array.
	// otherwise, if Increase, set to the first item, if decrease, set to the last item.
	if(IncreaseIndex)
		EquippedItem = Consumables.IsValidIndex(indexOfNextItem) ? Consumables[indexOfNextItem] : Consumables[0];
	else
		EquippedItem = Consumables.IsValidIndex(indexOfNextItem) ? Consumables[indexOfNextItem] : Consumables[Consumables.Num() - 1];

	OnItemUpdated.Broadcast(EquippedItem);
}

void UInventoryComponent::UseEquippedItem()
{
	UStatsComponent* StatsComp = Cast<UStatsComponent>(GetOwner()->GetComponentByClass(UStatsComponent::StaticClass()));
	
	// Make sure we actually have an item equipped
	if(!EquippedItem.ItemImage)
		return;

	// Duplicate equipped item, but set amount to 1
	// This allows me to reuse a function in the inventory.
	FInventoryItemStruct itemTemp = EquippedItem;
	itemTemp.AmountOfItem = 1;

	if(EquippedItem.ItemAffectsStat.MatchesTag(HealthTag))
	{
		// This sets the health.
		// Explanation: If the amount to affect by is bigger or equal to 0 then we add health.
		// If it's smaller than 0, we remove health. This allows for items to potentially hurt stats.
		StatsComp->ModifyHealth(EquippedItem.AffectAmount >= 0 ? EquippedItem.AffectAmount : -EquippedItem.AffectAmount);
	}

	if(EquippedItem.ItemAffectsStat.MatchesTag(ManaTag))
	{
		// Same as health but for mana, read comments for health for explanation.
		StatsComp->ModifyMana(EquippedItem.AffectAmount >= 0 ? EquippedItem.AffectAmount : -EquippedItem.AffectAmount);
	}

	// Only removes one of the item.
	RemoveItemFromInventory(itemTemp);
}

TArray<FInventoryItemStruct> UInventoryComponent::GetQuestItemStructs()
{

	TArray<FInventoryItemStruct> QuestItems;
	
	for(int i=0; i <= InventoryItems.Num() - 1; i++)
	{
		// Check if inventory item is valid.
		if(InventoryItems.IsValidIndex(i) && InventoryItems[i].TypeOfItem.MatchesTag(QuestItemTag))
			QuestItems.Push(InventoryItems[i]);
	}

	return QuestItems;
}

FInventoryItemStruct UInventoryComponent::GetFirstConsumableItem()
{
	FInventoryItemStruct item;

	// Will return a null item if inventory is empty;
	if(InventoryItems.IsEmpty())
		return item;
	
	for(int i=0; i <= InventoryItems.Num() - 1; i++)
	{
		if(InventoryItems[i].TypeOfItem.MatchesTag(ConsumableTag))
		{
			return InventoryItems[i];
		}
	}

	// Only called if for loop couldn't get a consumable item.
	return item;
}

