#pragma once
#include <regex>
#include <string>

#include "ModLoaderStorage.h"

//Unreal Shader Mod Loader String Library
namespace USMLStringLib
{
	static std::string escape_regex(const std::string& string)
	{
		std::regex specialChars { R"([-[\]{}()*+?.,\^$|#\s])" };
		std::string sanitized = std::regex_replace( string, specialChars, R"(\$&)" );
		return sanitized;
	};
	static std::regex get_pattern(const USModifications::modification& mod)
	{
		if (mod.literal_pattern) {
			const std::string escaped_pattern = escape_regex(mod.pattern);
			return std::regex(escaped_pattern);
		} else {
			return std::regex(mod.pattern);
		}
	};
};
