#include"Events.h"
#include "Settings.h"
#include <string.h>
#include<chrono>
#include<thread>
#include <forward_list>
#include <semaphore>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <fstream>
#include <iostream>
#include <limits>
#include <filesystem>
#include <deque>

#include "ActorManipulation.h"
#include "ActorInfo.h"
#include "Game.h"
#include "Distribution.h"
#include "Data.h"
#include "Compatibility.h"
#include "Statistics.h"
		
namespace Events
{
	using AlchemyEffect = AlchemyEffect;

	/// <summary>
	/// random number generator for processing probabilities
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	static std::mt19937 rand((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
	/// <summary>
	/// trims random numbers to 1 to 100
	/// </summary>
	static std::uniform_int_distribution<signed> rand100(1, 100);
	/// <summary>
	/// trims random numbers to 1 to 3
	/// </summary>
	static std::uniform_int_distribution<signed> rand3(1, 3);

#define Look(s) RE::TESForm::LookupByEditorID(s)

#pragma region Data

	static Data* data = nullptr;

	static Compatibility* comp = nullptr;

	/// <summary>
	/// determines whether events and functions are run
	/// </summary>
	static bool initialized = false;

	/// <summary>
	/// enables all active functions
	/// </summary>
	static bool enableProcessing = false;
#define EvalProcessing()   \
	if (!enableProcessing) \
		return;
#define GetProcessing() \
	enableProcessing
#define WaitProcessing() \
	while (!enableProcessing) \
		std::this_thread::sleep_for(100ms);
#define EvalProcessingEvent() \
	if (!enableProcessing)    \
		return EventResult::kContinue;
	/// <summary>
	/// Returns whether processing of actors is allowed
	/// </summary>
	/// <returns></returns>
	bool CanProcess()
	{
		return enableProcessing;
	}

	/// <summary>
	/// Temporarily locks processing for all functions
	/// </summary>
	/// <returns></returns>
	bool LockProcessing()
	{
		bool val = enableProcessing;
		enableProcessing = false;
		return val;
	}
	/// <summary>
	/// Unlocks processing for all functions
	/// </summary>
	void UnlockProcessing()
	{
		enableProcessing = true;
	}

	/// <summary>
	/// holds all active actors
	/// </summary>
	static std::unordered_set<ActorInfo*> acset{};
	/// <summary>
	/// semaphore used to sync access to actor handling, to prevent list changes while operations are done
	/// </summary>
	static std::binary_semaphore sem(1);

	/// <summary>
	/// since the TESDeathEvent seems to be able to fire more than once for an actor we need to track the deaths
	/// </summary>
	static std::unordered_set<RE::FormID> deads;

	/// <summary>
	/// list of npcs that are currently in combat
	/// </summary>
	std::forward_list<ActorInfo*> combatants;

	/// <summary>
	/// signals whether the player has died
	/// </summary>
	static bool playerdied = false;

#define CheckDeadEvent                       \
	LOG1_1("{}[PlayerDead] {}", playerdied); \
	if (playerdied == true) {                \
		return EventResult::kContinue;       \
	}

#define ReEvalPlayerDeath                                         \
	if (RE::PlayerCharacter::GetSingleton()->IsDead() == false) { \
		playerdied = false;                                       \
	}                                                             
	//LOG1_1("{}[ReevalPlayerDeath] {}", playerdied);

	/// <summary>
	/// thread which executes varying test functions
	/// </summary>
	std::thread* testhandler = nullptr;
	std::thread* removeitemshandler = nullptr;

	/// <summary>
	/// if set to true stops the CheckActors thread on its next iteration
	/// </summary>
	static bool stopactorhandler = false;
	static bool skipactorhandler = false;
	/// <summary>
	/// [true] if the actorhandler is running, [false] if the thread died
	/// </summary>
	static bool actorhandlerrunning = false;
	/// <summary>
	/// [true] if the actorhandler is in an active iteration, [false] if it is sleeping
	/// </summary>
	static bool actorhandlerworking = false;
	/// <summary>
	/// thread running the CheckActors function
	/// </summary>
	static std::thread* actorhandler = nullptr;

#define CheckDeadCheckHandlerLoop \
	if (playerdied) {             \
		break;                    \
	}

#pragma endregion

	/// <summary>
	/// initializes important variables, which need to be initialized every time a game is loaded
	/// </summary>
	void InitializeCompatibilityObjects()
	{
		EvalProcessing();
		// now that the game was loaded we can try to initialize all our variables we conuldn't before
		if (!initialized) {
			// if we are in com mode, try to find the needed items. If we cannot find them, deactivate comp mode
			initialized = true;
		}
	}

	/// <summary>
	/// Calculates the cooldowns of an actor for a specific effect
	/// </summary>
	void CalcActorCooldowns(ActorInfo* acinfo, AlchemyEffectBase effect, int dur)
	{
		EvalProcessing();
		if (effect & static_cast<uint64_t>(AlchemyEffect::kHealth)) {
			acinfo->durHealth = dur;
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kMagicka)) {
			acinfo->durMagicka = dur;
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kStamina)) {
			acinfo->durStamina = dur;
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kAnyRegen)) {
			acinfo->durRegeneration = dur;
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kAnyFortify)) {
			acinfo->durFortify = dur;
		}
	}

	/// <summary>
	/// Calculates poison effects based on [combatdata], [target], and [tcombatdata]
	/// </summary>
	/// <param name="combatdata">combatdata of the actor using poison</param>
	/// <param name="target">target</param>
	/// <param name="tcombatdata">combatdata of the target</param>
	/// <returns>valid poison effects</returns>
	uint64_t CalcPoisonEffects(uint32_t combatdata, RE::Actor* target, uint32_t tcombatdata)
	{
		LOG_4("{}[Events] [CalcPoisonEffects]");
		uint64_t effects = 0;
		effects |= static_cast<uint64_t>(AlchemyEffect::kDamageResist) |
		           static_cast<uint64_t>(AlchemyEffect::kResistMagic) |
		           static_cast<uint64_t>(AlchemyEffect::kPoisonResist) |
		           static_cast<uint64_t>(AlchemyEffect::kResistDisease) |
		           static_cast<uint64_t>(AlchemyEffect::kReflectDamage) |
		           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
		           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
		           static_cast<uint64_t>(AlchemyEffect::kHealth);
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kMagicka);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kBlock) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kTwoHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kBlock) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kArchery) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kBowSpeed) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kUnarmedDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina) |
			           static_cast<uint64_t>(AlchemyEffect::kStamina);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualStaff)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka) |
			           static_cast<uint64_t>(AlchemyEffect::kMagicka);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka) |
			           static_cast<uint64_t>(AlchemyEffect::kMagicka);
		}
		// magic related stuff
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicAlteration)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kAlteration);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicConjuration)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kConjuration);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kDestruction);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicIllusion)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kIllusion);
		}
		if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicRestoration)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kRestoration);
		}
		// resistance values based on our expected damage type
		if (combatdata != 0) {
			if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire)) {
				effects |= static_cast<uint64_t>(AlchemyEffect::kResistFire);
			}
			if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost)) {
				effects |= static_cast<uint64_t>(AlchemyEffect::kResistFrost);
			}
			if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock)) {
				effects |= static_cast<uint64_t>(AlchemyEffect::kResistShock);
			}
		}
		// light and heavy armor
		uint32_t armordata = Utility::GetArmorData(target);
		if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::LightArmor))
			effects |= static_cast<uint64_t>(AlchemyEffect::kLightArmor);
		if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::HeavyArmor))
			effects |= static_cast<uint64_t>(AlchemyEffect::kHeavyArmor);

		return effects;
	}

	/// <summary>
	/// Calculates all fortify effects that an actor is equitable for, based on their and their targets combat data
	/// </summary>
	/// <param name="acinfo">actoringo object</param>
	/// <param name="combatdata">combatdata of [acinfo]</param>
	/// <param name="tcombatdata">combatdata of target</param>
	/// <returns></returns>
	uint64_t CalcFortifyEffects(ActorInfo* acinfo, uint32_t combatdata, uint32_t tcombatdata = 0)
	{
		LOG_4("{}[Events] [CalcFortifyEffects]");
		uint64_t effects = 0;
		effects |= static_cast<uint64_t>(AlchemyEffect::kDamageResist) |
		           static_cast<uint64_t>(AlchemyEffect::kResistMagic) |
		           static_cast<uint64_t>(AlchemyEffect::kPoisonResist) |
		           static_cast<uint64_t>(AlchemyEffect::kResistDisease) |
		           static_cast<uint64_t>(AlchemyEffect::kReflectDamage);

		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kBlock) |
			           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kTwoHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kBlock) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kArchery) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kBowSpeed) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kCriticalChance) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kUnarmedDamage) |
			           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
			           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyHealth) |
			           static_cast<uint64_t>(AlchemyEffect::kFortifyStamina);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualStaff)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kFortifyMagicka);
		}
		// magic related stuff
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicAlteration)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kAlteration);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicConjuration)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kConjuration);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kDestruction);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicIllusion)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kIllusion);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicRestoration)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kRestoration);
		}
		// resistance values based on their enemies expected damage type
		if (tcombatdata != 0) {
			if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire)) {
				effects |= static_cast<uint64_t>(AlchemyEffect::kResistFire);
			}
			if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost)) {
				effects |= static_cast<uint64_t>(AlchemyEffect::kResistFrost);
			}
			if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock)) {
				effects |= static_cast<uint64_t>(AlchemyEffect::kResistShock);
			}
		}
		// light and heavy armor
		uint32_t armordata = Utility::GetArmorData(acinfo->actor);
		if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::LightArmor))
			effects |= static_cast<uint64_t>(AlchemyEffect::kLightArmor);
		if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::HeavyArmor))
			effects |= static_cast<uint64_t>(AlchemyEffect::kHeavyArmor);
		return effects;
	}

	/// <summary>
	/// Calculates all regeneration effects that an actor is equitable for, based on their combat data
	/// </summary>
	/// <param name="combatdata">combatdata of the actor</param>
	/// <returns>valid regeneration effects</returns>
	uint64_t CalcRegenEffects(uint32_t combatdata)
	{
		LOG_4("{}[Events] [CalcRegenEffects]");
		uint64_t effects = 0;
		effects |= static_cast<uint64_t>(AlchemyEffect::kHealRate) |
		           static_cast<uint64_t>(AlchemyEffect::kHealRateMult);

		if (combatdata &
			(	static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword))) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
		}
		if (combatdata &
			(	static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand))) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
		}
		if (combatdata &
			(	static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::DualStaff))) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult);
		}
		return effects;
	}
	/// <summary>
	/// Calculates all regeneration effects that an actor is equitable for, based on their combat data
	/// </summary>
	/// <param name="acinfo"></param>
	/// <param name="combatdata"></param>
	/// <returns></returns>
	uint64_t CalcRegenEffects(ActorInfo* /*acinfo*/, uint32_t combatdata) {
		return CalcRegenEffects(combatdata);
	} 

	void DecreaseActorCooldown(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->durHealth >= 0)
			acinfo->durHealth -= Settings::System::_cycletime;
		if (acinfo->durMagicka >= 0)
			acinfo->durMagicka -= Settings::System::_cycletime;
		if (acinfo->durStamina >= 0)
			acinfo->durStamina -= Settings::System::_cycletime;
		if (acinfo->durFortify >= 0)
			acinfo->durFortify -= Settings::System::_cycletime;
		if (acinfo->durRegeneration >= 0)
			acinfo->durRegeneration -= Settings::System::_cycletime;
		if (acinfo->globalCooldownTimer >= 0)
			acinfo->globalCooldownTimer -= Settings::System::_cycletime;
	}

	int tolerance = 0;
	int actorsincombat = 0;
	int hostileactors = 0;

	void HandleActorPotions(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorPotions] {}", Utility::PrintForm(acinfo->actor));
		AlchemyEffectBase alch = 0;
		AlchemyEffectBase alch2 = 0;
		AlchemyEffectBase alch3 = 0;
		if (!acinfo->actor->IsPlayerRef() || Settings::Player::_playerPotions) {
			LOG_2("{}[Events] [CheckActors] [potions]")
			// get combined effect for magicka, health, and stamina
			if (Settings::Potions::_enableHealthRestoration && acinfo->durHealth < tolerance && ACM::GetAVPercentage(acinfo->actor, RE::ActorValue::kHealth) < Settings::Potions::_healthThreshold)
				alch = static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth);
			else
				alch = 0;
			if (Settings::Potions::_enableMagickaRestoration && acinfo->durMagicka < tolerance && ACM::GetAVPercentage(acinfo->actor, RE::ActorValue::kMagicka) < Settings::Potions::_magickaThreshold)
				alch2 = static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka);
			else
				alch2 = 0;
			if (Settings::Potions::_enableStaminaRestoration && acinfo->durStamina < tolerance && ACM::GetAVPercentage(acinfo->actor, RE::ActorValue::kStamina) < Settings::Potions::_staminaThreshold)
				alch3 = static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina);
			else
				alch3 = 0;
			// construct combined effect
			alch |= alch2 | alch3;
			LOG4_4("{}[Events] [CheckActors] check for alchemyeffect {} with current dur health {} dur mag {} dur stam {} ", alch, acinfo->durHealth, acinfo->durMagicka, acinfo->durStamina);
			// use potions
			// do the first round
			if (alch != 0 && (Settings::Potions::_UsePotionChance == 100 || rand100(rand) < Settings::Potions::_UsePotionChance)) {
				auto avmag = acinfo->actor->GetActorValue(RE::ActorValue::kMagicka);
				auto avhealth = acinfo->actor->GetActorValue(RE::ActorValue::kHealth);
				auto avstam = acinfo->actor->GetActorValue(RE::ActorValue::kStamina);
				auto tup = ACM::ActorUsePotion(acinfo, alch, Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation);
				LOG1_2("{}[Events] [CheckActors] found potion has Alchemy Effect {}", static_cast<uint64_t>(std::get<1>(tup)));
				if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
					acinfo->durHealth = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;  // convert to milliseconds
					avhealth += std::get<0>(tup) * std::get<2>(tup);
					LOG2_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {}", acinfo->durHealth, std::get<0>(tup));
				}
				if (static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka) & std::get<1>(tup)) {
					acinfo->durMagicka = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
					avmag += std::get<0>(tup) * std::get<2>(tup);
					LOG2_4("{}[Events] [CheckActors] use magicka pot with duration {} and magnitude {}", acinfo->durMagicka, std::get<0>(tup));
				}
				if (static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina) & std::get<1>(tup)) {
					acinfo->durStamina = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
					avstam += std::get<0>(tup) * std::get<2>(tup);
					LOG2_4("{}[Events] [CheckActors] use stamina pot with duration {} and magnitude {}", acinfo->durStamina, std::get<0>(tup));
				}
				// check if we have a valid duration
				if (std::get<1>(tup) != -1) {
					acinfo->globalCooldownTimer = comp->GetGlobalCooldown();
				}
			}
		}
	}

	void HandleActorFortifyPotions(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorFortifyPotions] {}", Utility::PrintForm(acinfo->actor));
		if (acinfo->globalCooldownTimer <= tolerance &&
			Settings::FortifyPotions::_enableFortifyPotions &&
			(!(acinfo->actor->IsPlayerRef()) || Settings::Player::_playerFortifyPotions)) {
			//LOG_2("{}[Events] [CheckActors] [fortify]");

			if ((acinfo->IsFollower() || acinfo->actor->IsPlayerRef()) && !(Settings::FortifyPotions::_EnemyNumberThresholdFortify < hostileactors || (acinfo->target && acinfo->target->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::FortifyPotions::_EnemyLevelScalePlayerLevelFortify))) {
				return;
			}
			// handle fortify potions
			if ((Settings::FortifyPotions::_UseFortifyPotionChance == 100 || rand100(rand) < Settings::FortifyPotions::_UseFortifyPotionChance)) {
				// general stuff
				uint64_t effects = 0;

				// determine valid regeneration effects
				if (acinfo->durRegeneration < tolerance) {
					effects |= CalcRegenEffects(acinfo, acinfo->combatdata);
				}
				// determine valid fortify effects
				if (acinfo->durFortify < tolerance) {
					effects |= CalcFortifyEffects(acinfo, acinfo->combatdata, acinfo->tcombatdata);
				}

				// std::tuple<int, AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffect>>>
				//loginfo("take fortify with effects: {}", Utility::GetHex(effects));
				LOG1_4("{}[Events] [CheckActors] check for fortify potion with effect {}", Utility::GetHex(effects));
				auto tup = ACM::ActorUsePotion(acinfo, effects, Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify);
				if (std::get<0>(tup) != -1) {
					acinfo->globalCooldownTimer = comp->GetGlobalCooldown();
					AlchemyEffectBase eff = std::get<1>(tup);
					if (eff & Base(AlchemyEffect::kAnyRegen)) {
						CalcActorCooldowns(acinfo, eff, std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000);
						LOG3_4("{}[Events] [CheckActors] used regeneration potion with tracked duration {} {} and effect {}", acinfo->durRegeneration, std::get<0>(tup) * 1000, Utility::ToString(std::get<1>(tup)));
					}
					if (eff & Base(AlchemyEffect::kAnyFortify)) {
						CalcActorCooldowns(acinfo, eff, std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000);
						LOG3_4("{}[Events] [CheckActors] used fortify av potion with tracked duration {} {} and effect {}", acinfo->durFortify, std::get<0>(tup) * 1000, Utility::ToString(std::get<1>(tup)));
					}
				}
			}
		}
	}

	void HandleActorPoisons(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorPoisons] {}", Utility::PrintForm(acinfo->actor));
		if (acinfo->durCombat > 1000 &&
			acinfo->globalCooldownTimer <= tolerance &&
			Settings::Poisons::_enablePoisons &&
			(Settings::Poisons::_UsePoisonChance == 100 || rand100(rand) < Settings::Poisons::_UsePoisonChance) &&
			(!acinfo->actor->IsPlayerRef() || Settings::Player::_playerPoisons)) {
			LOG_2("{}[Events] [CheckActors] [poisons]");
			// handle poisons
			if (acinfo->combatdata != 0 && (acinfo->combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
				(acinfo->combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0 /* &&
									comp->AnPois_FindActorPoison(curr->actor->GetFormID()) == nullptr*/
				&&
				Utility::CanApplyPoison(acinfo->actor)) {
				// handle followers
				// they only use poisons if there are many npcs in the fight, or if the enemies they are targetting
				// have a high enough level, like starting at PlayerLevel*0.8 or so
				if (((acinfo->IsFollower() || acinfo->actor->IsPlayerRef()) &&
						(Settings::Poisons::_EnemyNumberThreshold < hostileactors || (acinfo->target && acinfo->target->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::Poisons::_EnemyLevelScalePlayerLevel))) ||
					acinfo->IsFollower() == false && acinfo->actor->IsPlayerRef() == false) {
					// time to use some poisons
					uint64_t effects = 0;
					// kResistMagic, kResistFire, kResistFrost, kResistMagic should only be used if the follower is a spellblade
					if (acinfo->combatdata & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) ||
						acinfo->combatdata & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword))) {
						effects |= static_cast<uint64_t>(AlchemyEffect::kResistMagic);
						if (acinfo->combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
							if (acinfo->combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire))
								effects |= static_cast<uint64_t>(AlchemyEffect::kResistFire);
							if (acinfo->combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost))
								effects |= static_cast<uint64_t>(AlchemyEffect::kResistFrost);
							if (acinfo->combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock))
								effects |= static_cast<uint64_t>(AlchemyEffect::kResistShock);
						}
					}
					// incorporate enemy specific data, player is recognized here
					if (acinfo->target) {
						// we can make the usage dependent on the target
						if (acinfo->target->GetRace()->HasKeyword(Settings::ActorTypeDwarven) || acinfo->target->GetActorBase()->HasKeyword(Settings::ActorTypeDwarven))
							return;
						effects |= CalcRegenEffects(acinfo->tcombatdata);
						effects |= CalcPoisonEffects(acinfo->combatdata, acinfo->target, acinfo->tcombatdata);
					} else {
						// we dont have a target so just use any poison
						effects |= static_cast<uint64_t>(AlchemyEffect::kAnyPoison);
					}
					LOG1_4("{}[Events] [CheckActors] check for poison with effect {}", effects);
					auto tup = ACM::ActorUsePoison(acinfo, effects);
					if (std::get<1>(tup) != -1)  // check whether an effect was applied
						acinfo->globalCooldownTimer = comp->GetGlobalCooldown();
				}
			}
			if (acinfo->combatdata == 0)
				LOG1_2("{}[Events] [CheckActors] couldn't determine combatdata for npc {}", Utility::PrintForm(acinfo->actor));
			// else Mage or Hand to Hand which cannot use poisons
		}
	}

	void HandleActorFood(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorFood] {}", Utility::PrintForm(acinfo->actor));
		if (acinfo->globalCooldownTimer <= tolerance &&
			Settings::Food::_enableFood &&
			RE::Calendar::GetSingleton()->GetDaysPassed() >= acinfo->nextFoodTime &&
			(!acinfo->actor->IsPlayerRef() || Settings::Player::_playerFood) &&
			(Settings::Food::_RestrictFoodToCombatStart == false || acinfo->durCombat < 2000)) {
			// use food at the beginning of the fight to simulate acinfo npc having eaten
			// calc effects that we want to be applied
			AlchemyEffectBase effects = 0;
			effects |= CalcFortifyEffects(acinfo, acinfo->combatdata, acinfo->tcombatdata);
			effects |= CalcRegenEffects(acinfo, acinfo->combatdata);
			auto [dur, effect] = ACM::ActorUseFood(acinfo, effects, false);
			if (effect == 0) {  // nothing found
				auto tup = ACM::ActorUseFood(acinfo);
				dur = std::get<0>(tup);
				effect = std::get<1>(tup);
			}
			LOG2_1("{}[Events] [CheckActors] current days passed: {}, next food time: {}", std::to_string(RE::Calendar::GetSingleton()->GetDaysPassed()), std::to_string(acinfo->nextFoodTime));
		}
	}

	void HandleActorOOCPotions(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == true || acinfo->handleactor == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorOOCPotions] {}", Utility::PrintForm(acinfo->actor));
		// we are only checking for health here
		if (Settings::Potions::_enableHealthRestoration && acinfo->durHealth < tolerance &&
			ACM::GetAVPercentage(acinfo->actor, RE::ActorValue::kHealth) < Settings::Potions::_healthThreshold) {
			auto tup = ACM::ActorUsePotion(acinfo, static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth), false);
			if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
				acinfo->durHealth = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;  // convert to milliseconds
				// update global cooldown
				acinfo->globalCooldownTimer = comp->GetGlobalCooldown();
				LOG2_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {}", acinfo->durHealth, std::get<0>(tup));
			}
		}
	}

	/// <summary>
	/// Refreshes important runtime data of an ActorInfo, including combatdata and status
	/// </summary>
	/// <param name="acinfo"></param>
	void HandleActorRuntimeData(ActorInfo* acinfo)
	{
		if (!acinfo->IsValid())
			return;
		// if global cooldown greater zero, we can skip everything
		if (acinfo->globalCooldownTimer > tolerance) {
			acinfo->handleactor = false;
			return;
		}
		LOG1_1("{}[Events] [CheckActors] [HandleActorRuntimeData] {}", Utility::PrintForm(acinfo->actor));
		if (acinfo->citems == nullptr)
			acinfo->citems = new ActorInfo::CustomItems();
		// check for staggered option
		// check for paralyzed
		if (comp->DisableItemUsageWhileParalyzed() &&
			(acinfo->actor->boolBits & RE::Actor::BOOL_BITS::kParalyzed ||
				acinfo->actor->IsFlying() ||
				acinfo->actor->IsInKillMove() ||
				acinfo->actor->IsInMidair() ||
				acinfo->actor->IsInRagdollState() ||
				acinfo->actor->IsUnconscious() ||
				acinfo->actor->actorState2.staggered ||
				acinfo->actor->IsBleedingOut())) {
			LOG_1("{}[Events] [CheckActors] [Actor] Actor is unable to use items");
			acinfo->handleactor = false;
			return;
		}
		// check for non-follower option
		if (Settings::Usage::_DisableNonFollowerNPCs && acinfo->IsFollower() == false && acinfo->actor->IsPlayerRef() == false) {
			LOG_1("{}[Events] [CheckActors] [Actor] Actor is not a follower, and non-follower processing has been disabled");
			acinfo->handleactor = false;
			return;
		}

		auto CheckHandle = [](RE::ActorHandle handle) {
			if (handle && handle.get() && handle.get().get())
				return handle.get().get();
			else
				return (RE::Actor*)nullptr;
		};

		// reset target
		acinfo->target = nullptr;
		acinfo->tcombatdata = 0;

		// only try to get combat target, if the actor is actually in combat
		if (acinfo->IsInCombat()) {
			// get combatdata of current actor
			acinfo->combatdata = Utility::GetCombatData(acinfo->actor);
			RE::ActorHandle handle;
			if (acinfo->actor->IsPlayerRef() == false) {
				// retrieve target of current actor if present
				acinfo->target = CheckHandle(acinfo->actor->currentCombatTarget);
				if (acinfo->target) {
					// we can make the usage dependent on the target
					acinfo->tcombatdata = Utility::GetCombatData(acinfo->target);
				}
			} else {
				// try to find out the players combat target, since we cannot get it the normal way

				// if we have access to the True Directional Movement API and target lock is activated
				// try to get the actor from there
				if (Settings::Interfaces::tdm_api != nullptr && Settings::Interfaces::tdm_api->GetTargetLockState() == true) {
					acinfo->target = CheckHandle(Settings::Interfaces::tdm_api->GetCurrentTarget());
				}
				if (acinfo->target == nullptr) {
					// try to infer the target from the npcs that are in combat
					// get the combatant with the shortest range to player, which is hostile to the player
					auto GetClosestEnemy = []() {
						ActorInfo* current = nullptr;
						for (auto aci : combatants) {
							if (current == nullptr || (aci != nullptr && aci->playerHostile && aci->playerDistance < current->playerDistance))
								current = aci;
						}
						return current != nullptr ? current->actor : nullptr;
					};
					acinfo->target = GetClosestEnemy();
				}
			}
		}

		// if actor is valid and not dead
		if (acinfo->actor && !(acinfo->actor->IsDead()) && acinfo->actor->GetActorValue(RE::ActorValue::kHealth) > 0) {
			acinfo->handleactor = true;
		} else
			acinfo->handleactor = false;

		if (acinfo->IsInCombat()) {
			// increase time spent in combat
			acinfo->durCombat += 1000;
		} else {
			// reset time spent in combat
			acinfo->durCombat = 0;
		}

	}

	/// <summary>
	/// Main routine that periodically checks the actors status, and applies items
	/// </summary>
	void CheckActors()
	{
		// wait until processing is allowed, or we should kill ourselves
		while (!GetProcessing() && stopactorhandler == false)
			std::this_thread::sleep_for(10ms);

		LOG_1("{}[Events] [CheckActors]");
		actorhandlerrunning = true;
		/// static section
		RE::UI* ui = RE::UI::GetSingleton();
		// profile
		auto begin = std::chrono::steady_clock::now();
		// tolerance for potion drinking, to diminish effects of computation times
		// on cycle time
		tolerance = Settings::System::_cycletime / 5;

		ActorInfo* playerinfo = data->FindActor(RE::PlayerCharacter::GetSingleton());

		/// temp section
		AlchemyEffectBase alch = 0;
		AlchemyEffectBase alch2 = 0;
		AlchemyEffectBase alch3 = 0;
		bool player = false;  // wether player was inserted into list

		auto datahandler = RE::TESDataHandler::GetSingleton();
		const RE::TESFile* file = nullptr;
		std::string_view name = std::string_view{ "" };

		// main loop, if the thread should be stopped, exit the loop
		while (!stopactorhandler) {
			if (!GetProcessing())
				goto CheckActorsSkipIteration;
			// update active actors
			actorhandlerworking = true;

			ReEvalPlayerDeath;
			// if we are in a paused menu (SoulsRE unpauses menus, which is supported by this)
			// do not compute, since nobody can actually take potions.
			if (!ui->GameIsPaused() && initialized && !playerdied) {
				// reset player var.
				player = false;
				// get starttime of iteration
				begin = std::chrono::steady_clock::now();

				// checking if player should be handled
				if ((Settings::Player::_playerPotions ||
						Settings::Player::_playerFortifyPotions ||
						Settings::Player::_playerPoisons ||
						Settings::Player::_playerFood)) {
					// inject player into the list and remove him later
					acset.insert(playerinfo);
					LOG_3("{}[Events] [CheckActors] Adding player to the list");
					player = true;
				}

				LOG1_1("{}[Events] [CheckActors] Handling {} registered Actors", std::to_string(acset.size()));

				if (!GetProcessing())
					goto CheckActorsSkipIteration;

				

				try {
					// store position of player character
					ActorInfo::SetPlayerPosition(playerinfo->actor->GetPosition());
					// reset player combat state, we don't want to include them in our checks
					playerinfo->combatstate = CombatState::OutOfCombat;

					// validate actorsets
					sem.acquire();
					auto itr = acset.begin();
					while (itr != acset.end()) {
						if ((*itr) == nullptr) {
							LOG_1("{}[Events] [CheckActors] Removed nullptr");
							acset.erase(itr);
						} else if ((*itr)->Update(); (*itr)->IsValid() == false) {
							LOG_1("{}[Events] [CheckActors] Removed invalid actor");
							acset.erase(itr);
						}
						itr++;
					}
					sem.release();

					LOG1_1("{}[Events] [CheckActors] Validated {} Actors", std::to_string(acset.size()));

					if (!GetProcessing())
						goto CheckActorsSkipIteration;
					CheckDeadCheckHandlerLoop;

					// first decrease all cooldowns for all registered actors
					sem.acquire();
					std::for_each(acset.begin(), acset.end(), DecreaseActorCooldown);
					// end decreasing durations

					// calc actors in combat
					// number of actors currently in combat, does not account for multiple combats taking place that are not related to each other
					actorsincombat = 0;
					hostileactors = 0;
					std::for_each(acset.begin(), acset.end(), [](ActorInfo* acinfo) {
						if (acinfo->IsInCombat()) {
							actorsincombat++;
							combatants.push_front(acinfo);
							if (acinfo->playerHostile)
								hostileactors++;
						}
					});
					sem.release();
					
					// the player should always be valid. If they don't the game doesn't work either anyway
					if (playerinfo->actor->IsInCombat())
						playerinfo->combatstate = CombatState::InCombat;
					else
						playerinfo->combatstate = CombatState::OutOfCombat;

					if (!GetProcessing())
						goto CheckActorsSkipIteration;
					CheckDeadCheckHandlerLoop;

					// collect actor runtime data
					sem.acquire();
					std::for_each(acset.begin(), acset.end(), HandleActorRuntimeData);
					sem.release();

					// handle potions
					sem.acquire();
					if (Settings::Usage::_DisableOutOfCombatProcessing == false) {
						std::for_each(acset.begin(), acset.end(), HandleActorOOCPotions);
					}
					std::for_each(acset.begin(), acset.end(), HandleActorPotions);
					sem.release();

					// handle fortify potions
					sem.acquire();
					std::for_each(acset.begin(), acset.end(), HandleActorFortifyPotions);
					sem.release();

					// handle poisons
					sem.acquire();
					std::for_each(acset.begin(), acset.end(), HandleActorPoisons);
					sem.release();

					// handle food
					sem.acquire();
					std::for_each(acset.begin(), acset.end(), HandleActorFood);
					sem.release();
				} catch (std::bad_alloc& e) {
					logcritical("[Events] [CheckActors] Failed to execute due to memory allocation issues: {}", std::string(e.what()));
					sem.release();
				}

				// if we inserted the player, remove them and get their applied values
				if (player) {
					LOG_3("{}[Events] [CheckActors] Removing Player from the list.");
					acset.erase(playerinfo);
					// we do not need to retrieve values, since wthey are written to the reference directly
				}
				// write execution time of iteration
				PROF2_1("{}[Events] [CheckActors] execution time for {} actors: {} µs", acset.size(), std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("{}[Events] [CheckActors] checked {} actors", std::to_string(acset.size()));
				// release lock.
			} else {
				LOG_1("{}[Events] [CheckActors] Skip round.");
			}
CheckActorsSkipIteration:
			// reset combatants
			combatants.clear();
			actorhandlerworking = false;
			// update the set again before sleeping, to account for all stuff that happended while we were busy
			// otherwise we may encounter already deleted actors and such dangerous stuff
			if (!stopactorhandler)
				std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(Settings::System::_cycletime));
		}
		LOG_1("{}[Events] [CheckActors] Exit.");
		stopactorhandler = false;
		actorhandlerrunning = false;
	}

	/// <summary>
	/// Processes the item distribution for an actor
	/// </summary>
	/// <param name="acinfo"></param>
	void ProcessDistribution(ActorInfo* acinfo)
	{
		// check wether this charackter maybe a follower
		if (acinfo->lastDistrTime == 0.0f || RE::Calendar::GetSingleton()->GetDaysPassed() - acinfo->lastDistrTime > 1) {
			if (!Distribution::ExcludedNPC(acinfo)) {
				// begin with compatibility mode removing items before distributing new ones
				if (Settings::Debug::_CompatibilityRemoveItemsBeforeDist) {
					auto items = ACM::GetAllPotions(acinfo);
					auto it = items.begin();
					while (it != items.end()) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", Utility::PrintForm(*it));
						it++;
					}
					items = ACM::GetAllPoisons(acinfo);
					it = items.begin();
					while (it != items.end()) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", Utility::PrintForm(*it));
						it++;
					}
					items = ACM::GetAllFood(acinfo);
					it = items.begin();
					while (it != items.end()) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", Utility::PrintForm(*it));
						it++;
					}
				}

				// if we have characters that should not get items, the function
				// just won't return anything, but we have to check for standard factions like CurrentFollowerFaction
				auto items = Distribution::GetDistrItems(acinfo);
				if (acinfo->actor->IsDead()) {
					return;
				}
				if (items.size() > 0) {
					for (int i = 0; i < items.size(); i++) {
						if (items[i] == nullptr) {
							continue;
						}
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->AddObjectToContainer(items[i], extra, 1, nullptr);
						LOG2_4("{}[Events] [ProcessDistribution] added item {} to actor {}", Utility::PrintForm(items[i]), Utility::PrintForm(acinfo->actor));
					}
					acinfo->lastDistrTime = RE::Calendar::GetSingleton()->GetDaysPassed();
				}
			}
		}
	}

	/// <summary>
	/// Teleports the player to every cell in the game and calculates the distribution rules for all actors
	/// </summary>
	void TestHandler() {
		EvalProcessing();
		std::this_thread::sleep_for(10s);
		RE::UI* ui = RE::UI::GetSingleton();
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrder.csv";
		std::string pathid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrderID.csv";
		std::string pathfail = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellFail.csv";
		std::string pathfailid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellFailID.csv";
		std::unordered_set<uint32_t> done;
		std::unordered_set<std::string> excluded;
		std::unordered_set<uint32_t> excludedid;
		if (std::filesystem::exists(path)) {
			std::ifstream inp(path);
			std::ifstream inpid(pathid);
			std::ifstream dones("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv");
			std::string lineinp;
			std::string lastdone;
			std::string tmp;
			std::string id;
			uint32_t formid = 0;
			while (std::getline(dones, tmp))
				lastdone = tmp;
			size_t pos = lastdone.find(';');
			if (pos != std::string::npos) {
				lastdone = lastdone.substr(0, pos);
			}
			bool flag = false;
			while (std::getline(inp, lineinp) && std::getline(inpid, id)) {
				if (flag == true) {
					break;
				}
				if (lastdone == lineinp && lineinp != "Wilderness")
					flag = true;
				try {
					formid = static_cast<uint32_t>(std::stol(id, nullptr, 16));
					done.insert(formid);
				} catch (std::exception&) {
					//loginfo("tryna fail");
				}
			}
			excluded.insert(lineinp);
			if (std::filesystem::exists(pathfail)) {
				std::ifstream fail(pathfail);
				std::ifstream failid(pathfailid);
				while (std::getline(fail, tmp)) {
					excluded.insert(tmp);
				}
				while (std::getline(failid, tmp)) {
					try {
						formid = static_cast<uint32_t>(std::stol(tmp, nullptr, 16));
						excludedid.insert(formid);
					} catch (std::exception&) {
						//loginfo("tryna fail fail");
					}
				}
				fail.close();
				failid.close();
			}
			inp.close();
			dones.close();
			std::ofstream failout(pathfail);
			std::ofstream failoutid(pathfailid);
			auto it = excluded.begin();
			while (it != excluded.end()) {
				failout << *it << "\n";
				it++;
			}
			auto itr = excludedid.begin();
			while (itr != excludedid.end()) {
				failoutid << Utility::GetHex(*itr) << "\n";
				itr++;
			}
		}
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::ofstream outid = std::ofstream(pathid, std::ofstream::out);
		
		RE::TESObjectCELL * cell = nullptr;
		std::vector<RE::TESObjectCELL*> cs;
		const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
		{
			const RE::BSReadLockGuard locker{ lock };
			auto iter = hashtable->begin();
			while (iter != hashtable->end()) {
				if ((*iter).second) {
					cell = ((*iter).second)->As<RE::TESObjectCELL>();
					if (cell) {
						cs.push_back(cell);
						out << cell->GetFormEditorID() << "\n";
						outid << Utility::GetHex(cell->GetFormID()) << "\n";
					}
				}
				iter++;
			}
		}
		//auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
		//auto iter = hashtable->begin();

		//loginfo("tryna start");
		//while (iter != hashtable->end()) {
		//	if ((*iter).second) {
		//		cell = ((*iter).second)->As<RE::TESObjectCELL>();
		//		if (cell) {
		//			out << cell->GetFormEditorID() << "\n";
		//			outid << Utility::GetHex(cell->GetFormID()) << "\n";
		//		}
		//	}
		//	iter++;
		//}
		out.close();
		outid.close();
		LogConsole("Start Test");
		loginfo("Start Test");
		//iter = hashtable->begin();
		//while (iter != hashtable->end()) {
		//	if ((*iter).second) {
		//		cell = ((*iter).second)->As<RE::TESObjectCELL>();
		for (size_t i = 0; i < cs.size(); i++) {
			EvalProcessing();
			cell = cs[(int)i];
				if (cell) {
					if (excludedid.contains(cell->GetFormID()) || done.contains(cell->GetFormID()) || std::string(cell->GetFormEditorID()) == "Wilderness") {
		//				iter++;
						continue;
					}
					while (ui->GameIsPaused()) {
						std::this_thread::sleep_for(100ms);
					}
					if (cell->references.size() > 0) {
						char buff[70] = "Moving to cell:\t";
						strcat_s(buff, 70, cell->GetFormEditorID());
						LogConsole(buff);
						loginfo("Moving to cell:\t{}", cell->GetFormEditorID());
						RE::PlayerCharacter::GetSingleton()->MoveTo((*(cell->references.begin())).get());
					}
					std::this_thread::sleep_for(7s);
				}
		//	}
		//	iter++;
		}
		LogConsole("Finished Test");
		loginfo("Finished Test");
		//loginfo("tryna end");
	}

	/// <summary>
	/// Calculates the distribution rules for all actors in all cells in the game
	/// </summary>
	void TestAllCells()
	{
		EvalProcessing();
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrder.csv";
		std::string pathid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrderID.csv";
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::ofstream outid = std::ofstream(pathid, std::ofstream::out);
		//loginfo("tryna start");
		auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
		auto iter = hashtable->begin();
		RE::TESObjectCELL* cell = nullptr;
		while (iter != hashtable->end()) {
			if ((*iter).second) {
				cell = ((*iter).second)->As<RE::TESObjectCELL>();
				if (cell) {
					out << cell->GetFormEditorID() << "\n";
					outid << Utility::GetHex(cell->GetFormID()) << "\n";
				}
			}
			iter++;
		}

		std::this_thread::sleep_for(10s);
		RE::UI* ui = RE::UI::GetSingleton();
		iter = hashtable->begin();
		while (iter != hashtable->end()) {
			EvalProcessing();
			if ((*iter).second) {
				cell = ((*iter).second)->As<RE::TESObjectCELL>();
				if (cell) {
					while (ui->GameIsPaused()) {
						std::this_thread::sleep_for(100ms);
					}
					Settings::CheckCellForActors(cell->GetFormID());
				}
			}
			iter++;
		}
	}

	/// <summary>
	/// Removes all distributable alchemy items from all actors in the game on loading a game
	/// </summary>
	void RemoveItemsOnStartup()
	{
		EvalProcessing();
		std::this_thread::sleep_for(5s);

		auto begin = std::chrono::steady_clock::now();

		auto datahandler = RE::TESDataHandler::GetSingleton();
		auto actors = datahandler->GetFormArray<RE::Actor>();
		RE::Actor* actor = nullptr;

		RE::TESObjectCELL* cell = nullptr;
		std::vector<RE::TESObjectCELL*> cs;
		const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
		{
			const RE::BSReadLockGuard locker{ lock };
			auto iter = hashtable->begin();
			while (iter != hashtable->end()) {
				EvalProcessing();
				if ((*iter).second) {
					actor = ((*iter).second)->As<RE::Actor>();
					if (Utility::ValidateActor(actor)) {
						ActorInfo* acinfo = data->FindActor(actor);
						auto items = ACM::GetAllPotions(acinfo);
						auto it = items.begin();
						while (it != items.end()) {
							if (Settings::Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
							LOG1_1("{}[Events] [RemoveItemsOnStartup] Removed item {}", Utility::PrintForm(*it));
							it++;
						}
						items = ACM::GetAllPoisons(acinfo);
						it = items.begin();
						while (it != items.end()) {
							if (Settings::Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
							LOG1_1("{}[Events] [RemoveItemsOnStartup] Removed item {}", Utility::PrintForm(*it));
							it++;
						}
						items = ACM::GetAllFood(acinfo);
						it = items.begin();
						while (it != items.end()) {
							if (Settings::Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
							LOG1_1("{}[Events] [RemoveItemsOnStartup] Removed item {}", Utility::PrintForm(*it));
							it++;
						}
					}
				}
				iter++;
			}
		}
		LogConsole("Finished Thread RemoveItemsOnStartup");

		PROF1_1("{}[Events] [RemoveItemsOnStartup] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
	}

	/// <summary>
	/// Registers an NPC for handling
	/// </summary>
	/// <param name="actor"></param>
	void RegisterNPC(RE::Actor* actor)
	{
		EvalProcessing();
		// exit if the actor is unsafe / not valid
		if (Utility::ValidateActor(actor) == false)
			return;
		LOG1_1("{}[Events] [RegisterNPC] Trying to register new actor for potion tracking: {}", Utility::PrintForm(actor));
		ActorInfo* acinfo = data->FindActor(actor);
		// if actor was deleted, exit
		if (acinfo->GetDeleted()) {
			LOG_1("{}[Events] [RegisterNPC] Actor already deleted");
			return;
		}
		// if actor was invalidated but not deleted, reset them
		if (acinfo->IsValid() == false)
			acinfo->Reset(actor);
		if (acinfo->IsValid() == false) {
			LOG_1("{}[Events] [RegisterNPC] Actor reset failed");
			return;
		}
		// find out whether to insert the actor, if yes insert him into the temp insert list
		sem.acquire();
		if (!acset.contains(acinfo)) {
			acset.insert(acinfo);
		} else {
			sem.release();
			return;
		}
		sem.release();

		ProcessDistribution(acinfo);
		EvalProcessing();
		if (actor->IsDead())
			return;

		LOG_1("{}[Events] [RegisterNPC] finished registering NPC");
	}

	/// <summary>
	/// Unregisters an NPC form handling
	/// </summary>
	/// <param name="actor"></param>
	void UnregisterNPC(RE::Actor* actor)
	{
		EvalProcessing();
		// exit if actor is unsafe / not valid
		if (Utility::ValidateActor(actor) == false)
			return;
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", Utility::PrintForm(actor));
		ActorInfo* acinfo = data->FindActor(actor);
		sem.acquire();
		acset.erase(acinfo);
		acinfo->durHealth = 0;
		acinfo->durMagicka = 0;
		acinfo->durStamina = 0;
		acinfo->durFortify = 0;
		acinfo->durRegeneration = 0;
		sem.release();
		LOG_1("{}[Events] [UnregisterNPC] Unregistered NPC");
	}

	/// <summary>
	/// Unregisters an NPC from handling
	/// </summary>
	/// <param name="acinfo"></param>
	void UnregisterNPC(ActorInfo* acinfo)
	{
		EvalProcessing();
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", acinfo->name);
		sem.acquire();
		acset.erase(acinfo);
		acinfo->durHealth = 0;
		acinfo->durMagicka = 0;
		acinfo->durStamina = 0;
		acinfo->durFortify = 0;
		acinfo->durRegeneration = 0;
		sem.release();
	}

	void UnregisterNPC(RE::FormID formid)
	{
		EvalProcessing();
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", Utility::GetHex(formid));
		sem.acquire();
		auto itr = acset.begin();
		while (itr != acset.end()) {
			if ((*itr)->formid == formid) {
				acset.erase(itr);
				break;
			}
		}
		sem.release();
	}

	/// <summary>
	/// Enables processing and starts handlers
	/// </summary>
	void LoadGameSub()
	{
		auto begin = std::chrono::steady_clock::now();
		LOG_1("{}[Events] [LoadGameSub]");
		// if we canceled the main thread, reset that
		stopactorhandler = false;
		initialized = false;
		if (actorhandlerrunning == false) {
			if (actorhandler != nullptr) {
				// if the thread is there, then destroy and delete it
				// if it is joinable and not running it has already finished, but needs to be joined before
				// it can be destroyed savely
				actorhandler->~thread();
				delete actorhandler;
				actorhandler = nullptr;
			}
			actorhandler = new std::thread(CheckActors);
			actorhandler->detach();
			LOG_1("{}[Events] [LoadGameSub] Started CheckActors");
		}
		// reset the list of actors that died
		deads.clear();
		// reset list of actors in combat
		combatants.clear();
		// set player to alive
		ReEvalPlayerDeath;

		enableProcessing = true;

		if (Settings::Debug::_Test) {
			if (testhandler == nullptr) {
				testhandler = new std::thread(TestAllCells);
				LOG_1("{}[Events] [LoadGameSub] Started TestHandler");
			}
		}

		if (Settings::Debug::_CompatibilityRemoveItemsStartup) {
			if (removeitemshandler == nullptr) {
				removeitemshandler = new std::thread(RemoveItemsOnStartup);
				LOG_1("{}[Events] [LoadGameSub] Started RemoveItemsHandler");
			}
		}

		// when loading the game, the attach detach events for actors aren't fired until cells have been changed
		// thus we need to get all currently loaded npcs manually
		RE::TESObjectCELL* cell = nullptr;
		std::vector<RE::TESObjectCELL*> gamecells;
		const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
		{
			const RE::BSReadLockGuard locker{ lock };
			auto iter = hashtable->begin();
			while (iter != hashtable->end()) {
				if ((*iter).second) {
					cell = ((*iter).second)->As<RE::TESObjectCELL>();
					if (cell) {
						gamecells.push_back(cell);
					}
				}
				iter++;
			}
		}
		LOG1_1("{}[Events] [LoadGameSub] found {} cells", std::to_string(gamecells.size()));
		for (int i = 0; i < (int)gamecells.size(); i++) {
			if (gamecells[i]->IsAttached()) {
				auto itr = gamecells[i]->references.begin();
				while (itr != gamecells[i]->references.end()) {
					if (itr->get()) {
						RE::Actor* actor = itr->get()->As<RE::Actor>();
						if (Utility::ValidateActor(actor) && deads.find(actor->GetFormID()) == deads.end() && !actor->IsDead() && !actor->IsPlayerRef()) {
							if (Distribution::ExcludedNPCFromHandling(actor) == false)
								RegisterNPC(actor);
						}
					}
					itr++;
				}
			}
		}

		InitializeCompatibilityObjects();

		LOG_1("{}[Events] [LoadGameSub] end");
		PROF1_1("{}[Events] [LoadGameSub] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
	}

	/// <summary>
	/// EventHandler for TESDeathEvent
	/// removed unused potions and poisons from actor, to avoid economy instability
	/// only registered if itemremoval is activated in the settings
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		Statistics::Events_TESDeathEvent++;
		EvalProcessingEvent();
		auto begin = std::chrono::steady_clock::now();
		LOG_1("{}[Events] [TESDeathEvent]");
		InitializeCompatibilityObjects();
		RE::Actor* actor = nullptr;
		if (a_event == nullptr || a_event->actorDying == nullptr) {
			LOG_4("{}[Events] [TESDeathEvent] Died due to invalid event");
			goto TESDeathEventEnd;
		}
		actor = a_event->actorDying->As<RE::Actor>();
		if (!Utility::ValidateActor(actor)) {
			LOG_4("{}[Events] [TESDeathEvent] Died due to actor validation fail");
			goto TESDeathEventEnd;
		}
		if (Utility::ValidateActor(actor)) {
			if (actor->IsPlayerRef()) {
				LOG_4("{}[Events] [TESDeathEvent] player died");
				playerdied = true;
			} else {
				// if not already dead, do stuff
				if (deads.contains(actor->GetFormID()) == false) {
					EvalProcessingEvent();
					// invalidate actor
					ActorInfo* acinfo = data->FindActor(actor);
					bool excluded = Distribution::ExcludedNPC(acinfo);
					acinfo->SetInvalid();
					// all npcs must be unregistered, even if distribution oes not apply to them
					UnregisterNPC(actor);
					// as with potion distribution, exlude excluded actors and potential followers
					if (!excluded) {
						// create and insert new event
						deads.insert(actor->GetFormID());
						if (Settings::Removal::_RemoveItemsOnDeath) {
							LOG1_1("{}[Events] [TESDeathEvent] Removing items from actor {}", std::to_string(actor->GetFormID()));
							auto items = Distribution::GetMatchingInventoryItems(acinfo);
							LOG1_1("{}[Events] [TESDeathEvent] found {} items", items.size());
							if (items.size() > 0) {
								// remove items that are too much
								while (items.size() > Settings::Removal::_MaxItemsLeft) {
									RE::ExtraDataList* extra = new RE::ExtraDataList();
									extra->SetOwner(actor);
									actor->RemoveItem(items.back(), 1 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
									LOG1_1("{}[Events] [TESDeathEvent] Removed item {}", Utility::PrintForm(items.back()));
									items.pop_back();
								}
								//loginfo("[Events] [TESDeathEvent] 3");
								// remove the rest of the items per chance
								if (Settings::Removal::_ChanceToRemoveItem < 100) {
									for (int i = (int)items.size() - 1; i >= 0; i--) {
										if (rand100(rand) <= Settings::Removal::_ChanceToRemoveItem) {
											RE::ExtraDataList* extra = new RE::ExtraDataList();
											extra->SetOwner(actor);
											actor->RemoveItem(items[i], 100 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
											LOG1_1("{}[Events] [TESDeathEvent] Removed item {}", Utility::PrintForm(items[i]));
										} else {
											LOG1_1("{}[Events] [TESDeathEvent] Did not remove item {}", Utility::PrintForm(items[i]));
										}
									}
								}
							}
						}

					} else {
						LOG1_4("{}[Events] [TESDeathEvent] actor {} is excluded or already dead", Utility::PrintForm(actor));
					}
					// distribute death items, independently of whether the npc is excluded
					auto ditems = acinfo->FilterCustomConditionsDistrItems(acinfo->citems->death);
					// item, chance, num, cond1, cond2
					for (int i = 0; i < ditems.size(); i++) {
						// calc chances
						if (rand100(rand) <= ditems[i]->chance) {
							// distr item
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->AddObjectToContainer(ditems[i]->object, extra, ditems[i]->num, nullptr);
						}
					}
					// delete actor from data
					data->DeleteActor(actor->GetFormID());
					comp->AnPois_RemoveActorPoison(actor->GetFormID());
					comp->AnPoti_RemoveActorPotion(actor->GetFormID());
					comp->AnPoti_RemoveActorPoison(actor->GetFormID());
				}
			}
		}
	TESDeathEventEnd:
		PROF1_1("{}[Events] [TESDeathEvent] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
		return EventResult::kContinue;
	}

	/// <summary>
	/// Processes TESHitEvents
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		Statistics::Events_TESHitEvent++;
		EvalProcessingEvent();
		
		if (a_event && a_event->target.get()) {
			RE::Actor* actor = a_event->target.get()->As<RE::Actor>();
			if (actor) {
				// check whether the actor is queued for poison application
				if (comp->LoadedAnimatedPoisons() && comp->AnPois_FindActorPoison(actor->GetFormID()) != nullptr) {
					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPoisonsHitEvent");
					ev->strArg = RE::BSFixedString();
					ev->numArg = 0.0f;
					ev->sender = actor;
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
				}
				if (comp->LoadedAnimatedPotions() && comp->AnPoti_FindActorPotion(actor->GetFormID()) != nullptr) {
					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPotionsHitEvent");
					ev->strArg = RE::BSFixedString();
					ev->numArg = 0.0f;
					ev->sender = actor;
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
				}
			}
		}

		return EventResult::kContinue;
	}

	/// <summary>
	/// handles TESCombatEvent
	/// registers the actor for tracking and handles giving them potions, poisons and food, beforehand.
	/// also makes then eat food before the fight.
	/// </summary>
	/// <param name="a_event">event parameters like the actor we need to handle</param>
	/// <param name=""></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
	{
		Statistics::Events_TESCombatEvent++;
		EvalProcessingEvent();
		//if (!Settings::_featDisableOutOfCombatProcessing)
		//	return EventResult::kContinue;
		auto begin = std::chrono::steady_clock::now();
		LOG_1("{}[Events] [TESCombatEvent]");
		InitializeCompatibilityObjects();
		auto actor = a_event->actor->As<RE::Actor>();
		if (Utility::ValidateActor(actor) && !actor->IsDead() && actor != RE::PlayerCharacter::GetSingleton() && actor->IsChild() == false) {
			// register / unregister
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				// register for tracking
				if (Distribution::ExcludedNPCFromHandling(actor) == false)
					RegisterNPC(actor);
			} else {
				if (Settings::Usage::_DisableOutOfCombatProcessing)
					UnregisterNPC(actor);
			}

			// save combat state of npc
			ActorInfo* acinfo = data->FindActor(actor);
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat)
				acinfo->combatstate = CombatState::InCombat;
			else if (a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching)
				acinfo->combatstate = CombatState::Searching;
			else if (a_event->newState == RE::ACTOR_COMBAT_STATE::kNone)
				acinfo->combatstate = CombatState::OutOfCombat;

		}
		PROF1_2("{}[Events] [TESCombatEvent] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
		return EventResult::kContinue;
	}

	/// <summary>
	/// EventHandler for Actors being attached / detached
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESCellAttachDetachEvent* a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>*)
	{
		Statistics::Events_TESCellAttachDetachEvent;
		EvalProcessingEvent();
		// return if feature disabled
		if (Settings::Usage::_DisableOutOfCombatProcessing)
			return EventResult::kContinue;
		ReEvalPlayerDeath;
		//auto begin = std::chrono::steady_clock::now();

		if (a_event && a_event->reference) {
			RE::Actor* actor = a_event->reference->As<RE::Actor>();
			if (Utility::ValidateActor(actor) && deads.find(actor->GetFormID()) == deads.end() && !actor->IsDead() && !actor->IsPlayerRef()) {
				if (a_event->attached) {
					if (Distribution::ExcludedNPCFromHandling(actor) == false)
						RegisterNPC(actor);
				} else {
					UnregisterNPC(actor);
				}
			}
			//PROF1_2("{}[Events] [CellAttachDetachEvent] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
		}
		return EventResult::kContinue;
	}

	std::unordered_set<RE::FormID> cells;
	/// <summary>
	/// EventHandler to fix not playing potion, poison, food sound
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*)
	{
		Statistics::Events_BGSActorCellEvent++;
		EvalProcessingEvent();
		//LOG_1("{}[Events] [BGSActorCellEvent]");
		if (cells.contains(a_event->cellID) == false) {
			cells.insert(a_event->cellID);
			Settings::CheckCellForActors(a_event->cellID);
		}
		return EventResult::kContinue;
	}

	/// <summary>
	/// EventHandler for Debug purposes. It calculates the distribution rules for all npcs in the cell
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
		Statistics::Events_TESEquipEvent++;
		EvalProcessingEvent();
		if (a_event->actor.get()) {
			if (a_event->actor->IsPlayerRef()) {
				auto audiomanager = RE::BSAudioManager::GetSingleton();

				RE::AlchemyItem* obj = RE::TESForm::LookupByID<RE::AlchemyItem>(a_event->baseObject);
				if (obj) {
					if (obj->data.consumptionSound) {
						RE::BSSoundHandle handle;
						audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
						handle.SetObjectToFollow(a_event->actor->Get3D());
						handle.SetVolume(1.0);
						handle.Play();
					}
				}
			}
		}

		return EventResult::kContinue;
	}

	/// <summary>
	/// Handles an item being removed from a container
	/// </summary>
	/// <param name="container">The container the item was removed from</param>
	/// <param name="baseObj">The base object that has been removed</param>
	/// <param name="count">The number of items that have been removed</param>
	/// <param name="destinationContainer">The container the items have been moved to</param>
	/// <param name="a_event">The event information</param>
	void OnItemRemoved(RE::TESObjectREFR* container, RE::TESBoundObject* baseObj, int /*count*/, RE::TESObjectREFR* /*destinationContainer*/, const RE::TESContainerChangedEvent* /*a_event*/)
	{
		LOG2_1("{}[Events] [OnItemRemovedEvent] {} removed from {}", Utility::PrintForm(baseObj), Utility::PrintForm(container));
		RE::Actor* actor = container->As<RE::Actor>();
		if (actor) {
			// handle event for an actor
			ActorInfo* acinfo = data->FindActor(actor);
			if (acinfo) {
				/* if (comp->LoadedAnimatedPoisons()) {
					// handle removed poison
					RE::AlchemyItem* alch = baseObj->As<RE::AlchemyItem>();
					if (alch && alch->IsPoison()) {
						LOG_1("{}[Events] [OnItemRemovedEvent] AnimatedPoison animation");

						//ACM::AnimatedPoison_ApplyPoison(acinfo, alch);

						//std::string AnimationEventString = "poisondamagehealth02";
						//acinfo->actor->NotifyAnimationGraph(AnimationEventString);
						
						//RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> point(nullptr);
						//a_vm->DispatchStaticCall("Debug", "SendAnimationEvent", SKSE::Impl::VMArg(actor, RE::BSFixedString("poisondamagehealth02")), point);
						//RE::MakeFunctionArguments(actor, RE::BSFixedString("poisondamagehealth02"));
					}
				}
				*/

			}
		}
		
		// handle event for generic reference
		
		return;
	}

	/// <summary>
	/// Handles an item being added to a container
	/// </summary>
	/// <param name="container">The container the item is added to</param>
	/// <param name="baseObj">The base object that has been added</param>
	/// <param name="count">The number of items added</param>
	/// <param name="sourceContainer">The container the item was in before</param>
	/// <param name="a_event">The event information</param>
	void OnItemAdded(RE::TESObjectREFR* container, RE::TESBoundObject* baseObj, int /*count*/, RE::TESObjectREFR* /*sourceContainer*/, const RE::TESContainerChangedEvent* /*a_event*/)
	{
		LOG2_1("{}[Events] [OnItemAddedEvent] {} added to {}", Utility::PrintForm(baseObj), Utility::PrintForm(container));
		RE::Actor* actor = container->As<RE::Actor>();
		if (actor) {
			// handle event for an actor
			ActorInfo* acinfo = data->FindActor(actor);
			if (acinfo) {





			}
		}
		
		// handle event for generic objects
		return;
	}

	EventResult EventHandler::ProcessEvent(const RE::TESContainerChangedEvent* a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>* /*a_eventSource*/)
	{
		// this event handles all object transfers between containers in the game
		// this can be deived into multiple base events: OnItemRemoved and OnItemAdded
		Statistics::Events_TESContainerChangedEvent++;
		EvalProcessingEvent();

		if (a_event && a_event->baseObj != 0 && a_event->itemCount != 0) {
			RE::TESObjectREFR* oldCont = RE::TESForm::LookupByID<RE::TESObjectREFR>(a_event->oldContainer);
			RE::TESObjectREFR* newCont = RE::TESForm::LookupByID<RE::TESObjectREFR>(a_event->newContainer);
			RE::TESBoundObject* baseObj = RE::TESForm::LookupByID<RE::TESBoundObject>(a_event->baseObj);
			if (baseObj && oldCont) {
				OnItemRemoved(oldCont, baseObj, a_event->itemCount, newCont, a_event);
			}
			if (baseObj && newCont) {
				OnItemAdded(newCont, baseObj, a_event->itemCount, oldCont, a_event);
			}
		}

		return EventResult::kContinue;
	}
	
	/// <summary>
	/// EventHandler for catching deleted forms / actors
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESFormDeleteEvent* a_event, RE::BSTEventSource<RE::TESFormDeleteEvent>*)
	{
		// very important event. Allows to catch actors and other stuff that gets deleted, without dying, which could cause CTDs otherwise
		Statistics::Events_TESFormDeleteEvent++;
		if (a_event && a_event->formID != 0) {
			data->DeleteActor(a_event->formID);
			UnregisterNPC(a_event->formID);
			data->DeleteFormCustom(a_event->formID);
			comp->AnPois_DeleteActorPoison(a_event->formID);
			comp->AnPoti_DeleteActorPotion(a_event->formID);
			comp->AnPoti_DeleteActorPoison(a_event->formID);
		}
		return EventResult::kContinue;
	}

	/// <summary>
	/// Callback on saving the game
	/// </summary>
	/// <param name=""></param>
	void SaveGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [SaveGameCallback]");

	}

	/// <summary>
	/// Callback on loading a save game, initializes actor processing
	/// </summary>
	/// <param name=""></param>
	void LoadGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [LoadGameCallback]");
		LoadGameSub();
	}

	/// <summary>
	/// Callback on reverting the game. Disables processing and stops all handlers
	/// </summary>
	/// <param name=""></param>
	void RevertGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [RevertGameCallback]");
		enableProcessing = false;
		stopactorhandler = true;
		std::this_thread::sleep_for(10ms);
		if (actorhandler != nullptr)
			actorhandler->~thread();
		LOG1_1("{}[PlayerDead] {}", playerdied);
		// reset actor processing list
		acset.clear();
	}

    /// <summary>
    /// returns singleton to the EventHandler
    /// </summary>
    /// <returns></returns>
    EventHandler* EventHandler::GetSingleton()
    {
        static EventHandler singleton;
        return std::addressof(singleton);
    }


    /// <summary>
    /// Registers us for all Events we want to receive
    /// </summary>
	void EventHandler::Register()
	{
		auto scriptEventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		scriptEventSourceHolder->GetEventSource<RE::TESHitEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESHitEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESCombatEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESCombatEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESEquipEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESEquipEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESDeathEvent).name());
		if (Settings::Debug::_CalculateCellRules) {
			RE::PlayerCharacter::GetSingleton()->GetEventSource<RE::BGSActorCellEvent>()->AddEventSink(EventHandler::GetSingleton());
			LOG1_1("{}Registered {}", typeid(RE::BGSActorCellEvent).name());
		}
		scriptEventSourceHolder->GetEventSource<RE::TESCellAttachDetachEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESCellAttachDetachEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESFormDeleteEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESFormDeleteEvent).name())
		scriptEventSourceHolder->GetEventSource<RE::TESContainerChangedEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESContainerChangedEvent).name())
		Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000001, LoadGameCallback);
		LOG1_1("{}Registered {}", typeid(LoadGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000002, RevertGameCallback);
		LOG1_1("{}Registered {}", typeid(RevertGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000003, SaveGameCallback);
		LOG1_1("{}Registered {}", typeid(SaveGameCallback).name());
		data = Data::GetSingleton();
		comp = Compatibility::GetSingleton();
	}

	/// <summary>
	/// Registers all EventHandlers, if we would have multiple
	/// </summary>
	void RegisterAllEventHandlers()
	{
		EventHandler::Register();
		LOG_1("{}Registered all event handlers"sv);
	}

	/// <summary>
	/// sets the main threads to stop on the next iteration
	/// </summary>
	void DisableThreads()
	{
		stopactorhandler = true;
		//killEventHandler = true;
	}
}
