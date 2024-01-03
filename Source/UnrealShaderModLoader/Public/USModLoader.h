#pragma once
#include "ModLoaderStorage.h"

//Unreal Shader Mod Loader
class UNREALSHADERMODLOADER_API USModLoader
{
public:
	USModLoader(const FString& BaseDir, const FString& EngineShaderPath, const FString& PluginShaderPath, const FString& PluginShaderOverridesPath) : BaseDir(BaseDir), EngineShaderPath(EngineShaderPath), PluginShaderPath(PluginShaderPath), PluginShaderOverridesPath(PluginShaderOverridesPath)
	{
		IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
		storage_module = static_cast<FModLoaderStorageModule*>(Module);
	}
	
	void applyModifications();
	void PrepareShaderDirectories();

private:
	FString BaseDir;
	FString EngineShaderPath;
	FString PluginShaderPath;
	FString PluginShaderOverridesPath;

	FModLoaderStorageModule* storage_module;
	
};
