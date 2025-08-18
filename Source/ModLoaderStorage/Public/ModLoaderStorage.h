
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
		insert_after,
		insert_before
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
	static void AddModification_Comment(FString filePath, FString pattern, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::comment,std::string(TCHAR_TO_UTF8(*pattern)), "", !regex_pattern});
		#endif
	}
	static void AddModification_Replace(FString filePath, FString pattern, FString replacement, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::replace,std::string(TCHAR_TO_UTF8(*pattern)), replacement, !regex_pattern});
		#endif
	}
	static void AddModification_InsertAfter(FString filePath, FString pattern, FString replacement, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::insert_after,std::string(TCHAR_TO_UTF8(*pattern)), replacement, !regex_pattern});
		#endif
	}
	static void AddModification_InsertBefore(FString filePath, FString pattern, FString replacement, bool regex_pattern = false)
	{
		#if WITH_EDITOR
				IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
				FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
				storage_module->mods.Add(USModifications::modification{filePath, USModifications::modification_type::insert_before,std::string(TCHAR_TO_UTF8(*pattern)), replacement, !regex_pattern});
		#endif
	}
}