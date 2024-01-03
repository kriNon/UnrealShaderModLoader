#pragma once
#include <string>


//Unreal Shader Modifications
namespace USModifications
{
	void Comment(std::string& code, std::string& matchedSegment, const ptrdiff_t matchStart, const ptrdiff_t matchLength);
	void Replace(std::string& code, const FString& replacement, const ptrdiff_t matchStart, const ptrdiff_t matchLength);
	void InsertAfter(std::string& code, const FString& replacement, const ptrdiff_t matchStart, const ptrdiff_t matchLength);
};
