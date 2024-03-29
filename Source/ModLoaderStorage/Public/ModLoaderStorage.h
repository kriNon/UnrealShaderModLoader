
#pragma once

#include <string>
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

namespace USModifications
{
	enum class modification_type
	{
		comment,
		replace,
		insert_after
	};

	struct modification {
		FString filePath; // Path to the file to be modified.
		modification_type type;
		std::string pattern;
		FString replacement;
		bool literal_pattern;
	};

	struct file_copy
	{
		FString SourcePath;
		FString TargetPath;
	};
}



class MODLOADERSTORAGE_API FModLoaderStorageModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	TArray<USModifications::modification> mods;
	TArray<USModifications::file_copy> file_copy_list;
};

namespace UnrealShaderModLoader
{
	static void AddModification_Comment(FString filePath, std::string pattern, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::comment,pattern, "", !regex_pattern});
		#endif
	}
	static void AddModification_Replace(FString filePath, std::string pattern, FString replacement, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::replace,pattern, replacement, !regex_pattern});
		#endif
	}
	static void AddModification_InsertAfter(FString filePath, std::string pattern, FString replacement, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::insert_after,pattern, replacement, !regex_pattern});
		#endif
	}
}