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


#define LOGE_1(s)             \
	if (Settings::EnableLog) \
		logger::info(s);

#define LOGE1_1(s, t)         \
	if (Settings::EnableLog) \
		logger::info(s, t);

#define LOGE_2(s)            \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s);

#define LOGE1_2(s, t)        \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, t);

#define LOGE2_2(s, t, u)                                   \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, t, u);

#define LOGE3_2(s, t, u, v)                                   \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, t, u, v);



#define LOG_1(s)               \
	if (Settings::EnableLog) \
		logger::info(s, Settings::TimePassed() + " | ");

#define LOG1_1(s, t)           \
	if (Settings::EnableLog) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define LOG2_1(s, t, u)         \
	if (Settings::EnableLog) \
		logger::info(s, Settings::TimePassed() + " | ", t, u);

#define LOG_2(s)                                        \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, Settings::TimePassed() + " | ");

#define LOG1_2(s, t)         \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define LOG2_2(s, t, u)                                    \
	if (Settings::EnableLog && Settings::LogLevel >= 1) \
		logger::info(s, Settings::TimePassed() + " | ", t, u);

#define LOG_3(s)             \
	if (Settings::EnableLog && Settings::LogLevel >= 2) \
		logger::info(s, Settings::TimePassed() + " | ");

#define LOG1_3(s, t)         \
	if (Settings::EnableLog && Settings::LogLevel >= 2) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define LOG_4(s)             \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s, Settings::TimePassed() + " | ");

#define LOG1_4(s, t)         \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define LOG2_4(s, t, u)                           \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s, Settings::TimePassed() + " | ", t, u);

#define LOG4_1(s, t)                           \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define LOG4_4(s, t, u, v, w)                                    \
	if (Settings::EnableLog && Settings::LogLevel >= 3) \
		logger::info(s, Settings::TimePassed() + " | ", t, u, v, w);

#define PROF_1(s)                    \
	if (Settings::EnableProfiling) \
		logger::info(s, Settings::TimePassed() + " | ");

#define PROF1_1(s, t)                \
	if (Settings::EnableProfiling) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define PROF_2(s)                    \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 1) \
		logger::info(s, Settings::TimePassed() + " | ");

#define PROF1_2(s, t)                \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 1) \
		logger::info(s, Settings::TimePassed() + " | ", t);

#define PROF_3(s)                    \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 2) \
		logger::info(s, Settings::TimePassed() + " | ");

#define PROF1_3(s, t)                \
	if (Settings::EnableProfiling && Settings::ProfileLevel >= 2) \
		logger::info(s, Settings::TimePassed() + " | ", t);


class Settings
{
public:
	static inline std::string PluginName = "NPCsUsePotions.esp";
	static inline std::chrono::time_point<std::chrono::system_clock> execstart = std::chrono::system_clock::now();
	
