#pragma once

#include "AlchemyEffect.h"
#include "CustomItem.h"
#include "Misc.h"
#include "Data.h"
#include "Settings.h"

#include <set>

	/// <summary>
/// Class handling all functions related to item distribution
/// </summary>
class Distribution
{
public:
	/// <summary>
	/// Supported Association types for distribution rules
	/// </summary>
	enum AssocType
	{
		kKeyword = 1,
		kFaction = 2,
		kRace = 4,
		kActor = 8,
		kNPC = 16,
		kItem = 32,
		kClass = 64,
		kCombatStyle = 128,
		kEffectSetting = 256,
	};

	class CustomItemStorage;

#define RandomRange 1000

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
			while (itr != effects.end())
			{
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
	class Rule
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

#define COPYMAP(map1, map2) \
	{                                                               \
		auto cpmiter = map1.begin();                                \
		while (cpmiter != map1.end()) {                             \
			map2.insert_or_assign(cpmiter->first, cpmiter->second); \
			cpmiter++;												\
		}															\
	}

		/// <summary>
		/// Creates a deep copy of this rule
		/// </summary>
		/// <returns>A deep copy of this rule</returns>
		Rule* Clone()
		{
			Rule* rl = new Rule();
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

		Rule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, bool _styleScaling, EffCategoryPreset* _potions, EffectPreset* _potionEffects, EffCategoryPreset* _poisons, EffectPreset* _poisonEffects, EffCategoryPreset* _fortify, EffectPreset* _fortifyEffects, EffCategoryPreset* _food, EffectPreset* _foodEffects) :
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
		Rule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, bool _styleScaling, int maxPotions,
			std::vector<int> potion1Chance, std::vector<int> potion2Chance, std::vector<int> potion3Chance, std::vector<int> potion4Chance,
			std::vector<int> potionAdditionalChance, int potionTierAdjust,
			int maxFortify, std::vector<int> fortify1Chance, std::vector<int> fortify2Chance, std::vector<int> fortify3Chance, std::vector<int> fortify4Chance, std::vector<int> fortifyAddtionalChance, int fortifyTierAdjust,
			int maxPoisons, std::vector<int> poison1Chance, std::vector<int> poison2Chance, std::vector<int> poison3Chance, std::vector<int> poison4Chance, std::vector<int> poisonAdditionalChance, int poisonTierAdjust,
			std::vector<int> foodChance,
			std::vector<std::tuple<int, AlchemicEffect>> potionDistr,
			std::vector<std::tuple<int, AlchemicEffect>> poisonDistr,
			std::vector<std::tuple<int, AlchemicEffect>> fortifyDistr,
			std::vector<std::tuple<int, AlchemicEffect>> foodDistr,
			AlchemicEffect validPotions, AlchemicEffect validPoisons, AlchemicEffect validFortifyPotions, AlchemicEffect validFood) :
			ruleVersion{ _ruleVersion },
			ruleType{ _ruleType },
			ruleName{ _ruleName },
			rulePriority{ _rulePriority },
			allowMixed{ _allowMixed },
			styleScaling{ _styleScaling }
		{
			potions = new Distribution::EffCategoryPreset();
			potions->name = ruleName + "_potions_catpreset";
			Distribution::_internEffectCategoryPresets.insert_or_assign(potions->name, potions);
			poisons = new Distribution::EffCategoryPreset();
			poisons->name = ruleName + "_poisons_catpreset";
			Distribution::_internEffectCategoryPresets.insert_or_assign(poisons->name, poisons);
			fortify = new Distribution::EffCategoryPreset();
			fortify->name = ruleName + "_fortify_catpreset";
			Distribution::_internEffectCategoryPresets.insert_or_assign(fortify->name, fortify);
			food = new Distribution::EffCategoryPreset();
			food->name = ruleName + "_food_catpreset";
			Distribution::_internEffectCategoryPresets.insert_or_assign(food->name, food);

			potionEffects = new Distribution::EffectPreset();
			potionEffects->name = ruleName + "_potion_effectspreset";
			Distribution::_internEffectPresets.insert_or_assign(potionEffects->name, potionEffects);
			poisonEffects = new Distribution::EffectPreset();
			poisonEffects->name = ruleName + "_poison_effectspreset";
			Distribution::_internEffectPresets.insert_or_assign(poisonEffects->name, poisonEffects);
			fortifyEffects = new Distribution::EffectPreset();
			fortifyEffects->name = ruleName + "_fortify_effectspreset";
			Distribution::_internEffectPresets.insert_or_assign(fortifyEffects->name, fortifyEffects);
			foodEffects = new Distribution::EffectPreset();
			foodEffects->name = ruleName + "_food_effectspreset";
			Distribution::_internEffectPresets.insert_or_assign(foodEffects->name, foodEffects);

			auto meanfalloff = [](std::vector<int> vec1, std::vector<int> vec2, std::vector<int> vec3, std::vector<int> vec4, int num) {
				float mean = 0;
				int mac = 0;
				if (vec1[num] != 0) {
					mean += (float)vec2[num] / (float)vec1[num];
					mac++;
				}
				if (vec2[num] != 0) {
					mean += (float)vec3[num] / (float)vec2[num];
					mac++;
				}
				if (vec3[num] != 0) {
					mean += (float)vec4[num] / (float)vec3[num];
					mac++;
				}
				if (mac != 0)
					mean = mean / (float)mac;
				return mean;
			};
			auto addcats = [](Distribution::EffCategoryPreset* preset) {
				for (int i = 0; i < 5; i++) {
					preset->cats[i] = new Distribution::EffCategory();
					preset->cats[i]->name = preset->name + "_" + std::to_string(i);
					Distribution::_internEffectCategories.insert_or_assign(preset->cats[i]->name, preset->cats[i]);
				}
			};
			auto setcat = [](Distribution::EffCategoryPreset* preset, int num, int base, float falloff, int max) {
				preset->cats[num]->baseChance = base;
				preset->cats[num]->falloff = falloff;
				preset->cats[num]->max = max;
			};
			addcats(potions);
			addcats(poisons);
			addcats(fortify);
			addcats(food);
			potions->tieradjust = potionTierAdjust;
			poisons->tieradjust = poisonTierAdjust;
			fortify->tieradjust = fortifyTierAdjust;
			food->tieradjust = 0;

			potionEffects->validEffects = validPotions;
			poisonEffects->validEffects = validPoisons;
			fortifyEffects->validEffects = validFortifyPotions;
			foodEffects->validEffects = validFood;

			setcat(potions, 0, potion1Chance[0], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 0), maxPotions);
			setcat(potions, 1, potion1Chance[1], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 1), maxPotions);
			setcat(potions, 2, potion1Chance[2], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 2), maxPotions);
			setcat(potions, 3, potion1Chance[3], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 3), maxPotions);
			setcat(potions, 4, potion1Chance[4], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 4), maxPotions);

			setcat(poisons, 0, poison1Chance[0], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 0), maxPoisons);
			setcat(poisons, 1, poison1Chance[1], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 1), maxPoisons);
			setcat(poisons, 2, poison1Chance[2], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 2), maxPoisons);
			setcat(poisons, 3, poison1Chance[3], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 3), maxPoisons);
			setcat(poisons, 4, poison1Chance[4], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 4), maxPoisons);

			setcat(fortify, 0, fortify1Chance[0], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 0), maxFortify);
			setcat(fortify, 1, fortify1Chance[1], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 1), maxFortify);
			setcat(fortify, 2, fortify1Chance[2], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 2), maxFortify);
			setcat(fortify, 3, fortify1Chance[3], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 3), maxFortify);
			setcat(fortify, 4, fortify1Chance[4], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 4), maxFortify);

			setcat(food, 0, foodChance[0], 0, 1);
			setcat(food, 1, foodChance[1], 0, 1);
			setcat(food, 2, foodChance[2], 0, 1);
			setcat(food, 3, foodChance[3], 0, 1);
			setcat(food, 4, foodChance[4], 0, 1);

			for (auto [weight, alch] : potionDistr) {
				Distribution::Effect eff;
				eff.effect = alch;
				eff.max = 0;
				eff.current = 0;
				eff.weight = 1000 * weight;
				potionEffects->effects.insert_or_assign(alch, eff);
			}
			potionEffects->standardDistr = Distribution::GetEffectDistribution(potionEffects->effects);
			potionEffects->validEffects = Distribution::SumAlchemyEffects(potionEffects->standardDistr);

			for (auto [weight, alch] : poisonDistr) {
				Distribution::Effect eff;
				eff.effect = alch;
				eff.max = 0;
				eff.current = 0;
				eff.weight = 1000 * weight;
				poisonEffects->effects.insert_or_assign(alch, eff);
			}
			poisonEffects->standardDistr = Distribution::GetEffectDistribution(poisonEffects->effects);
			poisonEffects->validEffects = Distribution::SumAlchemyEffects(poisonEffects->standardDistr);

			for (auto [weight, alch] : fortifyDistr) {
				Distribution::Effect eff;
				eff.effect = alch;
				eff.max = 0;
				eff.current = 0;
				eff.weight = 1000 * weight;
				fortifyEffects->effects.insert_or_assign(alch, eff);
			}
			fortifyEffects->standardDistr = Distribution::GetEffectDistribution(fortifyEffects->effects);
			fortifyEffects->validEffects = Distribution::SumAlchemyEffects(fortifyEffects->standardDistr);

			for (auto [weight, alch] : foodDistr) {
				Distribution::Effect eff;
				eff.effect = alch;
				eff.max = 0;
				eff.current = 0;
				eff.weight = 1000 * weight;
				foodEffects->effects.insert_or_assign(alch, eff);
			}
			foodEffects->standardDistr = Distribution::GetEffectDistribution(foodEffects->effects);
			foodEffects->validEffects = Distribution::SumAlchemyEffects(foodEffects->standardDistr);
		}
		Rule() {}
		/// <summary>
		/// initializes an empty rule
		/// </summary>
		/// <param name="invalid"></param>
		Rule(bool invalid)
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

	static EffectDistr GetEffectDistribution(std::unordered_map<AlchemicEffect, Effect>& effectmap)
	{
		EffectDistr distr;
		distr.total = 0;
		for (auto [eff, effect] : effectmap) {
			distr.effects.push_back({ effect.weight, effect });
			distr.total += effect.weight;
		}
		return distr;
	}

	static AlchemicEffect SumAlchemyEffects(EffectDistr& distr)
	{
		AlchemicEffect eff;
		for (auto [_, effect] : distr.effects)
		{
			eff |= effect.effect;
		}
		eff |= AlchemicEffect::kCustom;
		return eff;
	}

	// Storage for custom items
	class CustomItemStorage
	{
	public:
		/// <summary>
		/// id of the custom rule.
		/// </summary>
		std::string id;
		/// <summary>
		/// races, factions, kwds, classes, combatstyles, etc. which may get the items below
		/// </summary>
		std::unordered_set<RE::FormID> assocobjects;
		/// <summary>
		/// items associated with the objects above
		/// </summary>
		std::vector<CustomItem*> items;
		/// <summary>
		/// items that are given on death
		/// </summary>
		std::vector<CustomItem*> death;
		/// <summary>
		/// custom potions that may be given
		/// </summary>
		std::vector<CustomItemAlch*> potions;
		/// <summary>
		/// custom fortify potions that may be given
		/// </summary>
		std::vector<CustomItemAlch*> fortify;
		/// <summary>
		/// custom poisons that may be given
		/// </summary>
		std::vector<CustomItemAlch*> poisons;
		/// <summary>
		/// custom food that may be given
		/// </summary>
		std::vector<CustomItemAlch*> food;
	};

	struct StringLess
	{
		bool operator()(const Rule* lhs, const Rule* rhs) const
		{
			return lhs->ruleName < rhs->ruleName;
		}
	};

