#pragma once
#include"Settings.h"
#include <chrono>
#include <time.h>
#include <random>
#include <stdlib.h>
#include "Utility.h"


/// <summary>
/// provides actor related functions
/// </summary>
class ACM
{

public:

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
		return _actor->GetBaseActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av);
	}
	/// <summary>
	/// Returns the current percentage of an actor value (like percentag of health remaining)
	/// </summary>
	/// <param name="_actor">Actor to get av from</param>
	/// <param name="av">Actor value to compute</param>
	/// <returns></returns>
	static float GetAVPercentage(RE::Actor* _actor, RE::ActorValue av)
	{
		return _actor->GetActorValue(av) / (_actor->GetBaseActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
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
		return curr / (_actor->GetBaseActorValue(av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kPermanent, av) + _actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
	}
	#pragma endregion

	#pragma region AlchemyFunctions
	/// <summary>
	/// checks wether an item has the desired effect and additionally returns its magnitude
	/// </summary>
	/// <param name="item">item to check</param>
	/// <param name="eff">effect to look for</param>
	/// <returns>wether the item has the specified effect, the magnitude, and the duration</returns>
	static std::tuple<bool, float, int, Settings::AlchemyEffect> HasAlchemyEffect(RE::AlchemyItem* item, uint64_t alchemyEffect);

	/// <summary>
	/// Searches for potions with the effect [eff] in the inventory of the actor [actor]
	/// </summary>
	/// <param name="actor">actor to search</param>
	/// <param name="eff">effect to search for</param>
	/// <returns>list of matching items with magnitudes and durations</returns>
	static std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> GetMatchingPotions(RE::Actor* actor, uint64_t alchemyEffect);

	/// <summary>
	/// Searches for poisons with the effect [eff] in the inventory of the actor [actor]
	/// </summary>
	/// <param name="actor">actor to search</param>
	/// <param name="eff">effect to search for</param>
	/// <returns>list of matching items with magnitudes and durations</returns>
	static std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> GetMatchingPoisons(RE::Actor* actor, uint64_t alchemyEffect);

	/// <summary>
	/// Searches for food with the effect [eff] in the inventory of the actor [actor]
	/// </summary>
	/// <param name="actor">actor to search</param>
	/// <param name="eff">effect to search for</param>
	/// <returns>list of matching items with magnitude and durations</returns>
	static std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> GetMatchingFood(RE::Actor* actor, uint64_t alchemyEffect);

	/// <summary>
	/// tries to use a potion with the given effect [eff]
	/// </summary>
	/// <param name="_actor">actor to apply potion on</param>
	/// <param name="eff">effect to apply</param>
	/// <returns>Wether a potion was consumed</returns>
	static std::tuple<int, Settings::AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>>> ActorUsePotion(RE::Actor* _actor, uint64_t alchemyEffect);

	/// <summary>
	/// takes an already computed list and uses the first item in the list
	/// </summary>
	/// <param name="_actor"></param>
	/// <param name="ls"></param>
	/// <returns></returns>
	static std::tuple<int, Settings::AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>>> ActorUsePotion(RE::Actor* _actor, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> &ls);
	
	/// <summary>
	/// tries to use a food with the given effect [eff]
	/// </summary>
	/// <param name="_actor">actor to apply food to</param>
	/// <param name="eff">effect to apply</param>
	/// <returns>wether a food was used</returns>
	static std::pair<int, Settings::AlchemyEffect> ActorUseFood(RE::Actor* _actor, uint64_t alchemyEffect);

	/// <summary>
	/// tries to use a poison with the given effect [eff]
	/// </summary>
	/// <param name="_actor">actor which applies the poison</param>
	/// <param name="eff">effect to apply</param>
	/// <returns>wether a food was used</returns>
	static std::pair<int, Settings::AlchemyEffect> ActorUsePoison(RE::Actor* _actor, uint64_t alchemyEffect);

	/// <summary>
	/// tries to use any supported food on the actor
	/// </summary>
	/// <param name="_actor">actor to use on</param>
	/// <returns>wether any food was used</returns>
	static bool ActorUseAnyFood(RE::Actor* _actor)
	{
		if (std::get<0>(ActorUseFood(_actor, static_cast<uint64_t> (Settings::AlchemyEffect::kMagickaRate))) >= 0)
			return true;
		else if (std::get<0>(ActorUseFood(_actor, static_cast<uint64_t> (Settings::AlchemyEffect::kStaminaRate))) >= 0)
			return true;
		else if (std::get<0>(ActorUseFood(_actor, static_cast<uint64_t> (Settings::AlchemyEffect::kHealRate))) >= 0)
			return true;
		return false;
	}

	#pragma endregion
};
