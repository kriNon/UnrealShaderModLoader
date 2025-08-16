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
            continue; // Skip to next mod
        }

        std::string code(TCHAR_TO_UTF8(*FileContent));
        std::regex pattern = USMLStringLib::get_pattern(mod);

        // --- Phase 1: Find all matches and store their details ---
        // We use std::sregex_iterator to find all non-overlapping matches efficiently.
        std::vector<std::smatch> matches;
        auto matches_begin = std::sregex_iterator(code.begin(), code.end(), pattern);
        auto matches_end = std::sregex_iterator();

        for (std::sregex_iterator i = matches_begin; i != matches_end; ++i) {
            matches.push_back(*i);
        }

        // --- Phase 2: Check match count, issue warnings or errors ---
        if (matches.empty()) {
            UE_LOG(LogUnrealShaderModLoader, Error, TEXT("Expected at least one match for mod on '%s', but found 0."), *mod.filePath);
            continue; // Skip to the next mod
        }

        if (matches.size() > 1) {
            UE_LOG(LogUnrealShaderModLoader, Warning, TEXT("Mod for '%s' found %d matches. The modification will be applied to ALL of them."), *mod.filePath, matches.size());
        }

        // --- Phase 3: Apply modifications in reverse order ---
        // We iterate backwards through our stored matches. This is critical.
        // By modifying the string from the end to the beginning, we don't invalidate
        // the character positions of earlier matches.
        for (auto it = matches.rbegin(); it != matches.rend(); ++it) {
            const std::smatch& match = *it;
            const ptrdiff_t matchStart = match.position(0);
            const ptrdiff_t matchLength = match.length(0);
            std::string matchedSegment = match.str(0);

            switch (mod.type) {
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
        }

        // Convert the modified std::string back to FString before saving
        FString ModifiedFileContent = UTF8_TO_TCHAR(code.c_str());

        // Write the modified code back to the file
        if (!FFileHelper::SaveStringToFile(ModifiedFileContent, *Path)) {
            // Handle the error if the file can't be written
            UE_LOG(LogUnrealShaderModLoader, Error, TEXT("Failed to write to file: %s"), *Path);
        }
    }

    // Cleanup
    storage_module->mods.Empty();
}

void USModLoader::PrepareShaderDirectories()
{
	USMLFileHandler::ClearDirectoryWithCheck(PluginShaderPath, EngineShaderPath);
	USMLFileHandler::CopyFilesRecursively(EngineShaderPath, PluginShaderPath);
	USMLFileHandler::ExecuteFileCopyOperationsFromModLoader();
		
	applyModifications();
}