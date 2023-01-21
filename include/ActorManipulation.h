#pragma once
#include"Settings.h"
#include <chrono>
#include <time.h>
#include <random>
#include <stdlib.h>
#include "Utility.h"
#include "Data.h"
#include "Compatibility.h"


/// <summary>
/// provides actor related functions
/// </summary>
class ACM
{
	/// <summary>
	/// pointer to the singleton of the data class
	/// </summary>
	static inline Data* data;
	/// <summary>
	/// pointer to the singleton of the compatibility class
	/// </summary>
	static inline Compatibility* comp;

public:

	/// <summary>
	/// inits data access
	/// </summary>
	static void Init();

	#pragma region AVFunctions
	/// <summary>
	/// Returns the current maximum for an actor value.
	/// This takes the base av and any modifiers into account
	/// </summary>
	/// <param name="_actor"></param>
	/// <param name="av"></param>
	/// <returns></returns>
	static float GetAVMax(RE::Actor* _actor, RE::ActorValue av)
	{
		// add base value, permanent modifiers and temporary modifiers (magic effects for instance)
		return _actor->GetPermanentActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av);
	}
	/// <summary>
	/// Returns the current percentage of an actor value (like percentag of health remaining)
	/// </summary>
	/// <param name="_actor">Actor to get av from</param>
	/// <param name="av">Actor value to compute</param>
	/// <returns></returns>
	static float GetAVPercentage(RE::Actor* _actor, RE::ActorValue av)
	{
		return _actor->GetActorValue(av) / (_actor->GetPermanentActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
		/*float tmp = _actor->GetActorValue(av) / (_actor->GetPermanentActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
		logger::info("[GetAVPercentage] {}", tmp);
		return tmp;*/
	}
	/// <summary>
	/// Returns the actor value percentage of an actor calculated from their base value
	/// and a given current value
	/// </summary>
	/// <param name="_actor">Actor to get av from</param>
	/// <param name="av">Actor value to compute</param>
	/// <param name="curr">current value</param>
	/// <returns></returns>
	static float GetAVPercentageFromValue(RE::Actor* _actor, RE::ActorValue av, float curr)
	{
		return curr / (_actor->GetPermanentActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
		/*float tmp = curr / (_actor->GetPermanentActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
		logger::info("[GetAVPercentageFromValue] {}", tmp);
		return tmp;*/
	}
	#pragma endregion

	#pragma region AlchemyFunctions
	/// <summary>
	/// checks wether an item has the desired effect and additionally returns its magnitude
	/// </summary>
	/// <param name="item">item to check</param>
	/// <param name="eff">effect to look for</param>
	/// <returns>wether the item has the specified effect, the magnitude, and the duration, the total effects, whether there is a detrimental effect</returns>
	static std::tuple<bool, float, int, AlchemyEffectBase, bool> HasAlchemyEffect(RE::AlchemyItem* item, AlchemyEffectBase alchemyEffect);

	/// <summary>
	/// Searches for potions with the effect [eff] in the inventory of the actor [actor]
	/// </summary>
	/// <param name="actor">actor to search</param>
	/// <param name="eff">effect to search for</param>
	/// <returns>list of matching items with magnitudes and durations</returns>
	static std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> GetMatchingPotions(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect);

	/// <summary>
	/// Searches for and returns all potions in the actors inventory
	/// </summary>
	/// <param name="acinfo"></param>
	/// <returns></returns>
	static std::list<RE::AlchemyItem*> GetAllPotions(ActorInfo* acinfo);

	/// <summary>
	/// Searches for poisons with the effect [eff] in the inventory of the actor [actor]
	/// </summary>
	/// <param name="actor">actor to search</param>
	/// <param name="eff">effect to search for</param>
	/// <returns>list of matching items with magnitudes and durations</returns>
	static std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> GetMatchingPoisons(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect);

	/// <summary>
	/// Searches for and returns all poisons in the actors inventory
	/// </summary>
	/// <param name="acinfo"></param>
	/// <returns></returns>
	static std::list<RE::AlchemyItem*> GetAllPoisons(ActorInfo* acinfo);

	/// <summary>
	/// Searches for food with the effect [eff] in the inventory of the actor [actor]
	/// </summary>
	/// <param name="actor">actor to search</param>
	/// <param name="eff">effect to search for</param>
	/// <returns>list of matching items with magnitude and durations</returns>
	static std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> GetMatchingFood(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool raw);

	/// <summary>
	/// Searched for and returns all food in the actors inventory
	/// </summary>
	/// <param name="acinfo"></param>
	/// <returns></returns>
	static std::list<RE::AlchemyItem*> GetAllFood(ActorInfo* acinfo);

	/// <summary>
	/// Returns a random food item from the list of food items in an actors inventory
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> GetRandomFood(ActorInfo* acinfo);

	/// <summary>
	/// Returns the custom items that an actor posseses
	/// </summary>
	static std::unordered_map<uint32_t, int> GetCustomItems(ActorInfo* acinfo);
	/// <summary>
	/// Returns a vector of [5] maps that contain [all], [potion], [poison], [fortify], [food] CustomAlchItems in the inventory
	/// </summary>
	static std::vector<std::unordered_map<uint32_t, int>> GetCustomAlchItems(ActorInfo* acinfo);

	/// <summary>
	/// tries to use a potion with the given effect [eff]
	/// </summary>
	/// <param name="acinfo">actor to apply potion on</param>
	/// <param name="eff">effect to apply</param>
	/// <returns>Wether a potion was consumed</returns>
	static std::tuple<int, AlchemyEffectBase, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>> ActorUsePotion(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool compatiblity = false);

	/// <summary>
	/// takes an already computed list and uses the first item in the list
	/// </summary>
	/// <param name="acinfo"></param>
	/// <param name="ls"></param>
	/// <returns></returns>
	static std::tuple<int, AlchemyEffectBase, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>> ActorUsePotion(ActorInfo* acinfo, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>& ls, bool compatibility = false);
	
	/// <summary>
	/// tries to use a food with the given effect [eff]
	/// </summary>
	/// <param name="acinfo">actor to apply food to</param>
	/// <param name="eff">effect to apply</param>
	/// <returns>wether a food was used</returns>
	static std::pair<int, AlchemyEffectBase> ActorUseFood(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool raw);

	/// <summary>
	/// tries to use a random food item from an actors inventory
	/// </summary>
	/// <param name="acinfo">actor to apply food to</param>
	/// <returns>wether a food was used</returns>
	static std::pair<int, AlchemyEffectBase> ActorUseFood(ActorInfo* acinfo);

	/// <summary>
	/// tries to use a poison with the given effect [eff]
	/// </summary>
	/// <param name="acinfo">actor which applies the poison</param>
	/// <param name="eff">effect to apply</param>
	/// <returns>wether a food was used</returns>
	static std::pair<int, AlchemyEffectBase> ActorUsePoison(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect);

	/// <summary>
	/// tries to use any supported food on the actor
	/// </summary>
	/// <param name="acinfo">actor to use on</param>
	/// <param name="raw">whether the food should be raw</param>
	/// <returns>wether any food was used</returns>
	static bool ActorUseAnyFood(ActorInfo* acinfo, bool raw)
	{
		if (std::get<0>(ActorUseFood(acinfo, static_cast<uint64_t> (AlchemyEffect::kMagickaRate), raw)) >= 0)
			return true;
		else if (std::get<0>(ActorUseFood(acinfo, static_cast<uint64_t> (AlchemyEffect::kStaminaRate), raw)) >= 0)
			return true;
		else if (std::get<0>(ActorUseFood(acinfo, static_cast<uint64_t> (AlchemyEffect::kHealRate), raw)) >= 0)
			return true;
		return false;
	}
	
	/* static bool AnimatedPoison_ApplyPoison(ActorInfo* acinfo, RE::AlchemyItem* poison); */

	#pragma endregion
};
