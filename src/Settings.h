#include "SimpleIni.h"
#include <fstream>
#include <iostream>
#include <type_traits>
#include <utility>
#pragma once

#define LOG_1(s)               \
	if (Settings::EnableLog) \
		logger::info(s);

#define LOG1_1(s, t)           \
	if (Settings::EnableLog) \
		logger::info(s, t);

#define LOG_2(s)             \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s);

#define LOG1_2(s, t)         \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, t);

#define LOG_3(s)             \
	if (Settings::EnableLog && Settings::LogLevel >= 2) \
		logger::info(s);

#define LOG1_3(s, t)         \
	if (Settings::EnableLog && Settings::LogLevel >= 2) \
		logger::info(s, t);

#define LOG_4(s)             \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s);

#define LOG1_4(s, t)         \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s, t);



#define PROF_1(s)                    \
	if (Settings::EnableProfiling) \
		logger::info(s);

#define PROF1_1(s, t)                \
	if (Settings::EnableProfiling) \
		logger::info(s, t);

#define PROF_2(s)                    \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 1) \
		logger::info(s);

#define PROF1_2(s, t)                \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 1) \
		logger::info(s, t);

#define PROF_3(s)                    \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 2) \
		logger::info(s);

#define PROF1_3(s, t)                \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 2) \
		logger::info(s, t);


