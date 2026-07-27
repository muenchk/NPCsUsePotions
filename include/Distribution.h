#pragma once

#include "CustomItem.h"
#include "Data.h"
#include "DistributionRule.h"

#include <set>

/// <summary>
/// Class handling all functions related to item distribution
/// </summary>
class Distribution
{
public:

	class CustomItemStorage;

#define RandomRange 1000

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
		bool operator()(const DistributionRule* lhs, const DistributionRule* rhs) const
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
	static inline std::set<DistributionRule*, StringLess> _rules;
	/// <summary>
	/// internal map which maps npc -> Rules
	/// </summary>
	static inline std::unordered_map<RE::FormID, DistributionRule*> _npcMap;
	/// <summary>
	/// internal map which maps association objects -> Rules
	/// </summary>
	static inline std::unordered_map<RE::FormID, std::pair<int, DistributionRule*>> _assocMap;
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
	/// set that contains any form that is hard excluded from all handling by this mod
	/// </summary>
	static inline std::unordered_set<RE::FormID> _hardExclusions;
	/// <summary>
	/// set that contains indexes for plugins whichs NPCs are hard excluded from all handling by this mod
	/// </summary>
	static inline std::unordered_set<uint32_t> _hardExclusions_Plugins_NPCs;
	/// <summary>
	/// set of whitelisted npcs
	/// </summary>
	static inline std::unordered_set<RE::FormID> _whitelistNPCs;
	/// <summary>
	/// set of plugin indexes for plugins, which npcs are whitelisted
	/// </summary>
	static inline std::unordered_set<uint32_t> _whitelistNPCsPlugin;
	/// <summary>
	/// set of races that are known, but that do not have dedicated rules attached to them by design
	/// </summary>
	static inline std::unordered_set<uint32_t> _knownRaces;
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

	static inline std::set<DistributionRule*, StringLess> _dummyVecR;
	static inline std::unordered_map<RE::FormID, DistributionRule*> _dummyMapN;
	static inline std::unordered_map<uint32_t, std::vector<CustomItemStorage*>> _dummyMapC;
	static inline std::unordered_map<RE::FormID, std::pair<int, DistributionRule*>> _dummyMap2;
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
	static const std::set<DistributionRule*, StringLess>* rules() { return initialised ? &_rules : &_dummyVecR; }
	/// <summary>
	/// Returns the map mapping npcs -> Rules
	/// </summary>
	/// <returns></returns>
	static const std::unordered_map<RE::FormID, DistributionRule*>* npcMap() { return initialised ? &_npcMap : &_dummyMapN; }
	/// <summary>
	/// Returns the map mapping association objects -> DistributionRules
	/// </summary>
	/// <returns></returns>
	static const std::unordered_map<RE::FormID, std::pair<int, DistributionRule*>>* assocMap() { return initialised ? &_assocMap : &_dummyMap2; }
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
	/// returns the set contains anything that is hard excluded from any handling
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* hardExclusions() { return initialised ? &_hardExclusions : &_dummySet1; }
	/// <summary>
	/// returns the set contains anything that is hard excluded from any handling
	/// </summary>
	/// <returns></returns>
	static const std::unordered_set<RE::FormID>* hardExclusionsPlugins_NPCs() { return initialised ? &_hardExclusions_Plugins_NPCs : &_dummySet5; }
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
	/// returns the set of known races that do not have explicit rules attached to them
	/// </summary>
	/// <returns></returns>
	static inline std::unordered_set<RE::FormID>* knownRaces() { return initialised ? &_knownRaces : &_dummySet1; }
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
	static inline DistributionRule* defaultRule = nullptr;
	/// <summary>
	/// Default rule, in case custom items should be distributed but no rules apply to an actor
	/// </summary>
	static inline DistributionRule* defaultCustomRule = nullptr;
	/// <summary>
	/// Generic empty rule
	/// </summary>
	static inline DistributionRule* emptyRule = new DistributionRule(false);

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

	friend void Settings::CheckForPluginsWithoutRules();
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
	friend class DistributionRule;

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
	static DistributionRule* CalcRule(RE::TESNPC* actor, ActorStrength& acs, ItemStrength& is, UtilityBase::NPCTPLTInfo* tpltinfo = nullptr, CustomItemStorage* custItems = nullptr);

public:
	/// <summary>
	/// Calculates the rule, actor strength, item strength, and the custom items for an NPC
	/// </summary>
	/// <param name="acinfo">the ActorInfo of the actor to calculate for [infomation us updated]</param>
	/// <param name="tpltinfo">template information of the actor, if available</param>
	/// <returns></returns>
	static DistributionRule* CalcRule(std::shared_ptr<ActorInfo> const& acinfo, UtilityBase::NPCTPLTInfo* tpltinfo = nullptr);
	static std::vector<std::tuple<int, DistributionRule*, std::string>> CalcAllRules(RE::Actor* actor, ActorStrength& acs, ItemStrength& is);

private:
	/// <summary>
	/// Finds the Rule with [name]
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	static DistributionRule* FindRule(std::string name)
	{
		for (auto& r : _rules) {
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
