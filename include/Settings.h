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
	/// <summary>
	/// Plain name of this plugin
	/// </summary>
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

	/// <summary>
	/// Max duration tracked for potions in milliseconds
	/// </summary>
	static inline int _MaxDuration = 10000;
	/// <summary>
	/// Maximum duration tracked for fortify potions in milliseconds
	/// </summary>
	static inline int _MaxFortifyDuration = 180000;

	/// <summary>
	/// Settings for the system, can be ignored by user
	/// </summary>
	struct System
	{
		/// <summary>
		/// [Settings] period for actor handling
		/// </summary>
		static inline long _cycletime = 1000;
		/// <summary>
		/// [Settings] max potions used per Cycle
		/// </summary>
		static const long _maxPotionsPerCycle = 1;
	};

	/// <summary>
	/// General settings
	/// </summary>
	struct Usage
	{
		/// <summary>
		/// [Settings] Global cooldown of using items in milliseconds
		/// </summary>
		static inline long _globalCooldown = 1000;
		/// <summary>
		/// [Settings] Whether to disable item usage while an actor is staggered (or otherwise indisponated)
		/// </summary>
		static inline bool _DisableItemUsageWhileStaggered = false;  // disables potion and poison usage while the npc is staggered
		/// <summary>
		/// [Settings] Disables all actors that are neither followers nor the player
		/// </summary>
		static inline bool _DisableNonFollowerNPCs = false;          // disable item usage for npcs that aren't followers or the player.
		/// <summary>
		/// [Settings] Disables the processing of actors out-of-combat
		/// </summary>
		static inline bool _DisableOutOfCombatProcessing = false;    // disables npc processing if they are out-of-combat
	};

	/// <summary>
	/// Settings related to potions
	/// </summary>
	struct Potions
	{
		/// <summary>
		/// [Settings] Whether the usage of magicka potions is enabled
		/// </summary>
		static inline bool _enableMagickaRestoration = true;  // enables automatic usage of magicka potions
		/// <summary>
		/// [Settings] Whether the usage of stamina potions is enabled
		/// </summary>
		static inline bool _enableStaminaRestoration = true;  // enables automatic usage of stamina potions
		/// <summary>
		/// [Settings] Whether the usage of health potions is enabled
		/// </summary>
		static inline bool _enableHealthRestoration = true;   // enables automatic usage of health potions
		/// <summary>
		/// [Settings] Whether to allow potions with detrimental effects
		/// </summary>
		static inline bool _AllowDetrimentalEffects = false;  // allows / disallows npcs to use potions with detrimental effects

		/// <summary>
		/// [Settings] Health threshold at which NPCs will begin using health potions
		/// </summary>
		static inline float _healthThreshold = 0.5f;
		/// <summary>
		/// [Settings] Magicka threshold at which NPCs will begin using magicka potions
		/// </summary>
		static inline float _magickaThreshold = 0.5f;
		/// <summary>
		/// [Settings] Stamina threshold at which NPCs will begin using stamina potions
		/// </summary>
		static inline float _staminaThreshold = 0.5f;
		/// <summary>
		/// [Settings] Chance that a NPC will use a health, magicka, or stamina potions if they can
		/// </summary>
		static inline int _UsePotionChance = 100;  // Chance that a potion will be used when appropiate
	};

	/// <summary>
	/// Settings related to poisons
	/// </summary>
	struct Poisons
	{
		/// <summary>
		/// [Settings] Whether the usage of poisons is enabled
		/// </summary>
		static inline bool _enablePoisons = true;  // enables automatic usage of poisons for npcs
		/// <summary>
		/// [Settings] Whether to allow poisons with positive effects
		/// </summary>
		static inline bool _AllowPositiveEffects = false;  // allows / disallows npcs to use poisons with positive effects

		/// <summary>
		/// [Settings] Scaling factor for when an NPC is considered powerful enough to warrant the usage of poisons
		/// </summary>
		static inline float _EnemyLevelScalePlayerLevel = 0.8f;  // how high the level of an enemy must be for followers to use poisons
		/// <summary>
		/// [Settings]  Number of NPCs that must be in combat, to disable the level scaling
		/// </summary>
		static inline int _EnemyNumberThreshold = 5;             // how many npcs must be fighting, for followers to use poisons regardless of the enemies level
		/// <summary>
		/// [Settings] Chance that an NPC will use a poison if they can
		/// </summary>
		static inline int _UsePoisonChance = 100;                // Chance that a poison will be used when possible

		/// <summary>
		/// [Internal] Static base dosage of poisons
		/// </summary>
		static const int _BaseDosage = 1;		   // base dosage. if dosage is different dosage rules are ignored
		/// <summary>
		/// [Settings] Base dosage all poisons should have
		/// </summary>
		static inline int _Dosage = _BaseDosage;  //  dosage applied to poisons
	};

	/// <summary>
	/// Settings related to fortify potions
	/// </summary>
	struct FortifyPotions
	{
		/// <summary>
		/// [Settings] Whether the usage of fortify potions is enabled
		/// </summary>
		static inline bool _enableFortifyPotions = true;  // enables automatic usage of fortify potions for npcs

		/// <summary>
		/// [Settings] Scaling factor for when an NPC is considered powerful enough to warrant the usage of fortify potions
		/// </summary>
		static inline float _EnemyLevelScalePlayerLevelFortify = 0.8f;  // how high the level of an enemy must be for followers to use fortify potions
		/// <summary>
		/// [Settings] Number of NPCs that must be in combat, to disable the level scaling
		/// </summary>
		static inline int _EnemyNumberThresholdFortify = 5;             // how many npcs must be fighting, for followers to use fortify potions regardless of the enemies level
		/// <summary>
		/// [Settings] Chance that an NPC will use a fortify potion if they can
		/// </summary>
		static inline int _UseFortifyPotionChance = 100;                // Chance that a fortify potion will be used when possible
	};

	/// <summary>
	/// Settings related to food
	/// </summary>
	struct Food
	{
		/// <summary>
		/// [Settings] Whether the usage of food items is enabled
		/// </summary>
		static inline bool _enableFood = true;				// enables automatic usage of food for npcs
		/// <summary>
		/// [Settings] Whether to allow food with detrimental effects
		/// </summary>
		static inline bool _AllowDetrimentalEffects = false;
		/// <summary>
		/// [Settings] Whether to restrict food usage to the beginning of combat
		/// </summary>
		static inline bool _RestrictFoodToCombatStart = false;  // restricts the usage of food to the beginning of the combat
	};

	/// <summary>
	/// settings related to the player character
	/// </summary>
	struct Player
	{
		/// <summary>
		/// [Settings] Whether the player will automatically consume potions
		/// </summary>
		static inline bool _playerPotions = false;  // enables automatic usage of potions for the player
		/// <summary>
		/// [Settings] Whether the player will automatically use poisons
		/// </summary>
		static inline bool _playerPoisons = false;          // enables automatic usage of poisons for player
		/// <summary>
		/// [Settings] Whether the player will automatically consume fortify potions
		/// </summary>
		static inline bool _playerFortifyPotions = false;   // enables automatic usage of fortify potions for player
		/// <summary>
		/// [Settings] Whether the player will automatically consume food
		/// </summary>
		static inline bool _playerFood = false;             // enables automatic usage of food for player
	};

	/// <summary>
	/// Settings related to distribution
	/// </summary>
	struct Distr
	{
		/// <summary>
		/// [Settings] Whether poisons should be distributed [player excluded]
		/// </summary>
		static inline bool _DistributePoisons = true;         // player is excluded from distribution options, as well as followers
		/// <summary>
		/// [Settings] Whether potions should be distributed [player excluded]
		/// </summary>
		static inline bool _DistributePotions = true;         // player is excluded from distribution options, as well as followers
		/// <summary>
		/// [Settings] Whether fortify potions should be distributed [player excluded]
		/// </summary>
		static inline bool _DistributeFortifyPotions = true;  // player is excluded from distribution options, as well as followers
		/// <summary>
		/// [Settings] Whether food should be distributed [player excluded]
		/// </summary>
		static inline bool _DistributeFood = true;            // player is excluded from distribution options, as well as followers
		/// <summary>
		/// [Settings] Whether custom items should be distribtued [player excluded]
		/// </summary>
		static inline bool _DistributeCustomItems = true;     // distributes custom items/death items to actor, does not affect custom potions etc.

		/// <summary>
		/// [Settings] Level of an NPC to be considered Weak
		/// </summary>
		static inline int _LevelEasy = 20;       // only distribute "weak" potions and poisons
		/// <summary>
		/// [Settings] Level of an NPC to be considered Normal
		/// </summary>
		static inline int _LevelNormal = 35;     // may distribute "standard" potions and poisons
		/// <summary>
		/// [Settings] Level of an NPC to be considered Difficult
		/// </summary>
		static inline int _LevelDifficult = 50;  // may distribute "potent" potions and poisons
		/// <summary>
		/// [Settings] Level of an NPC to be considered Insane
		/// </summary>
		static inline int _LevelInsane = 70;     // may have Insane tear potions

		/// <summary>
		/// [Settings] NPC difficulty scaling is bound to game difficulty
		/// </summary>
		static inline bool _GameDifficultyScaling = false;  // ties the strength of the actors not to levels, but the game difficulty

		/// <summary>
		/// [Settings] Maximum magnitude of weak potions
		/// </summary>
		static inline int _MaxMagnitudeWeak = 30;      // max potion / poison magnitude to be considered "weak"
		/// <summary>
		/// [Settings] Maximum magnitude of Standard potions
		/// </summary>
		static inline int _MaxMagnitudeStandard = 60;  // max potion / poison magnitude to be considered "standard"
		/// <summary>
		/// [Settings] Maximum magnitude of potent potions
		/// </summary>
		static inline int _MaxMagnitudePotent = 150;   // max potion / poison magnitude to be considered "potent"
													   // anything above this won't be distributed

		/// <summary>
		/// [Settings] Scaling factor for Alchemy Effects matching primary combat style of an NPC
		/// </summary>
		static inline float _StyleScalingPrimary = 1.20f;    // base scaling factor applied to potions etc. that are considered useful for the the primary combattype of an actor
		/// <summary>
		/// [Settings] Scaling factor for Alchemy Effects matching secondary combat style of an NPC
		/// </summary>
		static inline float _StyleScalingSecondary = 1.10f;  // base scaling factor applied to potions etc. that are considered useful for the secondary combattype of an actor
	};

	/// <summary>
	/// Settings related to removal options
	/// </summary>
	struct Removal
	{
		/// <summary>
		/// [Settings] Whether AlchemyItems should be removed from actors upon their death
		/// </summary>
		static inline bool _RemoveItemsOnDeath = true;  // remove unused items on death, if activated chances for removal can be set

		/// <summary>
		/// [Settings] The Chance that a single item is removed from an actor
		/// </summary>
		static inline int _ChanceToRemoveItem = 90;         // chance for an item to be removed
		/// <summary>
		/// [Settings] The maximum number of alchemy items that may remain of an actor after their death
		/// </summary>
		static inline int _MaxItemsLeft = 2;                // maximum number of items that may remain, from those to be removed
	};

	/// <summary>
	/// Settings for the whitelist mode
	/// </summary>
	struct Whitelist
	{
		/// <summary>
		/// [Settings] Whether whitelist mode is enabled
		/// </summary>
		static inline bool Enabled = false;
	};

	/// <summary>
	/// Settings related to fixes
	/// </summary>
	struct Fixes
	{
		/// <summary>
		/// [Settings] Distributes the Skill Boost perks to all valid NPCs
		/// </summary>
		static inline bool _ApplySkillBoostPerks = true;  // Distributes the two Perks AlchemySkillBoosts and PerkSkillBoosts to npcs which are needed for fortify etc. potions to apply
		/// <summary>
		/// [Settings] Forciblx fixes all potion, poison and food sounds for third person usage
		/// </summary>
		static inline bool _ForceFixPotionSounds = true;
	};

	/// <summary>
	/// General settings for compatibility
	/// </summary>
	struct Compatibility
	{
		/// <summary>
		/// [Setting] Whether the general compatibility mode has been activated
		/// </summary>
		static inline bool _CompatibilityMode = false;  // Use Items with Papyrus, needs the plugin
		/// <summary>
		/// [Settings] Disables all npcs with the ActorTypeCreature or ActorTypeAnimal keywords, if they do not have a dedicated rule. May cause npcs to be disabled, that should not be
		/// </summary>
		static inline bool _DisableCreaturesWithoutRules = true;

		struct UltimatePotionAnimation
		{
			/// <summary>
			/// [Setting] Whether the compatibility mode for zxlice's Ultimate Potion Animation has been activate for Potions
			/// </summary>
			static inline bool _CompatibilityPotionAnimation = false;					// Use Potions with Papyrus
			/// <summary>
			/// [Setting] Whether the compatibility mode for zxlice's Ultimate Potion Animation has been activate for Fortify Potions
			/// </summary>
			static inline bool _CompatibilityPotionAnimationFortify = false;                   // Use Potions with Papyrus
		};

		struct PotionAnimatedFx
		{
			/// <summary>
			/// [Automatic] Whether the compatibility mode for Animated Potion Fx has been activated
			/// </summary>
			static inline bool _CompatibilityPotionAnimatedFx = false;
		};
		struct CACO
		{
			/// <summary>
			/// [Automatic] Whether the compatibility mode for CACO has been activated
			/// </summary>
			static inline bool _CompatibilityCACO = false;
		};

		struct Apothecary
		{
			/// <summary>
			/// [Automatic] Whether the compatibility mode for Apothecary has been activated
			/// </summary>
			static inline bool _CompatibilityApothecary = false;
		};

		struct AnimatedPoisons
		{
			/// <summary>
			/// [Automatic] Whether the compatibility mode for Animated Poisons has been activated
			/// </summary>
			static inline bool _CompatibilityAnimatedPoisons = false;
			/// <summary>
			/// [Setting] Whether the usage of Animated Poisons is enabled
			/// </summary>
			static inline bool _Enable = true;
			/// <summary>
			/// [Setting] enables the poison dosage mechanic of Animated Poisons if the mod is found
			/// </summary>
			static inline bool _UsePoisonDosage = false;
		};

		struct AnimatedPotions
		{
			/// <summary>
			/// [Automatic] whether the compatibility mode for Animated Potions has been activated
			/// </summary>
			static inline bool _CompatibilityAnimatedPotions = false; 
			/// <summary>
			/// [Setting] Whether the usage of Animated Potions is enabled
			/// </summary>
			static inline bool _Enable = true;
		};
	};

	/// <summary>
	/// Settings for debug features
	/// </summary>
	struct Debug
	{
		/// <summary>
		/// [Setting] Removes all Excluded items from actors before the distribution of new items
		/// </summary>
		static inline bool _CompatibilityRemoveItemsBeforeDist = false;
		/// <summary>
		/// [Setting] Removes all excluded items from actors at startup
		/// </summary>
		static inline bool _CompatibilityRemoveItemsStartup = false;
		/// <summary>
		/// [Setting] Removes all excluded items from all actors shortly after loading a game
		/// </summary>
		static inline bool _CompatibilityRemoveItemsStartup_OnlyExcluded = false;  

		/// <summary>
		/// [Setting] Enables general logging
		/// </summary>
		static inline bool EnableLog = false;      
		/// <summary>
		/// [Setting] Enables logging for plugin load
		/// </summary>
		static inline bool EnableLoadLog = false;  

		/// <summary>
		/// [Setting] log level
		/// 0 - only highest level
		/// 1 - highest to layer 1 function logging
		/// 2 - highest to layer 2 function logging
		/// 3 - highest to layer 3 function logging
		/// </summary>
		static inline int LogLevel = 0;        
		/// <summary>
		/// [Setting] profiling level
		/// 0 - highest level only
		/// 1 - highest and layer 1
		/// 2 - highest and layer 2
		/// </summary>
		static inline int ProfileLevel = 0;       
		/// <summary>
		/// [Setting] Enables profiling
		/// </summary>
		static inline bool EnableProfiling = false;
		/// <summary>
		/// [Setting] Calculates the Rules of all actors in a cell, on cell change
		/// </summary>
		static inline bool _CalculateCellRules = false;

		/// <summary>
		/// [Setting] checks for actors which do not have any rules, and prints their information to the, logfile
		/// </summary>
		static inline bool _CheckActorsWithoutRules = false;
		/// <summary>
		/// [Setting] tests the rules assigned to actors for all cells in game
		/// </summary>
		static inline bool _Test = false;
	};

	class Internal
	{
	public:
		/// <summary>
		/// whether the cure disease effect is activated
		/// </summary>
		static inline bool _useCureDiseaseEffect = true;
	};

	// intern
