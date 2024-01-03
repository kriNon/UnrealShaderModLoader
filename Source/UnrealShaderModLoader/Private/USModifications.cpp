#include "USModifications.h"

void USModifications::Comment(std::string& code, std::string& matchedSegment, const ptrdiff_t matchStart, const ptrdiff_t matchLength)
{
	if (matchedSegment.find('\n') != std::string::npos) {
		// Multi-line comment
		code.replace(matchStart, matchLength, "/*" + matchedSegment + "*/");
	} else {
		// Single-line comment
		code.replace(matchStart, matchLength, "//" + matchedSegment);
	}
}

void USModifications::Replace(std::string& code, const FString& replacement, const ptrdiff_t matchStart, const ptrdiff_t matchLength)
{
	// Replace the matched string with the replacement text
	//this is allowed to be replaced by an empty string
	code.replace(matchStart, matchLength, TCHAR_TO_UTF8(*replacement));
}

void USModifications::InsertAfter(std::string& code, const FString& replacement, const ptrdiff_t matchStart, const ptrdiff_t matchLength)
{
	ptrdiff_t insertPosition = matchStart + matchLength; // Position right after the matched text

	// Find the position of the next newline character after the match
	const size_t endOfLinePos = code.find('\n', insertPosition);
	if (endOfLinePos != std::string::npos) {
		// If a newline is found, adjust the insertPosition to be after the newline
		insertPosition = endOfLinePos + 1;
	} else {
		// If no newline is found, this means we're at the last line or the file doesn't end with a newline
		// You might want to add a newline to the end before inserting
		code += '\n';
		insertPosition = code.length();
	}

	// Check if mod.replacement ends with a newline character, and if not, add one
	std::string replacementText = TCHAR_TO_UTF8(*replacement);
	if (!replacementText.empty() && replacementText.back() != '\n') {
		replacementText += '\n';
	}

	// Insert the modified replacement text at the calculated position
	code.insert(insertPosition, replacementText);
}
