#pragma once

#include "AlchemyEffect.h"
#include "CustomItem.h"
#include "Misc.h"
#include "Data.h"
#include "Settings.h"

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
		std::string assocObjects;
		std::string potionProperties;
		std::string fortifyproperties;
		std::string poisonProperties;
		std::string foodProperties;
		bool allowMixed = true;
		bool styleScaling = true;
		// chances and options
		int maxPotions = 5;
		int maxFortify = 5;
		int maxPoisons = 5;
		std::vector<int> potion1Chance;
		std::vector<int> potion2Chance;
		std::vector<int> potion3Chance;
		std::vector<int> potion4Chance;
		std::vector<int> potionAdditionalChance;
		int potionTierAdjust = 0;
		std::vector<int> fortify1Chance;
		std::vector<int> fortify2Chance;
		std::vector<int> fortify3Chance;
		std::vector<int> fortify4Chance;
		std::vector<int> fortifyAdditionalChance;
		int fortifyTierAdjust = 0;
		std::vector<int> poison1Chance;
		std::vector<int> poison2Chance;
		std::vector<int> poison3Chance;
		std::vector<int> poison4Chance;
		std::vector<int> poisonAdditionalChance;
		int poisonTierAdjust = 0;
		std::vector<int> foodChance;

		// distributions for different item types
		std::vector<std::tuple<int, AlchemyEffect>> potionDistr;
		std::vector<std::tuple<int, AlchemyEffect>> potionDistrChance;
		std::vector<std::tuple<int, AlchemyEffect>> poisonDistr;
		std::vector<std::tuple<int, AlchemyEffect>> poisonDistrChance;
		std::vector<std::tuple<int, AlchemyEffect>> fortifyDistr;
		std::vector<std::tuple<int, AlchemyEffect>> fortifyDistrChance;
		std::vector<std::tuple<int, AlchemyEffect>> foodDistr;
		std::vector<std::tuple<int, AlchemyEffect>> foodDistrChance;

		// raw effect maps for the item types
		std::map<AlchemyEffect, float> potionEffectMap;
		std::map<AlchemyEffect, float> poisonEffectMap;
		std::map<AlchemyEffect, float> fortifyEffectMap;
		std::map<AlchemyEffect, float> foodEffectMap;

		/// <summary>
		/// accumulated alchemy effects valid for potions
		/// </summary>
		uint64_t validPotions = 0;
		/// <summary>
		/// accumulated alchemy effect valid for poisons
		/// </summary>
		uint64_t validPoisons = 0;
		/// <summary>
		/// accumulated alchemy effects valid for fortify potions
		/// </summary>
		uint64_t validFortifyPotions = 0;
		/// <summary>
		/// accumulated alchemy effects valid for food
		/// </summary>
		uint64_t validFood = 0;

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
		std::vector<std::tuple<int, AlchemyEffect>> GetScaledDistribution(Settings::ItemType type, std::shared_ptr<ActorInfo> const& acinfo);

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
			rl->assocObjects = assocObjects;
			rl->potionProperties = potionProperties;
			rl->fortifyproperties = fortifyproperties;
			rl->poisonProperties = poisonProperties;
			rl->foodProperties = foodProperties;
			rl->allowMixed = allowMixed;
			rl->styleScaling = styleScaling;
			rl->maxPotions = maxPotions;
			rl->maxFortify = maxFortify;
			rl->maxPoisons = maxPoisons;
			rl->potionTierAdjust = potionTierAdjust;
			rl->fortifyTierAdjust = fortifyTierAdjust;
			rl->poisonTierAdjust = poisonTierAdjust;
			COPY(potion1Chance, rl->potion1Chance);
			COPY(potion2Chance, rl->potion2Chance);
			COPY(potion3Chance, rl->potion3Chance);
			COPY(potion4Chance, rl->potion4Chance);
			COPY(potionAdditionalChance, rl->potionAdditionalChance);
			COPY(fortify1Chance, rl->fortify1Chance);
			COPY(fortify2Chance, rl->fortify2Chance);
			COPY(fortify3Chance, rl->fortify3Chance);
			COPY(fortify4Chance, rl->fortify4Chance);
			COPY(fortifyAdditionalChance, rl->fortifyAdditionalChance);
			COPY(poison1Chance, rl->poison1Chance);
			COPY(poison2Chance, rl->poison2Chance);
			COPY(poison3Chance, rl->poison3Chance);
			COPY(poison4Chance, rl->poison4Chance);
			COPY(poisonAdditionalChance, rl->poisonAdditionalChance);
			COPY(foodChance, rl->foodChance);
			COPY(potionDistr, rl->potionDistr);
			COPY(potionDistrChance, rl->potionDistrChance);
			COPY(poisonDistr, rl->poisonDistr);
			COPY(poisonDistrChance, rl->poisonDistrChance);
			COPY(fortifyDistr, rl->fortifyDistr);
			COPY(fortifyDistrChance, rl->fortifyDistrChance);
			COPY(foodDistr, rl->foodDistr);
			COPY(foodDistrChance, rl->foodDistrChance);
			rl->validPotions = validPotions;
			rl->validPoisons = validPoisons;
			rl->validFortifyPotions = validFortifyPotions;
			rl->validFood = validFood;
			COPYMAP(potionEffectMap, rl->potionEffectMap);
			COPYMAP(poisonEffectMap, rl->poisonEffectMap);
			COPYMAP(fortifyEffectMap, rl->fortifyEffectMap);
			COPYMAP(foodEffectMap, rl->foodEffectMap);
			return rl;
		}

		Rule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, bool _styleScaling, int _maxPotions,
			std::vector<int> _potion1Chance, std::vector<int> _potion2Chance, std::vector<int> _potion3Chance, std::vector<int> _potion4Chance,
			std::vector<int> _potionAdditionalChance, int _potionTierAdjust,
			int _maxFortify, std::vector<int> _fortify1Chance, std::vector<int> _fortify2Chance, std::vector<int> _fortify3Chance, std::vector<int> _fortify4Chance, std::vector<int> _fortifyAddtionalChance, int _fortifyTierAdjust,
			int _maxPoisons, std::vector<int> _poison1Chance, std::vector<int> _poison2Chance, std::vector<int> _poison3Chance, std::vector<int> _poison4Chance, std::vector<int> _poisonAdditionalChance, int _poisonTierAdjust, 
			std::vector<int> _foodChance, 
			std::vector<std::tuple<int, AlchemyEffect>> _potionDistr,
			std::vector<std::tuple<int, AlchemyEffect>> _poisonDistr,
			std::vector<std::tuple<int, AlchemyEffect>> _fortifyDistr,
			std::vector<std::tuple<int, AlchemyEffect>> _foodDistr, 
			uint64_t _validPotions, uint64_t _validPoisons, uint64_t _validFortifyPotions, uint64_t _validFood) :
			ruleVersion{ _ruleVersion },
			ruleType{ _ruleType },
			ruleName{ _ruleName },
			rulePriority{ _rulePriority },
			allowMixed{ _allowMixed },
			styleScaling{ _styleScaling },
			maxPotions{ _maxPotions },
			maxFortify{ _maxFortify },
			maxPoisons{ _maxPoisons },
			potion1Chance{ _potion1Chance },
			potion2Chance{ _potion2Chance },
			potion3Chance{ _potion3Chance },
			potion4Chance{ _potion4Chance },
			potionAdditionalChance{ _potionAdditionalChance },
			poison1Chance{ _poison1Chance },
			poison2Chance{ _poison2Chance },
			poison3Chance{ _poison3Chance },
			poison4Chance{ _poison4Chance },
			poisonAdditionalChance{ _poisonAdditionalChance },
			fortify1Chance{ _fortify1Chance },
			fortify2Chance{ _fortify2Chance },
			fortify3Chance{ _fortify3Chance },
			fortify4Chance{ _fortify4Chance },
			fortifyAdditionalChance{ _fortifyAddtionalChance },
			potionTierAdjust{ _potionTierAdjust },
			fortifyTierAdjust{ _fortifyTierAdjust },
			poisonTierAdjust{ _poisonTierAdjust },
			foodChance{ _foodChance },
			potionDistr{ _potionDistr },
			poisonDistr{ _poisonDistr },
			fortifyDistr{ _fortifyDistr },
			foodDistr{ _foodDistr },
			validPotions{ _validPotions },
			validPoisons{ _validPoisons },
			validFortifyPotions{ _validFortifyPotions },
			validFood{ _validFood }
		{
			potionDistrChance = std::vector<std::tuple<int, AlchemyEffect>>(_potionDistr);
			potionDistrChance.push_back({ RandomRange, AlchemyEffect::kCustom });
			poisonDistrChance = std::vector<std::tuple<int, AlchemyEffect>>(_poisonDistr);
			poisonDistrChance.push_back({ RandomRange, AlchemyEffect::kCustom });
			fortifyDistrChance = std::vector<std::tuple<int, AlchemyEffect>>(_fortifyDistr);
			fortifyDistrChance.push_back({ RandomRange, AlchemyEffect::kCustom });
			foodDistrChance = std::vector<std::tuple<int, AlchemyEffect>>(_foodDistr);
			foodDistrChance.push_back({ RandomRange, AlchemyEffect::kCustom });
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
		/// Calculates and returns the first random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion1(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the second random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion2(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the third random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion3(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the fourth random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion4(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns an additional random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotionAdditional(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Returns an according to potion properties randomly chosen potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion(int str, std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the first random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison1(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the second random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison2(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the third random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison3(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the fourth random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison4(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns addtional random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoisonAdditional(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Returns an according to poison properties randomly chosen poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison(int str, std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the first random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion1(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the second random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion2(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the third random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion3(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns the fourth random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion4(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns additional random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotionAdditional(std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Returns an according to fortify properties randomly chosen fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion(int str, std::shared_ptr<ActorInfo> const& acinfo);
		/// <summary>
		/// Calculates and returns a random food item
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFood_intern(std::shared_ptr<ActorInfo> const& acinfo);

		/// <summary>
		/// Returns a random effect accoring to the rules item effect properties
		/// </summary>
		/// <param name="type">Determines which items effect property is consulted</param>
		/// <returns></returns>
		AlchemyEffect GetRandomEffect(Settings::ItemType type);
		/// <summary>
		/// Returns a random effect according to the given distribution
		/// </summary>
		/// <param name="distribution">distribution to pull effect from</param>
		/// <returns></returns>
		AlchemyEffect GetRandomEffect(std::vector<std::tuple<int, AlchemyEffect>> distribution);
		/// <summary>
		/// Returns a random effect accoring to the rules item effect properties with additional custom item chance
		/// </summary>
		/// <param name="type">Determines which items effect property is consulted</param>
		/// <returns></returns>
		AlchemyEffect GetRandomEffectChance(Settings::ItemType type);

	public:
		/// <summary>
		/// Removes an AlchemyEffect from the list of valid potion effects
		/// </summary>
		/// <param name="effect"></param>
		void RemoveAlchemyEffectPotion(AlchemyEffect effect);
		/// <summary>
		/// Removes an AlchemyEffect from the list of valid poison effects
		/// </summary>
		/// <param name="effect"></param>
		void RemoveAlchemyEffectPoison(AlchemyEffect effect);
		/// <summary>
		/// Removes an AlchemyEffect from the list of valid fortify potion effects
		/// </summary>
		/// <param name="effect"></param>
		void RemoveAlchemyEffectFortifyPotion(AlchemyEffect effect);
		/// <summary>
		/// Removes an AlchemyEffect from the list of valid food effects
		/// </summary>
		/// <param name="effect"></param>
		void RemoveAlchemyEffectFood(AlchemyEffect effect);
	};

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

