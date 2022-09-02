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
	/// Contains values used for compatibility
	/// </summary>
	class Compatibility
	{
	public:
		static inline std::string Plugin_PotionAnimatedfx = "PotionAnimatedfx.esp";
		static inline RE::EffectSetting* PAF_NPCDrinkingCoolDownEffect = nullptr;
		static inline RE::SpellItem* PAF_NPCDrinkingCoolDownSpell = nullptr;
		static inline std::string PAF_NPCDrinkingCoolDownEffect_name = "PAF_NPCDrinkingCoolDownEffect";
		static inline std::string PAF_NPCDrinkingCoolDownSpell_name = "PAF_NPCDrinkingCoolDownSpell";
	};
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
	static inline bool _featDistributePoisons = true;			// player is excluded from distribution options, as well as followers
	static inline bool _featDistributePotions = true;			// player is excluded from distribution options, as well as followers
	static inline bool _featDistributeFortifyPotions = true;	// player is excluded from distribution options, as well as followers
	static inline bool _featDistributeFood = true;				// player is excluded from distribution options, as well as followers
	static inline bool _featUseDeathItems = true;				// the npc will be given potions that may appear in their deathItems if available
	static inline bool _featRemoveItemsOnDeath = true;		// remove unused items on death, if activated chances for removal can be set
	static inline bool _featDisableItemUsageWhileStaggered = false;		// disables potion and poison usage while the npc is staggered

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
	static inline bool _CompatibilityCACO = false;	// automatic
	static inline bool _CompatibilityApothecary = false; // automatic

	static inline bool _CompatibilityWhitelist = false;

	// debug
	static inline bool EnableLog = false;
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
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsInsane{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodmagicka{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodstamina{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodhealth{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodall{};

	static inline bool _itemsInit = false;

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
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsWeak_main() { return _itemsInit ? &_poisonsWeak_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsWeak_rest() { return _itemsInit ? &_poisonsWeak_rest : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsStandard_main() { return _itemsInit ? &_poisonsStandard_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsStandard_rest() { return _itemsInit ? &_poisonsStandard_rest : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsPotent_main() { return _itemsInit ? &_poisonsPotent_main : &_dummylist; }
	static std::list<std::pair<uint64_t, RE::AlchemyItem*>>* poisonsPotent_rest() { return _itemsInit ? &_poisonsPotent_rest : &_dummylist; }
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

	static inline RE::TESFaction* CurrentFollowerFaction;
	static inline RE::TESFaction* CurrentHirelingFaction;

	static inline RE::BGSPerk* AlchemySkillBoosts;
	static inline RE::BGSPerk* PerkSkillBoosts;

	static inline RE::BGSSoundDescriptorForm* PotionUse;
	static inline RE::BGSSoundDescriptorForm* PoisonUse;
	static inline RE::BGSSoundDescriptorForm* FoodEat;
	static inline bool FixedPotionUse = true;
	static inline bool FixedPoisonUse = true;
	static inline bool FixedFoodEat = true;

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
	static bool CompatibilityPotionPlugin(RE::Actor* actor)
	{
		if (Settings::_CompatibilityPotionAnimatedFx) {
			// compatibility mode for PotionAnimatedfx.esp activated, we may only use a potion if it is not on cooldown
			// if the actor does not have the cooldown effect we may use a potion
			if (!(actor->HasMagicEffect(Compatibility::PAF_NPCDrinkingCoolDownEffect))) {
				return true;
			} else
				return false;
		}
		return true;
	}

	static AlchemyEffect ConvertToAlchemyEffect(RE::EffectSetting* effect)
	{
		return ConvertToAlchemyEffectPrimary(effect);
	}

	static AlchemyEffect ConvertToAlchemyEffectPrimary(RE::EffectSetting* effect)
	{
		if (effect) {
			AlchemyEffect eff = ConvertToAlchemyEffect(effect->data.primaryAV);
			auto eff2 = ConvertToAlchemyEffectIDs(effect);
			if (eff2 != AlchemyEffect::kNone)
				return eff2;
			else
				return eff;
		}
		return AlchemyEffect::kNone;
	}

	static AlchemyEffect ConvertToAlchemyEffectSecondary(RE::EffectSetting* effect)
	{
		if (effect) {
			AlchemyEffect eff = ConvertToAlchemyEffect(effect->data.secondaryAV);
			//auto eff2 = ConvertToAlchemyEffectIDs(effect);
			//if (eff2 != AlchemyEffect::kNone)
			//	return eff2;
			//else
				return eff;
		}
		return AlchemyEffect::kNone;
	}

	static AlchemyEffect ConvertToAlchemyEffectIDs(RE::EffectSetting* effect)
	{
		if (effect)
		{
			AlchemyEffect eff = AlchemyEffect::kNone;
			RE::FormID id = effect->GetFormID();
			if (id == 0x73F30)  // Paralysis
				eff = AlchemyEffect::kParalysis;
			if (id == 0xAE722)  // CureDisease
				eff = AlchemyEffect::kCureDisease;
			if (id == 0x109ADD)	// CurePoison
				eff = AlchemyEffect::kCurePoison;
			if (id == 0x3AC2D)  // Waterbreathing
				eff = AlchemyEffect::kWaterbreathing;
			if (id == 0xD6947)  // Persuasion
				eff = AlchemyEffect::kPersuasion;
			// COMPATIBILITY FOR CACO
			if (Settings::_CompatibilityCACO) {
				// DamageStaminaRavage
				if (id == 0x73F23)
					eff = AlchemyEffect::kStamina;
				// DamageMagickaRavage
				if (id == 0x73F27)
					eff = AlchemyEffect::kMagicka;
			}
			// COMPATIBILITY FOR APOTHECARY
			if (Settings::_CompatibilityApothecary) {
				// DamageWeapon
				if (id == 0x73F26)
					eff = AlchemyEffect::kAttackDamageMult;
				// Silence
				if (id == 0x73F2B)
					eff = AlchemyEffect::kMagickaRate;
			}
			return eff;
		}
		return AlchemyEffect::kNone;
	}

	static AlchemyEffect ConvertToAlchemyEffect(RE::ActorValue val)
	{
		switch (val) {
		case RE::ActorValue::kHealth:
			return (AlchemyEffect::kHealth);
			break;
		case RE::ActorValue::kMagicka:
			return (AlchemyEffect::kMagicka);
			break;
		case RE::ActorValue::kStamina:
			return (AlchemyEffect::kStamina);
			break;
		case RE::ActorValue::kOneHanded:
		case RE::ActorValue::kOneHandedModifier:
		case RE::ActorValue::kOneHandedPowerModifier:
			return (AlchemyEffect::kOneHanded);
			break;
		case RE::ActorValue::kTwoHanded:
		case RE::ActorValue::kTwoHandedModifier:
		case RE::ActorValue::kTwoHandedPowerModifier:
			return (AlchemyEffect::kTwoHanded);
			break;
		case RE::ActorValue::kArchery:
		case RE::ActorValue::kMarksmanModifier:
		case RE::ActorValue::kMarksmanPowerModifier:
			return (AlchemyEffect::kArchery);
			break;
		case RE::ActorValue::kBlock:
		case RE::ActorValue::kBlockModifier:
		case RE::ActorValue::kBlockPowerModifier:
			return (AlchemyEffect::kBlock);
			break;
		case RE::ActorValue::kSmithing:
		case RE::ActorValue::kSmithingModifier:
		case RE::ActorValue::kSmithingPowerModifier:
			return (AlchemyEffect::kSmithing); 
			break;
		case RE::ActorValue::kHeavyArmor:
		case RE::ActorValue::kHeavyArmorModifier:
		case RE::ActorValue::kHeavyArmorPowerModifier:
			return (AlchemyEffect::kHeavyArmor);
			break;
		case RE::ActorValue::kLightArmor:
		case RE::ActorValue::kLightArmorModifier:
		case RE::ActorValue::kLightArmorSkillAdvance:
			return (AlchemyEffect::kLightArmor);
			break;
		case RE::ActorValue::kPickpocket:
		case RE::ActorValue::kPickpocketModifier:
		case RE::ActorValue::kPickpocketPowerModifier:
			return (AlchemyEffect::kPickpocket);
			break;
		case RE::ActorValue::kLockpicking:
		case RE::ActorValue::kLockpickingModifier:
		case RE::ActorValue::kLockpickingPowerModifier:
			return (AlchemyEffect::kLockpicking);
			break;
		case RE::ActorValue::kSneak:
		case RE::ActorValue::kSneakingModifier:
		case RE::ActorValue::kSneakingPowerModifier:
			return (AlchemyEffect::kSneak);
			break;
		case RE::ActorValue::kAlchemy:
		case RE::ActorValue::kAlchemyModifier:
		case RE::ActorValue::kAlchemyPowerModifier:
			return (AlchemyEffect::kAlchemy);
			break;
		case RE::ActorValue::kSpeech:
		case RE::ActorValue::kSpeechcraftModifier:
		case RE::ActorValue::kSpeechcraftPowerModifier:
			return (AlchemyEffect::kSpeech);
			break;
		case RE::ActorValue::kAlteration:
		case RE::ActorValue::kAlterationModifier:
		case RE::ActorValue::kAlterationPowerModifier:
			return (AlchemyEffect::kAlteration);
			break;
		case RE::ActorValue::kConjuration:
		case RE::ActorValue::kConjurationModifier:
		case RE::ActorValue::kConjurationPowerModifier:
			return (AlchemyEffect::kConjuration);
			break;
		case RE::ActorValue::kDestruction:
		case RE::ActorValue::kDestructionModifier:
		case RE::ActorValue::kDestructionPowerModifier:
			return (AlchemyEffect::kDestruction);
			break;
		case RE::ActorValue::kIllusion:
		case RE::ActorValue::kIllusionModifier:
		case RE::ActorValue::kIllusionPowerModifier:
			return (AlchemyEffect::kIllusion);
			break;
		case RE::ActorValue::kRestoration:
		case RE::ActorValue::kRestorationModifier:
		case RE::ActorValue::kRestorationPowerModifier:
			return (AlchemyEffect::kRestoration);
			break;
		case RE::ActorValue::kEnchanting:
		case RE::ActorValue::kEnchantingModifier:
		case RE::ActorValue::kEnchantingPowerModifier:
			return (AlchemyEffect::kEnchanting);
			break;
		case RE::ActorValue::kHealRate:
			return (AlchemyEffect::kHealRate);
			break;
		case RE::ActorValue::kMagickaRate:
			return (AlchemyEffect::kMagickaRate);
			break;
		case RE::ActorValue::KStaminaRate:
			return (AlchemyEffect::kStaminaRate);
			break;
		case RE::ActorValue::kSpeedMult:
			return (AlchemyEffect::kSpeedMult);
			break;
		//case RE::ActorValue::kInventoryWeight:
		//	break;
		case RE::ActorValue::kCarryWeight:
			return (AlchemyEffect::kCarryWeight);
			break;
		case RE::ActorValue::kCriticalChance:
			return (AlchemyEffect::kCriticalChance);
			break;
		case RE::ActorValue::kMeleeDamage:
			return (AlchemyEffect::kMeleeDamage);
			break;
		case RE::ActorValue::kUnarmedDamage:
			return (AlchemyEffect::kUnarmedDamage);
			break;
		case RE::ActorValue::kDamageResist:
			return (AlchemyEffect::kDamageResist);
			break;
		case RE::ActorValue::kPoisonResist:
			return (AlchemyEffect::kPoisonResist);
			break;
		case RE::ActorValue::kResistFire:
			return (AlchemyEffect::kResistFire);
			break;
		case RE::ActorValue::kResistShock:
			return (AlchemyEffect::kResistShock);
			break;
		case RE::ActorValue::kResistFrost:
			return (AlchemyEffect::kResistFrost);
			break;
		case RE::ActorValue::kResistMagic:
			return (AlchemyEffect::kResistMagic);
			break;
		case RE::ActorValue::kResistDisease:
			return (AlchemyEffect::kResistDisease);
			break;
		case RE::ActorValue::kParalysis:
			return (AlchemyEffect::kParalysis);
			break;
		case RE::ActorValue::kInvisibility:
			return (AlchemyEffect::kInvisibility);
			break;
		case RE::ActorValue::kWeaponSpeedMult:
		case RE::ActorValue::kLeftWeaponSpeedMultiply:
			return (AlchemyEffect::kWeaponSpeedMult);
			break;
		case RE::ActorValue::kBowSpeedBonus:
			return (AlchemyEffect::kBowSpeed);
			break;
		case RE::ActorValue::kAttackDamageMult:
			return (AlchemyEffect::kAttackDamageMult);
			break;
		case RE::ActorValue::kHealRateMult:
			return (AlchemyEffect::kHealRateMult);
			break;
		case RE::ActorValue::kMagickaRateMult:
			return (AlchemyEffect::kMagickaRateMult);
			break;
		case RE::ActorValue::kStaminaRateMult:
			return (AlchemyEffect::kStaminaRateMult);
			break;
		case RE::ActorValue::kAggresion:
			return (AlchemyEffect::kFrenzy);
			break;
		case RE::ActorValue::kConfidence:
			return (AlchemyEffect::kFear);
			break;
		case RE::ActorValue::kReflectDamage:
			return (AlchemyEffect::kReflectDamage);
			break;
		default:
			return AlchemyEffect::kNone;
			break;
		}
	}

	/// <summary>
	/// converts an AlchemyEffect into RE::ActorValue
	/// </summary>
	/// <param name="eff"></param>
	/// <returns></returns>
	static RE::ActorValue ConvertAlchemyEffect(AlchemyEffect eff)
	{
		switch (eff) {
		case AlchemyEffect::kNone:
			return RE::ActorValue::kNone;
			break;
		case AlchemyEffect::kHealth:
			return RE::ActorValue::kHealth;
			break;
		case AlchemyEffect::kMagicka:
			return RE::ActorValue::kMagicka;
			break;
		case AlchemyEffect::kStamina:
			return RE::ActorValue::kStamina;
			break;
		case AlchemyEffect::kOneHanded:
			return RE::ActorValue::kOneHanded;
			break;
		case AlchemyEffect::kTwoHanded:
			return RE::ActorValue::kTwoHanded;
			break;
		case AlchemyEffect::kArchery:
			return RE::ActorValue::kArchery;
			break;
		case AlchemyEffect::kHeavyArmor:
			return RE::ActorValue::kHeavyArmor;
			break;
		case AlchemyEffect::kLightArmor:
			return RE::ActorValue::kLightArmor;
			break;
		case AlchemyEffect::kAlteration:
			return RE::ActorValue::kAlteration;
			break;
		case AlchemyEffect::kConjuration:
			return RE::ActorValue::kConjuration;
			break;
		case AlchemyEffect::kDestruction:
			return RE::ActorValue::kDestruction;
			break;
		case AlchemyEffect::kIllusion:
			return RE::ActorValue::kIllusion;
			break;
		case AlchemyEffect::kRestoration:
			return RE::ActorValue::kRestoration;
			break;
		case AlchemyEffect::kHealRate:
			return RE::ActorValue::kHealRate;
			break;
		case AlchemyEffect::kMagickaRate:
			return RE::ActorValue::kMagickaRate;
			break;
		case AlchemyEffect::kStaminaRate:
			return RE::ActorValue::KStaminaRate;
			break;
		case AlchemyEffect::kSpeedMult:
			return RE::ActorValue::kSpeedMult;
			break;
		case AlchemyEffect::kCriticalChance:
			return RE::ActorValue::kCriticalChance;
			break;
		case AlchemyEffect::kMeleeDamage:
			return RE::ActorValue::kMeleeDamage;
			break;
		case AlchemyEffect::kUnarmedDamage:
			return RE::ActorValue::kUnarmedDamage;
			break;
		case AlchemyEffect::kDamageResist:
			return RE::ActorValue::kDamageResist;
			break;
		case AlchemyEffect::kPoisonResist:
			return RE::ActorValue::kPoisonResist;
			break;
		case AlchemyEffect::kResistFire:
			return RE::ActorValue::kResistFire;
			break;
		case AlchemyEffect::kResistShock:
			return RE::ActorValue::kResistShock;
			break;
		case AlchemyEffect::kResistFrost:
			return RE::ActorValue::kResistFrost;
			break;
		case AlchemyEffect::kResistMagic:
			return RE::ActorValue::kResistMagic;
			break;
		case AlchemyEffect::kResistDisease:
			return RE::ActorValue::kResistDisease;
			break;
		case AlchemyEffect::kParalysis:
			return RE::ActorValue::kParalysis;
			break;
		case AlchemyEffect::kInvisibility:
			return RE::ActorValue::kInvisibility;
			break;
		case AlchemyEffect::kWeaponSpeedMult:
			return RE::ActorValue::kWeaponSpeedMult;
			break;
		case AlchemyEffect::kAttackDamageMult:
			return RE::ActorValue::kAttackDamageMult;
			break;
		case AlchemyEffect::kBowSpeed:
			return RE::ActorValue::kBowSpeedBonus;
			break;
		case AlchemyEffect::kHealRateMult:
			return RE::ActorValue::kHealRateMult;
			break;
		case AlchemyEffect::kMagickaRateMult:
			return RE::ActorValue::kMagickaRateMult;
			break;
		case AlchemyEffect::kStaminaRateMult:
			return RE::ActorValue::kStaminaRateMult;
			break;
		case AlchemyEffect::kPickpocket:
			return RE::ActorValue::kPickpocket;
			break;
		case AlchemyEffect::kLockpicking:
			return RE::ActorValue::kLockpicking;
			break;
		case AlchemyEffect::kSneak:
			return RE::ActorValue::kSneak;
			break;
		case AlchemyEffect::kFear:
			return RE::ActorValue::kConfidence;
			break;
		case AlchemyEffect::kFrenzy:
			return RE::ActorValue::kAggresion;
			break;
		case AlchemyEffect::kReflectDamage:
			return RE::ActorValue::kReflectDamage;
			break;
		case AlchemyEffect::kEnchanting:
			return RE::ActorValue::kEnchanting;
			break;
		case AlchemyEffect::kSmithing:
			return RE::ActorValue::kSmithing;
			break;
		case AlchemyEffect::kSpeech:
			return RE::ActorValue::kSpeech;
		case AlchemyEffect::kCarryWeight:
			return RE::ActorValue::kCarryWeight;
			break;
		case AlchemyEffect::kAlchemy:
			return RE::ActorValue::kAlchemy;
			break;
		//case AlchemyEffect::kCureDisease:
		//case AlchemyEffect::kCurePoison:
		//case AlchemyEffect::kWaterbreathing:
		//case AlchemyEffect::kPersuasion:
		default:
			return RE::ActorValue::kNone;
			break;
		}
	}

	/// <summary>
	/// classifies a single item based on its effects
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	static std::tuple<uint64_t, ItemStrength, ItemType> ClassifyItem(RE::AlchemyItem* item);

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
