#pragma once

UENUM(BlueprintType)
enum class AIState : uint8
{
	Idle					UMETA(DisplayName = "Idle/Disallowed"),
	Moving					UMETA(DisplayName = "Moving"),
	Combat					UMETA(DisplayName = "Combat"),
};