private:
	/// <summary>
	/// Wether the distribution rules are loaded
	/// </summary>
	static inline bool initialised = false;

	/// <summary>
	/// internal vector holding all distribution rules
	/// </summary>
	static inline std::set<Rule*, StringLess> _rules;
	/// <summary>
	/// internal map which maps npc -> Rules
	/// </summary>
	static inline std::unordered_map<RE::FormID, Rule*> _npcMap;
	/// <summary>
	/// internal map which maps association objects -> Rules
	/// </summary>
	static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> _assocMap;
	/// <summary>
	/// set that contains assoc objects declared as bosses
	/// </summary>
	static inline std::unordered_set<RE::FormID> _bosses;
	/// <summary>
	/// set that contains npcs excluded from distribution
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedNPCs;
	/// <summary>
	/// set that contains association objects excluded from distribution
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedAssoc;
	/// <summary>
	/// set that contains items that may not be distributed or used
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedItems;
	/// <summary>
	/// set that contains items that may not be distributed
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedDistrItems;
	/// <summary>
	/// set that contains items that may not be distributed
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedItemsBackup;
	/// <summary>
	/// set that contains item IDs that are excluded for the player only
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedItemsPlayer;
	/// <summary>
	/// set that contains association objects excluded from baseline distribution
	/// </summary>
	static inline std::unordered_set<RE::FormID> _baselineExclusions;
	/// <summary>
	/// contains items that have beeen added to the whitelists. Unused if whitelist feature is disabled
	/// </summary>
	static inline std::unordered_set<RE::FormID> _whitelistItems;
	/// <summary>
	/// contains associations and custom items that may be distributed
	/// </summary>
	static inline std::unordered_map<uint32_t, std::vector<CustomItemStorage*>> _customItems;
	/// <summary>
	/// from distribution excluded plugins
	/// </summary>
	static inline std::unordered_set<uint32_t> _excludedPlugins;
	/// <summary>
	/// map that contains rule based item strength associations
	/// </summary>
	static inline std::unordered_map<RE::FormID, ItemStrength> _itemStrengthMap;
	/// <summary>
	/// map that contains rule based actor strength associations
	/// </summary>
	static inline std::unordered_map<RE::FormID, int> _actorStrengthMap;
	/// <summary>
	/// set that contains factions, whichs members are considered followers of the player
	/// </summary>
	static inline std::unordered_set<RE::FormID> _followerFactions;
	/// <summary>
	/// map that contains the dosages set for specific items [forced, usesetting, integer]
	/// </summary>
	static inline std::unordered_map<RE::FormID, std::tuple<bool,bool,int>> _dosageItemMap;
	/// <summary>
	/// map that contains the dosages set for alchemic effects (lower priority than items) [forced, usesetting, integer]
	/// </summary>
	static inline std::unordered_map<AlchemicEffect, std::tuple<bool, bool, int>> _dosageEffectMap;
	/// <summary>
	/// map that contains from distribution excluded effects
	/// </summary>
	static inline std::unordered_set<AlchemicEffect> _excludedEffects;
	/// <summary>
	/// set of plugin indexes for plugins, which npcs are excluded
	/// </summary>
	static inline std::unordered_set<uint32_t> _excludedPlugins_NPCs;
	/// <summary>
	/// set of whitelisted npcs
	/// </summary>
	static inline std::unordered_set<RE::FormID> _whitelistNPCs;
	/// <summary>
	/// set of plugin indexes for plugins, which npcs are whitelisted
	/// </summary>
	static inline std::unordered_set<uint32_t> _whitelistNPCsPlugin;
	/// <summary>
	/// set that contains the IDs of alcoholic items
	/// </summary>
	static inline std::unordered_set<RE::FormID> _alcohol;
	/// <summary>
	/// map that defines overwrites for AlchemyEffects for MagicEffects
	/// </summary>
	static inline std::unordered_map<RE::FormID, AlchemicEffect> _magicEffectAlchMap;
	/// <summary>
	/// map of all effect categories read during rule loading
	/// </summary>
	static inline std::unordered_map<std::string, EffCategory*> _internEffectCategories;
	/// <summary>
	/// map of all category presets read during rule loading
	/// </summary>
	static inline std::unordered_map<std::string, EffCategoryPreset*> _internEffectCategoryPresets;
	/// <summary>
	/// map of all effect preset read during rule loading
	/// </summary>
	static inline std::unordered_map<std::string, EffectPreset*> _internEffectPresets;
	/// <summary>
	/// multiplcative factors for the alchemyeffect distribution probabilities for potions
	/// </summary>
	static inline std::unordered_map<AlchemicEffect, float> _probabilityAdjustersPotion;
	/// <summary>
	/// multiplcative factors for the alchemyeffect distribution probabilities for poisons
	/// </summary>
	static inline std::unordered_map<AlchemicEffect, float> _probabilityAdjustersPoison;
	/// <summary>
	/// multiplcative factors for the alchemyeffect distribution probabilities for fortify potions
	/// </summary>
	static inline std::unordered_map<AlchemicEffect, float> _probabilityAdjustersFortify;
	/// <summary>
	/// multiplcative factors for the alchemyeffect distribution probabilities for food
	/// </summary>
	static inline std::unordered_map<AlchemicEffect, float> _probabilityAdjustersFood;

	
