// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestComponent.h"

#include "InventoryComponent.h"

// Sets default values for this component's properties
UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UQuestComponent::CompleteQuest(FQuestDataStruct QuestData, bool Failed)
{
	
	if(!HasQuest(QuestData, true) || !IsValidQuest(QuestData))
	{
		return;
	}

	ObtainedQuests.Remove(QuestData);

	if(Failed)
		FailedQuests.Add(QuestData);
	else
		CompletedQuests.Add(QuestData);

	if(ActiveQuest == QuestData)
	{
		ActiveQuest.ClearQuest();
	}

	if(Failed)
		OnQuestFailed.Broadcast(QuestData);
	else
		OnQuestCompleted.Broadcast(QuestData);
}

void UQuestComponent::SetActiveQuest(FQuestDataStruct QuestData)
{
	// Make sure the quest is valid and not already completed.
	if(!HasQuest(QuestData, false) || ActiveQuest == QuestData)
		return;
	
	ActiveQuest = QuestData;
	OnActiveQuestChanged.Broadcast(ActiveQuest);
}

bool UQuestComponent::HasQuest(FQuestDataStruct QuestData, bool CheckCompleted)
{
	// I could see this possibly being an issue but, with this project, it won't be.
	if(QuestData.Repeatable)
	{
		return false;
	}

	// Just an easy check to possibly reduce computational time.
	if(ActiveQuest == QuestData)
	{
		return true;
	}

	// If ObtainedQuests contains QuestData then return true otherwise check if "CheckCompleted" is true,
	// if CheckCompleted is true, then see if CompletedQuests contains QuestData, otherwise force false.
	return ObtainedQuests.Contains(QuestData) || (CheckCompleted ? CompletedQuests.Contains(QuestData) : false);
}

bool UQuestComponent::IsValidQuest(FQuestDataStruct QuestData)
{
	return !QuestData.DisplayName.IsEmpty() &&
			!QuestData.DisplayName.Equals("Quest Name Here") && 
			!QuestData.Description.Equals("About the Quest") &&
			!QuestData.Description.IsEmpty();
}

void UQuestComponent::AddQuest(FQuestDataStruct QuestData, bool SilentAdd)
{
	// If the player has the quest or it's not valid, return.
	if(HasQuest(QuestData, true) || !IsValidQuest(QuestData))
		return;

	ObtainedQuests.Add(QuestData);
	
	if(!SilentAdd)
	{
		ActiveQuest = QuestData;

		// Delegates are ALWAYS called last, otherwise you add the possibility
		// of quests not displaying with correct information.
		OnQuestObtained.Broadcast(QuestData);
	}

	CheckQuests();
}

void UQuestComponent::AddEnemyKilled(FString EnemyName)
{
	// Don't allow an invalid enemy.
	if(EnemyName.IsEmpty())
		return;

	for(FQuestDataStruct quest : ObtainedQuests)
	{
		// If the quest doesn't want this enemy then simply skip this one.
		if(!quest.EnemiesToKill.Contains(EnemyName))
			continue;

		// Because we know the array does want this enemy, we just either add 1
		// to the array or increment the number that comes with the key (EnemyName).
		if(EnemiesKilled.Contains(EnemyName))
		{
			// Increment the amount of EnemiesKilled in the array.
			int amount = EnemiesKilled.FindRef(EnemyName);

			amount++;

			EnemiesKilled.Add(EnemyName, amount);
		} else
		{
			EnemiesKilled.Add(EnemyName, 1);
		}
	}

	CheckQuests();
}

void UQuestComponent::AddNPC_TalkedTo(FString NPC)
{
	// Don't allow an invalid enemy.
	if(NPC.IsEmpty())
		return;

	// We've already talked to this NPC so no point trying to see if we should add it again.
	if(NPCsTalkedTo.Contains(NPC))
		return;

	for(FQuestDataStruct quest : ObtainedQuests)
	{
		// If the quest doesn't want this NPC then simply skip this one.
		if(!quest.NPCsToTalkTo.Contains(NPC))
			continue;

		NPCsTalkedTo.Add(NPC);
		// We don't break here in-case another quest also wants this NPC. This could technically break as you couldn't
		// do two different dialogues but this game doesn't have that anyways so no bother fixing
		// as it won't raise any issues.
	}

	CheckQuests();
}

