// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealShaderModLoader : ModuleRules
{
	public UnrealShaderModLoader(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[] {"Core", "Projects", "RenderCore", "Engine", "ModLoaderStorage" });
	}
}
