#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Settings.h"
#include "Distribution.h"
#include <tuple>
#include <utility>
#include <unordered_map>

using ActorInfoPtr = std::weak_ptr<ActorInfo>;

/// <summary>
/// Provides generic functions
/// </summary>
class Utility : public UtilityBase
{
public:

	#pragma region Comparison
	struct EqualsWeakPtrActorInfo
	{
		bool operator()(const ActorInfoPtr& lhs, const ActorInfoPtr& rhs)
		{
			return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
		}
	};

	template<typename T, typename U> 
	bool EqualsSharedPtr(const std::weak_ptr<T>& t, const std::weak_ptr<U>& u)
	{
		return !t.owner_before(u) && !u.owner_before(t);
	}

	/* template <typename T, typename U>
	inline bool EqualsSharedPtr(const std::weak_ptr<T>& t, const std::shared_ptr<U>& u)
	{
		return !t.owner_before(u) && !u.owner_before(t);
	}*/
	#pragma endregion

	#pragma region SortingFunctions
	// comparator used to sort magnitude - duration - AlchemyItem* lists for maximum magnitude descending
	/// <summary>
	/// Compares the magnitude of two alchemy items
	/// </summary>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	static bool SortMagnitude(std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> first, std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> second);

	// comparator used to sort magnitude - duration - AlchemyItem* lists for minimal amount of effects and max magnitude
	/// <summary>
	/// Compares the magnitude of two alchemy items
	/// </summary>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	static bool SortPotion(std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> first, std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> second);

	// comparator used to sort magnitude - duration - AlchemyItem* lists for no restorative effects, minimal amount of effects and max magnitude
	/// <summary>
	/// Compares the magnitude of two alchemy items
	/// </summary>
	/// <param name="first"></param>
	/// <param name="second"></param>
	/// <returns></returns>
	static bool SortFortify(std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> first, std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> second);
	#pragma endregion

	/// <summary>
	/// Returns a string representing the given form
	/// </summary>
	/// <param name="form"></param>
	/// <returns></returns>
	template <class T>
	static std::string PrintForm(T* form)
	{
		if (form == nullptr || form->GetFormID() == 0 || Logging::EnableGenericLogging == false)
			return "None";
		std::string plugin = "";
		if ((form->GetFormID() & 0xFF000000) != 0xFE000000) {
			plugin = Settings::pluginnames[(form->GetFormID() >> 24)];
		} else
			plugin = Settings::pluginnames[256 + (((form->GetFormID() & 0x00FFF000)) >> 12)];

		return std::string("[") + typeid(T).name() + "<" + Utility::GetHex(form->GetFormID()) + "><" + form->GetName() + "><" + plugin + ">]";
	}
	static std::string PrintForm(ActorInfo* acinfo);
	static std::string PrintForm(std::shared_ptr<ActorInfo> const& acinfo);
	static std::string PrintForm(std::weak_ptr<ActorInfo> acweak);

	/// <summary>
	/// Returns a string representing the given form
	/// </summary>
	/// <param name="form"></param>
	/// <returns></returns>
	template <class T>
	static std::string PrintFormNonDebug(T* form)
	{
		if (form == nullptr || form->GetFormID() == 0)
			return "None";
		std::string plugin = "";
		if ((form->GetFormID() & 0xFF000000) != 0xFE000000) {
			plugin = Settings::pluginnames[(form->GetFormID() >> 24)];
		} else
			plugin = Settings::pluginnames[256 + (((form->GetFormID() & 0x00FFF000)) >> 12)];

		return std::string("[") + typeid(T).name() + "<" + Utility::GetHex(form->GetFormID()) + "><" + form->GetName() + "><" + plugin + ">]";
	}
	static std::string PrintFormNonDebug(std::shared_ptr<ActorInfo> const& acinfo);

	static std::string PrintEffectDistr(Distribution::EffectDistr* distr)
	{
		std::string res = "";
		for (int i = 0; i < distr->effects.size(); i++)
			res += "<" + std::to_string(distr->effects[i].first) + "," + UtilityBase::ToString(distr->effects[i].second.effect) + ">";
		return res;
	}

	using UtilityBase::ToString; 
	/// <summary>
	/// converts an ActorStrength into a string
	/// </summary>
	/// <param name="acs"></param>
	/// <returns></returns>
	static std::string ToString(ActorStrength acs);

	/// <summary>
	/// Converts an ItemStrength into a string
	/// </summary>
	/// <param name="is"></param>
	/// <returns></returns>
	static std::string ToString(ItemStrength is);

	/// <summary>
	/// Converst an AlchemicEffect to a localized string
	/// </summary>
	/// <param name="ae"></param>
	/// <returns></returns>
	static std::string ToStringLocalized(AlchemicEffect ae);

	/// <summary>
	/// Returns a string representation of a distribution
	/// </summary>
	/// <param name="distribution"></param>
	/// <returns></returns>
	static std::string PrintDistribution(std::vector<std::tuple<int, AlchemicEffect>> distribution);