struct Settings
{
	static void Load()
	{
		constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(path);

		// Features
		_featMagickaRestoration = ini.GetValue("Features", "EnableMagickaRestoration") ? ini.GetBoolValue("Features", "EnableMagickaRestoration") : true;
		logger::info("[SETTINGS] {} {}", "EnableMagickaRestoration", std::to_string(_featMagickaRestoration));
		_featStaminaRestoration = ini.GetValue("Features", "EnableStaminaRestoration") ? ini.GetBoolValue("Features", "EnableStaminaRestoration") : true;
		logger::info("[SETTINGS] {} {}", "EnableStaminaRestoration", std::to_string(_featStaminaRestoration));
		_featHealthRestoration = ini.GetValue("Features", "EnableHealthRestoration") ? ini.GetBoolValue("Features", "EnableHealthRestoration") : true;
		logger::info("[SETTINGS] {} {}", "EnableHealthRestoration", std::to_string(_featHealthRestoration));
		_playerRestorationEnabled = ini.GetValue("Features", "EnablePlayerRestoration") ? ini.GetBoolValue("Features", "EnablePlayerRestoration") : false;
		logger::info("[SETTINGS] {} {}", "EnablePlayerRestoration", std::to_string(_playerRestorationEnabled));
		// Restoration Thresholds
		_healthThresholdLower = ini.GetValue("Restoration", "HealthThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdLowerPercent", 0.3f)) : 0.3f;
		if (_healthThresholdLower > 0.95f)
			_healthThresholdLower = 0.95f;
		logger::info("[SETTINGS] {} {}", "HealthThresholdLowerPercent", std::to_string(_healthThresholdLower));
		_healthThresholdUpper = ini.GetValue("Restoration", "HealthThresholdUpperPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdUpperPercent", 0.3f)) : 0.5f;
		if (_healthThresholdUpper > 0.95f)
			_healthThresholdUpper = 0.95f;
		logger::info("[SETTINGS] {} {}", "HealthThresholdUpperPercent", std::to_string(_healthThresholdUpper));
		_magickaThresholdLower = ini.GetValue("Restoration", "MagickaThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdLowerPercent", 0.3f)) : 0.3f;
		if (_magickaThresholdLower > 0.95f)
			_magickaThresholdLower = 0.95f;
		logger::info("[SETTINGS] {} {}", "MagickaThresholdLowerPercent", std::to_string(_magickaThresholdLower));
		_magickaThresholdUpper = ini.GetValue("Restoration", "MagickaThresholdUpperPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdUpperPercent", 0.3f)) : 0.5f;
		if (_magickaThresholdUpper > 0.95f)
			_magickaThresholdUpper = 0.95f;
		logger::info("[SETTINGS] {} {}", "MagickaThresholdUpperPercent", std::to_string(_magickaThresholdUpper));
		_staminaThresholdLower = ini.GetValue("Restoration", "StaminaThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdLowerPercent", 0.3f)) : 0.3f;
		if (_staminaThresholdLower > 0.95f)
			_staminaThresholdLower = 0.95f;
		logger::info("[SETTINGS] {} {}", "StaminaThresholdLowerPercent", std::to_string(_staminaThresholdLower));
		_staminaThresholdUpper = ini.GetValue("Restoration", "StaminaThresholdUpperPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdUpperPercent", 0.3f)) : 0.5f;
		if (_staminaThresholdUpper > 0.95f)
			_staminaThresholdUpper = 0.95f;
		logger::info("[SETTINGS] {} {}", "StaminaThresholdUpperPercent", std::to_string(_staminaThresholdUpper));
		
		// general
		_maxPotionsPerCycle = ini.GetValue("General", "MaxPotionsPerCycle") ? ini.GetLongValue("General", "MaxPotionsPerCycle", 2) : 2;
		logger::info("[SETTINGS] {} {}", "MaxPotionsPerCycle", std::to_string(_maxPotionsPerCycle));
		_cycletime = ini.GetValue("General", "CycleWaitTime") ? ini.GetLongValue("General", "CycleWaitTime", 2) : 500;
		logger::info("[SETTINGS] {} {}", "CycleWaitTime", std::to_string(_cycletime));

		// Debugging
		EnableLog = ini.GetValue("Debug", "EnableLogging") ? ini.GetBoolValue("Debug", "EnableLogging") : false;
		logger::info("[SETTINGS] {} {}", "EnableLogging", std::to_string(EnableLog));
		EnableProfiling = ini.GetValue("Debug", "EnableProfiling") ? ini.GetBoolValue("Debug", "EnableProfiling") : false;
		logger::info("[SETTINGS] {} {}", "EnableProfiling", std::to_string(EnableProfiling));
	}

	static void Save()
	{
		constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

		CSimpleIniA ini;

		ini.SetUnicode();

		// features
		ini.SetBoolValue("Features", "EnableHealthRestoration", _featHealthRestoration);
		ini.SetBoolValue("Features", "EnableMagickaRestoration", _featMagickaRestoration);
		ini.SetBoolValue("Features", "EnableStaminaRestoration", _featStaminaRestoration);
		ini.SetBoolValue("Features", "EnablePlayerRestoration", _playerRestorationEnabled);
		ini.SetBoolValue("Features", "DistributePotions", _featDistributePotions);
		ini.SetBoolValue("Features", "DistributePoisons", _featDistributePoisons);
		ini.SetBoolValue("Features", "DistributeFood", _featDistributeFood);
		
		// compatibility
		ini.SetBoolValue("Compatibility", "UltimatePotionAnimation", _CompatibilityPotionAnimation, "Compatibility mode for \"zxlice's ultimate potion animation\". Requires the Skyrim esp plugin. This is automatically enabled if zxlice's mod is detected");
		ini.SetBoolValue("Compatibility", "Compatibility", _CompatibilityMode, "General Compatibility Mode. If set to true, all items will be equiped using Papyrus workaround. Requires the Skyrim esp plugin.");

		// restoration thresholds
		ini.SetDoubleValue("Restoration", "HealthThresholdLowerPercent", _healthThresholdLower);
		ini.SetDoubleValue("Restoration", "HealthThresholdUpperPercent", _healthThresholdUpper);
		ini.SetDoubleValue("Restoration", "MagickaThresholdLowerPercent", _magickaThresholdLower);
		ini.SetDoubleValue("Restoration", "MagickaThresholdUpperPercent", _magickaThresholdUpper);
		ini.SetDoubleValue("Restoration", "StaminaThresholdLowerPercent", _staminaThresholdLower);
		ini.SetDoubleValue("Restoration", "StaminaThresholdUpperPercent", _staminaThresholdUpper);

		// distribution options

		// general
		ini.SetLongValue("General", "MaxPotionsPerCycle", _maxPotionsPerCycle);
		//logger::info("[SETTINGS] writing {} {}", "MaxPotionsPerCycle", std::to_string(_maxPotionsPerCycle));
		ini.SetLongValue("General", "CycleWaitTime", _cycletime);
		//logger::info("[SETTINGS] writing {} {}", "CycleWaitTime", std::to_string(_cycletime));

		// debugging
		ini.SetBoolValue("Debug", "EnableLogging", EnableLog);
		ini.SetLongValue("Debug", "LogLevel", LogLevel, "1 - layer 0 log entries, 2 - layer 1 log entries, 3 - layer 3 log entries, 4 - layer 4 log entries. Affects which functions write log entries, as well as what is written by those functions. ");
		ini.SetBoolValue("Debug", "EnableProfiling", EnableProfiling);
		ini.SetLongValue("Debug", "ProfileLevel", ProfileLevel, "1 - only highest level functions write their executions times to the log, 1 - lower level functions are written, 2 - lowest level functions are written. Be aware that not all functions are supported as Profiling costs execution time.");

		ini.SaveFile(path);
	}

	// threshold variables
	static inline float _healthThresholdLower = 0.3f;
	static inline float _healthThresholdUpper = 0.5f;
	static inline float _magickaThresholdLower = 0.3f;
	static inline float _magickaThresholdUpper = 0.5f;
	static inline float _staminaThresholdLower = 0.3f;
	static inline float _staminaThresholdUpper = 0.5f;

	static inline long _maxPotionsPerCycle = 2;
	static inline long _cycletime = 500;
	
	// features
	static inline bool _featMagickaRestoration = true;
	static inline bool _featStaminaRestoration = true;
	static inline bool _featHealthRestoration = true;
	static inline bool _playerRestorationEnabled = false;
	static inline bool _featDistributePoisons = true; // player is excluded from distribution options, as well as followers
	static inline bool _featDistributePotions = true; // player is excluded from distribution options, as well as followers
	static inline bool _featDistributeFood = true; // player is excluded from distribution options, as well as followers
	static inline bool _featUseDeathItems = true; // the npc will be given potions that may appear in their deathItems if available
	//static inline bool _featRemovePotionsOnDeath = false;
	//static inline bool _featRemovePoisonsOnDeath = false;

	// compatibility
	static inline bool _CompatibilityMode = false; // Use Items with Papyrus, needs the plugin
	static inline bool _CompatibilityPotionAnimation = false; // Use Potions with Papyrus

	static inline bool EnableLog = false;
	static inline int LogLevel = 0; // 0 - only highest level
									// 1 - highest to layer 1 function logging
									// 2 - highest to layer 2 function logging
									// 3 - highest to layer 3 function logging
	static inline int ProfileLevel = 0;	// 0 - highest level only
										// 1 - highest and layer 1
										// 2 - highest and layer 2
	static inline bool EnableProfiling = false;

	// distribution options
	static inline int _DistPoisChance1 = 50; // chance for npcs to receive one poison
	static inline int _DistPoisChance2 = 20; // chance for npcs to receive a second poison
	static inline int _DistPotChance1 = 50; // chance to distribute one stat potion
	static inline int _DistPotChance2 = 20;	// chance to distribute a second stat potion
	static inline int _DistPotChance3 = 10; // chance to distribute a third stat potion
	static inline int _DistPotChanceAdd = 5; // chance to receive additional stat potions
	static inline int _DistPotMax = 5;  // max number of potions to give
	static inline int _DistFPotChance1 = 50;
	static inline int _DistFPotChance2 = 20;

	static inline int _LevelEasy = 20; // only distribute "weak" potions and poisons
	static inline int _LevelNormal = 35; // may distribute "standard" potions and poisons
	static inline int _LevelDifficult = 50; // may distribute "potent" potions and poisons
	static inline int _LevelInsane = 70; // may have Insane tear potions

	static inline int _MaxMagnitudeWeak = 30; // max potion / poison magnitude to be considered "weak"
	static inline int _MaxMagnitudeStandard = 60; // max potion / poison magnitude to be considered "standard"
	static inline int _MaxMagnitudePotent = 300; // max potion / poison magnitude to be considered "potent"
												// anything above this won't be distributed

	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsWeak_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsWeak_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsStandard_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsStandard_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsPotent_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsPotent_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsInsane_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _potionsInsane_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsWeak_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsStandard_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent_main{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _poisonsPotent_rest{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodmagicka{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodstamina{};
	static inline std::list<std::pair<uint64_t, RE::AlchemyItem*>> _foodhealth{};

	static inline std::list<RE::AlchemyItem*> alitems{};
	static inline std::list<RE::AlchemyItem*> potions{};
	static inline std::list<RE::AlchemyItem*> food{};
	static inline std::list<RE::AlchemyItem*> poisons{};


	enum class AlchemyEffect : unsigned __int64
	{
		kNone = 0,
		kHealth = 1 << 0,
		kMagicka = 1 << 1,
		kStamina = 1 << 2,
		kOneHanded = 1 << 3,
		kTwoHanded = 1 << 4,
		kArchery = 1 << 5,
		kBlock = 1 << 6,
		kHeavyArmor = 1 << 7,
		kLightArmor = 1 << 8,
		kAlteration = 1 << 9,
		kConjuration = 1 << 10,
		kDestruction = 1 << 11,
		kIllusion = 1 << 12,
		kRestoration = 1 << 13,
		kHealRate = 1 << 14,
		kMagickaRate = 1 << 15,
		kStaminaRate = 1 << 16,
		kSpeedMult = 1 << 17,
		kCriticalChance = 1 << 18,
		kMeleeDamage = 1 << 19,
		kUnarmedDamage = 1 << 20,
		kDamageResist = 1 << 21,
		kPoisonResist = 1 << 22,
		kResistFire = 1 << 23,
		kResistShock = 1 << 24,
		kResistFrost = 1 << 25,
		kResistMagic = 1 << 26,
		kResistDisease = 1 << 27,
		kParalysis = 1 << 28,
		kInvisibility = 1 << 29,
		kWeaponSpeedMult = 1 << 30,
		kAttackDamageMult = (unsigned __int64)1 << 31,
		kHealRateMult = (unsigned __int64)1 << 32,
		kMagickaRateMult = (unsigned __int64)1 << 33,
		kStaminaRateMult = (unsigned __int64)1 << 34,
	};
	enum class ItemStrength
	{
		kWeak = 1,
		kStandard = 2,
		kPotent = 4,
		kInsane = 8
	};
	enum class ItemType
	{
		kPoison = 1,
		kPotion = 2,
		kFood = 4
	};

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
			return (AlchemyEffect::kOneHanded);
			break;
		case RE::ActorValue::kTwoHanded:
			return (AlchemyEffect::kTwoHanded);
			break;
		case RE::ActorValue::kArchery:
			return (AlchemyEffect::kArchery);
			break;
		case RE::ActorValue::kBlock:
			return (AlchemyEffect::kBlock);
			break;
		//case RE::ActorValue::kSmithing:
		//	break;
		case RE::ActorValue::kHeavyArmor:
			return (AlchemyEffect::kHeavyArmor);
			break;
		case RE::ActorValue::kLightArmor:
			return (AlchemyEffect::kLightArmor);
			break;
		//case RE::ActorValue::kPickpocket:
		//	break;
		//case RE::ActorValue::kLockpicking:
		//	break;
		//case RE::ActorValue::kSneak:
		//	break;
		//case RE::ActorValue::kAlchemy:
		//	break;
		//case RE::ActorValue::kSpeech:
		//	break;
		case RE::ActorValue::kAlteration:
			return (AlchemyEffect::kAlteration);
			break;
		case RE::ActorValue::kConjuration:
			return (AlchemyEffect::kConjuration);
			break;
		case RE::ActorValue::kDestruction:
			return (AlchemyEffect::kDestruction);
			break;
		case RE::ActorValue::kIllusion:
			return (AlchemyEffect::kIllusion);
			break;
		case RE::ActorValue::kRestoration:
			return (AlchemyEffect::kRestoration);
			break;
		//case RE::ActorValue::kEnchanting:
		//	break;
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
		//case RE::ActorValue::kCarryWeight:
		//	break;
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
			return (AlchemyEffect::kWeaponSpeedMult);
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
		case AlchemyEffect::kHealRateMult:
			return RE::ActorValue::kHealRateMult;
			break;
		case AlchemyEffect::kMagickaRateMult:
			return RE::ActorValue::kMagickaRateMult;
			break;
		case AlchemyEffect::kStaminaRateMult:
			return RE::ActorValue::kStaminaRateMult;
			break;
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
	static std::tuple<uint64_t, ItemStrength, ItemType> ClassifyItem(RE::AlchemyItem* item)
	{
		RE::EffectSetting* sett = nullptr;
		if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
			return {0, ItemStrength::kStandard, ItemType::kFood};
		}
		// we look at max 4 effects
		RE::ActorValue av[4]{
			RE::ActorValue::kAlchemy,
			RE::ActorValue::kAlchemy,
			RE::ActorValue::kAlchemy,
			RE::ActorValue::kAlchemy
		};
		float mag[]{
			0,
			0,
			0,
			0
		};
		// we will not abort the loop, since the number of effects on one item is normally very
		// limited, so we don't have much iterations
		if (item->effects.size() > 0) {
			for (uint32_t i = 0; i < item->effects.size() && i < 4; i++) {
				sett = item->effects[i]->baseEffect;
				// just retrieve the effects, we will analyze them later
				if (sett) {
					av[i] = sett->data.primaryAV;
					mag[i] = item->effects[i]->effectItem.magnitude;
				}
			}
		} else {
			RE::MagicItem::SkillUsageData err;
			item->GetSkillUsageData(err);
			switch (item->avEffectSetting->data.primaryAV) {
			case RE::ActorValue::kHealth:
				av[0] = item->avEffectSetting->data.primaryAV;
				mag[0] = err.magnitude;
				break;
			case RE::ActorValue::kMagicka:
				av[0] = item->avEffectSetting->data.primaryAV;
				mag[0] = err.magnitude;
				break;
			case RE::ActorValue::kStamina:
				av[0] = item->avEffectSetting->data.primaryAV;
				mag[0] = err.magnitude;
				break;
			}
		}
		// analyze the effect types
		uint64_t alch = static_cast<uint64_t>(AlchemyEffect::kNone);
		ItemStrength str = ItemStrength::kWeak;
		float maxmag = 0;
		for (int i = 0; i < 4; i++) {
			switch (av[i]) {
			case RE::ActorValue::kHealth:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHealth);
				if (mag[i] < maxmag)
					maxmag = mag[i];
				break;
			case RE::ActorValue::kMagicka:
				alch |= static_cast<uint64_t>(AlchemyEffect::kMagicka);
				if (mag[i] < maxmag)
					maxmag = mag[i];
				break;
			case RE::ActorValue::kStamina:
				alch |= static_cast<uint64_t>(AlchemyEffect::kStamina);
				if (mag[i] < maxmag)
					maxmag = mag[i];
				break;
			case RE::ActorValue::kOneHanded:
				alch |= static_cast<uint64_t>(AlchemyEffect::kOneHanded);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kTwoHanded:
				alch |= static_cast<uint64_t>(AlchemyEffect::kTwoHanded);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kArchery:
				alch |= static_cast<uint64_t>(AlchemyEffect::kArchery);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kBlock:
				alch |= static_cast<uint64_t>(AlchemyEffect::kBlock);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kSmithing:
			//	break;
			case RE::ActorValue::kHeavyArmor:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHeavyArmor);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kLightArmor:
				alch |= static_cast<uint64_t>(AlchemyEffect::kLightArmor);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kPickpocket:
			//	break;
			//case RE::ActorValue::kLockpicking:
			//	break;
			//case RE::ActorValue::kSneak:
			//	break;
			//case RE::ActorValue::kAlchemy:
			//	break;
			//case RE::ActorValue::kSpeech:
			//	break;
			case RE::ActorValue::kAlteration:
				alch |= static_cast<uint64_t>(AlchemyEffect::kAlteration);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kConjuration:
				alch |= static_cast<uint64_t>(AlchemyEffect::kConjuration);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kDestruction:
				alch |= static_cast<uint64_t>(AlchemyEffect::kDestruction);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kIllusion:
				alch |= static_cast<uint64_t>(AlchemyEffect::kIllusion);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kRestoration:
				alch |= static_cast<uint64_t>(AlchemyEffect::kRestoration);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kEnchanting:
			//	break;
			case RE::ActorValue::kHealRate:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHealRate);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kMagickaRate:
				alch |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::KStaminaRate:
				alch |= static_cast<uint64_t>(AlchemyEffect::kStaminaRate);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kSpeedMult:
				alch |= static_cast<uint64_t>(AlchemyEffect::kSpeedMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kInventoryWeight:
			//	break;
			//case RE::ActorValue::kCarryWeight:
			//	break;
			case RE::ActorValue::kCriticalChance:
				alch |= static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kMeleeDamage:
				alch |= static_cast<uint64_t>(AlchemyEffect::kMeleeDamage);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kUnarmedDamage:
				alch |= static_cast<uint64_t>(AlchemyEffect::kUnarmedDamage);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kDamageResist:
				alch |= static_cast<uint64_t>(AlchemyEffect::kDamageResist);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kPoisonResist:
				alch |= static_cast<uint64_t>(AlchemyEffect::kPoisonResist);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kResistFire:
				alch |= static_cast<uint64_t>(AlchemyEffect::kResistFire);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kResistShock:
				alch |= static_cast<uint64_t>(AlchemyEffect::kResistShock);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kResistFrost:
				alch |= static_cast<uint64_t>(AlchemyEffect::kResistFrost);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kResistMagic:
				alch |= static_cast<uint64_t>(AlchemyEffect::kResistMagic);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kResistDisease:
				alch |= static_cast<uint64_t>(AlchemyEffect::kResistDisease);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kParalysis:
				alch |= static_cast<uint64_t>(AlchemyEffect::kParalysis);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kInvisibility:
				alch |= static_cast<uint64_t>(AlchemyEffect::kInvisibility);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kNightEye:
			//	break;
			//case RE::ActorValue::kDetectLifeRange:
			//	break;
			//case RE::ActorValue::kWaterBreathing:
			//	break;
			//case RE::ActorValue::kWaterWalking:
			//	break;
			case RE::ActorValue::kWeaponSpeedMult:
				alch |= static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kAttackDamageMult:
				alch |= static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kHealRateMult:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHealRateMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kMagickaRateMult:
				alch |= static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kStaminaRateMult:
				alch |= static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			}
		}
		if (std::string(item->GetName()).find(std::string("Weak")) != std::string::npos)
			str = ItemStrength::kWeak;
		else if (std::string(item->GetName()).find(std::string("Standard")) != std::string::npos)
			str = ItemStrength::kStandard;
		else if (std::string(item->GetName()).find(std::string("Potent")) != std::string::npos)
			str = ItemStrength::kPotent;
		else if (maxmag == 0)
			str = ItemStrength::kStandard;
		else if (maxmag <= _MaxMagnitudeWeak)
			str = ItemStrength::kWeak;
		else if (maxmag <= _MaxMagnitudeStandard)
			str = ItemStrength::kStandard;
		else if (maxmag <= _MaxMagnitudePotent)
			str = ItemStrength::kPotent;
		else
			str = ItemStrength::kInsane;
		ItemType t = ItemType::kPotion;
		if (item->IsFood())
			t = ItemType::kFood;
		else if (item->IsPoison())
			t = ItemType::kPoison;
		return { alch, str , t};
	}

	/// <summary>
	/// classifies all AlchemyItems in the game according to its effects
	/// </summary>
	static void ClassifyItems()
	{
		auto begin = std::chrono::steady_clock::now();
		auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
		auto end = hashtable->end();
		auto iter = hashtable->begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != end) {
			if ((*iter).second && (*iter).second->IsMagicItem()) {
				item = (*iter).second->As<RE::AlchemyItem>();
				if (item) {
					auto clas = ClassifyItem(item);
					// determine the type of item
					if (std::get<2>(clas) == ItemType::kFood)
					{
						// we will only classify food which works on stamina, magicka or health for now
						if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealRate)) > 0) {
							_foodhealth.insert(_foodhealth.end(), { std::get<0>(clas), item });
						} else if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagicka)) > 0 ||
								   (std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagickaRate)) > 0) {
							_foodmagicka.insert(_foodmagicka.end(), { std::get<0>(clas), item });
						} else if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStamina)) > 0 ||
								   (std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStaminaRate)) > 0) {
							_foodstamina.insert(_foodstamina.end(), { std::get<0>(clas), item });
						}
					} else if (std::get<2>(clas) == ItemType::kPoison) {
						if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagicka)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStamina)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealRate)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagickaRate)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStaminaRate)) > 0) {
							switch (std::get<1>(clas)) {
							case ItemStrength::kWeak:
								_poisonsWeak_main.insert(_poisonsWeak_main.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kStandard:
								_poisonsStandard_main.insert(_poisonsStandard_main.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kPotent:
								_poisonsPotent_main.insert(_poisonsPotent_main.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kInsane:
								break;
							}
						} else if (std::get<0>(clas) != static_cast<uint64_t>(AlchemyEffect::kNone)) {
							switch (std::get<1>(clas)) {
							case ItemStrength::kWeak:
								_poisonsWeak_rest.insert(_poisonsWeak_rest.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kStandard:
								_poisonsStandard_rest.insert(_poisonsStandard_rest.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kPotent:
								_poisonsPotent_rest.insert(_poisonsPotent_rest.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kInsane:
								break;
							}
						}
					} else if (std::get<2>(clas) == ItemType::kPotion) {
						if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagicka)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStamina)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealRate)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagickaRate)) > 0 ||
							(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStaminaRate)) > 0) {
							switch (std::get<1>(clas)) {
							case ItemStrength::kWeak:
								_potionsWeak_main.insert(_potionsWeak_main.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kStandard:
								_potionsStandard_main.insert(_potionsStandard_main.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kPotent:
								_potionsPotent_main.insert(_potionsPotent_main.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kInsane:
								_potionsInsane_main.insert(_potionsPotent_main.end(), { std::get<0>(clas), item });
								break;
							}
						} else if (std::get<0>(clas) != static_cast<uint64_t>(AlchemyEffect::kNone)) {
							switch (std::get<1>(clas)) {
							case ItemStrength::kWeak:
								_potionsWeak_rest.insert(_potionsWeak_rest.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kStandard:
								_potionsStandard_rest.insert(_potionsStandard_rest.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kPotent:
								_potionsPotent_rest.insert(_potionsPotent_rest.end(), { std::get<0>(clas), item });
								break;
							case ItemStrength::kInsane:
								_potionsInsane_rest.insert(_potionsInsane_rest.end(), { std::get<0>(clas), item });
								break;
							}
						}
					}
				}
			}
			iter++;
		}
		PROF1_1("[PROF] [ClassifyItems] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
	}
};