public:

	static inline std::set<Rule*, StringLess> _dummyVecR;
	static inline std::unordered_map<RE::FormID, Rule*> _dummyMapN;
	static inline std::unordered_map<uint32_t, std::vector<CustomItemStorage*>> _dummyMapC;
	static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> _dummyMap2;
	static inline std::unordered_map<RE::FormID, ItemStrength> _dummyMap3;
	static inline std::unordered_map<RE::FormID, int> _dummyMap4;
	static inline std::unordered_map<RE::FormID, std::tuple<bool, bool, int>> _dummyMap5;
	static inline std::unordered_map<AlchemicEffect, std::tuple<bool, bool, int>> _dummyMap6;
	static inline std::unordered_map<RE::FormID, AlchemicEffect> _dummyMap7;
	static inline std::unordered_map<AlchemicEffect, float> _dummyMap8;
	static inline std::unordered_set<RE::FormID> _dummySet1;
	static inline std::unordered_set<uint8_t> _dummySet2;
	static inline std::unordered_set<uint16_t> _dummySet3;
	static inline std::unordered_set<AlchemicEffect> _dummySet4;
	static inline std::unordered_set<uint32_t> _dummySet5;

	/// <summary>
	/// Returns the vector containing all rules
	/// </summary>
	/// <returns></returns>
	static const std::set<Rule*, StringLess>* rules() { return initialised ? &_rules : &_dummyVecR; }
	/// <summary>
	/// Returns the map mapping npcs -> Rules
	/// </summary>
	/// <returns></returns>
	static const std::unordered_map<RE::FormID, Rule*>* npcMap() { return initialised ? &_npcMap : &_dummyMapN; }
	/// <summary>
	/// Returns the map mapping association objects -> Rules
	/// </summary>
	/// <returns></returns>
	static const std::unordered_map<RE::FormID, std::pair<int, Rule*>>* assocMap() { return initialised ? &_assocMap : &_dummyMap2; }
	/// <summary>
	/// Returns the set that contains assoc objects declared as bosses
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* bosses() { return initialised ? &_bosses : &_dummySet1; }
	/// <summary>
	/// returns the set containing from distribution excluded npcs
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* excludedNPCs() { return initialised ? &_excludedNPCs : &_dummySet1; }
	/// <summary>
	/// returns the set containing from distribution excluded assoc objects
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* excludedAssoc() { return initialised ? &_excludedAssoc : &_dummySet1; }
	/// <summary>
	/// returns the set of items excluded from distribution and usage
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* excludedItems() { return initialised ? &_excludedItems : &_dummySet1; }
	/// <summary>
	/// returns the set of items excluded from distribution
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* excludedDistrItems() { return initialised ? &_excludedDistrItems : &_dummySet1; }
	/// <summary>
	/// returns the set that contains item IDs that are excluded for the player only
	/// </summary>
	static const std::unordered_set<RE::FormID>* excludedItemsPlayer() { return initialised ? &_excludedItemsPlayer : &_dummySet1; }
	/// <summary>
	/// returns the set of assoc objects excluded from baseline distribution
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* baselineExclusions() { return initialised ? &_baselineExclusions : &_dummySet1; }
	/// <summary>
	/// returns the set of whitelisted items
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* whitelistItems() { return initialised ? &_whitelistItems : &_dummySet1; }
	/// <summary>
	/// returns the map of custom item associations
	/// </summary>
	/// <returns></returns>
	static const std::unordered_map<uint32_t, std::vector<CustomItemStorage*>>* customItems() { return initialised ? &_customItems : &_dummyMapC; }
	/// <summary>
	/// returns the set of from distribution excluded plugins
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<std::uint32_t>* excludedPlugins() { return initialised ? &_excludedPlugins : &_dummySet5; }
	/// <summary>
	/// returns the map that contains rule based item strength associations
	/// </summary>
	static inline std::unordered_map<RE::FormID, ItemStrength>* itemStrengthMap() { return initialised ? &_itemStrengthMap : &_dummyMap3; }
	/// <summary>
	/// returns the map that contains rule based actor strength associations
	/// </summary>
	static inline std::unordered_map<RE::FormID, int>* actorStrengthMap() { return initialised ? &_actorStrengthMap : &_dummyMap4; }
	/// <summary>
	/// returns the set that contains factions whichs members are considered followers of the player
	/// </summary>
	static inline std::unordered_set<RE::FormID>* followerFactions() { return initialised ? &_followerFactions : &_dummySet1; }
	/// <summary>
	/// return the map that contains the dosages for individual items
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_map<RE::FormID, std::tuple<bool, bool, int>>* dosageItemMap() { return initialised ? &_dosageItemMap : &_dummyMap5; }
	/// <summary>
	/// return the map that contains the dosages for alchemic effects
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_map<AlchemicEffect, std::tuple<bool, bool, int>>* dosageEffectMap() { return initialised ? &_dosageEffectMap : &_dummyMap6; }
	/// <summary>
	/// return the set of excluded alchemy effects
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_set<AlchemicEffect>* excludedEffects() { return initialised ? &_excludedEffects : &_dummySet4; }
	/// <summary>
	/// returns the set of plugin indexes for plugins which npcs are excluded
	/// </summary>
	static inline std::unordered_set<uint32_t>* excludedPlugins_NPCs() { return initialised ? &_excludedPlugins_NPCs : &_dummySet5; }
	/// <summary>
	/// returns the set of whitelisted npcs
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_set<RE::FormID>* whitelistNPCs() { return initialised ? &_whitelistNPCs : &_dummySet1; }
	/// <summary>
	/// returns the set of plugin indexes for plugins, which npcs are whitelisted
	/// </summary>
	static inline std::unordered_set<uint32_t>* whitelistNPCsPlugin() { return initialised ? &_whitelistNPCsPlugin : &_dummySet5; }
	/// <summary>
	/// returns the set that contains the IDs of alcoholic items
	/// </summary>
	static inline std::unordered_set<RE::FormID>* alcohol() { return initialised ? &_alcohol : &_dummySet1; }
	/// <summary>
	/// return the map that defines overwrites for AlchemyEffects for MagicEffects
	/// </summary>
	static inline std::unordered_map<RE::FormID, AlchemicEffect>* magicEffectAlchMap() { return initialised ? &_magicEffectAlchMap : &_dummyMap7; }

	/// <summary>
	/// returns the map that contains probability adjusters for potions
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_map<AlchemicEffect, float>* probabilityAdjustersPotion() { return initialised ? &_probabilityAdjustersPotion : &_dummyMap8; }
	/// <summary>
	/// returns the map that contains probability adjusters for poisons
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_map<AlchemicEffect, float>* probabilityAdjustersPoison() { return initialised ? &_probabilityAdjustersPoison : &_dummyMap8; }
	/// <summary>
	/// returns the map that contains probability adjusters for fortify potions
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_map<AlchemicEffect, float>* probabilityAdjustersFortify() { return initialised ? &_probabilityAdjustersFortify : &_dummyMap8; }
	/// <summary>
	/// returns the map that contains probability adjusters for food
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_map<AlchemicEffect, float>* probabilityAdjustersFood() { return initialised ? &_probabilityAdjustersFood : &_dummyMap8; }


	static std::vector<std::tuple<int, AlchemicEffect>> GetVector(int i, AlchemicEffect alch)
	{
		std::vector<std::tuple<int, AlchemicEffect>> vec;
		vec.push_back({ i, alch });
		return vec;
	}