	static std::string TimePassed()
	{
		std::stringstream ss;
		ss << std::setw(12) << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - execstart);
		return ss.str();
	}

	class Compatibility
	{
	public:
		static inline std::string Plugin_PotionAnimatedfx = "PotionAnimatedfx.esp";
		static inline RE::EffectSetting* PAF_NPCDrinkingCoolDownEffect = nullptr;
		static inline RE::SpellItem* PAF_NPCDrinkingCoolDownSpell = nullptr;
		static inline std::string PAF_NPCDrinkingCoolDownEffect_name = "PAF_NPCDrinkingCoolDownEffect";
		static inline std::string PAF_NPCDrinkingCoolDownSpell_name = "PAF_NPCDrinkingCoolDownSpell";
	};

	enum class AlchemyEffect : unsigned __int64
	{
		kNone = 0,							// 0
		kHealth = 1 << 0,					// 1
		kMagicka = 1 << 1,					// 2
		kStamina = 1 << 2,					// 4
		kOneHanded = 1 << 3,				// 8
		kTwoHanded = 1 << 4,				// 10
		kArchery = 1 << 5,					// 20
		kBlock = 1 << 6,					// 40
		kHeavyArmor = 1 << 7,				// 80
		kLightArmor = 1 << 8,				// 100
		kAlteration = 1 << 9,				// 200
		kConjuration = 1 << 10,				// 400
		kDestruction = 1 << 11,				// 800
		kIllusion = 1 << 12,				// 1000
		kRestoration = 1 << 13,				// 2000
		kHealRate = 1 << 14,				// 4000
		kMagickaRate = 1 << 15,				// 8000
		kStaminaRate = 1 << 16,				// 10000
		kSpeedMult = 1 << 17,				// 20000
		kCriticalChance = 1 << 18,			// 40000
		kMeleeDamage = 1 << 19,				// 80000
		kUnarmedDamage = 1 << 20,			// 100000
		kDamageResist = 1 << 21,			// 200000
		kPoisonResist = 1 << 22,			// 400000
		kResistFire = 1 << 23,				// 800000
		kResistShock = 1 << 24,				// 1000000
		kResistFrost = 1 << 25,				// 2000000
		kResistMagic = 1 << 26,				// 4000000
		kResistDisease = 1 << 27,			// 8000000
		kParalysis = 1 << 28,				// 10000000
		kInvisibility = 1 << 29,			// 20000000
		kWeaponSpeedMult = 1 << 30,			// 40000000
		kAttackDamageMult = (unsigned __int64)1 << 31,		// 80000000
		kHealRateMult = (unsigned __int64)1 << 32,			// 100000000
		kMagickaRateMult = (unsigned __int64)1 << 33,		// 200000000
		kStaminaRateMult = (unsigned __int64)1 << 34,		// 400000000
		kBlood = (unsigned __int64)1 << 35,					// 800000000
		kPickpocket = (unsigned __int64)1 << 36,			// 1000000000
		kLockpicking = (unsigned __int64)1 << 37,			// 2000000000
		kSneak = (unsigned __int64)1 << 38,					// 4000000000
		kFrenzy = (unsigned __int64)1 << 39,				// 8000000000
		kFear = (unsigned __int64)1 << 40,					// 10000000000
		kBowSpeed = (unsigned __int64)1 << 41,              // 20000000000
		// 2000007
		kAnyPotion = static_cast<uint64_t>(kHealth) | static_cast<uint64_t>(kMagicka) | static_cast<uint64_t>(kStamina) | static_cast<uint64_t>(kInvisibility),
		// 180D7E3C007
		kAnyPoison = static_cast<uint64_t>(kHealth) | static_cast<uint64_t>(kMagicka) | static_cast<uint64_t>(kStamina) | static_cast<uint64_t>(kMagickaRate) | static_cast<uint64_t>(kStaminaRate) | static_cast<uint64_t>(kHealRate) | static_cast<uint64_t>(kParalysis) | static_cast<uint64_t>(kSpeedMult) | static_cast<uint64_t>(kDamageResist) | static_cast<uint64_t>(kPoisonResist) | static_cast<uint64_t>(kWeaponSpeedMult) | static_cast<uint64_t>(kAttackDamageMult) | static_cast<uint64_t>(kResistFire) | static_cast<uint64_t>(kResistFrost) | static_cast<uint64_t>(kResistMagic) | static_cast<uint64_t>(kResistShock) | static_cast<uint64_t>(kFrenzy) | static_cast<uint64_t>(kFear),
		// CBFA3FF8
		kAnyFortify = static_cast<uint64_t>(kOneHanded) | static_cast<uint64_t>(kTwoHanded) | static_cast<uint64_t>(kArchery) | static_cast<uint64_t>(kBlock) | static_cast<uint64_t>(kHeavyArmor) | static_cast<uint64_t>(kLightArmor) | static_cast<uint64_t>(kAlteration) | static_cast<uint64_t>(kConjuration) | static_cast<uint64_t>(kDestruction) | static_cast<uint64_t>(kIllusion) | static_cast<uint64_t>(kRestoration) | static_cast<uint64_t>(kSpeedMult) | static_cast<uint64_t>(kMeleeDamage) | static_cast<uint64_t>(kUnarmedDamage) | static_cast<uint64_t>(kDamageResist) | static_cast<uint64_t>(kPoisonResist) | static_cast<uint64_t>(kResistFire) | static_cast<uint64_t>(kResistFrost) | static_cast<uint64_t>(kResistShock) | static_cast<uint64_t>(kResistDisease) | static_cast<uint64_t>(kWeaponSpeedMult) | static_cast<uint64_t>(kAttackDamageMult),
		// 1C007
		kAnyFood = static_cast<uint64_t>(kHealth) | static_cast<uint64_t>(kMagicka) | static_cast<uint64_t>(kStamina) | static_cast<uint64_t>(kHealRate) | static_cast<uint64_t>(kMagickaRate) | static_cast<uint64_t>(kStaminaRate),
		
	};
	enum class ItemStrength
	{
		kWeak = 1,
		kStandard = 2,
		kPotent = 3,
		kInsane = 4
	};
	enum class ActorStrength
	{
		Weak = 0,
		Normal = 1,
		Powerful = 2,
		Insane = 3,
		Boss = 4,
	};
	enum class ItemType
	{
		kPoison = 1,
		kPotion = 2,
		kFood = 4,
		kFortifyPotion = 8,
	};

	static void CheckActorsForRules();

	class Distribution
	{
	public:
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

		class Rule
		{
		public:
			// if set to false this rule is effectively empty
			bool				valid = true;
			///
			int					ruleVersion;
			int					ruleType;
			std::string			ruleName;

			/// TYPE 1 - Rule
			// general
			int					rulePriority;
			std::string			assocObjects;
			std::string			potionProperties;
			std::string			fortifyproperties;
			std::string			poisonProperties;
			std::string			foodProperties;
			bool				allowMixed;
			// chances and options
			int					maxPotions;
			int					maxPoisons;
			std::vector<int>	potion1Chance;
			std::vector<int>	potion2Chance;
			std::vector<int>	potion3Chance;
			std::vector<int>	potionAdditionalChance;
			std::vector<int>	fortify1Chance;
			std::vector<int>	fortify2Chance;
			int					potionTierAdjust;
			std::vector<int>	poison1Chance;
			std::vector<int>	poison2Chance;
			std::vector<int>	poison3Chance;
			std::vector<int>	poisonAdditionalChance;
			int					poisonTierAdjust;
			std::vector<int>	foodChance;

			/// TYPE 2 - Exclusion
			//std::string			assocRuleName;
			//std::string			assocExclusions;

			std::vector<std::tuple<int, Settings::AlchemyEffect>> potionDistr;
			std::vector<std::tuple<int, Settings::AlchemyEffect>> poisonDistr;
			std::vector<std::tuple<int, Settings::AlchemyEffect>> fortifyDistr;
			std::vector<std::tuple<int, Settings::AlchemyEffect>> foodDistr;

			uint64_t validPotions;
			uint64_t validPoisons;
			uint64_t validFortifyPotions;
			uint64_t validFood;

			std::vector<RE::AlchemyItem*> GetRandomPotions(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			std::vector<RE::AlchemyItem*> GetRandomPoisons(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			std::vector<RE::AlchemyItem*> GetRandomFortifyPotions(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			std::vector<RE::AlchemyItem*> GetRandomFood(Settings::ItemStrength strength, Settings::ActorStrength acstrength);

			Rule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, int _maxPotions,
				std::vector<int> _potion1Chance, std::vector<int> _potion2Chance, std::vector<int> _potion3Chance,
				std::vector<int> _potionAdditionalChance, std::vector<int> _fortify1Chance, std::vector<int> _fortify2Chance,
				int _potionTierAdjust, int _maxPoisons, std::vector<int> _poison1Chance,
				std::vector<int> _poison2Chance, std::vector<int> _poison3Chance, std::vector<int> _poisonAdditionalChance,
				int _poisonTierAdjust, std::vector<int> _foodChance, std::vector<std::tuple<int, Settings::AlchemyEffect>> _potionDistr,
				std::vector<std::tuple<int, Settings::AlchemyEffect>> _poisonDistr,
				std::vector<std::tuple<int, Settings::AlchemyEffect>> _fortifyDistr,
				std::vector<std::tuple<int, Settings::AlchemyEffect>> _foodDistr, uint64_t _validPotions, uint64_t _validPoisons,
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
			{ }
			Rule() {}
			Rule(bool invalid) { valid = invalid; }

		private:
			RE::AlchemyItem* GetRandomPotion1(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPotion2(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPotion3(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPotionAdditional(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPotion(int str);
			RE::AlchemyItem* GetRandomPoison1(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPoison2(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPoison3(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPoisonAdditional(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomPoison(int str);
			RE::AlchemyItem* GetRandomFortifyPotion1(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomFortifyPotion2(Settings::ItemStrength strength, Settings::ActorStrength acstrength);
			RE::AlchemyItem* GetRandomFortifyPotion(int str);
			RE::AlchemyItem* GetRandomFood_intern(Settings::ItemStrength strength, Settings::ActorStrength acstrength);

			Settings::AlchemyEffect GetRandomEffect(Settings::ItemType type);
			
		};
		static inline std::vector<Rule*> rules;
		static inline std::unordered_map<RE::FormID, Rule*> npcMap;
		static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> keywordMap;
		static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> factionMap;
		static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> raceMap;
		static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> classMap;
		static inline std::unordered_map<RE::FormID, std::pair<int, Rule*>> combatStyleMap;
		static inline std::unordered_set<RE::FormID> bosses;
		static inline std::unordered_set<RE::FormID> excludedNPCs;
		static inline std::unordered_set<RE::TESFaction*> excludedFactions;
		static inline std::unordered_set<RE::BGSKeyword*> excludedKeywords;
		static inline std::unordered_set<RE::FormID> excludedRaces;
		static inline std::unordered_set<RE::FormID> excludedItems;
		static inline std::unordered_set<RE::FormID> baselineExclusions;

		#define RandomRange 1000

		static std::vector<std::tuple<int, Settings::AlchemyEffect>> GetVector(int i, Settings::AlchemyEffect alch)
		{
			std::vector<std::tuple<int, Settings::AlchemyEffect>> vec;
			vec.push_back({ i, alch });
			return vec;
		}

		#define DefaultRuleName "DefaultRule"

		static inline Rule* defaultRule = nullptr;
		static inline Rule* emptyRule = new Rule(false);

		static Rule* FindRule(std::string name)
		{
			for (Rule* r : rules) {
				if (r->ruleName == name)
					return r;
			}
			return nullptr;
		}

		static std::vector<RE::AlchemyItem*> GetDistrItems(RE::Actor* actor);
		static std::vector<RE::AlchemyItem*> GetDistrPotions(RE::Actor* actor);
		static std::vector<RE::AlchemyItem*> GetDistrPoisons(RE::Actor* actor);
		static std::vector<RE::AlchemyItem*> GetDistrFortifyPotions(RE::Actor* actor);
		static std::vector<RE::AlchemyItem*> GetDistrFood(RE::Actor* actor);

		static std::vector<RE::AlchemyItem*> GetMatchingInventoryItemsUnique(RE::Actor* actor);
		static std::vector<RE::AlchemyItem*> GetMatchingInventoryItems(RE::Actor* actor);

		friend void Settings::CheckActorsForRules();

	private:
		static void CalcStrength(RE::Actor* actor, ActorStrength& acs, ItemStrength& is);
		static Rule* CalcRule(RE::Actor* actor);
		static Rule* CalcRule(RE::TESNPC* npc);
	};

	static void LoadDistrConfig();

	static inline int _MaxDuration = 10000;
	static inline int _MaxFortifyDuration = 60000;

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

	// compatibility
	static inline bool _CompatibilityMode = false;								// Use Items with Papyrus, needs the plugin
	static inline bool _CompatibilityPotionAnimation = false;					// Use Potions with Papyrus
	static inline bool _CompatibilityDisableAutomaticAdjustments = false;		// Disables most automatic adjustments made to settings for compatibility
	static inline bool _CompatibilityPotionAnimatedFx = false;					// no settings entry | Compatiblity Mode for Mods
																				// 1) Animated Potion Drinking SE
																				// 2) Potion Animated fix (SE)
	static inline bool _CompatibilityPotionAnimatedFX_UseAnimations = false;	// if PotionAnimatedfx.esp is loaded, should their animations be used on all potions?

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


	// intern
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

	static inline std::list<RE::AlchemyItem*> alitems{};
	static inline std::list<RE::AlchemyItem*> potions{};
	static inline std::list<RE::AlchemyItem*> food{};
	static inline std::list<RE::AlchemyItem*> poisons{};

	static inline RE::BGSKeyword* VendorItemPotion;
	static inline RE::BGSKeyword* VendorItemFood;
	static inline RE::BGSKeyword* VendorItemFoodRaw;
	static inline RE::BGSKeyword* VendorItemPoison;

	static inline RE::TESFaction* CurrentFollowerFaction;

	static void Load()
	{
		constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

		bool Ultimateoptions = false;

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
		_featUsePoisons = ini.GetValue("Features", "EnablePoisonUsage") ? ini.GetBoolValue("Features", "EnablePoisonUsage") : true;
		logger::info("[SETTINGS] {} {}", "EnablePoisonUsage", std::to_string(_featUsePoisons));
		_featUseFortifyPotions = ini.GetValue("Features", "EnableFortifyPotionUsage") ? ini.GetBoolValue("Features", "EnableFortifyPotionUsage") : true;
		logger::info("[SETTINGS] {} {}", "EnableFortifyPotionUsage", std::to_string(_featUseFortifyPotions));
		_featUseFood = ini.GetValue("Features", "EnableFoodUsage") ? ini.GetBoolValue("Features", "EnableFoodUsage") : true;
		logger::info("[SETTINGS] {} {}", "EnableFoodUsage", std::to_string(_featUseFood));

		_playerRestorationEnabled = ini.GetValue("Features", "EnablePlayerRestoration") ? ini.GetBoolValue("Features", "EnablePlayerRestoration") : false;
		logger::info("[SETTINGS] {} {}", "EnablePlayerRestoration", std::to_string(_playerRestorationEnabled));
		_playerUsePoisons = ini.GetValue("Features", "EnablePlayerPoisonUsage") ? ini.GetBoolValue("Features", "EnablePlayerPoisonUsage") : false;
		logger::info("[SETTINGS] {} {}", "EnablePlayerPoisonUsage", std::to_string(_playerUsePoisons));
		_playerUseFortifyPotions = ini.GetValue("Features", "EnablePlayerFortifyPotionUsage") ? ini.GetBoolValue("Features", "EnablePlayerFortifyPotionUsage") : false;
		logger::info("[SETTINGS] {} {}", "EnablePlayerFortifyPotionUsage", std::to_string(_playerUseFortifyPotions));

		_featDistributePotions = ini.GetValue("Features", "DistributePotions") ? ini.GetBoolValue("Features", "DistributePotions") : true;
		logger::info("[SETTINGS] {} {}", "DistributePotions", std::to_string(_featDistributePotions));
		_featDistributePoisons = ini.GetValue("Features", "DistributePoisons") ? ini.GetBoolValue("Features", "DistributePoisons") : true;
		logger::info("[SETTINGS] {} {}", "DistributePoisons", std::to_string(_featDistributePoisons));
		_featDistributeFood = ini.GetValue("Features", "DistributeFood") ? ini.GetBoolValue("Features", "DistributeFood") : true;
		logger::info("[SETTINGS] {} {}", "DistributeFood", std::to_string(_featDistributeFood));
		_featDistributeFortifyPotions = ini.GetValue("Features", "DistributeFortifyPotions") ? ini.GetBoolValue("Features", "DistributeFortifyPotions") : true;
		logger::info("[SETTINGS] {} {}", "DistributeFortifyPotions", std::to_string(_featDistributeFortifyPotions));

		_featRemoveItemsOnDeath = ini.GetValue("Features", "RemoveItemsOnDeath") ? ini.GetBoolValue("Features", "RemoveItemsOnDeath") : true;
		logger::info("[SETTINGS] {} {}", "RemoveItemsOnDeath", std::to_string(_featRemoveItemsOnDeath));
		
		// compatibility
		_CompatibilityPotionAnimation = ini.GetValue("Compatibility", "UltimatePotionAnimation") ? ini.GetBoolValue("Compatibility", "UltimatePotionAnimation") : false;
		logger::info("[SETTINGS] {} {}", "UltimatePotionAnimation", std::to_string(_CompatibilityPotionAnimation));
		// get wether zxlice's Ultimate Potion Animation is present
		auto constexpr folder = R"(Data\SKSE\Plugins\)";
		for (const auto& entry : std::filesystem::directory_iterator(folder)) {
			if (entry.exists() && !entry.path().empty() && entry.path().filename() == "zxlice's ultimate potion animation.dll") {
				Ultimateoptions = true;
				logger::info("[SETTINGS] zxlice's Ultimate Potion Animation has been detected");
			}
		}
		_CompatibilityMode = ini.GetValue("Compatibility", "Compatibility") ? ini.GetBoolValue("Compatibility", "Compatibility") : false;
		logger::info("[SETTINGS] {} {}", "Compatibility", std::to_string(_CompatibilityMode));
		_CompatibilityDisableAutomaticAdjustments = ini.GetValue("Compatibility", "DisableAutomaticAdjustments") ? ini.GetBoolValue("Compatibility", "DisableAutomaticAdjustments") : false;
		logger::info("[SETTINGS] {} {}", "DisableAutomaticAdjustments", std::to_string(_CompatibilityDisableAutomaticAdjustments));
		_CompatibilityPotionAnimatedFX_UseAnimations = ini.GetValue("Compatibility", "PotionAnimatedfx.esp_UseAnimations") ? ini.GetBoolValue("Compatibility", "PotionAnimatedfx.esp_UseAnimations") : false;
		logger::info("[SETTINGS] {} {}", "PotionAnimatedfx.esp_UseAnimations", std::to_string(_CompatibilityPotionAnimatedFX_UseAnimations));


		// distribution
		_LevelEasy = ini.GetValue("Distribution", "LevelEasy") ? ini.GetLongValue("Distribution", "LevelEasy") : _LevelEasy;
		logger::info("[SETTINGS] {} {}", "LevelEasy", std::to_string(_LevelEasy));
		_LevelNormal = ini.GetValue("Distribution", "LevelNormal") ? ini.GetLongValue("Distribution", "LevelNormal") : _LevelNormal;
		logger::info("[SETTINGS] {} {}", "LevelNormal", std::to_string(_LevelNormal));
		_LevelDifficult = ini.GetValue("Distribution", "LevelDifficult") ? ini.GetLongValue("Distribution", "LevelDifficult") : _LevelDifficult;
		logger::info("[SETTINGS] {} {}", "LevelDifficult", std::to_string(_LevelDifficult));
		_LevelInsane = ini.GetValue("Distribution", "LevelInsane") ? ini.GetLongValue("Distribution", "LevelInsane") : _LevelInsane;
		logger::info("[SETTINGS] {} {}", "LevelInsane", std::to_string(_LevelInsane));
		
		_GameDifficultyScaling = ini.GetValue("Distribution", "GameDifficultyScaling") ? ini.GetBoolValue("Distribution", "GameDifficultyScaling") : false;
		logger::info("[SETTINGS] {} {}", "GameDifficultyScaling", std::to_string(_GameDifficultyScaling));
		
		_MaxMagnitudeWeak = ini.GetValue("Distribution", "MaxMagnitudeWeak") ? ini.GetLongValue("Distribution", "MaxMagnitudeWeak") : _MaxMagnitudeWeak;
		logger::info("[SETTINGS] {} {}", "MaxMagnitudeWeak", std::to_string(_MaxMagnitudeWeak));
		_MaxMagnitudeStandard = ini.GetValue("Distribution", "MaxMagnitudeStandard") ? ini.GetLongValue("Distribution", "MaxMagnitudeStandard") : _MaxMagnitudeStandard;
		logger::info("[SETTINGS] {} {}", "MaxMagnitudeStandard", std::to_string(_MaxMagnitudeStandard));
		_MaxMagnitudePotent = ini.GetValue("Distribution", "MaxMagnitudePotent") ? ini.GetLongValue("Distribution", "MaxMagnitudePotent") : _MaxMagnitudePotent;
		logger::info("[SETTINGS] {} {}", "MaxMagnitudePotent", std::to_string(_MaxMagnitudePotent));


		// Restoration Thresholds
		_healthThreshold = ini.GetValue("Restoration", "HealthThresholdPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdPercent")) : _healthThreshold;
		_healthThreshold = ini.GetValue("Restoration", "HealthThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdLowerPercent")) : _healthThreshold;
		if (_healthThreshold > 0.95f)
			_healthThreshold = 0.95f;
		logger::info("[SETTINGS] {} {}", "HealthThresholdPercent", std::to_string(_healthThreshold));
		_magickaThreshold = ini.GetValue("Restoration", "MagickaThresholdPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdPercent")) : _magickaThreshold;
		_magickaThreshold = ini.GetValue("Restoration", "MagickaThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdLowerPercent")) : _magickaThreshold;
		if (_magickaThreshold > 0.95f)
			_magickaThreshold = 0.95f;
		logger::info("[SETTINGS] {} {}", "MagickaThresholdPercent", std::to_string(_magickaThreshold));
		_staminaThreshold = ini.GetValue("Restoration", "StaminaThresholdPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdPercent")) : _staminaThreshold;
		_staminaThreshold = ini.GetValue("Restoration", "StaminaThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdLowerPercent")) : _staminaThreshold;
		if (_staminaThreshold > 0.95f)
			_staminaThreshold = 0.95f;
		logger::info("[SETTINGS] {} {}", "StaminaThresholdPercent", std::to_string(_staminaThreshold));
		_UsePotionChance = ini.GetValue("Restoration", "UsePotionChance") ? static_cast<int>(ini.GetLongValue("Restoration", "UsePotionChance")) : _UsePotionChance;
		logger::info("[SETTINGS] {} {}", "UsePotionChance", std::to_string(_UsePotionChance));


		// Poisonusage options
		_EnemyLevelScalePlayerLevel = ini.GetValue("Poisons", "EnemyLevelScalePlayerLevel") ? static_cast<float>(ini.GetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel")) : _EnemyLevelScalePlayerLevel;
		logger::info("[SETTINGS] {} {}", "EnemyLevelScalePlayerLevel", std::to_string(_EnemyLevelScalePlayerLevel));
		_EnemyNumberThreshold = ini.GetValue("Poisons", "FightingNPCsNumberThreshold") ? ini.GetLongValue("Poisons", "FightingNPCsNumberThreshold") : _EnemyNumberThreshold;
		logger::info("[SETTINGS] {} {}", "FightingNPCsNumberThreshold", std::to_string(_EnemyNumberThreshold));
		_UsePoisonChance = ini.GetValue("Poisons", "UsePoisonChance") ? static_cast<int>(ini.GetLongValue("Poisons", "UsePoisonChance")) : _UsePoisonChance;
		logger::info("[SETTINGS] {} {}", "UsePoisonChance", std::to_string(_UsePoisonChance));


		// fortify options
		_EnemyLevelScalePlayerLevelFortify = ini.GetValue("Fortify", "EnemyLevelScalePlayerLevelFortify") ? static_cast<float>(ini.GetDoubleValue("Fortify", "EnemyLevelScalePlayerLevelFortify")) : _EnemyLevelScalePlayerLevelFortify;
		logger::info("[SETTINGS] {} {}", "EnemyLevelScalePlayerLevelFortify", std::to_string(_EnemyLevelScalePlayerLevelFortify));
		_EnemyNumberThresholdFortify = ini.GetValue("Fortify", "FightingNPCsNumberThresholdFortify") ? ini.GetLongValue("Fortify", "FightingNPCsNumberThresholdFortify") : _EnemyNumberThresholdFortify;
		logger::info("[SETTINGS] {} {}", "FightingNPCsNumberThresholdFortify", std::to_string(_EnemyNumberThresholdFortify));
		_UseFortifyPotionChance = ini.GetValue("Fortify", "UseFortifyPotionChance") ? static_cast<int>(ini.GetLongValue("Fortify", "UseFortifyPotionChance")) : _UseFortifyPotionChance;
		logger::info("[SETTINGS] {} {}", "UseFortifyPotionChance", std::to_string(_UseFortifyPotionChance));


		// removal options
		_ChanceToRemoveItem = ini.GetValue("Removal", "ChanceToRemoveItem") ? ini.GetLongValue("Removal", "ChanceToRemoveItem") : _ChanceToRemoveItem;
		logger::info("[SETTINGS] {} {}", "ChanceToRemoveItem", std::to_string(_ChanceToRemoveItem));
		_MaxItemsLeft = ini.GetValue("Removal", "MaxItemsLeftAfterRemoval") ? ini.GetLongValue("Removal", "MaxItemsLeftAfterRemoval") : _MaxItemsLeft;
		logger::info("[SETTINGS] {} {}", "MaxItemsLeftAfterRemoval", std::to_string(_MaxItemsLeft));


		// general
		_maxPotionsPerCycle = ini.GetValue("General", "MaxPotionsPerCycle") ? ini.GetLongValue("General", "MaxPotionsPerCycle", 2) : 2;
		logger::info("[SETTINGS] {} {}", "MaxPotionsPerCycle", std::to_string(_maxPotionsPerCycle));
		_cycletime = ini.GetValue("General", "CycleWaitTime") ? ini.GetLongValue("General", "CycleWaitTime", 2) : 500;
		logger::info("[SETTINGS] {} {}", "CycleWaitTime", std::to_string(_cycletime));
		_DisableEquipSounds = ini.GetValue("General", "DisableEquipSounds") ? ini.GetBoolValue("General", "DisableEquipSounds", false) : false;
		logger::info("[SETTINGS] {} {}", "DisableEquipSounds", std::to_string(_DisableEquipSounds));


		// Debugging
		EnableLog = ini.GetValue("Debug", "EnableLogging") ? ini.GetBoolValue("Debug", "EnableLogging") : false;
		logger::info("[SETTINGS] {} {}", "EnableLogging", std::to_string(EnableLog));
		LogLevel = ini.GetValue("Debug", "LogLevel") ? ini.GetLongValue("Debug", "LogLevel") : 0;
		logger::info("[SETTINGS] {} {}", "LogLevel", std::to_string(LogLevel));
		EnableProfiling = ini.GetValue("Debug", "EnableProfiling") ? ini.GetBoolValue("Debug", "EnableProfiling") : false;
		logger::info("[SETTINGS] {} {}", "EnableProfiling", std::to_string(EnableProfiling));
		ProfileLevel = ini.GetValue("Debug", "ProfileLevel") ? ini.GetLongValue("Debug", "ProfileLevel") : 0;
		logger::info("[SETTINGS] {} {}", "ProfileLevel", std::to_string(LogLevel));

		_CheckActorsWithoutRules = ini.GetBoolValue("Debug", "CheckActorWithoutRules", false);
		logger::info("[SETTINGS] {} {}", "CheckActorWithoutRules", std::to_string(_CheckActorsWithoutRules));

		// save user settings, before applying adjustments
		Save();

		// apply settings for ultimate potions
		if (Ultimateoptions) {
			logger::info("[SETTINGS] [OVERRIDE] Adapting Settings for zxlice's Ultimate Potion Animation");
			_CompatibilityPotionAnimation = true;
			logger::info("[SETTINGS] [OVERRIDE] Compatibility - {} hase been overwritten and set to true", "UltimatePotionAnimation");
			if (_cycletime < 2500){
				if (!_CompatibilityDisableAutomaticAdjustments && _playerRestorationEnabled) {
					_cycletime = 2500;
					logger::info(
						"[SETTINGS] [OVERRIDE] General - {} has been set to 2500, to avoid spamming potions while in animation. (get those nasty soudns off off me)",
						"CycleWaitTime");
				} else
					logger::info("[SETTINGS] [OVERRIDE] General - {} has NOT been adjusted, either due to adjustment policy or player features being disabled.", "CycleWaitTime");
			}
			if (_maxPotionsPerCycle > 1) {
				_maxPotionsPerCycle = 1;
				logger::info("[SETTINGS] [OVERRIDE] General - {} has been set to 1, since only one potion can be applied at a time.", "MaxPotionsPerCycle");
			}
		}
		auto datahandler = RE::TESDataHandler::GetSingleton();
		logger::info("[SETTINGS] checking for plugins");


		// search for PotionAnimatedFx.esp for compatibility
		if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ Compatibility::Plugin_PotionAnimatedfx }); plugin) {
			_CompatibilityPotionAnimatedFx = true;
			logger::info("[SETTINGS] Found plugin PotionAnimatedfx.esp and activated compatibility mode");
		} else {
			// if we cannot find the plugin then we need to disable all related compatibility options, otherwise we WILL get CTDs
			if (_CompatibilityPotionAnimatedFX_UseAnimations) {
				_CompatibilityPotionAnimatedFX_UseAnimations = false;
				logger::info("[SETTINGS] [OVERRIDE] \"PotionAnimatedfx.esp_UseAnimations\" was forcelly set to \"false\" due to the corresponding plugin missing.");
			}
		}
		logger::info("[SETTINGS] checking for plugins2");

		// plugin check
		if (_CompatibilityMode) {
			if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ PluginName }); plugin) {
				logger::info("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
			} else {
				logger::info("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, all use of potions, poisons and food is effectively disabled, except you have another sink for the papyrus events. Distribution is not affected");
			}
		} else if (_CompatibilityPotionAnimation) {
			if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ PluginName }); plugin) {
				logger::info("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
			} else {
				logger::info("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, Potion drinking will be effectively disabled, except you have another plugin that listens to the Papyrus Mod Events. Other functionality is not affected");
			}
		} else if (_CompatibilityPotionAnimatedFX_UseAnimations) {
			if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ PluginName }); plugin) {
				logger::info("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
			} else {
				logger::info("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, Potion drinking will be effectively disabled, except you have another plugin that listens to the Papyrus Mod Events. Other functionality is not affected");
			}
		}
		if (_CompatibilityPotionAnimation && _CompatibilityPotionAnimatedFx) {
			_CompatibilityPotionAnimatedFx = false;
			_CompatibilityPotionAnimatedFX_UseAnimations = false;
			logger::info("[SETTINGS] [WARNING] Compatibility modes for zxlice's Ultimate Potion Animation and PotionAnimatedfx.esp have been activated simultaneously. To prevent issues the Compatibility mode for PotionAnimatedfx.esp has been deactivated.");
		}
		logger::info("[SETTINGS] checking for plugins end");

		// change potion sound output model to not always play on the player
		{
			RE::TESForm* SOM_player1st = RE::TESForm::LookupByID(0xb4058);
			RE::TESForm* SOM_verb = RE::TESForm::LookupByID(0xd78b4);
			if (SOM_player1st && SOM_verb) {
				RE::BGSSoundOutput* SOMMono01400_verb = SOM_verb->As<RE::BGSSoundOutput>();
				RE::BGSSoundOutput* SOMMono01400Player1st = SOM_player1st->As<RE::BGSSoundOutput>();
				RE::TESForm* PotionUseF = RE::TESForm::LookupByID(0xB6435);
				RE::BGSSoundDescriptorForm* PotionUse = nullptr;
				if (PotionUseF)
					PotionUse = PotionUseF->As<RE::BGSSoundDescriptorForm>();
				RE::BGSSoundDescriptor* PotionUseSD = PotionUse->soundDescriptor;
				RE::BGSStandardSoundDef* PotionUseOM = (RE::BGSStandardSoundDef*)PotionUseSD;
				if (PotionUseOM->outputModel->GetFormID() == SOMMono01400Player1st->GetFormID()) {
					PotionUseOM->outputModel = SOMMono01400_verb;
					logger::info("[SETTINGS] changed output model for potion drink sound effect");
				}
			}
		}
	}

	static void Save()
	{
		constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

		CSimpleIniA ini;

		ini.SetUnicode();

		// features
		ini.SetBoolValue("Features", "EnableHealthRestoration", _featHealthRestoration, ";NPCs use health potions to restore their missing hp in combat.");
		ini.SetBoolValue("Features", "EnableMagickaRestoration", _featMagickaRestoration, ";NPCs use magicka potions to restore their missing magicka in combat.");
		ini.SetBoolValue("Features", "EnableStaminaRestoration", _featStaminaRestoration, ";NPCs use stamina potions to restore their missing stamina in combat.");
		ini.SetBoolValue("Features", "EnablePoisonUsage", _featUsePoisons, ";NPCs use poisons in combat.\n;Followers will use poisons only on appropiate enemies.\n;Generic NPCs will randomly use their poisons.");
		ini.SetBoolValue("Features", "EnableFortifyPotionUsage", _featUseFortifyPotions, ";NPCs use fortify potions in combat.\n;Potions are used based on the equipped weapons and spells.");
		ini.SetBoolValue("Features", "EnableFoodUsage", _featUseFood, ";Normally one would assume that NPCs eat during the day. This features simulates");
		
		ini.SetBoolValue("Features", "EnablePlayerRestoration", _playerRestorationEnabled, ";All activated restoration features are applied to the player, while they are in Combat.");
		ini.SetBoolValue("Features", "EnablePlayerPoisonUsage", _playerUsePoisons, ";Player will automatically use poisons.");
		ini.SetBoolValue("Features", "EnablePlayerFortifyPotionUsage", _playerUseFortifyPotions, ";Player will use fortify potions the way followers do.");

		ini.SetBoolValue("Features", "DistributePotions", _featDistributePotions, ";NPCs are given potions when they enter combat.");
		ini.SetBoolValue("Features", "DistributePoisons", _featDistributePoisons, ";NPCs are give poisons when they enter combat.");
		ini.SetBoolValue("Features", "DistributeFood", _featDistributeFood, ";NPCs are given food items when they enter combat, and will use them immediately.");
		ini.SetBoolValue("Features", "DistributeFortifyPotions", _featDistributeFortifyPotions, "NPCs are give fortify potions when they enter combat.");
		ini.SetBoolValue("Features", "RemoveItemsOnDeath", _featRemoveItemsOnDeath, ";Remove items from NPCs after they died.");

		// compatibility
		ini.SetBoolValue("Compatibility", "UltimatePotionAnimation", _CompatibilityPotionAnimation, ";Compatibility mode for \"zxlice's ultimate potion animation\". Requires the Skyrim esp plugin. This is automatically enabled if zxlice's mod is detected");
		ini.SetBoolValue("Compatibility", "Compatibility", _CompatibilityMode, ";General Compatibility Mode. If set to true, all items will be equiped using Papyrus workaround. Requires the Skyrim esp plugin.");
		ini.SetBoolValue("Compatibility", "DisableAutomaticAdjustments", _CompatibilityDisableAutomaticAdjustments, ";Disables automatic changes made to settings, due to compatibility.\n;Not all changes can be disabled.\n;1) Changes to \"MaxPotionsPerCycle\" when using Potion Animation Mods.\n;2) Enabling of \"UltimatePotionAnimation\" if zxlice's dll is found in your plugin folder. Since it would very likely result in a crash with this option enabled.");
		if (_CompatibilityPotionAnimatedFX_UseAnimations)
			ini.SetBoolValue("Compatibility", "PotionAnimatedfx.esp_UseAnimations", _CompatibilityPotionAnimatedFX_UseAnimations, ";If you have one of the mods \"Animated Potion Drinking SE\", \"Potion Animated fix (SE)\" and the plugin \"PotionAnimatedfx.eso\" is found you may activate this.\n;This does NOT activate the compatibility mode for that mod, that happens automatically. Instead this determines wether the animations of that mod, are played for any mod that is drunken automatically.");

		// distribution options
		ini.SetLongValue("Distribution", "LevelEasy", _LevelEasy, ";NPC lower or equal this level are considered weak.");
		ini.SetLongValue("Distribution", "LevelNormal", _LevelNormal, ";NPC lower or equal this level are considered normal in terms of strength.");
		ini.SetLongValue("Distribution", "LevelDifficult", _LevelDifficult, ";NPC lower or equal this level are considered difficutl.");
		ini.SetLongValue("Distribution", "LevelInsane", _LevelInsane, ";NPC lower or equal this level are considered insane. Everything above this is always treated as a boss.");

		ini.SetBoolValue("Distribution", "GameDifficultyScaling", _GameDifficultyScaling, ";Disables NPC level scaling, but scales chance according to game difficulty.");

		ini.SetLongValue("Distribution", "MaxMagnitudeWeak", _MaxMagnitudeWeak, ";Items with this or lower magnitude*duration are considered weak.");
		ini.SetLongValue("Distribution", "MaxMagnitudeStandard", _MaxMagnitudeStandard, ";Items with this or lower magnitude*duration are considered normal.");
		ini.SetLongValue("Distribution", "MaxMagnitudePotent", _MaxMagnitudePotent, ";Items with this or lower magnitude*duration are considered potent. Everything above this is considered Insane tier");
		
		// potion options
		ini.SetDoubleValue("Restoration", "HealthThresholdPercent", _healthThreshold, ";Upon reaching this threshold, NPCs will start to use health potions");
		ini.SetDoubleValue("Restoration", "MagickaThresholdPercent", _magickaThreshold, ";Upon reaching this threshold, NPCs will start to use magicka potions");
		ini.SetDoubleValue("Restoration", "StaminaThresholdPercent", _staminaThreshold, ";Upon reaching this threshold, NPCs will start to use stamina potions");
		ini.SetLongValue("Restoration", "UsePotionChance", _UsePotionChance, ";Chance that an NPC will use a potion if they can. Set to 100 to always take a potion, when appropiate.");

		// Poison usage options
		ini.SetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", _EnemyLevelScalePlayerLevel, ";Scaling factor when NPCs start using poisons on enemies.\n;If the enemy they are facing has a level greater equal 'this value' * PlayerLevel followers use poisons.");
		ini.SetLongValue("Poisons", "FightingNPCsNumberThreshold", _EnemyNumberThreshold, ";When the number of NPCs in a fight is at least at this value, followers start to use poisons regardless of the enemies level, to faster help out the player.");
		ini.SetLongValue("Poisons", "UseFortifyPotionChance", _UseFortifyPotionChance, ";Chance that an NPC will use a fortify potion if they can.");

		// fortify options
		ini.SetDoubleValue("Fortify", "EnemyLevelScalePlayerLevelFortify", _EnemyLevelScalePlayerLevelFortify, ";Scaling factor when NPCs start using fortify potions on enemies.\n;If the enemy they are facing has a level greater equal 'this value' * PlayerLevel followers use fortify potions.");
		ini.SetLongValue("Fortify", "FightingNPCsNumberThresholdFortify", _EnemyNumberThresholdFortify, ";When the number of NPCs in a fight is at least at this value, followers start to use fortify potions regardless of the enemies level.");
		ini.SetLongValue("Fortify", "UseFortifyPotionChance", _UsePotionChance, ";Chance that an NPC will use a potion if they can. Set to 100 to always take a potion, when appropiate.");

		// removal options
		ini.SetLongValue("Removal", "ChanceToRemoveItem", _ChanceToRemoveItem, "Chance to remove items on death of NPC. (range: 0 to 100)");
		ini.SetLongValue("Removal", "MaxItemsLeftAfterRemoval", _MaxItemsLeft, "Maximum number of items chances are rolled for during removal. Everything that goes above this value is always removed.");
		
		// general
		ini.SetLongValue("General", "MaxPotionsPerCycle", _maxPotionsPerCycle, ";Maximum number of potions NPCs can use each Period");
		//logger::info("[SETTINGS] writing {} {}", "MaxPotionsPerCycle", std::to_string(_maxPotionsPerCycle));
		ini.SetLongValue("General", "CycleWaitTime", _cycletime, ";Time between two periods in milliseconds.");
		//logger::info("[SETTINGS] writing {} {}", "CycleWaitTime", std::to_string(_cycletime));
		ini.SetLongValue("General", "DisableEquipSounds", _DisableEquipSounds, ";Disable Sounds when equipping Items.");
		//logger::info("[SETTINGS] writing {} {}", "DisableEquipSounds", std::to_string(_DisableEquipSounds));

		// debugging
		ini.SetBoolValue("Debug", "EnableLogging", EnableLog, ";Enables logging output. Use with care as log may get very large");
		ini.SetLongValue("Debug", "LogLevel", LogLevel, ";1 - layer 0 log entries, 2 - layer 1 log entries, 3 - layer 3 log entries, 4 - layer 4 log entries. Affects which functions write log entries, as well as what is written by those functions. ");
		ini.SetBoolValue("Debug", "EnableProfiling", EnableProfiling, ";Enables profiling output.");
		ini.SetLongValue("Debug", "ProfileLevel", ProfileLevel, ";1 - only highest level functions write their executions times to the log, 2 - lower level functions are written, 3 - lowest level functions are written. Be aware that not all functions are supported as Profiling costs execution time.");

		if (_CheckActorsWithoutRules)
			ini.SetBoolValue("Debug", "CheckActorWithoutRules", _CheckActorsWithoutRules);

		ini.SaveFile(path);
	}


	static bool CompatibilityFoodPapyrus()
	{
		return Settings::_CompatibilityMode;
	}
	static bool CompatibilityPoisonPapyrus()
	{
		return Settings::_CompatibilityMode;
	}
	static bool CompatibilityPotionPapyrus()
	{
		return Settings::_CompatibilityMode | Settings::_CompatibilityPotionAnimation | Settings::_CompatibilityPotionAnimatedFX_UseAnimations;
	}
	static void ApplyCompatibilityPotionAnimatedPapyrus(RE::Actor* actor)
	{
		if (Settings::_CompatibilityPotionAnimatedFX_UseAnimations) {
			actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, Compatibility::PAF_NPCDrinkingCoolDownSpell);
			LOG_4("[CompatibilityPotionPlugin] cast potion cooldown spell from PotionAnimatedFx");
		}
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
		//case RE::ActorValue::kSmithing:
		//case RE::ActorValue::kSmithingModifier:
		//case RE::ActorValue::kSmithingPowerModifier:
		//	break;
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
		//case RE::ActorValue::kAlchemy:
		//case RE::ActorValue::kAlchemyModifier:
		//case RE::ActorValue::kAlchemyPowerModifier:
		//	break;
		//case RE::ActorValue::kSpeech:
		//case RE::ActorValue::kSpeechcraftModifier:
		//case RE::ActorValue::kSpeechcraftPowerModifier:
		//	break;
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
		//case RE::ActorValue::kEnchanting:
		//case RE::ActorValue::kEnchantingModifier:
		//case RE::ActorValue::kEnchantingPowerModifier:
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
		int dur[]{
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
					dur[i] = item->effects[i]->effectItem.duration;
					// we only need this for magnitude calculations, so its not used as cooldown
					if (dur[i] == 0)
						dur[i] = 1; 

				}
			}
		} else {
			RE::MagicItem::SkillUsageData err;
			item->GetSkillUsageData(err);
			switch (item->avEffectSetting->data.primaryAV) {
			case RE::ActorValue::kHealth:
				av[0] = item->avEffectSetting->data.primaryAV;
				mag[0] = err.magnitude;
				dur[0] = 1;
				break;
			case RE::ActorValue::kMagicka:
				av[0] = item->avEffectSetting->data.primaryAV;
				mag[0] = err.magnitude;
				dur[0] = 1;
				break;
			case RE::ActorValue::kStamina:
				av[0] = item->avEffectSetting->data.primaryAV;
				mag[0] = err.magnitude;
				dur[0] = 1;
				break;
			}
		}
		// analyze the effect types
		uint64_t alch = static_cast<uint64_t>(AlchemyEffect::kNone);
		ItemStrength str = ItemStrength::kWeak;
		float maxmag = 0;
		for (int i = 0; i < 4; i++) {
			if (dur[i] > 10)
				dur[i] = 10;
			switch (av[i]) {
			case RE::ActorValue::kHealth:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHealth);
				if (mag[i] * dur[i] > maxmag)
					maxmag = mag[i] * dur[i];
				break;
			case RE::ActorValue::kMagicka:
				alch |= static_cast<uint64_t>(AlchemyEffect::kMagicka);
				if (mag[i] * dur[i] > maxmag)
					maxmag = mag[i] * dur[i];
				break;
			case RE::ActorValue::kStamina:
				alch |= static_cast<uint64_t>(AlchemyEffect::kStamina);
				if (mag[i] * dur[i] > maxmag)
					maxmag = mag[i] * dur[i];
				break;
			case RE::ActorValue::kOneHanded:
			case RE::ActorValue::kOneHandedModifier:
			case RE::ActorValue::kOneHandedPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kOneHanded);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kTwoHanded:
			case RE::ActorValue::kTwoHandedModifier:
			case RE::ActorValue::kTwoHandedPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kTwoHanded);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kArchery:
			case RE::ActorValue::kMarksmanModifier:
			case RE::ActorValue::kMarksmanPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kArchery);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kBlock:
			case RE::ActorValue::kBlockModifier:
			case RE::ActorValue::kBlockPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kBlock);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kSmithing:
			//	break;
			case RE::ActorValue::kHeavyArmor:
			case RE::ActorValue::kHeavyArmorModifier:
			case RE::ActorValue::kHeavyArmorPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHeavyArmor);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kLightArmor:
			case RE::ActorValue::kLightArmorModifier:
			case RE::ActorValue::kLightArmorSkillAdvance:
				alch |= static_cast<uint64_t>(AlchemyEffect::kLightArmor);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kPickpocket:
			case RE::ActorValue::kPickpocketModifier:
			case RE::ActorValue::kPickpocketPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kPickpocket);
				break;
			case RE::ActorValue::kLockpicking:
			case RE::ActorValue::kLockpickingModifier:
			case RE::ActorValue::kLockpickingPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kLockpicking);
				break;
			case RE::ActorValue::kSneak:
			case RE::ActorValue::kSneakingModifier:
			case RE::ActorValue::kSneakingPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kSneak);
				break;
			//case RE::ActorValue::kAlchemy:
			//case RE::ActorValue::kAlchemyModifier:
			//case RE::ActorValue::kAlchemyPowerModifier:
			//	break;
			//case RE::ActorValue::kSpeech:
			//case RE::ActorValue::kSpeechcraftModifier:
			//case RE::ActorValue::kSpeechcraftPowerModifier:
			//	break;
			case RE::ActorValue::kAlteration:
			case RE::ActorValue::kAlterationModifier:
			case RE::ActorValue::kAlterationPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kAlteration);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kConjuration:
			case RE::ActorValue::kConjurationModifier:
			case RE::ActorValue::kConjurationPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kConjuration);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kDestruction:
			case RE::ActorValue::kDestructionModifier:
			case RE::ActorValue::kDestructionPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kDestruction);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kIllusion:
			case RE::ActorValue::kIllusionModifier:
			case RE::ActorValue::kIllusionPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kIllusion);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kRestoration:
			case RE::ActorValue::kRestorationModifier:
			case RE::ActorValue::kRestorationPowerModifier:
				alch |= static_cast<uint64_t>(AlchemyEffect::kRestoration);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			//case RE::ActorValue::kEnchanting:
		//case RE::ActorValue::kEnchantingModifier:
		//case RE::ActorValue::kEnchantingPowerModifier:
			//	break;
			case RE::ActorValue::kHealRate:
				alch |= static_cast<uint64_t>(AlchemyEffect::kHealRate);
				if (mag[i] > maxmag)
					maxmag = mag[i];
				break;
			case RE::ActorValue::kMagickaRate:
				alch |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate);
				if (mag[i] > maxmag)
					maxmag = mag[i];
				break;
			case RE::ActorValue::KStaminaRate:
				alch |= static_cast<uint64_t>(AlchemyEffect::kStaminaRate);
				if (mag[i] > maxmag)
					maxmag = mag[i];
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
			case RE::ActorValue::kLeftWeaponSpeedMultiply:
				alch |= static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult);
				//if (mag[i] < maxmag)
				//	maxmag = mag[i];
				break;
			case RE::ActorValue::kBowSpeedBonus:
				alch |= static_cast<uint64_t>(AlchemyEffect::kBowSpeed);
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

		// if the potion is a blood potion it should only ever appear on vampires, no the
		// effects are overriden to AlchemyEffect::kBlood
		if (std::string(item->GetName()).find(std::string("Blood")) != std::string::npos &&
			std::string(item->GetName()).find(std::string("Potion")) != std::string::npos) {
			alch = static_cast<uint64_t>(AlchemyEffect::kBlood);
		}
		if (std::string(item->GetName()).find(std::string("Fear")) != std::string::npos) {
			alch = static_cast<uint64_t>(AlchemyEffect::kFear);
		}
		if (std::string(item->GetName()).find(std::string("Frenzy")) != std::string::npos) {
			alch = static_cast<uint64_t>(AlchemyEffect::kFrenzy);
		}

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
					// unnamed items cannot appear in anyones inventory normally so son't add them to our lists
					if (item->GetName() == nullptr || item->GetName() == (const char*)"" || strlen(item->GetName()) == 0) {
						iter++;
						continue;
					}
					auto clas = ClassifyItem(item);
					// check wether item is excluded
					if (Settings::Distribution::excludedItems.contains(item->GetFormID())) {
						iter++;
						continue;
					}
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
						/*if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
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
						}*/
						switch (std::get<1>(clas)) {
						case ItemStrength::kWeak:
							_poisonsWeak.insert(_poisonsWeak.end(), { std::get<0>(clas), item });
							break;
						case ItemStrength::kStandard:
							_poisonsStandard.insert(_poisonsStandard.end(), { std::get<0>(clas), item });
							break;
						case ItemStrength::kPotent:
							_poisonsPotent.insert(_poisonsPotent.end(), { std::get<0>(clas), item });
							break;
						case ItemStrength::kInsane:
							_poisonsInsane.insert(_poisonsInsane.end(), { std::get<0>(clas), item });
							break;
						}
					} else if (std::get<2>(clas) == ItemType::kPotion) {
						if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kBlood)) > 0)
							_potionsBlood.insert(_potionsBlood.end(), { std::get<0>(clas), item });
						else if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
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
		LOG1_1("{}[ClassifyItems] _potionsWeak_main {}", _potionsWeak_main.size());
		LOG1_1("{}[ClassifyItems] _potionsWeak_rest {}", _potionsWeak_rest.size());
		LOG1_1("{}[ClassifyItems] _potionsStandard_main {}", _potionsStandard_main.size());
		LOG1_1("{}[ClassifyItems] _potionsStandard_rest {}", _potionsStandard_rest.size());
		LOG1_1("{}[ClassifyItems] _potionsPotent_main {}", _potionsPotent_main.size());
		LOG1_1("{}[ClassifyItems] _potionsPotent_rest {}", _potionsPotent_rest.size());
		LOG1_1("{}[ClassifyItems] _potionsInsane_main {}", _potionsInsane_main.size());
		LOG1_1("{}[ClassifyItems] _potionsInsane_rest {}", _potionsInsane_rest.size());
		LOG1_1("{}[ClassifyItems] _potionsBlood {}", _potionsBlood.size());
		LOG1_1("{}[ClassifyItems] _poisonsWeak_main {}", _poisonsWeak_main.size());
		LOG1_1("{}[ClassifyItems] _poisonsWeak_rest {}", _poisonsWeak_rest.size());
		LOG1_1("{}[ClassifyItems] _poisonsStandard_main {}", _poisonsStandard_main.size());
		LOG1_1("{}[ClassifyItems] _poisonsStandard_rest {}", _poisonsStandard_rest.size());
		LOG1_1("{}[ClassifyItems] _poisonsPotent_main {}", _poisonsPotent_main.size());
		LOG1_1("{}[ClassifyItems] _poisonsPotent_rest {}", _poisonsPotent_rest.size());
		LOG1_1("{}[ClassifyItems] _poisonsWeak {}", _poisonsWeak.size());
		LOG1_1("{}[ClassifyItems] _poisonsStandard {}", _poisonsStandard.size());
		LOG1_1("{}[ClassifyItems] _poisonsPotent {}", _poisonsPotent.size());
		LOG1_1("{}[ClassifyItems] _poisonsInsane {}", _poisonsInsane.size());
		LOG1_1("{}[ClassifyItems] _foodmagicka {}", _foodmagicka.size());
		LOG1_1("{}[ClassifyItems] _foodstamina {}", _foodstamina.size());
		LOG1_1("{}[ClassifyItems] _foodhealth {}", _foodhealth.size());
	}

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
