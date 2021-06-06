// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class U5Tutorial_1 : ModuleRules
{
	public U5Tutorial_1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
			{ "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG" });
	}
}