#define DefaultRuleName "DefaultRule"

	/// <summary>
	/// Active default distribution rule
	/// </summary>
	static inline Rule* defaultRule = nullptr;
	/// <summary>
	/// Default rule, in case custom items should be distributed but no rules apply to an actor
	/// </summary>
	static inline Rule* defaultCustomRule = nullptr;
	/// <summary>
	/// Generic empty rule
	/// </summary>
	static inline Rule* emptyRule = new Rule(false);

	/// <summary>
	/// Returns items that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::TESBoundObject*> GetDistrItems(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Returns potions that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrPotions(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Returns poisons that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrPoisons(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Returns fortify potions that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrFortifyPotions(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Returns food that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrFood(std::shared_ptr<ActorInfo> const& acinfo);

	/// <summary>
	/// Returns all unique inventory items matching the distribution rule
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetMatchingInventoryItemsUnique(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Returns all inventory items (duplicates as extra vector entries) matching the distribution rule
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetMatchingInventoryItems(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Returns all inventory items (duplicates as extra vector entries)
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetAllInventoryItems(std::shared_ptr<ActorInfo> const& acinfo);
	/// <summary>
	/// Removes all items from the given list that are excluded from distribution only
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static void FilterDistributionExcludedItems(std::vector<RE::AlchemyItem*>& items);
	/// <summary>
	/// Returns the dosage of a poison
	/// </summary>
	/// <param name="poison"></param>
	/// <param name="effects"></param>
	/// <param name="force">force the dosage above 0</param>
	/// <returns></returns>
	static int GetPoisonDosage(RE::AlchemyItem* poison, AlchemicEffect effects, bool force = false);

	/// <summary>
	/// Returns whether an actor has been excluded from distribution
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static bool ExcludedNPC(RE::Actor* actor);
	/// <summary>
	/// Returns whether a NPC has been excluded from distribution
	/// </summary>
	/// <param name="npc"></param>
	/// <returns></returns>
	static bool ExcludedNPC(RE::TESNPC* npc);
	/// <summary>
	/// Returns whether a NPC has been excluded from distribution
	/// </summary>
	/// <param name="acinfo"></param>
	/// <returns></returns>
	static bool ExcludedNPC(std::shared_ptr<ActorInfo> const&acinfo);
	/// <summary>
	/// Returns whether an actor has been excluded from handling
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static bool ExcludedNPCFromHandling(RE::Actor* actor);

	/// <summary>
	/// Forcefully excludes an NPC from distribution and Handling
	/// </summary>
	/// <param name="actorid"></param>
	/// <returns></returns>
	static bool ForceExcludeNPC(uint32_t actorid);

	// friends
	// i.e. functions that may access private class members

	friend void Settings::CheckActorsForRules();
	friend void Settings::CheckCellForActors(RE::FormID cellid);
	friend bool Console::CalcRule::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&);
	friend void Settings::LoadDistrConfig();
	friend void Settings::ResetDistrConfig();
	friend void Settings::ApplySkillBoostPerks();
	friend void Settings::ClassifyItems();
	friend void Settings::UpdateSettings();
	friend void Settings::Load();
	friend void Settings::Save();
	friend void Settings::StaticExclusions(RE::TESDataHandler* datahandler);

private:
	/// <summary>
	/// Calculates the distribution rule, actor strength, item strength, and the custom items for an NPC
	/// </summary>
	/// <param name="actor">NPC to calculate for</param>
	/// <param name="acs">[overwrite] the actor strength</param>
	/// <param name="is">[overwrite] the item strength</param>
	/// <param name="tpltinfo">template information of the NPC if available</param>
	/// <param name="custItems">[overwrite] custom items of the NPC</param>
	/// <returns></returns>
	static Rule* CalcRule(RE::TESNPC* actor, ActorStrength& acs, ItemStrength& is, Misc::NPCTPLTInfo* tpltinfo = nullptr, CustomItemStorage* custItems = nullptr);

public:
	/// <summary>
	/// Calculates the rule, actor strength, item strength, and the custom items for an NPC
	/// </summary>
	/// <param name="acinfo">the ActorInfo of the actor to calculate for [infomation us updated]</param>
	/// <param name="tpltinfo">template information of the actor, if available</param>
	/// <returns></returns>
	static Rule* CalcRule(std::shared_ptr<ActorInfo> const&acinfo, Misc::NPCTPLTInfo* tpltinfo = nullptr);
	static std::vector<std::tuple<int, Distribution::Rule*, std::string>> CalcAllRules(RE::Actor* actor, ActorStrength& acs, ItemStrength& is);

private:
	/// <summary>
	/// Finds the Rule with [name]
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	static Rule* FindRule(std::string name)
	{
		for (Rule* r : _rules) {
			if (r->ruleName == name)
				return r;
		}
		return nullptr;
	}

	/// <summary>
	/// Resets all custom items
	/// </summary>
	static void ResetCustomItems();

	/// <summary>
	/// Resets all distribution rules
	/// </summary>
	static void ResetRules();
};