private:
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _dummylist{};
	/// <summary>
	/// list of Weak-tiered potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsWeak_main{};
	/// <summary>
	/// list of Weak-tiered fortify potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsWeak_rest{};
	/// <summary>
	/// list of Standard-tiered potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsStandard_main{};
	/// <summary>
	/// list of Standard-tiered fortify potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsStandard_rest{};
	/// <summary>
	/// list of Potent-tiered potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsPotent_main{};
	/// <summary>
	/// list of Potent-tiered fortify potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsPotent_rest{};
	/// <summary>
	/// list of Insane-tiered potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsInsane_main{};
	/// <summary>
	/// list of Insane-tiered fortify potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsInsane_rest{};
	/// <summary>
	/// list of blood potions
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsBlood{};
	/// <summary>
	/// list of Weak-tiered poisons
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak{};
	/// <summary>
	/// list of Standard-tiered poisons
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard{};
	/// <summary>
	/// list of Potent-tiered poisons
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent{};
	/// <summary>
	/// list of Insane-tiered poisons
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsInsane{};
	/// <summary>
	/// list of all food
	/// </summary>
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodall{};

	static inline bool _itemsInit = false;

	// tuples of effect, magnitude and duration
	/// <summary>
	/// Maps items to effect, magnitude and duration
	/// </summary>
	static inline std::unordered_map<RE::FormID, std::tuple<uint64_t, float, int>> _itemEffectMap;