	/// <summary>
	/// Returns a string representation of an effect map [old version]
	/// </summary>
	/// <param name="distribution"></param>
	/// <returns></returns>
	static std::string PrintEffectMap(std::map<AlchemicEffect, float> effectMap);

	/// <summary>
	/// Returns a string representation of an effect map
	/// </summary>
	/// <param name="effectMap"></param>
	/// <returns></returns>
	static std::string PrintEffectMap(std::unordered_map<AlchemicEffect, Distribution::Effect>& effectMap);

	#pragma region Parsing

	/// <summary>
	/// Reads probability Adjusters
	/// </summary>
	/// <param name="input"></param>
	/// <param name="adjusterMap"></param>
	/// <param name="error"></param>
	static void ReadAdjusters(std::string input, std::unordered_map<AlchemicEffect, float>* adjusterMap, bool& error);

	/// <summary>
	/// Writes the probability adjusters to a string
	/// </summary>
	/// <param name="adjusterMap"></param>
	/// <returns></returns>
	static std::string WriteAdjusters(std::unordered_map<AlchemicEffect, float>* adjusterMap);

	/// <summary>
	/// Parses objects for distribution rules from a string input with an optional chance for items
	/// </summary>
	/// <param name="input">the string to parse</param>
	/// <param name="error">will be overwritten with [true] if an error occurs</param>
	/// <param name="file">the relative path of the file that contains the string</param>
	/// <param name="line">the line in the file that contains the string</param>
	/// <returns>a vector of parsed and validated objects and their chances</returns>
	static std::vector<std::tuple<AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> ParseCustomObjects(std::string input, bool& error, std::string file, std::string line);

	/// <summary>
	/// Parses AlchemyEffects from an input string.
	/// </summary>
	/// <param name="input">string to parse</param>
	/// <param name="error">Overrisable value, which is set to true if there is an error during parsing.</param>
	/// <returns>A vector of AlchemyEffects, Weights, and max occurences</returns>
	static std::vector<std::tuple<AlchemicEffect, float /*weight*/, int /*max*/>> ParseAlchemyEffects(std::string input, bool& error);

	/// <summary>
	/// parses alchemy effects from an input string for Rule version 3
	/// </summary>
	/// <param name="input"></param>
	/// <param name="preset"></param>
	static bool ParseAlchemyEffects(std::string input, Distribution::EffectPreset* preset);

	/// <summary>
	/// Computes a distribution from an effectmap.
	/// </summary>
	/// <param name="effectmap">effectmap containing effects and weights which will be translated into the distribution</param>
	/// <param name="range">range the distribution chances are computed for</param>
	/// <returns>Weighted Distribution</returns>
	static std::vector<std::tuple<int, AlchemicEffect>> GetDistribution(std::vector<std::tuple<AlchemicEffect, float, int>> effectmap, int range, bool chance = false);

	/// <summary>
	/// Computes a distribution from a unified effect map
	/// <param name="map">unified effect map the distribution is calculated from</param>
	/// <param name="range">range the distribution chances are computed for</param>
	/// </summary>
	static std::vector<std::tuple<int, AlchemicEffect>> GetDistribution(std::map<AlchemicEffect, float> map, int range, bool chance = false);

	/// <summary>
	/// Calculates a unified effect map, that contains at most one entry per AlchemyEffect present
	/// </summary>
	/// <param name="effectmap">effectmap containing effects and weights that shal be unified</param>
	/// <returns>map with alchemyeffects and their weights</returns>
	static std::map<AlchemicEffect, float> UnifyEffectMap(std::vector<std::tuple<AlchemicEffect, float, int>> effectmap);

	/// <summary>
	/// Sums the Alchemyeffects in [list]
	/// </summary>
	/// <param name="list">list with AlchemyEffects to sum</param>
	/// <returns>Combined value with all Alchemyeffects</returns>
	static AlchemicEffect SumAlchemyEffects(std::vector<std::tuple<int, AlchemicEffect>> list, bool chance = false);

	/// <summary>
	/// Checks whether poison can be applied to the weapons of an actor
	/// </summary>
	/// <param name="actor">Actor to check</param>
	/// <returns>Whether poison can be applied to the actors weapons</returns>
	static bool CanApplyPoison(std::shared_ptr<ActorInfo> const& actor);

	/// <summary>
	/// Returns whether an NPC has poison applied to their weapons, and the poison applied
	/// </summary>
	/// <param name="actor">Actor to check</param>
	/// <param name="pois">Poison found [out]</param>
	/// <returns></returns>
	static bool GetAppliedPoison(RE::Actor* actor, RE::ExtraPoison*& pois);

	/// <summary>
	/// Verifies that acinfo is a valid object
	/// <param name="acinfo">ActorInfo to verify</param>
	/// </summary>
	static bool VerifyActorInfo(std::shared_ptr<ActorInfo> const& acinfo);

	#pragma endregion

};
