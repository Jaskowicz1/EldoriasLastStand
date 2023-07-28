#pragma once

UENUM(BlueprintType)
enum class QuestType : uint8
{
	Collect     UMETA(DisplayName = "Collect Item(s)"),
	Kill		UMETA(DisplayName = "Kill Enemies"),
	TalkTo		UMETA(DisplayName = "Talk To NPC"),
	ReachPoint	UMETA(DisplayName = "Reach a Point"),
};
