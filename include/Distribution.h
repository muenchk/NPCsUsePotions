#pragma once

#include <AlchemyEffect.h>

	/// <summary>
/// Class handling all functions related to item distribution
/// </summary>
class Distribution
{
public:
	/// <summary>
	/// Supported Association types for distribution rules
	/// </summary>
	enum class AssocType
	{
		kKeyword = 1,
		kFaction = 2,
		kRace = 4,
		kActor = 8,
		kNPC = 16,
		kItem = 32,
		kClass = 64,
		kCombatStyle = 128,
	};

	class CustomItemStorage;

	/// <summary>
	/// A distribution rule
	/// </summary>
	class Rule
	{
	public:
		// if set to false this rule is effectively empty
		bool valid = true;
		///
		int ruleVersion;
		int ruleType;
		std::string ruleName;

		/// TYPE 1 - Rule
		// general
		int rulePriority;
		std::string assocObjects;
		std::string potionProperties;
		std::string fortifyproperties;
		std::string poisonProperties;
		std::string foodProperties;
		bool allowMixed;
		// chances and options
		int maxPotions;
		int maxPoisons;
		std::vector<int> potion1Chance;
		std::vector<int> potion2Chance;
		std::vector<int> potion3Chance;
		std::vector<int> potionAdditionalChance;
		std::vector<int> fortify1Chance;
		std::vector<int> fortify2Chance;
		int potionTierAdjust;
		std::vector<int> poison1Chance;
		std::vector<int> poison2Chance;
		std::vector<int> poison3Chance;
		std::vector<int> poisonAdditionalChance;
		int poisonTierAdjust;
		std::vector<int> foodChance;

		/// TYPE 2 - Exclusion
		//std::string			assocRuleName;
		//std::string			assocExclusions;

		std::vector<std::tuple<int, AlchemyEffect>> potionDistr;
		std::vector<std::tuple<int, AlchemyEffect>> potionDistrChance;
		std::vector<std::tuple<int, AlchemyEffect>> poisonDistr;
		std::vector<std::tuple<int, AlchemyEffect>> poisonDistrChance;
		std::vector<std::tuple<int, AlchemyEffect>> fortifyDistr;
		std::vector<std::tuple<int, AlchemyEffect>> fortifyDistrChance;
		std::vector<std::tuple<int, AlchemyEffect>> foodDistr;
		std::vector<std::tuple<int, AlchemyEffect>> foodDistrChance;

		uint64_t validPotions;
		uint64_t validPoisons;
		uint64_t validFortifyPotions;
		uint64_t validFood;

		/// <summary>
		/// returns a random potion according to [strength] and [acsstrength]
		/// </summary>
		/// <param name="
		/// ">information about the actor the item is for</param>
		/// <returns>A randomly chosen potion according to the rule</returns>
		std::vector<RE::AlchemyItem*> GetRandomPotions(ActorInfo* acinfo);
		/// <summary>
		/// returns a random popoisontion according to [strength] and [acsstrength]
		/// </summary>v
		/// <returns>A randomly chosen poison according to the rule</returns>
		std::vector<RE::AlchemyItem*> GetRandomPoisons(ActorInfo* acinfo);
		/// <summary>
		/// returns a random fortify potion according to [strength] and [acsstrength]
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns>A randomly chosen fortify potion according to the rule</returns>
		std::vector<RE::AlchemyItem*> GetRandomFortifyPotions(ActorInfo* acinfo);
		/// <summary>
		/// returns a random food according to [strength] and [acsstrength]
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns>A randomly chosen food according to the rule</returns>
		std::vector<RE::AlchemyItem*> GetRandomFood(ActorInfo* acinfo);

#define COPY(vec1, vec2)       \
	vec2.reserve(vec1.size()); \
	std::copy(vec1.begin(), vec1.end(), vec2.begin());

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
			rl->maxPotions = maxPotions;
			rl->maxPoisons = maxPoisons;
			rl->potionTierAdjust = potionTierAdjust;
			rl->poisonTierAdjust = poisonTierAdjust;
			COPY(potion1Chance, rl->potion1Chance);
			COPY(potion2Chance, rl->potion2Chance);
			COPY(potion3Chance, rl->potion3Chance);
			COPY(potionAdditionalChance, rl->potionAdditionalChance);
			COPY(fortify1Chance, rl->fortify1Chance);
			COPY(fortify2Chance, rl->fortify2Chance);
			COPY(poison1Chance, rl->poison1Chance);
			COPY(poison2Chance, rl->poison2Chance);
			COPY(poison3Chance, rl->poison3Chance);
			COPY(poisonAdditionalChance, rl->poisonAdditionalChance);
			COPY(foodChance, rl->foodChance);
			return rl;
		}

