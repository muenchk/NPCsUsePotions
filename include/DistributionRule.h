#pragma once

#include "Settings.h"

class ActorInfo;

enum class FalloffFunction
{
	Exponential = 0x1,
	Linear = 0x2,
};

struct EffCategory
{
	/// <summary>
	/// name of effect category
	/// </summary>
	std::string name = "";
	/// <summary>
	/// base chance of cat
	/// </summary>
	int baseChance = 50;
	/// <summary>
	/// falloff for further potions
	/// [BaseChance * pow(falloff, numpot)
	/// </summary>
	float falloff;
	/// <summary>
	/// delay until falloff is applied
	/// </summary>
	int falloffdelay = 0;
	/// <summary>
	/// type of falloff calculation
	/// </summary>
	FalloffFunction falloffFunc = FalloffFunction::Exponential;
	/// <summary>
	/// max number of items given in this category
	/// </summary>
	int max;
};

struct EffCategoryPreset
{
	/// <summary>
	/// name of the preset
	/// </summary>
	std::string name;
	/// <summary>
	/// categories for actor strength
	/// </summary>
	std::vector<EffCategory*> cats = {
		nullptr, nullptr, nullptr, nullptr, nullptr
	};
	/// <summary>
	/// item tier adjust
	/// </summary>
	int tieradjust = 0;
};

struct Effect
{
	AlchemicEffect effect;
	float weight;
	int max;
	int current = 0;
};

struct EffectLess
{
	bool operator()(const Effect& lhs, const Effect& rhs) const
	{
		return lhs.effect < rhs.effect;
	}
};

struct EffectDistr
{
	std::vector<std::pair<float, Effect>> effects;
	int total = 0;

	void RemoveEffect(AlchemicEffect eff)
	{
		auto itr = effects.begin();
		while (itr != effects.end()) {
			if (itr->second.effect == eff) {
				total -= itr->first;
				effects.erase(itr);
			}
			itr++;
		}
	}
};

struct EffectPreset
{
	/// <summary>
	/// name of the preset
	/// </summary>
	std::string name;
	std::unordered_map<AlchemicEffect, Effect> effects;
	EffectDistr standardDistr;

	AlchemicEffect validEffects;
};

/// <summary>
/// A distribution rule
/// </summary>
class DistributionRule
{
public:
	// if set to false this rule is effectively empty
	/// <summary>
	/// whether the rule is valid, if [false] this rule is effectively empty
	/// </summary>
	bool valid = true;
	/// <summary>
	/// Version of the rule
	/// </summary>
	int ruleVersion = 1;
	/// <summary>
	/// Type of the rule
	/// </summary>
	int ruleType = 1;
	/// <summary>
	/// Unique name of the rule
	/// </summary>
	std::string ruleName;

	/// TYPE 1 - Rule
	// general
	int rulePriority = 0;
	bool allowMixed = true;
	bool styleScaling = true;
	int numPotionEffects = 100;
	int numFortifyEffects = 100;
	int numPoisonEffects = 100;
	int numFoodEffects = 100;

	EffCategoryPreset* potions;
	EffectPreset* potionEffects;
	EffCategoryPreset* poisons;
	EffectPreset* poisonEffects;
	EffCategoryPreset* fortify;
	EffectPreset* fortifyEffects;
	EffCategoryPreset* food;
	EffectPreset* foodEffects;

