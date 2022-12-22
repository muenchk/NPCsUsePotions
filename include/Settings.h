#pragma once

#include "SimpleIni.h"
#include <fstream>
#include <iostream>
#include <type_traits>
#include <utility>
#include <string_view>
#include <chrono>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <time.h>
#include <random>
#include <tuple>
#include <vector>

#include <string.h>
#include<thread>
#include <forward_list>
#include <semaphore>
#include <limits>
#include "ActorInfo.h"
#include <Console.h>
#include <Logging.h>
#include <AlchemyEffect.h>

class Settings
{
public:
	/// <summary>
	/// Name of this plugin
	/// </summary>
	static inline std::string PluginName = "NPCsUsePotions.esp";
	static inline std::string PluginNamePlain = "NPCsUsePotions";

	/// <summary>
	/// Supported types of Items
	/// </summary>
	enum class ItemType
	{
		kPoison = 1,
		kPotion = 2,
		kFood = 4,
		kFortifyPotion = 8,
	};

	/// <summary>
	/// Calculated the distribution rules for all actors in the game
	/// </summary>
	static void CheckActorsForRules();

	/// <summary>
	/// Loads the distribution configuration
	/// </summary>
	static void LoadDistrConfig();

	/// <summary>
	/// Calculates the distribution rules of all actors in the cell with [cellid]
	/// </summary>
	/// <param name="cellid">id of the cell the check</param>
	static void CheckCellForActors(RE::FormID cellid);

	/// <summary>
	/// Applies the SkillBoost perks to all npcs in the game
	/// </summary>
	static void ApplySkillBoostPerks();

	/// <summary>
	/// changes the output model of all consumable sounds to third person
	/// </summary>
	static void FixConsumables();

	static inline int _MaxDuration = 10000;
	static inline int _MaxFortifyDuration = 180000;

	//general
	static inline long _maxPotionsPerCycle = 2;
	static inline long _cycletime = 500;
	static inline bool _DisableEquipSounds = false;

	// features
	static inline bool _featMagickaRestoration = true;			// enables automatic usage of magicka potions
	static inline bool _featStaminaRestoration = true;			// enables automatic usage of stamina potions
	static inline bool _featHealthRestoration = true;			// enables automatic usage of health potions
	static inline bool _playerRestorationEnabled = false;		// enables automatic usage of potions for the player
	static inline bool _featUsePoisons = true;             // enables automatic usage of poisons for npcs
	static inline bool _featUseFortifyPotions = true;		// enables automatic usage of fortify potions for npcs
	static inline bool _featUseFood = true;				// enables automatic usage of food for npcs
	static inline bool _playerUsePoisons = false;          // enables automatic usage of poisons for player
	static inline bool _playerUseFortifyPotions = false;   // enables automatic usage of fortify potions for player
	static inline bool _playerUseFood = false;				// enables automatic usage of food for player
	static inline bool _featDistributePoisons = true;			// player is excluded from distribution options, as well as followers
	static inline bool _featDistributePotions = true;			// player is excluded from distribution options, as well as followers
	static inline bool _featDistributeFortifyPotions = true;	// player is excluded from distribution options, as well as followers
	static inline bool _featDistributeFood = true;				// player is excluded from distribution options, as well as followers
	static inline bool _featDistributeCustomItems = true;		// distributes custom items/death items to actor, does not affect custom potions etc.
	static inline bool _featUseDeathItems = true;				// the npc will be given potions that may appear in their deathItems if available
	static inline bool _featRemoveItemsOnDeath = true;		// remove unused items on death, if activated chances for removal can be set
	static inline bool _featDisableItemUsageWhileStaggered = false;		// disables potion and poison usage while the npc is staggered
	static inline bool _featDisableNonFollowerNPCs = false;		// disable item usage for npcs that aren't followers or the player.
	static inline bool _featDisableOutOfCombatProcessing = false; // disables npc processing if they are out-of-combat

	// compatibility
	static inline bool _CompatibilityMode = false;								// Use Items with Papyrus, needs the plugin
	static inline bool _CompatibilityPotionAnimation = false;					// Use Potions with Papyrus
	static inline bool _CompatibilityPotionAnimationFortify = false;                   // Use Potions with Papyrus
	static inline bool _CompatibilityDisableAutomaticAdjustments = false;		// Disables most automatic adjustments made to settings for compatibility
	static inline bool _CompatibilityPotionAnimatedFx = false;					// no settings entry | Compatiblity Mode for Mods
																				// 1) Animated Potion Drinking SE
																				// 2) Potion Animated fix (SE)
	static inline bool _CompatibilityRemoveItemsBeforeDist = false;				// removes items from actors before the distribution of new items
	static inline bool _CompatibilityRemoveItemsStartup = false;				// removes items from actors at startup
	static inline bool _CompatibilityRemoveItemsStartup_OnlyExcluded = false;	// removes only excluded items shortly after loading a game
	static inline bool _ApplySkillBoostPerks = true;							// Distributes the two Perks AlchemySkillBoosts and PerkSkillBoosts to npcs which are needed for fortify etc. potions to apply
	static inline bool _ForceFixPotionSounds = true;
	static inline bool _CompatibilityCACO = false;	// automatic
	static inline bool _CompatibilityApothecary = false; // automatic
	static inline bool _CompatibilityAnimatedPoisons = false; // automatic
	static inline bool _CompatibilityAnimatedPotions = false; // automatic

