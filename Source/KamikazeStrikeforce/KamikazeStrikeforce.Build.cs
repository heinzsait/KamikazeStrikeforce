// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class KamikazeStrikeforce : ModuleRules
{
	public KamikazeStrikeforce(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG" });
	}
}
