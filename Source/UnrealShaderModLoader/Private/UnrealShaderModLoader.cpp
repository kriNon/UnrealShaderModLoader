// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealShaderModLoader.h"

#include "USModLoader.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FUnrealShaderModLoaderModule"
DEFINE_LOG_CATEGORY(LogUnrealShaderModLoader);

void FUnrealShaderModLoaderModule::StartupModule()
{
	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin(TEXT("UnrealShaderModLoader"))->GetBaseDir();//todo
	
	//Get Paths
	const FString EngineShaderPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::EngineDir(), TEXT("Shaders/Private")));
	const FString PluginShaderPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(BaseDir, TEXT("Shaders")));
	const FString PluginShaderOverridesPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(BaseDir, TEXT("ShaderOverrides")));

#if WITH_EDITOR
	USModLoader ModLoader(BaseDir, EngineShaderPath, PluginShaderPath, PluginShaderOverridesPath);
	ModLoader.PrepareShaderDirectories();
#endif

	// Register the shader directory
	AddShaderSourceDirectoryMapping(TEXT("/Engine/Private"), PluginShaderPath);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUnrealShaderModLoaderModule, UnrealShaderModLoader)