	static inline bool _CompatibilityWhitelist = false;

	static inline bool _DisableCreaturesWithoutRules = true;					// disables all npcs with the ActorTypeCreature or ActorTypeAnimal keywords, if they do not have a dedicated rule. May cause npcs to be disabled, that should not be

	// debug
	static inline bool EnableLog = false;			// Enables general logging
	static inline bool EnableLoadLog = false;		// Enables logging for plugin load
	static inline int LogLevel = 0;					// 0 - only highest level
													// 1 - highest to layer 1 function logging
													// 2 - highest to layer 2 function logging
													// 3 - highest to layer 3 function logging
	static inline int ProfileLevel = 0;				// 0 - highest level only
													// 1 - highest and layer 1
													// 2 - highest and layer 2
	static inline bool EnableProfiling = false;
	static inline bool _CalculateCellRules = false;

	// distribution
	static inline int _LevelEasy = 20;				// only distribute "weak" potions and poisons
	static inline int _LevelNormal = 35;			// may distribute "standard" potions and poisons
	static inline int _LevelDifficult = 50;			// may distribute "potent" potions and poisons
	static inline int _LevelInsane = 70;			// may have Insane tear potions

	static inline bool _GameDifficultyScaling = false; // ties the strength of the actors not to levels, but the game difficulty

	static inline int _MaxMagnitudeWeak = 30;		// max potion / poison magnitude to be considered "weak"
	static inline int _MaxMagnitudeStandard = 60;	// max potion / poison magnitude to be considered "standard"
	static inline int _MaxMagnitudePotent = 150;	// max potion / poison magnitude to be considered "potent"
													// anything above this won't be distributed
	static inline float _StyleScalingPrimary = 1.20f;	// base scaling factor applied to potions etc. that are considered useful for the the primary combattype of an actor
	static inline float _StyleScalingSecondary = 1.10f;  // base scaling factor applied to potions etc. that are considered useful for the secondary combattype of an actor

	// potion usage
	static inline float _healthThreshold = 0.5f;
	static inline float _magickaThreshold = 0.5f;
	static inline float _staminaThreshold = 0.5f;
	static inline int _UsePotionChance = 100;		// Chance that a potion will be used when appropiate

	// poison usage
	static inline float _EnemyLevelScalePlayerLevel = 0.8f;		// how high the level of an enemy must be for followers to use poisons
	static inline int _EnemyNumberThreshold = 5;				// how many npcs must be fighting, for followers to use poisons regardless of the enemies level
	static inline int _UsePoisonChance = 100;					// Chance that a poison will be used when possible

	// fortify potions
	static inline float _EnemyLevelScalePlayerLevelFortify = 0.8f;  // how high the level of an enemy must be for followers to use fortify potions
	static inline int _EnemyNumberThresholdFortify = 5;             // how many npcs must be fighting, for followers to use fortify potions regardless of the enemies level
	static inline int _UseFortifyPotionChance = 100;				// Chance that a fortify potion will be used when possible

	// food usage
	static inline bool _RestrictFoodToCombatStart = false;			// restricts the usage of food to the beginning of the combat

	// removal
	static inline int _ChanceToRemoveItem = 90;		// chance for an item to be removed
	static inline int _MaxItemsLeft = 2;			// maximum number of items that may remain, from those to be removed

	// intern
	static inline bool _CheckActorsWithoutRules = false;	// checks for actors which do not have any rules, and prints their information to the, logfile
	static inline bool _Test = false;

	class Internal
	{
	public:
		static inline bool _useCureDiseaseEffect = true;
	};

