// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "QuestData.h"
#include "Components/ActorComponent.h"
#include "QuestComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestCompleted, const FQuestDataStruct, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestObtained, const FQuestDataStruct, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActiveQuestChanged, const FQuestDataStruct, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestFailed, const FQuestDataStruct, Quest);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDORIASLASTSTAND_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

#pragma region Quests

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quests)
	TArray<FQuestDataStruct> ObtainedQuests;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quests)
	TArray<FQuestDataStruct> CompletedQuests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quests)
	TArray<FQuestDataStruct> FailedQuests;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Quests)
	FQuestDataStruct ActiveQuest;

#pragma endregion

#pragma region QuestData

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quests|Data")
	TMap<FString, int> EnemiesKilled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Quests|Data")
	TArray<FString> NPCsTalkedTo;

#pragma endregion

#pragma region QuestDelegates

	UPROPERTY(BlueprintAssignable)
	FQuestCompleted OnQuestCompleted;

	UPROPERTY(BlueprintAssignable)
	FQuestFailed OnQuestFailed;
		
	UPROPERTY(BlueprintAssignable)
	FQuestObtained OnQuestObtained;

	UPROPERTY(BlueprintAssignable)
	FActiveQuestChanged OnActiveQuestChanged;

#pragma endregion

#pragma region QuestFunctions
	
	/**
	 * Sets a quest as complete and fires "OnQuestCompleted".
	 * @param QuestData The quest to mark as complete.
	 * @param Failed Did the user fail the quest?
	 */
	UFUNCTION(BlueprintCallable, Category=Quests)
	void CompleteQuest(FQuestDataStruct QuestData, bool Failed);

	/**
	 * Update the Active Quest whilst also calling OnActiveQuestChanged.
	 * @param QuestData The quest to set the new ActiveQuest as.
	 */
	UFUNCTION(BlueprintCallable, Category=Quests)
	void SetActiveQuest(FQuestDataStruct QuestData);

	/**
	 * Checks to see if the player has the quest in "ObtainedQuests" or "CompletedQuests".
	 * @param QuestData The quest to check if the player has. <b>If the quest is repeatable, this will return false.</b>
	 * @param CheckCompleted If we should check if the player has completed the quest.
	 * <b>If the player doesn't have the quest and this is false, this will return false.</b>
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Quests)
	bool HasQuest(FQuestDataStruct QuestData, bool CheckCompleted);

	/**
	 * Checks to see if the quest specified is valid or is either null or default.
	 * @param QuestData The quest to check if valid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category=Quests)
	bool IsValidQuest(FQuestDataStruct QuestData);

	/**
	 * Adds a quest to "ObtainedQuests"
	 * @param QuestData The quest to add.
	 * @param SilentAdd Should we notify the player that they obtained a quest?
	 */
	UFUNCTION(BlueprintCallable, Category=Quests)
	void AddQuest(FQuestDataStruct QuestData, bool SilentAdd);

	/**
	 * This adds an enemy that you have killed into "EnemiesKilled" if you have the quest required for it. 
	 * <b>If the owner does not have a quest (that is not completed) that requires the enemy inputted, no enemy will be added.</b>
	 * @param EnemyName The name of the enemy to add to the EnemiesKilled array.
	 */
	UFUNCTION(BlueprintCallable, Category="Quests|Data")
	void AddEnemyKilled(FString EnemyName);
	
	/**
	 * This adds an NPC to the "NPCsTalkedTo" array, if you have the quest required for it. 
	 * * <b>If the owner does not have a quest (that is not completed) that requires the NPC inputted, no NPC will be added.</b>
	 * @param NPC The name of the NLC to add to the NPCsTalkedTo array.
	 */
	UFUNCTION(BlueprintCallable, Category="Quests|Data")
	void AddNPC_TalkedTo(FString NPC);

#pragma endregion

	/**
	 * This checks all the current quests and if the player matches the criteria for those quests.
	 * If the player does, it automatically completes the quest.
	 * The only type of quest that can't be completed here is "ReachPoint" quests as they require reaching a certain point.
	 */
	void CheckQuests();
		
};
