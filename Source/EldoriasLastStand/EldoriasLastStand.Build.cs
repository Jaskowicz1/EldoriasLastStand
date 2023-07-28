// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EldoriasLastStand : ModuleRules
{
	public EldoriasLastStand(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "GameplayTasks" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "CommonUI", "CommonInput", "UMG", "GameplayTags" });
	}
}