	// intern
private:
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _dummylist{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsWeak_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsWeak_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsStandard_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsStandard_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsPotent_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsPotent_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsInsane_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsInsane_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsBlood{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsInsane{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodmagicka{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodstamina{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodhealth{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodall{};

	static inline bool _itemsInit = false;

	// tuples of effect, magnitude and duration
	static inline std::unordered_map<RE::FormID, std::tuple<uint64_t, float, int>> _itemEffectMap;

public:
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsWeak_main() { return _itemsInit ? &_potionsWeak_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsWeak_rest() { return _itemsInit ? &_potionsWeak_rest : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsStandard_main() { return _itemsInit ? &_potionsStandard_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsStandard_rest() { return _itemsInit ? &_potionsStandard_rest : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsPotent_main() { return _itemsInit ? &_potionsPotent_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsPotent_rest() { return _itemsInit ? &_potionsPotent_rest : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsInsane_main() { return _itemsInit ? &_potionsInsane_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsInsane_rest() { return _itemsInit ? &_potionsInsane_rest : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsBlood() { return _itemsInit ? &_potionsBlood : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsWeak() { return _itemsInit ? &_poisonsWeak : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsStandard() { return _itemsInit ? &_poisonsStandard : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsPotent() { return _itemsInit ? &_poisonsPotent : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsInsane() { return _itemsInit ? &_poisonsInsane : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* foodmagicka() { return _itemsInit ? &_foodmagicka : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* foodstamina() { return _itemsInit ? &_foodstamina : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* foodhealth() { return _itemsInit ? &_foodhealth : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* foodall() { return _itemsInit ? &_foodall : &_dummylist; }

	//static inline std::list<RE::AlchemyItem*> alitems{};
	//static inline std::list<RE::AlchemyItem*> potions{};
	//static inline std::list<RE::AlchemyItem*> food{};
	//static inline std::list<RE::AlchemyItem*> poisons{};

	static inline RE::BGSKeyword* VendorItemPotion;
	static inline RE::BGSKeyword* VendorItemFood;
	static inline RE::BGSKeyword* VendorItemFoodRaw;
	static inline RE::BGSKeyword* VendorItemPoison;
	static inline RE::BGSKeyword* ActorTypeDwarven;
	static inline RE::BGSKeyword* ActorTypeCreature;
	static inline RE::BGSKeyword* ActorTypeAnimal;

	static inline RE::TESFaction* CurrentFollowerFaction;
	static inline RE::TESFaction* CurrentHirelingFaction;

	static inline RE::BGSPerk* AlchemySkillBoosts;
	static inline RE::BGSPerk* PerkSkillBoosts;

	static inline RE::BGSEquipSlot* Equip_LeftHand;
	static inline RE::BGSEquipSlot* Equip_RightHand;
	static inline RE::BGSEquipSlot* Equip_EitherHand;
	static inline RE::BGSEquipSlot* Equip_BothHands;
	static inline RE::BGSEquipSlot* Equip_Shield;
	static inline RE::BGSEquipSlot* Equip_Voice;
	static inline RE::BGSEquipSlot* Equip_Potion;

	[[deprecated]] static inline RE::BGSSoundDescriptorForm* PotionUse;
	static inline RE::BGSSoundDescriptorForm* PoisonUse;
	[[deprecated]] static inline RE::BGSSoundDescriptorForm* FoodEat;
	[[deprecated]] static inline bool FixedPotionUse = true;
	[[deprecated]] static inline bool FixedPoisonUse = true;
	[[deprecated]] static inline bool FixedFoodEat = true;

	static inline std::string pluginnames[256+4096];

	/// <summary>
	/// Loads game objects
	/// </summary>
	static void InitGameStuff();

	/// <summary>
	/// Loads the plugin configuration
	/// </summary>
	static void Load();
	
	/// <summary>
	/// Saves the plugin configuration
	/// </summary>
	static void Save();

	/// <summary>
	/// returns whether food items should be used in compatibility mode
	/// </summary>
	/// <returns></returns>
	static bool CompatibilityFoodPapyrus()
	{
		return Settings::_CompatibilityMode;
	}
	/// <summary>
	/// returns whether potions whould be used in compatibility mode
	/// </summary>
	/// <returns></returns>
	static bool CompatibilityPotionPapyrus()
	{
		return Settings::_CompatibilityMode;
	}

	/// <summary>
	/// classifies a single item based on its effects
	/// </summary>
	/// <param name="item"></param>
	/// <returns>effects, strength, type, duration, magnitude, whether an effect is detrimental</returns>
	static std::tuple<uint64_t, ItemStrength, ItemType, int, float, bool> ClassifyItem(RE::AlchemyItem* item);

	/// <summary>
	/// classifies all AlchemyItems in the game according to its effects
	/// </summary>
	static void ClassifyItems();

	/// <summary>
	/// returns all item from [list] with the alchemy effect [effect]
	/// </summary>
	/// <param name="list"></param>
	/// <param name="effect"></param>
	/// <returns></returns>
	static std::vector<RE::AlchemyItem*> GetMatchingItems(std::list<std::pair<uint64_t, RE::AlchemyItem*>>& list, uint64_t effect)
	{
		std::vector<RE::AlchemyItem*> ret;
		for (auto entry : list) {
			if ((std::get<0>(entry) & effect) > 0)
				ret.push_back(std::get<1>(entry));
		}
		return ret;
	}
};
