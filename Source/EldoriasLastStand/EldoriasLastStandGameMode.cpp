// Copyright Epic Games, Inc. All Rights Reserved.

#include "EldoriasLastStandGameMode.h"
#include "EldoriasLastStandCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEldoriasLastStandGameMode::AEldoriasLastStandGameMode()
{

	// Do this in the Blueprint Gamemode.
	/*
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	*/
}