private:
	/// <summary>
	/// Wether the distribution rules are loaded
	/// </summary>
	static inline bool initialised = false;

	/// <summary>
	/// internal vector holding all distribution rules
	/// </summary>
	static inline std::vector<Rule*> _rules;
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
	/// set that contains items that may not be distributed
	/// </summary>
	static inline std::unordered_set<RE::FormID> _excludedItems;
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
	static inline std::unordered_map<AlchemyEffect, std::tuple<bool, bool, int>> _dosageEffectMap;
	/// <summary>
	/// map that contains from distribution excluded effects
	/// </summary>
	static inline std::unordered_set<AlchemyEffect> _excludedEffects;
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
	static inline std::unordered_map<RE::FormID, AlchemyEffect> _magicEffectAlchMap;

	
public:
	static inline std::vector<Rule*> _dummyVecR;
	static inline std::unordered_map<RE::FormID, Rule*> _dummyMapN;
	static inline std::unordered_map<uint32_t, std::vector<CustomItemStorage*>> _dummyMapC;
	static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> _dummyMap2;
	static inline std::unordered_map<RE::FormID, ItemStrength> _dummyMap3;
	static inline std::unordered_map<RE::FormID, int> _dummyMap4;
	static inline std::unordered_map<RE::FormID, std::tuple<bool, bool, int>> _dummyMap5;
	static inline std::unordered_map<AlchemyEffect, std::tuple<bool, bool, int>> _dummyMap6;
	static inline std::unordered_map<RE::FormID, AlchemyEffect> _dummyMap7;
	static inline std::unordered_set<RE::FormID> _dummySet1;
	static inline std::unordered_set<uint8_t> _dummySet2;
	static inline std::unordered_set<uint16_t> _dummySet3;
	static inline std::unordered_set<AlchemyEffect> _dummySet4;
	static inline std::unordered_set<uint32_t> _dummySet5;

	/// <summary>
	/// Returns the vector containing all rules
	/// </summary>
	/// <returns></returns>
	static const std::vector<Rule*>* rules() { return initialised ? &_rules : &_dummyVecR; }
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
	/// returns the set of items excluded from distribution
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* excludedItems() { return initialised ? &_excludedItems : &_dummySet1; }
	/// <summary>
	/// returns the set that contains item IDs that are excluded for the player only
	/// </summary>
	static inline std::unordered_set<RE::FormID>* excludedItemsPlayer() { return initialised ? &_excludedItemsPlayer : &_dummySet1; }
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
	static inline std::unordered_map<AlchemyEffect, std::tuple<bool, bool, int>>* dosageEffectMap() { return initialised ? &_dosageEffectMap : &_dummyMap6; }
	/// <summary>
	/// return the set of excluded alchemy effects
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_set<AlchemyEffect>* excludedEffects() { return initialised ? &_excludedEffects : &_dummySet4; }
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
	static inline std::unordered_map<RE::FormID, AlchemyEffect>* magicEffectAlchMap() { return initialised ? &_magicEffectAlchMap : &_dummyMap7; }



	static std::vector<std::tuple<int, AlchemyEffect>> GetVector(int i, AlchemyEffect alch)
	{
		std::vector<std::tuple<int, AlchemyEffect>> vec;
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
	/// Returns the dosage of a poison
	/// </summary>
	/// <param name="poison"></param>
	/// <param name="effects"></param>
	/// <returns></returns>
	static int GetPoisonDosage(RE::AlchemyItem* poison, AlchemyEffectBase effects);

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
	friend void Settings::ApplySkillBoostPerks();
	friend void Settings::ClassifyItems();

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