	/// <summary>
	/// returns a random potion according to [strength] and [acsstrength]
	/// </summary>
	/// <param name="
	/// ">information about the actor the item is for</param>
	/// <returns>A randomly chosen potion according to the rule</returns>
	std::vector<RE::AlchemyItem*> GetRandomPotions(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// returns a random popoisontion according to [strength] and [acsstrength]
	/// </summary>v
	/// <returns>A randomly chosen poison according to the rule</returns>
	std::vector<RE::AlchemyItem*> GetRandomPoisons(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// returns a random fortify potion according to [strength] and [acsstrength]
	/// </summary>
	/// <param name="acinfo">information about the actor the item is for</param>
	/// <returns>A randomly chosen fortify potion according to the rule</returns>
	std::vector<RE::AlchemyItem*> GetRandomFortifyPotions(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// returns a random food according to [strength] and [acsstrength]
	/// </summary>
	/// <param name="acinfo">information about the actor the item is for</param>
	/// <returns>A randomly chosen food according to the rule</returns>
	std::vector<RE::AlchemyItem*> GetRandomFood(std::shared_ptr<ActorInfo> const& acinfo);

	/// <summary>
	/// Returns a distribution scaled according to the number of custom items of an actor and their combat style
	/// </summary>
	EffectDistr GetScaledDistribution(Settings::ItemType type, EffectPreset* preset, std::shared_ptr<ActorInfo> const& acinfo);

#define COPY(vec1, vec2)       \
	vec2.reserve(vec1.size()); \
	std::copy(vec1.begin(), vec1.end(), vec2.begin());

#define COPYMAP(map1, map2)                                         \
	{                                                               \
		auto cpmiter = map1.begin();                                \
		while (cpmiter != map1.end()) {                             \
			map2.insert_or_assign(cpmiter->first, cpmiter->second); \
			cpmiter++;                                              \
		}                                                           \
	}

	/// <summary>
	/// Creates a deep copy of this rule
	/// </summary>
	/// <returns>A deep copy of this rule</returns>
	DistributionRule* Clone()
	{
		DistributionRule* rl = new DistributionRule();
		rl->ruleVersion = ruleVersion;
		rl->valid = valid;
		rl->ruleType = ruleType;
		rl->ruleName = ruleName;
		rl->rulePriority = rulePriority;
		rl->allowMixed = allowMixed;
		rl->styleScaling = styleScaling;
		rl->potions = potions;
		rl->potionEffects = potionEffects;
		rl->poisons = poisons;
		rl->poisonEffects = poisonEffects;
		rl->fortify = fortify;
		rl->fortifyEffects = fortifyEffects;
		rl->food = food;
		rl->foodEffects = foodEffects;
		return rl;
	}

	DistributionRule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, bool _styleScaling, EffCategoryPreset* _potions, EffectPreset* _potionEffects, EffCategoryPreset* _poisons, EffectPreset* _poisonEffects, EffCategoryPreset* _fortify, EffectPreset* _fortifyEffects, EffCategoryPreset* _food, EffectPreset* _foodEffects) :
		ruleVersion{ _ruleVersion },
		ruleType{ _ruleType },
		ruleName{ _ruleName },
		rulePriority{ _rulePriority },
		allowMixed{ _allowMixed },
		styleScaling{ _styleScaling },
		potions{ _potions },
		poisons{ _poisons },
		fortify{ _fortify },
		food{ _food },
		potionEffects{ _potionEffects },
		poisonEffects{ _poisonEffects },
		fortifyEffects{ _fortifyEffects },
		foodEffects{ _foodEffects }
	{
	}
	DistributionRule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, bool _styleScaling, int maxPotions,
		std::vector<int> potion1Chance, std::vector<int> potion2Chance, std::vector<int> potion3Chance, std::vector<int> potion4Chance,
		std::vector<int> potionAdditionalChance, int potionTierAdjust,
		int maxFortify, std::vector<int> fortify1Chance, std::vector<int> fortify2Chance, std::vector<int> fortify3Chance, std::vector<int> fortify4Chance, std::vector<int> fortifyAddtionalChance, int fortifyTierAdjust,
		int maxPoisons, std::vector<int> poison1Chance, std::vector<int> poison2Chance, std::vector<int> poison3Chance, std::vector<int> poison4Chance, std::vector<int> poisonAdditionalChance, int poisonTierAdjust,
		std::vector<int> foodChance,
		std::vector<std::tuple<int, AlchemicEffect>> potionDistr,
		std::vector<std::tuple<int, AlchemicEffect>> poisonDistr,
		std::vector<std::tuple<int, AlchemicEffect>> fortifyDistr,
		std::vector<std::tuple<int, AlchemicEffect>> foodDistr,
		AlchemicEffect validPotions, AlchemicEffect validPoisons, AlchemicEffect validFortifyPotions, AlchemicEffect validFood);
	
	DistributionRule() {}
	/// <summary>
	/// initializes an empty rule
	/// </summary>
	/// <param name="invalid"></param>
	DistributionRule(bool invalid)
	{
		valid = invalid;
		ruleName = "empty";
	}

private:
	/// <summary>
	/// Returns an according to potion properties randomly chosen potion
	/// </summary>
	/// <param name="acinfo">information about the actor the item is for</param>
	/// <returns></returns>
	RE::AlchemyItem* GetRandomPotion(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr);
	/// <summary>
	/// Returns an according to poison properties randomly chosen poison
	/// </summary>
	/// <param name="acinfo">information about the actor the item is for</param>
	/// <returns></returns>
	RE::AlchemyItem* GetRandomPoison(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr);
	/// <summary>
	/// Returns an according to fortify properties randomly chosen fortify potion
	/// </summary>
	/// <param name="acinfo">information about the actor the item is for</param>
	/// <returns></returns>
	RE::AlchemyItem* GetRandomFortifyPotion(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr);
	/// <summary>
	/// Calculates and returns a random food item
	/// </summary>
	/// <param name="acinfo">information about the actor the item is for</param>
	/// <returns></returns>
	RE::AlchemyItem* GetRandomFood_intern(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr);

	/// <summary>
	/// Returns a random effect accoring to the rules item effect properties
	/// </summary>
	/// <param name="type">Determines which items effect property is consulted</param>
	/// <returns></returns>
	AlchemicEffect GetRandomEffect(Settings::ItemType type);
	/// <summary>
	/// Returns a random effect according to the given distribution
	/// </summary>
	/// <param name="distribution">distribution to pull effect from</param>
	/// <returns></returns>
	AlchemicEffect GetRandomEffect(EffectDistr& distr);
	/// <summary>
	/// Returns a random effect accoring to the rules item effect properties with additional custom item chance
	/// </summary>
	/// <param name="type">Determines which items effect property is consulted</param>
	/// <returns></returns>
	AlchemicEffect GetRandomEffectChance(Settings::ItemType type);

public:
	/// <summary>
	/// Removes an AlchemyEffect from the list of valid potion effects
	/// </summary>
	/// <param name="effect"></param>
	void RemoveAlchemyEffectPotion(AlchemicEffect effect);
	/// <summary>
	/// Removes an AlchemyEffect from the list of valid poison effects
	/// </summary>
	/// <param name="effect"></param>
	void RemoveAlchemyEffectPoison(AlchemicEffect effect);
	/// <summary>
	/// Removes an AlchemyEffect from the list of valid fortify potion effects
	/// </summary>
	/// <param name="effect"></param>
	void RemoveAlchemyEffectFortifyPotion(AlchemicEffect effect);
	/// <summary>
	/// Removes an AlchemyEffect from the list of valid food effects
	/// </summary>
	/// <param name="effect"></param>
	void RemoveAlchemyEffectFood(AlchemicEffect effect);
};
