#pragma once
#include "ModLoaderStorage.h"
#include "Interfaces/IPluginManager.h"

//Unreal Shader Mod Loader File Handler
namespace USMLFileHandler
{
	// Function to clear a directory with a check against a reference directory.
	static void ClearDirectoryWithCheck(const FString& DirectoryPath, const FString& ReferenceDirectoryPath) {
		// Get the platform file manager
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

		// Iterator to go through all files and directories in the specified path
		IFileManager& FileManager = IFileManager::Get();
		TArray<FString> FilesAndDirs;
		FileManager.FindFilesRecursive(FilesAndDirs, *DirectoryPath, TEXT("*"), true, true, false);

		// Loop through each found item and delete it if it doesn't exist in the reference directory
		for (const FString& Item : FilesAndDirs) {
			// Check if the item is the .gitignore file
			if (Item.EndsWith(TEXT(".gitignore"))) {
				continue; // Skip this file
			}

			// Construct the corresponding path in the reference directory
			FString CorrespondingReferencePath = ReferenceDirectoryPath + Item.RightChop(DirectoryPath.Len());

			// Check if the item exists in the reference directory
			bool bExistsInReference = PlatformFile.FileExists(*CorrespondingReferencePath) || PlatformFile.DirectoryExists(*CorrespondingReferencePath);
			if (!bExistsInReference) {
				if (PlatformFile.DirectoryExists(*Item)) {
					// If it's a directory and doesn't exist in the reference, delete it
					PlatformFile.DeleteDirectoryRecursively(*Item);
				} else {
					// If it's a file and doesn't exist in the reference, delete the file
					PlatformFile.DeleteFile(*Item);
				}
			}
		}
	}
	
	static void CopyFilesRecursively(const FString& SourcePath, const FString& TargetPath)
	{
		// Create the target directory if it doesn't exist
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		PlatformFile.CreateDirectoryTree(*TargetPath);

		// Copy each file in the source directory and its subdirectories
		IFileManager& FileManager = IFileManager::Get();
		TArray<FString> FileNames;
		FileManager.FindFilesRecursive(FileNames, *SourcePath, TEXT("*.*"), true, false);

		for (FString FileName : FileNames)
		{
			// Calculate the path in the target directory
			FString RelativePath = FileName;
			RelativePath.RemoveFromStart(SourcePath);
			FString Dest = TargetPath / RelativePath;

			// Create the directory in the target path if it doesn't exist
			PlatformFile.CreateDirectoryTree(*FPaths::GetPath(Dest));

			// Copy file
			if (!PlatformFile.CopyFile(*Dest, *FileName))
			{
				UE_LOG(LogTemp, Warning, TEXT("Error copying file from %s to %s"), *FileName, *Dest);
			}
		}
	};
	
	static FString FindFileInDirectory(const FString& Directory, const FString& FileName)
	{
		TArray<FString> FoundFiles;
		IFileManager::Get().FindFilesRecursive(FoundFiles, *Directory, *FileName, true, false);

		if (FoundFiles.Num() > 0)
		{
			return FoundFiles[0]; // Return the first found file
		}

		return FString();
	};

	//This function attempts to find matches of files in ShaderOverrides folder in the Shaders folder.
	//This means that the structure of ShaderOverrides does not need to match Shaders 100%
	static void CopyFilesWithMatchingNames(const FString& SourcePath, const FString& TargetPath)
	{
		IFileManager& FileManager = IFileManager::Get();
		TArray<FString> SourceFiles;
		FileManager.FindFilesRecursive(SourceFiles, *SourcePath, TEXT("*.*"), true, false);

		for (FString SourceFile : SourceFiles)
		{
			FString FileName = FPaths::GetCleanFilename(SourceFile);
			FString RelativePath = FPaths::GetPath(SourceFile).RightChop(SourcePath.Len()); // Get the relative path
			FString TargetFile = FPaths::Combine(TargetPath, RelativePath, FileName); // Directly combine to create the target path

			// Ensure the target directory exists before copying
			FString TargetDirectory = FPaths::GetPath(TargetFile);
			FileManager.MakeDirectory(*TargetDirectory, true);

			// Add the source and target file pair to the file_copy_list for later processing or copying
			IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
			FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
			storage_module->file_copy_list.Add({SourceFile, TargetFile});
		}
	};

	static void ExecuteFileCopyOperationsFromModLoader()
	{
		IModuleInterface* Module = FModuleManager::Get().GetModule("ModLoaderStorage");
		FModLoaderStorageModule* storage_module = static_cast<FModLoaderStorageModule*>(Module);
		if(storage_module == nullptr) {
			UE_LOG(LogTemp, Error, TEXT("ModLoaderStorage module not found."));
			return;
		}

		for(auto operation : storage_module->file_copy_list)
		{
			// Extract the directory from the target path
			FString TargetDirectory = FPaths::GetPath(operation.TargetPath);

			// Check if the directory exists, and if not, create it
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			if (!PlatformFile.DirectoryExists(*TargetDirectory))
			{
				if (!PlatformFile.CreateDirectoryTree(*TargetDirectory))
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to create directory: %s"), *TargetDirectory);
					continue; // Skip this file copy operation if the directory couldn't be created
				}
				UE_LOG(LogTemp, Log, TEXT("Created directory: %s"), *TargetDirectory);
			}

			// Perform the file copy
			if (PlatformFile.CopyFile(*operation.TargetPath, *operation.SourcePath))
			{
				UE_LOG(LogTemp, Log, TEXT("Copied %s to %s"), *operation.SourcePath, *operation.TargetPath);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to copy %s to %s"), *operation.SourcePath, *operation.TargetPath);
			}
		}
		storage_module->file_copy_list.Empty();
	}

	static void CopyShaderOverridesToUnrealShaderModLoader(const FString& SourcePluginName)
	{
		FString UnrealShaderModLoaderBaseDir = IPluginManager::Get().FindPlugin(TEXT("UnrealShaderModLoader"))->GetBaseDir();
		FString BaseDir = IPluginManager::Get().FindPlugin(SourcePluginName)->GetBaseDir();
	
		//Get Paths
		const FString PluginShaderPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(UnrealShaderModLoaderBaseDir, TEXT("Shaders")));
		const FString PluginShaderOverridesPath = FPaths::ConvertRelativePathToFull(FPaths::Combine(BaseDir, TEXT("ShaderOverrides")));
		CopyFilesWithMatchingNames(PluginShaderOverridesPath, PluginShaderPath);
	};
};
