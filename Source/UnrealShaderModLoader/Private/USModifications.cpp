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
	// 1. Prepare the replacement text, ensuring it ends with a newline.
	std::string textToInsert = TCHAR_TO_UTF8(*replacement);
	if (textToInsert.empty() || textToInsert.back() != '\n')
	{
		textToInsert += '\n';
	}

	// 2. Find the end of the line that contains the match.
	const size_t endOfMatch = matchStart + matchLength;
	const size_t endOfLinePos = code.find('\n', endOfMatch);

	// 3. Insert the text.
	if (endOfLinePos != std::string::npos)
	{
		// A newline was found after the match, so we insert the text on the next line.
		code.insert(endOfLinePos + 1, textToInsert);
	}
	else
	{
		// No newline found; the match is on the last line of the string.
		// Append a newline followed by the replacement text.
		code += '\n';
		code += textToInsert;
	}
}

void USModifications::InsertBefore(std::string& code, const FString& replacement, const ptrdiff_t matchStart, const ptrdiff_t matchLength)
{
	// 1. Prepare the replacement text, ensuring it ends with a newline.
	std::string textToInsert = TCHAR_TO_UTF8(*replacement);
	if (textToInsert.empty() || textToInsert.back() != '\n')
	{
		textToInsert += '\n';
	}

	// 2. Find the beginning of the line that contains the match
	// by searching backwards from the match's start for a newline.
	const size_t prevNewlinePos = code.rfind('\n', matchStart);
	
	// 3. Insert the text.
	if (prevNewlinePos != std::string::npos)
	{
		// A previous newline was found. Insert the text right after it,
		// placing it on its own line just before the matched line.
		code.insert(prevNewlinePos + 1, textToInsert);
	}
	else
	{
		// No previous newline; the match is on the very first line.
		// Insert the text at the beginning of the entire string.
		code.insert(0, textToInsert);
	}
}