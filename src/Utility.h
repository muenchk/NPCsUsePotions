#pragma once

#include <iostream>
#include <string>
#include <sstream>

/// <summary>
/// Provides generic functions
/// </summary>
class Utility
{
public:

	#pragma region SortingFunctions
	// comparator used to sort magnitude - duration - AlchemyItem* lists for maximum magnitude descending
	static bool SortMagnitude(std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect> first, std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect> second)
	{
		return std::get<0>(first) > std::get<0>(second);
	}
	#pragma endregion

	static std::string GetHex(long val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}

private:

};