		Rule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, int _maxPotions,
			std::vector<int> _potion1Chance, std::vector<int> _potion2Chance, std::vector<int> _potion3Chance,
			std::vector<int> _potionAdditionalChance, std::vector<int> _fortify1Chance, std::vector<int> _fortify2Chance,
			int _potionTierAdjust, int _maxPoisons, std::vector<int> _poison1Chance,
			std::vector<int> _poison2Chance, std::vector<int> _poison3Chance, std::vector<int> _poisonAdditionalChance,
			int _poisonTierAdjust, std::vector<int> _foodChance, std::vector<std::tuple<int, AlchemyEffect>> _potionDistr,
			std::vector<std::tuple<int, AlchemyEffect>> _poisonDistr,
			std::vector<std::tuple<int, AlchemyEffect>> _fortifyDistr,
			std::vector<std::tuple<int, AlchemyEffect>> _foodDistr, uint64_t _validPotions, uint64_t _validPoisons,
			uint64_t _validFortifyPotions, uint64_t _validFood) :
			ruleVersion{ _ruleVersion },
			ruleType{ _ruleType },
			ruleName{ _ruleName },
			rulePriority{ _rulePriority },
			allowMixed{ _allowMixed },
			maxPotions{ _maxPotions },
			maxPoisons{ _maxPoisons },
			potion1Chance{ _potion1Chance },
			potion2Chance{ _potion2Chance },
			potion3Chance{ _potion3Chance },
			potionAdditionalChance{ _potionAdditionalChance },
			poison1Chance{ _poison1Chance },
			poison2Chance{ _poison2Chance },
			poison3Chance{ _poison3Chance },
			poisonAdditionalChance{ _poisonAdditionalChance },
			fortify1Chance{ _fortify1Chance },
			fortify2Chance{ _fortify2Chance },
			potionTierAdjust{ _potionTierAdjust },
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
		{}
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
		RE::AlchemyItem* GetRandomPotion1(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the second random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion2(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the third random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion3(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns an additional random potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotionAdditional(ActorInfo* acinfo);
		/// <summary>
		/// Returns an according to potion properties randomly chosen potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPotion(int str, ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the first random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison1(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the second random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison2(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the third random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison3(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns addtional random poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoisonAdditional(ActorInfo* acinfo);
		/// <summary>
		/// Returns an according to poison properties randomly chosen poison
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomPoison(int str, ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the first random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion1(ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns the second random fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion2(ActorInfo* acinfo);
		/// <summary>
		/// Returns an according to fortify properties randomly chosen fortify potion
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFortifyPotion(int str, ActorInfo* acinfo);
		/// <summary>
		/// Calculates and returns a random food item
		/// </summary>
		/// <param name="acinfo">information about the actor the item is for</param>
		/// <returns></returns>
		RE::AlchemyItem* GetRandomFood_intern(ActorInfo* acinfo);

		/// <summary>
		/// Returns a random effect accoring to the rules item effect properties
		/// </summary>
		/// <param name="type">Determines which items effect property is consulted</param>
		/// <returns></returns>
		AlchemyEffect GetRandomEffect(Settings::ItemType type);
		/// <summary>
		/// Returns a random effect accoring to the rules item effect properties with additional custom item chance
		/// </summary>
		/// <param name="type">Determines which items effect property is consulted</param>
		/// <returns></returns>
		AlchemyEffect GetRandomEffectChance(Settings::ItemType type);
	};

	/// <summary>
	/// Stores information about a template npc
	/// </summary>
	class NPCTPLTInfo
	{
	public:
		/// <summary>
		/// factions associated with the template npc
		/// </summary>
		std::vector<RE::TESFaction*> tpltfactions;
		/// <summary>
		/// keywords associated with the template npc
		/// </summary>
		std::vector<RE::BGSKeyword*> tpltkeywords;
		/// <summary>
		/// race of the template npc
		/// </summary>
		RE::TESRace* tpltrace = nullptr;
		/// <summary>
		/// combat style of the template npc
		/// </summary>
		RE::TESCombatStyle* tpltstyle = nullptr;
		/// <summary>
		/// class of the template npc
		/// </summary>
		RE::TESClass* tpltclass = nullptr;
	};

	class CustomItemStorage
	{
	public:
		/// <summary>
		/// races, factions, kwds, classes, combatstyles, etc. which may get the items below
		/// </summary>
		std::unordered_set<RE::FormID> assocobjects;
		/// <summary>
		/// items associated with the objects above
		/// </summary>
		std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> items;
		std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> death;
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> potions;
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> fortify;
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> poisons;
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> food;
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

public:
	static inline std::vector<Rule*> _dummyVecR;
	static inline std::unordered_map<RE::FormID, Rule*> _dummyMapN;
	static inline std::unordered_map<uint32_t, std::vector<CustomItemStorage*>> _dummyMapC;
	static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> _dummyMap2;
	static inline std::unordered_set<RE::FormID> _dummySet1;

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

#define RandomRange 1000

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
	/// Generic empty rule
	/// </summary>
	static inline Rule* emptyRule = new Rule(false);

	/// <summary>
	/// Returns items that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::TESBoundObject*> GetDistrItems(ActorInfo* acinfo);
	/// <summary>
	/// Returns potions that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrPotions(ActorInfo* acinfo);
	/// <summary>
	/// Returns poisons that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrPoisons(ActorInfo* acinfo);
	/// <summary>
	/// Returns fortify potions that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrFortifyPotions(ActorInfo* acinfo);
	/// <summary>
	/// Returns food that shall be distributed to [actor]
	/// </summary>
	/// <param name="actor">actor to calculate items for</param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetDistrFood(ActorInfo* acinfo);

	/// <summary>
	/// Returns all unique inventory items matching the distribution rule
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetMatchingInventoryItemsUnique(ActorInfo* acinfo);
	/// <summary>
	/// Returns all inventory items (duplicates as extra vector entries) matching the distribution rule
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetMatchingInventoryItems(ActorInfo* acinfo);

	static bool ExcludedNPC(RE::Actor* actor);
	static bool ExcludedNPC(RE::TESNPC* npc);

	friend void Settings::CheckActorsForRules();
	friend void Settings::CheckCellForActors(RE::FormID cellid);
	friend bool Console::CalcRule::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&);
	friend void Settings::LoadDistrConfig();
	friend void ActorInfo::CalcCustomItems();

private:
	//static Rule* CalcRule(RE::Actor* actor);
	//static Rule* CalcRule(RE::Actor* actor, CustomItemStorage* custItems);
	//static Rule* CalcRule(RE::TESNPC* npc);
	//static Rule* CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is);
	//static Rule* CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is, CustomItemStorage* custItems);
	//static Rule* CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is, NPCTPLTInfo* tpltinfo, CustomItemStorage* custItems = nullptr);
	static Rule* CalcRule(RE::TESNPC* actor, ActorStrength& acs, ItemStrength& is, NPCTPLTInfo* tpltinfo = nullptr, CustomItemStorage* custItems = nullptr);
	static Rule* CalcRule(ActorInfo* acinfo, NPCTPLTInfo* tpltinfo = nullptr);
	static std::vector<std::tuple<int, Distribution::Rule*, std::string>> CalcAllRules(RE::Actor* actor, ActorStrength& acs, ItemStrength& is);

	static Rule* FindRule(std::string name)
	{
		for (Rule* r : _rules) {
			if (r->ruleName == name)
				return r;
		}
		return nullptr;
	}

	static NPCTPLTInfo ExtractTemplateInfo(RE::TESNPC* npc);
	static NPCTPLTInfo ExtractTemplateInfo(RE::TESLevCharacter* lvl);
};