void UQuestComponent::CheckQuests()
{
	// Technically, this could cause lag if the player has A LOT of quests not completed, but that will never happen.
	// Checking every quest everytime this calls is also a good fail-safe in-case a quest somehow doesn't complete
	// from one execution point meaning if another quest tries to complete, this should (hopefully) force the broken
	// quest to auto-complete.
	
	UInventoryComponent* InventoryComponent = Cast<UInventoryComponent>(GetOwner()->GetComponentByClass(UInventoryComponent::StaticClass()));
	
	// Used to check for the ActiveQuest at first to prevent more checks but now there is no needed
	// (as explained above).

	// Create a temp array so the for loop size doesn't change which can cause crashes.
	TArray<FQuestDataStruct> tempQuestArray = ObtainedQuests;
	
	// Loop through all obtained quests (but through a temp array).
	for(FQuestDataStruct Quest : tempQuestArray)
	{
		// If Collect Quest
		if(Quest.TypeOfQuest == QuestType::Collect)
		{
			// Make an array of items to store that have been completed for this quest.
			TArray<FInventoryItemStruct> CompletedItems;
			
			for(FInventoryItemStruct itemToGet : Quest.ItemsToCollect)
			{
				int itemIndex = InventoryComponent->InventoryItems.Find(itemToGet);

				// If the itemIndex is none (-1) or the InventoryItems doesn't contain the item
				// then just scrap this item and continue looping through.
				// Technically we only need the last check here but oh well.
				if(!InventoryComponent->InventoryItems.Contains(itemToGet) || itemIndex == INDEX_NONE)
					continue;

				// If item in the inventory matches the amount of the item that the quest wants.
				if(InventoryComponent->InventoryItems[itemIndex].AmountOfItem == itemToGet.AmountOfItem)
				{
					CompletedItems.Push(InventoryComponent->InventoryItems[itemIndex]);
				}
			}

			// Because the amount of items needed matches the amount of items obtained, we can make the quest complete.
			if(CompletedItems.Num() == Quest.ItemsToCollect.Num())
			{
				// Do this here so we don't remove items if we haven't completed the quest, otherwise we could possibly
				// end up with a never-ending quest.
				for(FInventoryItemStruct item : CompletedItems)
				{
					// This will remove the amount of items that the quest specified.
					// Meaning if the player has more than the quest wants, we don't just remove all of them.
					InventoryComponent->RemoveItemFromInventory(item);
				}
				
				CompleteQuest(Quest, false);
			}
		}
		else if(Quest.TypeOfQuest == QuestType::Kill)
		{
			// Does not need to be a TMap to follow the structure defines as Quest.EnemiesToKill
			TArray<FString> CompletedKills;

			// Make an array of keys, allowing us to iterate through this array
			// and then get the value (amount of kills needed).
			TArray<FString> KillsNeeded;
			Quest.EnemiesToKill.GetKeys(KillsNeeded);
			
			for(FString EnemyToKill : KillsNeeded)
			{
				// No point doing more checks if EnemiesKilled doesn't contain the quest enemy needed.
				if(!EnemiesKilled.Contains(EnemyToKill))
					continue;
				
				int AmountOfKillsNeeded = Quest.EnemiesToKill.FindRef(EnemyToKill);
				int AmountOfKillsDone = EnemiesKilled.FindRef(EnemyToKill);

				// If the itemIndex is none (-1) or zero
				// then just scrap this item and continue looping through.
				if(AmountOfKillsNeeded == INDEX_NONE || AmountOfKillsNeeded == 0 || AmountOfKillsDone == 0)
					continue;

				// If item in the inventory matches the amount of the item that the quest wants.
				if(AmountOfKillsNeeded == AmountOfKillsDone)
				{
					CompletedKills.Push(EnemyToKill);
				}
			}

			if(CompletedKills.Num() == Quest.EnemiesToKill.Num())
			{
				for(FString Enemy : CompletedKills)
				{
					EnemiesKilled.Remove(Enemy);
				}
				
				CompleteQuest(Quest, false);
			}
		}
		else if(Quest.TypeOfQuest == QuestType::TalkTo)
		{
			TArray<FString> CompletedTalks;
			
			for(FString EnemyToKill : Quest.NPCsToTalkTo)
			{
				if(NPCsTalkedTo.Contains(EnemyToKill))
					CompletedTalks.Push(EnemyToKill);
			}

			if(CompletedTalks.Num() == Quest.NPCsToTalkTo.Num())
			{
				// Loop from CompletedTalks as going off "NPCsTalkedTo" will cause crashes by modifying
				// the array at runtime whilst looping through it.
				for(FString NPC : CompletedTalks)
				{
					NPCsTalkedTo.Remove(NPC);
				}
				
				CompleteQuest(Quest, false);
			}
		}
	}
}
