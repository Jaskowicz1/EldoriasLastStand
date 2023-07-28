#pragma once

UENUM(BlueprintType)
enum class MovementState : uint8
{
	Walking					UMETA(DisplayName = "Walking"),
	Sprinting				UMETA(DisplayName = "Sprinting"),
	Dodging					UMETA(DisplayName = "Dodging"),
	Jetpack					UMETA(DisplayName = "Using Jetpack"),
	Idle					UMETA(DisplayName = "Idle/Disallowed"),
};