public:
	/// <summary>
	/// Returns the list of Weak-tiered potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsWeak_main() { return _itemsInit ? &_potionsWeak_main : &_dummylist; }
	/// <summary>
	/// Returns the list of Weak-tiered fortify potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsWeak_rest() { return _itemsInit ? &_potionsWeak_rest : &_dummylist; }
	/// <summary>
	/// Returns the list of Standard-tiered potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsStandard_main() { return _itemsInit ? &_potionsStandard_main : &_dummylist; }
	/// <summary>
	/// Returns the list of Standard-tiered fortify potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsStandard_rest() { return _itemsInit ? &_potionsStandard_rest : &_dummylist; }
	/// <summary>
	/// Returns the list of Potent-tiered potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsPotent_main() { return _itemsInit ? &_potionsPotent_main : &_dummylist; }
	/// <summary>
	/// returns the list of Potent-tiered fortify potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsPotent_rest() { return _itemsInit ? &_potionsPotent_rest : &_dummylist; }
	/// <summary>
	/// Returns the list of Insane-tiered potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsInsane_main() { return _itemsInit ? &_potionsInsane_main : &_dummylist; }
	/// <summary>
	/// Returns the list of Insane-tiered fortify potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsInsane_rest() { return _itemsInit ? &_potionsInsane_rest : &_dummylist; }
	/// <summary>
	/// Returns the list of blood potions
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* potionsBlood() { return _itemsInit ? &_potionsBlood : &_dummylist; }
	/// <summary>
	/// Returns the list of Weak-tiered poison
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsWeak() { return _itemsInit ? &_poisonsWeak : &_dummylist; }
	/// <summary>
	/// Returns the list of Standard-tiered poisons
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsStandard() { return _itemsInit ? &_poisonsStandard : &_dummylist; }
	/// <summary>
	/// Returns the list of Potent-tiered poisons
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsPotent() { return _itemsInit ? &_poisonsPotent : &_dummylist; }
	/// <summary>
	/// Returns the list of Insane-tiered poisons
	/// </summary>
	/// <returns></returns>
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsInsane() { return _itemsInit ? &_poisonsInsane : &_dummylist; }
	/// <summary>
	/// Returns the list of all food items
	/// </summary>
	/// <returns></returns>
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

	/// <summary>
	/// Game perk for Alchemy Skill Boosts
	/// </summary>
	static inline RE::BGSPerk* AlchemySkillBoosts;
	/// <summary>
	/// Game perk for Skill Boosts
	/// </summary>
	static inline RE::BGSPerk* PerkSkillBoosts;

	/// <summary>
	/// Game equip slot for the left hand
	/// </summary>
	static inline RE::BGSEquipSlot* Equip_LeftHand;
	/// <summary>
	/// Game equip slot for the right hand
	/// </summary>
	static inline RE::BGSEquipSlot* Equip_RightHand;
	/// <summary>
	/// Game equip slot for either hand
	/// </summary>
	static inline RE::BGSEquipSlot* Equip_EitherHand;
	/// <summary>
	/// Game equip slot for both hands
	/// </summary>
	static inline RE::BGSEquipSlot* Equip_BothHands;
	/// <summary>
	/// Game equip slots for shields
	/// </summary>
	static inline RE::BGSEquipSlot* Equip_Shield;
	/// <summary>
	/// Game equip slot for voice
	/// </summary>
	static inline RE::BGSEquipSlot* Equip_Voice;
	/// <summary>
	/// Game equip slots for potions
	/// </summary>
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
		return Settings::Compatibility::_CompatibilityMode;
	}
	/// <summary>
	/// returns whether potions whould be used in compatibility mode
	/// </summary>
	/// <returns></returns>
	static bool CompatibilityPotionPapyrus()
	{
		return Settings::Compatibility::_CompatibilityMode;
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
