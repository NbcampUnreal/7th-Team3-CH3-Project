// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Team3_Project : ModuleRules
{
	public Team3_Project(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "AIModule", "NavigationSystem", "GameplayTasks" });
	}
}
