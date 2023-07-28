// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItem.h"
#include "QuestType.h"
#include "QuestData.generated.h"

USTRUCT(BlueprintType)
struct FQuestDataStruct
{
	GENERATED_BODY()

	FQuestDataStruct()
	{
		DisplayName = "Quest Name Here";
		Description = "About the Quest";
		TypeOfQuest = QuestType::Collect;
		Repeatable = false;
	}

	/**
	 * @brief The name of the Quest.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	FString DisplayName;

	/**
	 * @brief The description of the Quest.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	FString Description;

	/**
	 * @brief The type of quest.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	QuestType TypeOfQuest;

	/**
	 * @brief Should the quest be repeatable?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	bool Repeatable;

	/**
	 * @brief Items to collect (Will only be used if the type of quest is a collect quest).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	TArray<FInventoryItemStruct> ItemsToCollect;

	/**
	 * @brief Enemies to kill (int is the amount to kill, will only be used if the type of quest is a kill quest.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	TMap<FString, int> EnemiesToKill;

	/**
	 * @brief NPCs to talk to (Will only be used if the type of quest is a TalkTo quest).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quest")
	TArray<FString> NPCsToTalkTo;

	// This allows the QuestData to be compared via .Equals() because
	// otherwise we have to get complicated in comparing.
	friend bool operator ==(const FQuestDataStruct& x, const FQuestDataStruct& y)
	{
		return (x.DisplayName == y.DisplayName) && (x.TypeOfQuest == y.TypeOfQuest);
	}
	
	friend bool operator !=(const FQuestDataStruct& x, const FQuestDataStruct& y)
    {
    	return (x.DisplayName != y.DisplayName) || (x.TypeOfQuest != y.TypeOfQuest);
    }

	void ClearQuest()
	{
		DisplayName = "";
		Description = "";
	}
};

// Possibly not needed anymore, but will keep.
// Was created as a way to fix an error from TMap but we no longer use TMap for the Inventory.
inline uint32 GetTypeHash(const FQuestDataStruct& Value)
{
	return FCrc::MemCrc32(&Value, sizeof(FQuestDataStruct));
}


