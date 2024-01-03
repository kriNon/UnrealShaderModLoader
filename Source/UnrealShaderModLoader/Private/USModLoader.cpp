#include "USModLoader.h"

#include <regex>
#include "FileHandler.h"
#include "StringLib.h"
#include "UnrealShaderModLoader.h"
#include "USModifications.h"

void USModLoader::applyModifications() {
    for (const auto& mod : storage_module->mods) {
        // Read the file from the path
        FString Path = FPaths::Combine(PluginShaderPath, *mod.filePath);
        FString FileContent;
        if (!FFileHelper::LoadFileToString(FileContent, *Path)) {
        	UE_LOG(LogUnrealShaderModLoader, Error, TEXT("Failed to load file at path: %s"), *Path);
        }

        std::string code(TCHAR_TO_UTF8(*FileContent));  // Convert FString to std::string
    	std::regex pattern = USMLStringLib::get_pattern(mod);
    	
        std::smatch match;
        int matchCount = 0;
    	
    	//Check for match
    	std::string searchCode = code; // Copy of the code for searching
    	if(std::regex_search(searchCode, match, pattern))
    	{
    		matchCount++;
    		searchCode = match.suffix().str(); // Continue searching the remainder

    		// Preserve the original match by using a separate match_2 for the second search
    		std::smatch match_2 = match; 
    		if (std::regex_search(searchCode, match_2, pattern)) {
    			matchCount++;
    			break; // Exit if a second match is found
    		}
    	}

    	// Check match count
    	if (matchCount != 1) {
    		UE_LOG(LogUnrealShaderModLoader, Error, TEXT("Expected exactly one match, found %i"), matchCount);
    		continue; // Skip to the next mod
    	}

    	// Second Segment: Perform modifications using the stored first match details
        // Perform the regex search and modification
    	const ptrdiff_t matchStart = match.position(0);
    	const ptrdiff_t matchLength = match.length(0);
    	std::string matchedSegment = match.str(0);
    	
    	switch (mod.type)
    	{
    	case USModifications::modification_type::comment:
    		{
    			USModifications::Comment(code, matchedSegment, matchStart, matchLength);
    			break;
    		}
    	case USModifications::modification_type::replace:
    		{
    			USModifications::Replace(code, mod.replacement, matchStart, matchLength);
    			break;
    		}
    	case USModifications::modification_type::insert_after:
    		{
    			USModifications::InsertAfter(code, mod.replacement, matchStart, matchLength);
    			break;
    		}
    	}

        // Convert the modified std::string back to FString before saving
        FString ModifiedFileContent = UTF8_TO_TCHAR(code.c_str());

        // Write the modified code back to the file
        if (!FFileHelper::SaveStringToFile(ModifiedFileContent, *Path)) {
            // Handle the error if the file can't be written
            UE_LOG(LogUnrealShaderModLoader, Error, TEXT("Failed to write to file: %s"), *Path);
        }
    }

	//Cleanup
	storage_module->mods.Empty();
}

void USModLoader::PrepareShaderDirectories()
{
	USMLFileHandler::ClearDirectoryWithCheck(PluginShaderPath, EngineShaderPath);
	USMLFileHandler::CopyFilesRecursively(EngineShaderPath, PluginShaderPath);
	USMLFileHandler::ExecuteFileCopyOperationsFromModLoader();
		
	applyModifications();
}