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
#define EvalProcessingEvent() \
	if (!enableProcessing)    \
		return EventResult::kContinue;
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
	/// contains actors to insert into the active list
	/// </summary>
	static std::unordered_set<RE::Actor*> acinsert{};
	/// <summary>
	/// conatins actors to remove from the active list
	/// </summary>
	static std::unordered_set<RE::Actor*> acremove{};
	/// <summary>
	/// semaphore used to sync access to actor handling, to prevent list changes while operations are done
	/// </summary>
	static std::binary_semaphore sem(1);

	/// <summary>
	/// since the TESDeathEvent seems to be able to fire more than once for an actor we need to track the deaths
	/// </summary>
	static std::unordered_set<RE::FormID> deads;

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
	/// thread running the CheckActors function
	/// </summary>
	static std::thread* actorhandler = nullptr;

#define CheckDeadCheckHandlerLoop \
	if (playerdied) {             \
		break;                    \
	}

#pragma endregion

	/// <summary>
	/// initializes importent variables, which need to be initialized every time a game is loaded
	/// </summary>
	void InitializeCompatibilityObjects()
	{
		EvalProcessing();
		// now that the game was loaded we can try to initialize all our variables we conuldn't before
		if (!initialized) {
			// if we are in com mode, try to find the needed items. If we cannot find them, deactivate comp mode
			if (Settings::_CompatibilityPotionAnimatedFx) {
				RE::TESForm* tmp = RE::TESForm::LookupByEditorID(std::string_view{ Settings::Compatibility::PAF_NPCDrinkingCoolDownEffect_name });
				if (tmp)
					Settings::Compatibility::PAF_NPCDrinkingCoolDownEffect = tmp->As<RE::EffectSetting>();
				tmp = RE::TESForm::LookupByEditorID(std::string_view{ Settings::Compatibility::PAF_NPCDrinkingCoolDownSpell_name });
				if (tmp)
					Settings::Compatibility::PAF_NPCDrinkingCoolDownSpell = tmp->As<RE::SpellItem>();
				if (!(Settings::Compatibility::PAF_NPCDrinkingCoolDownEffect && Settings::Compatibility::PAF_NPCDrinkingCoolDownSpell)) {
					Settings::_CompatibilityPotionAnimatedFx = false;
					loginfo("[Events] [INIT] Some Forms from PotionAnimatedfx.esp seem to be missing. Forcefully deactivated compatibility mode");
				}
			}
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
	/// Calculated all regeneration effects that an actor is equitable for, based on their combat data
	/// </summary>
	/// <param name="acinfo"></param>
	/// <param name="combatdata"></param>
	/// <returns></returns>
	uint64_t CalcRegenEffects(ActorInfo* /*acinfo*/, uint32_t combatdata)
	{
		LOG_4("{}[Events] [CalcRegenEffects]");
		uint64_t effects = 0;
		effects |= static_cast<uint64_t>(AlchemyEffect::kHealRate) |
		           static_cast<uint64_t>(AlchemyEffect::kHealRateMult);

		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
		}
		if (combatdata &
			(static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand))) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
		}
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult);
		}
		return effects;
	}

	void UpdateAcSet()
	{
		logwarn("[UpdateAcSet] begin");
		sem.acquire();
		ActorInfo* acinfo = nullptr;
		logwarn("[UpdateAcSet] remove {}", acremove.size());
		auto itr = acremove.begin();
		while (itr != acremove.end() && *itr != nullptr) {
			logwarn("[UpdateAcSet] remove actor {}", Utility::GetHex((*itr)->GetFormID()));
			acinfo = data->FindActor(*itr);
			acset.erase(acinfo);
			acinfo->durHealth = 0;
			acinfo->durMagicka = 0;
			acinfo->durStamina = 0;
			acinfo->durFortify = 0;
			acinfo->durRegeneration = 0;
			itr++;
		}
		acremove.clear();
		logwarn("[UpdateAcSet] insert {}", acinsert.size());
		auto itra = acinsert.begin();
		while (itra != acinsert.end() && *itra != nullptr) {
			logwarn("[UpdateAcSet] insert actor {}", Utility::GetHex((*itra)->GetFormID()));
			acinfo = data->FindActor(*itra);
			acset.insert(acinfo);
			itra++;
		}
		acinsert.clear();
		sem.release();
		logwarn("[UpdateAcSet] end");
	}

	/// <summary>
	/// Main routine that periodically checks the actors status, and applies items
	/// </summary>
	void CheckActors()
	{
		EvalProcessing();
		LOG_1("{}[Events] [CheckActors]");
		actorhandlerrunning = true;
		/// static section
		RE::UI* ui = RE::UI::GetSingleton();
		// profile
		auto begin = std::chrono::steady_clock::now();
		// tolerance for potion drinking, to diminish effects of computation times
		// on cycle time
		int tolerance = Settings::_cycletime / 5;

		/// player vars
		/* int durhp = 0;  // duration health player
		int durmp = 0; // duration magicka player
		int dursp = 0; // duration stamina player
		// to get this to 0 you would need to play nearly 600 hours
		int durotherp = 0;  //INT_MAX; // duration of buff potions for the player
		int durregp = 0;    //INT_MAX; // duration of reg potions for the player*/

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
			EvalProcessing();
			// update active actors
			UpdateAcSet();
			// current actor
			ActorInfo* curr;
			ReEvalPlayerDeath;
			// if we are in a paused menu (SoulsRE unpauses menus, which is supported by this)
			// do not compute, since nobody can actually take potions.
			if (!ui->GameIsPaused() && initialized && !playerdied) {
				// reset player var.
				player = false;
				// get starttime of iteration
				begin = std::chrono::steady_clock::now();

				// checking if player should be handled
				if ((Settings::_playerRestorationEnabled || Settings::_playerUseFortifyPotions || Settings::_playerUsePoisons) /* && RE::PlayerCharacter::GetSingleton()->IsInCombat() */) {
					// inject player into the list and remove him later
					acset.insert(playerinfo);
					LOG_3("{}[Events] [CheckActors] Adding player to the list");
					player = true;
				}
				LOG1_1("{}[Events] [CheckActors] Handling all registered Actors {}", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count()));
				// handle all registered actors
				// the list does not change while doing this
				auto iterset = acset.begin();
				// calc actors in combat
				// number of actors currently in combat, does not account for multiple combats taking place that are not related to each other
				int actorsincombat = 0;
				while (iterset != acset.end()) {
					curr = *iterset;
					if (curr == nullptr || curr->actor == nullptr || curr->actor->GetFormID() == 0) {
						acset.erase(iterset);
						iterset--;
						continue;
					}
					if (curr->actor->IsInCombat())
						actorsincombat++;
					iterset++;
				}
				iterset = acset.begin();
				while (iterset != acset.end()) {
					EvalProcessing();
					CheckDeadCheckHandlerLoop;
					curr = *iterset;
					if (curr == nullptr || curr->actor == nullptr || curr->actor->GetFormID() == 0) {
						acset.erase(iterset);
						iterset--;
						continue;
					}
					iterset++;
					if (curr->citems == nullptr)
						curr->citems = new ActorInfo::CustomItems();
					// check for staggered option
					if (Settings::_featDisableItemUsageWhileStaggered && curr->actor->actorState2.staggered) {
						LOG_1("{}[Events] [CheckActors] [Actor] Actor is staggered, abort round");
						continue;
					}
					// check for non-follower option
					if (Settings::_featDisableNonFollowerNPCs && curr->IsFollower() == false && curr->actor->IsPlayerRef() == false) {
						LOG_1("{}[Events] [CheckActors] [Actor] Actor is not a follower, and non-follower processing has been disabled");
						continue;
					}
					if (Settings::EnableLog) {
						name = std::string_view{ "" };
						if ((curr->actor->GetFormID() >> 24) != 0xFE) {
							file = datahandler->LookupLoadedModByIndex((uint8_t)(curr->actor->GetFormID() >> 24));
							if (file) {
								name = file->GetFilename();
							}
						}
						if (name.empty()) {
							file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((curr->actor->GetFormID() & 0x00FFF000)) >> 12));
							if (file) {
								name = file->GetFilename();
							}
						}
						LOG3_1("{}[Events] [CheckActors] [Actor] {} named {} from {}", Utility::GetHex((curr->actor)->GetFormID()), curr->actor->GetName(), name);
					}
					// if actor is valid and not dead
					if (curr->actor && !(curr->actor->IsDead()) && curr->actor->GetActorValue(RE::ActorValue::kHealth) > 0) {
						// update durations
						curr->durHealth -= Settings::_cycletime;
						curr->durMagicka -= Settings::_cycletime;
						curr->durStamina -= Settings::_cycletime;
						curr->durFortify -= Settings::_cycletime;
						curr->durRegeneration -= Settings::_cycletime;
						LOG5_1("{}[Events] [CheckActors] [Actor] cooldown: {} {} {} {} {}", curr->durHealth, curr->durMagicka, curr->durStamina, curr->durFortify, curr->durRegeneration);

						// check for out-of-combat option
						if (curr->actor->IsInCombat() == false) {
							if (Settings::_featDisableOutOfCombatProcessing == false) {
								// option deactivated -> do someting, otherwise don't do anything
								// we are only checking for health here
								if (Settings::_featHealthRestoration && curr->durHealth < tolerance && 
									ACM::GetAVPercentage(curr->actor, RE::ActorValue::kHealth) < Settings::_healthThreshold) {
									auto tup = ACM::ActorUsePotion(curr, static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth), false);
									if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
										curr->durHealth = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;  // convert to milliseconds
										LOG2_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {}", curr->durHealth, std::get<0>(tup));
									}
								}
							}
						} else {
							// handle potions

							// potions used this cycle
							int counter = 0;

							if (!curr->actor->IsPlayerRef() || Settings::_playerRestorationEnabled) {
								LOG_2("{}[Events] [CheckActors] [potions]")
								// get combined effect for magicka, health, and stamina
								if (Settings::_featHealthRestoration && curr->durHealth < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kHealth) < Settings::_healthThreshold)
									alch = static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth);
								else
									alch = 0;
								if (Settings::_featMagickaRestoration && curr->durMagicka < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kMagicka) < Settings::_magickaThreshold)
									alch2 = static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka);
								else
									alch2 = 0;
								if (Settings::_featStaminaRestoration && curr->durStamina < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kStamina) < Settings::_staminaThreshold)
									alch3 = static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina);
								else
									alch3 = 0;
								// construct combined effect
								alch |= alch2 | alch3;
								LOG4_4("{}[Events] [CheckActors] check for alchemyeffect {} with current dur health {} dur mag {} dur stam {} ", alch, curr->durHealth, curr->durMagicka, curr->durStamina);
								// use potions
								// do the first round
								if (alch != 0 && (Settings::_UsePotionChance == 100 || rand100(rand) < Settings::_UsePotionChance)) {
									auto avmag = curr->actor->GetActorValue(RE::ActorValue::kMagicka);
									auto avhealth = curr->actor->GetActorValue(RE::ActorValue::kHealth);
									auto avstam = curr->actor->GetActorValue(RE::ActorValue::kStamina);
									auto tup = ACM::ActorUsePotion(curr, alch, Settings::_CompatibilityPotionAnimation);
									LOG1_2("{}[Events] [CheckActors] found potion has Alchemy Effect {}", static_cast<uint64_t>(std::get<1>(tup)));
									if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
										curr->durHealth = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;  // convert to milliseconds
										avhealth += std::get<0>(tup) * std::get<2>(tup);
										LOG2_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {}", curr->durHealth, std::get<0>(tup));
									}
									if (static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka) & std::get<1>(tup)) {
										curr->durMagicka = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
										avmag += std::get<0>(tup) * std::get<2>(tup);
										LOG2_4("{}[Events] [CheckActors] use magicka pot with duration {} and magnitude {}", curr->durMagicka, std::get<0>(tup));
									}
									if (static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina) & std::get<1>(tup)) {
										curr->durStamina = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
										avstam += std::get<0>(tup) * std::get<2>(tup);
										LOG2_4("{}[Events] [CheckActors] use stamina pot with duration {} and magnitude {}", curr->durStamina, std::get<0>(tup));
									}
									if (std::get<1>(tup) != 0) {
										counter++;
									}
									// do the rest of the rounds
									for (int c = 1; c < Settings::_maxPotionsPerCycle; c++) {
										// get combined effect for magicka, health, and stamina
										if (Settings::_featHealthRestoration && curr->durHealth < tolerance && ACM::GetAVPercentageFromValue(curr->actor, RE::ActorValue::kHealth, avhealth) < Settings::_healthThreshold)
											alch = static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth);
										else
											alch = 0;
										if (Settings::_featMagickaRestoration && curr->durMagicka < tolerance && ACM::GetAVPercentageFromValue(curr->actor, RE::ActorValue::kMagicka, avmag) < Settings::_magickaThreshold)
											alch2 = static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka);
										else
											alch2 = 0;
										if (Settings::_featStaminaRestoration && curr->durStamina < tolerance && ACM::GetAVPercentageFromValue(curr->actor, RE::ActorValue::kStamina, avstam) < Settings::_staminaThreshold)
											alch3 = static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina);
										else
											alch3 = 0;
										// construct combined effect
										alch |= alch2 | alch3;
										if (alch != 0) {
											tup = ACM::ActorUsePotion(curr, std::get<3>(tup), Settings::_CompatibilityPotionAnimation);
											if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
												CalcActorCooldowns(curr, std::get<1>(tup), std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);
												avhealth += std::get<0>(tup) * std::get<2>(tup);
												LOG3_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {} and effect {}", curr->durHealth, std::get<0>(tup), Utility::ToString(std::get<1>(tup)));
												counter++;
											}
											if (static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka) & std::get<1>(tup)) {
												CalcActorCooldowns(curr, std::get<1>(tup), std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);
												avmag += std::get<0>(tup) * std::get<2>(tup);
												LOG3_4("{}[Events] [CheckActors] use magicka pot with duration {} and magnitude {} and effect {}", curr->durMagicka, std::get<0>(tup), Utility::ToString(std::get<1>(tup)));
												counter++;
											}
											if (static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina) & std::get<1>(tup)) {
												CalcActorCooldowns(curr, std::get<1>(tup), std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);
												avstam += std::get<0>(tup) * std::get<2>(tup);
												LOG3_4("{}[Events] [CheckActors] use stamina pot with duration {} and magnitude {} and effect {}", curr->durStamina, std::get<0>(tup), Utility::ToString(std::get<1>(tup)));
												counter++;
											}
											if (std::get<1>(tup) != 0) {
												counter++;
											}
										} else
											break;
									}
								}
							}
							CheckDeadCheckHandlerLoop;

							// get combatdata of current actor
							uint32_t combatdata = Utility::GetCombatData(curr->actor);
							uint32_t tcombatdata = 0;
							// retrieve target of current actor if present
							RE::ActorHandle handle = curr->actor->currentCombatTarget;

							if (Settings::_featUsePoisons && (Settings::_UsePoisonChance == 100 || rand100(rand) < Settings::_UsePoisonChance) && (!curr->actor->IsPlayerRef() || Settings::_playerUsePoisons)) {
								LOG_2("{}[Events] [CheckActors] [poisons]");
								// handle poisons
								if (curr->IsFollower() || curr->actor->IsPlayerRef()) {
									if (combatdata != 0 && (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
										(combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0 && Utility::CanApplyPoison(curr->actor)) {
										// handle followers
										// they only use poisons if there are many npcs in the fight, or if the enemies they are targetting
										// have a high enough level, like starting at PlayerLevel*0.8 or so
										if (Settings::_EnemyNumberThreshold < actorsincombat || (handle && handle.get() && handle.get().get() && handle.get().get()->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::_EnemyLevelScalePlayerLevel)) {
											// time to use some potions
											uint64_t effects = 0;
											// kResistMagic, kResistFire, kResistFrost, kResistMagic should only be used if the follower is a spellblade
											if (combatdata & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) ||
												combatdata & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword))) {
												effects |= static_cast<uint64_t>(AlchemyEffect::kResistMagic);
												if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
													if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire))
														effects |= static_cast<uint64_t>(AlchemyEffect::kResistFire);
													if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost))
														effects |= static_cast<uint64_t>(AlchemyEffect::kResistFrost);
													if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock))
														effects |= static_cast<uint64_t>(AlchemyEffect::kResistShock);
												}
											}
											// incorporate enemy specific data
											if (handle && handle.get() && handle.get().get()) {
												// we can make the usage dependent on the target
												RE::Actor* target = handle.get().get();
												if (target->GetRace()->HasKeyword(Settings::ActorTypeDwarven) || target->GetActorBase()->HasKeyword(Settings::ActorTypeDwarven))
													goto SkipPoison;
												tcombatdata = Utility::GetCombatData(target);
												// determine main actor value of enemy. That is the one we want to target ideally
												float tmag = target->GetBaseActorValue(RE::ActorValue::kMagicka);
												float smag = target->GetBaseActorValue(RE::ActorValue::kStamina);
												//float shea = target->GetBaseActorValue(RE::ActorValue::kHealth);

												if (tmag > smag) {
													// the enemy is probably a mage
													// appropiate potions are:
													// kParalysis, kHealth, kMagicka, kMagickaRate, kHealRate, kSpeedMult, kDamageResist, kPoisonResist, kFrenzy, kFear
													effects |= static_cast<uint64_t>(AlchemyEffect::kHealth) |
													           static_cast<uint64_t>(AlchemyEffect::kMagicka) |
													           static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
													           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult) |
													           static_cast<uint64_t>(AlchemyEffect::kHealRate) |
													           static_cast<uint64_t>(AlchemyEffect::kHealRateMult) |
													           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
													           static_cast<uint64_t>(AlchemyEffect::kDamageResist) |
													           static_cast<uint64_t>(AlchemyEffect::kPoisonResist) |
													           static_cast<uint64_t>(AlchemyEffect::kParalysis) |
													           static_cast<uint64_t>(AlchemyEffect::kFrenzy) |
													           static_cast<uint64_t>(AlchemyEffect::kFear);
												} else {
													// the enemy is probably a meele charackter
													// kParalysis, kHealth, kStamina, kHealRate, kStaminaRate, kDamageResist, kPoisonResist, kFrenzy, kFear, kSpeedMult, kWeaponSpeedMult, kAttackDamageMult
													effects |= static_cast<uint64_t>(AlchemyEffect::kHealth) |
													           static_cast<uint64_t>(AlchemyEffect::kStamina) |
													           static_cast<uint64_t>(AlchemyEffect::kHealRate) |
													           static_cast<uint64_t>(AlchemyEffect::kHealRateMult) |
													           static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
													           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult) |
													           static_cast<uint64_t>(AlchemyEffect::kDamageResist) |
													           static_cast<uint64_t>(AlchemyEffect::kPoisonResist) |
													           static_cast<uint64_t>(AlchemyEffect::kFrenzy) |
													           static_cast<uint64_t>(AlchemyEffect::kFear) |
													           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
													           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
													           static_cast<uint64_t>(AlchemyEffect::kParalysis) |
													           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult);
												}
											} else {
												// we dont have a target so just use any poison
												effects |= static_cast<uint64_t>(AlchemyEffect::kHealth) |
												           static_cast<uint64_t>(AlchemyEffect::kMagicka) |
												           static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
												           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult) |
												           static_cast<uint64_t>(AlchemyEffect::kHealRate) |
												           static_cast<uint64_t>(AlchemyEffect::kHealRateMult) |
												           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
												           static_cast<uint64_t>(AlchemyEffect::kDamageResist) |
												           static_cast<uint64_t>(AlchemyEffect::kPoisonResist) |
												           static_cast<uint64_t>(AlchemyEffect::kFrenzy) |
												           static_cast<uint64_t>(AlchemyEffect::kFear) |
												           static_cast<uint64_t>(AlchemyEffect::kParalysis) |
												           static_cast<uint64_t>(AlchemyEffect::kStamina) |
												           static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
												           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult) |
												           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
												           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult);
											}
											LOG1_4("{}[Events] [CheckActors] check for poison with effect {}", effects);
											auto tup = ACM::ActorUsePoison(curr, effects);
											//if (std::get<1>(tup) != AlchemyEffect::kNone)
											//	loginfo("Used poison on actor:\t{}", curr->actor->GetName());
										}
									}
									if (combatdata == 0)
										LOG2_2("{}[Events] [CheckActors] couldn't determine combatdata for npc {} {}", curr->actor->GetName(), Utility::GetHex(curr->actor->GetFormID()));
									// else Mage of Hand to Hand which cannot use poisons

								} else if ((combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
										   (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0 && Utility::CanApplyPoison(curr->actor)) {
									LOG_2("{}[Events] [Events] [CheckActors] [poisonsnpc]");
									// we dont handle a follower, so just let the enemy use any poison they have
									uint64_t effects = static_cast<uint64_t>(AlchemyEffect::kAnyPoison);
									LOG1_4("{}[Events] [CheckActors] check for poison with effect {}", Utility::GetHex(effects));
									ACM::ActorUsePoison(curr, effects);
								}
								// else Mage or Hand to Hand which cannot use poisons
							}
SkipPoison:;
							CheckDeadCheckHandlerLoop;

							if (Settings::_featUseFortifyPotions &&
								counter < Settings::_maxPotionsPerCycle &&
								(!(curr->actor->IsPlayerRef()) || Settings::_playerUseFortifyPotions)) {
								//loginfo("fortify potions stuff");
								LOG_2("{}[Events] [CheckActors] [fortify]");

								if (curr->actor->IsInFaction(Settings::CurrentFollowerFaction) || curr->actor->IsPlayerRef() && !(Settings::_EnemyNumberThreshold < actorsincombat || (handle && handle.get() && handle.get().get() && handle.get().get()->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::_EnemyLevelScalePlayerLevel))) {
									goto SkipFortify;
								}
								// handle fortify potions
								if ((Settings::_UseFortifyPotionChance == 100 || rand100(rand) < Settings::_UseFortifyPotionChance)) {
									// general stuff
									uint64_t effects = 0;

									// determine valid regeneration effects
									if (curr->durRegeneration < tolerance) {
										effects |= CalcRegenEffects(curr, combatdata);
									}
									// determine valid fortify effects
									if (curr->durFortify < tolerance) {
										effects |= CalcFortifyEffects(curr, combatdata, tcombatdata);
									}

									// std::tuple<int, AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffect>>>
									//loginfo("take fortify with effects: {}", Utility::GetHex(effects));
									LOG1_4("{}[Events] [CheckActors] check for fortify potion with effect {}", Utility::GetHex(effects));
									auto tup = ACM::ActorUsePotion(curr, effects, Settings::_CompatibilityPotionAnimationFortify);
									if (std::get<0>(tup) != -1) {
										AlchemyEffectBase eff = std::get<1>(tup);
										if (eff & Base(AlchemyEffect::kAnyRegen)) {
											//curr->durRegeneration = std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000;
											CalcActorCooldowns(curr, eff, std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000);
											counter++;
											LOG3_4("{}[Events] [CheckActors] used regeneration potion with tracked duration {} {} and effect {}", curr->durRegeneration, std::get<0>(tup) * 1000, Utility::ToString(std::get<1>(tup)));
										}
										if (eff & Base(AlchemyEffect::kAnyFortify)) {
											//curr->durFortify = std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000;
											CalcActorCooldowns(curr, eff, std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000);
											counter++;
											LOG3_4("{}[Events] [CheckActors] used fortify av potion with tracked duration {} {} and effect {}", curr->durFortify, std::get<0>(tup) * 1000, Utility::ToString(std::get<1>(tup)));
											break;
										}
									}
								}
							}
SkipFortify:;

							if (Settings::_featUseFood &&
								RE::Calendar::GetSingleton()->GetDaysPassed() >= curr->nextFoodTime &&
								(!curr->actor->IsPlayerRef() || Settings::_playerUseFood)) {
								// use food at the beginning of the fight to simulate the npc having eaten
								// calc effects that we want to be applied
								AlchemyEffectBase effects = 0;
								effects |= CalcFortifyEffects(curr, combatdata, tcombatdata);
								effects |= CalcRegenEffects(curr, combatdata);
								auto [dur, effect] = ACM::ActorUseFood(curr, effects, false);
								if (effect == 0) {  // nothing found
									auto tup = ACM::ActorUseFood(curr);
									dur = std::get<0>(tup);
									effect = std::get<1>(tup);
								}
								if (effect != 0) {
									LogConsole((std::string("Eating food ") + curr->actor->GetName()).c_str());
									LogConsole((std::string("Old Time: ") + std::to_string(curr->nextFoodTime)).c_str());
									curr->nextFoodTime = RE::Calendar::GetSingleton()->GetDaysPassed() + dur * RE::Calendar::GetSingleton()->GetTimescale() / 60 / 60 / 24;
									LogConsole((std::string("New Time: ") + std::to_string(curr->nextFoodTime)).c_str());
									LogConsole((std::string("TimeScale: ") + std::to_string(RE::Calendar::GetSingleton()->GetTimescale())).c_str());
								}
								LOG2_1("[Events] [CheckActors] current days passed: {}, next food time: {}", std::to_string(RE::Calendar::GetSingleton()->GetDaysPassed()), std::to_string(curr->nextFoodTime));
							}

							// end all values are up to date
						}
					} else {
						// actor dead or invalid
						// dont remove it, since we would need an iterator for that ... which we don't have
						// the list doesn't persist between game starts, so it doesn't hurt leaving it
						// and an actor is removed from combat once they die anyway, so this case shouldn't happen
					}
				}
				EvalProcessing();

				// if we inserted the player, remove them and get their applied values
				if (player) {
					LOG_3("{}[Events] [CheckActors] Removing Player from the list.");
					acset.erase(playerinfo);
					// we do not need to retrieve values, since wthey are written to the reference directly
				}
				// write execution time of iteration
				PROF1_1("{}[PROF] [Events]  [CheckActors] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("{}[Events] [CheckActors] checked {} actors", std::to_string(acset.size()));
				// release lock.
			} else {
				LOG_1("{}[Events] [CheckActors] Skip round.")
			}
			std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(Settings::_cycletime));
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
			if (!Distribution::ExcludedNPC(acinfo->actor)) {
				// begin with compatibility mode removing items before distributing new ones
				if (Settings::_CompatibilityRemoveItemsBeforeDist) {
					auto items = ACM::GetAllPotions(acinfo);
					auto it = items.begin();
					while (it != items.end()) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", (*it)->GetName());
						it++;
					}
					items = ACM::GetAllPoisons(acinfo);
					it = items.begin();
					while (it != items.end()) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", (*it)->GetName());
						it++;
					}
					items = ACM::GetAllFood(acinfo);
					it = items.begin();
					while (it != items.end()) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", (*it)->GetName());
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
						std::string name = items[i]->GetName();
						std::string id = Utility::GetHex(items[i]->GetFormID());
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(acinfo->actor);
						acinfo->actor->AddObjectToContainer(items[i], extra, 1, nullptr);
						LOG2_4("{}[Events] [ProcessDistribution] added item {} to actor {}", Utility::GetHex(items[i]->GetFormID()), Utility::GetHex(acinfo->actor->GetFormID()));
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

	void RemoveItemsOnStartup()
	{
		EvalProcessing();
		std::this_thread::sleep_for(5s);

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
					if (actor) {
						ActorInfo* acinfo = data->FindActor(actor);
						auto items = ACM::GetAllPotions(acinfo);
						auto it = items.begin();
						while (it != items.end()) {
							if (Settings::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
							LOG1_1("{}[Events] [RemoveItemsOnStartup] Removed item {}", (*it)->GetName());
							it++;
						}
						items = ACM::GetAllPoisons(acinfo);
						it = items.begin();
						while (it != items.end()) {
							if (Settings::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
							LOG1_1("{}[Events] [RemoveItemsOnStartup] Removed item {}", (*it)->GetName());
							it++;
						}
						items = ACM::GetAllFood(acinfo);
						it = items.begin();
						while (it != items.end()) {
							if (Settings::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							RE::ExtraDataList* extra = new RE::ExtraDataList();
							extra->SetOwner(actor);
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
							LOG1_1("{}[Events] [RemoveItemsOnStartup] Removed item {}", (*it)->GetName());
							it++;
						}
					}
				}
				iter++;
			}
		}
		LogConsole("Finished Thread RemoveItemsOnStartup");
	}

	void RegisterNPC(RE::Actor* actor)
	{
		LOG1_1("{}[Events] [RegisterNPC] Trying to register new actor for potion tracking: {}", actor->GetName());

		ActorInfo* acinfo = data->FindActor(actor);
		EvalProcessing();
		// find out whether to insert the actor, if yes insert him into the temp insert list
		sem.acquire();
		if (!acset.contains(acinfo)) {
			acinsert.insert(actor);
			if (acremove.contains(actor)) {
				acremove.erase(actor);
			}
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

	void UnregisterNPC(RE::Actor* actor)
	{
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", actor->GetName());
		EvalProcessing();
		ActorInfo* acinfo = data->FindActor(actor);
		sem.acquire();
		if (acset.contains(acinfo) || acinsert.contains(actor)) {
			acinsert.erase(actor);
			acremove.insert(actor);
		}
		sem.release();
		LOG_1("{}[Events] [UnregisterNPC] Unregistered NPC");
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
		LOG_1("{}[Events] [TESDeathEvent]");
		InitializeCompatibilityObjects();
		auto actor = a_event->actorDying->As<RE::Actor>();
		if (actor->IsPlayerRef()) {
			playerdied = true;
		} else if (actor && actor != RE::PlayerCharacter::GetSingleton()) {
			// as with potion distribution, exlude excluded actors and potential followers
			if (!Distribution::ExcludedNPC(actor) && deads.contains(actor->GetFormID()) == false) {
				// create and insert new event
				EvalProcessingEvent();
				ActorInfo* acinfo = data->FindActor(actor);
				deads.insert(actor->GetFormID());
				LOG1_1("{}[Events] [TESDeathEvent] Removing items from actor {}", std::to_string(actor->GetFormID()));
				auto items = Distribution::GetMatchingInventoryItems(acinfo);
				LOG1_1("{}[Events] [TESDeathEvent] found {} items", items.size());
				if (items.size() > 0) {
					// remove items that are too much
					while (items.size() > Settings::_MaxItemsLeft) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(actor);
						actor->RemoveItem(items.back(), 1 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[Events] [TESDeathEvent] Removed item {}", items.back()->GetName());
						items.pop_back();
					}
					//loginfo("[Events] [TESDeathEvent] 3");
					// remove the rest of the items per chance
					if (Settings::_ChanceToRemoveItem < 100) {
						for (int i = (int)items.size() - 1; i >= 0; i--) {
							if (rand100(rand) <= Settings::_ChanceToRemoveItem) {
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->RemoveItem(items[i], 100 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
								LOG1_1("{}[Events] [TESDeathEvent] Removed item {}", items[i]->GetName());
							} else {
								LOG1_1("{}[Events] [TESDeathEvent] Did not remove item {}", items[i]->GetName());
							}
						}
					}
				}
				// distribute death items
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
			}
			// delete actor from data
			data->DeleteActor(actor->GetFormID());
		}

		return EventResult::kContinue;
	}

	/// <summary>
	/// Processes TESHitEvents
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESHitEvent* /*a_event*/, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		EvalProcessingEvent();
		// LOG_3("{}[Events] [TESHitEvent]");
		// currently unused
		// MARK TO REMOVE
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
		if (!Settings::_featDisableOutOfCombatProcessing)
			return EventResult::kContinue;
		LOG_1("{}[Events] [TESCombatEvent]");
		EvalProcessingEvent();
		InitializeCompatibilityObjects();
		auto actor = a_event->actor->As<RE::Actor>();
		if (actor && !actor->IsDead() && actor != RE::PlayerCharacter::GetSingleton() && actor->IsChild() == false) {
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				RegisterNPC(actor);
			} else {
				UnregisterNPC(actor);
			}
		}

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
		// return if feature disabled
		if (Settings::_featDisableOutOfCombatProcessing)
			return EventResult::kContinue;
		EvalProcessingEvent();
		ReEvalPlayerDeath;

		if (a_event && a_event->reference) {
			RE::Actor* actor = a_event->reference->As<RE::Actor>();
			if (actor && deads.find(actor->GetFormID()) == deads.end() && !actor->IsDead() && !actor->IsPlayerRef()) {
				if (a_event->attached) {
					RegisterNPC(actor);
				} else {
					UnregisterNPC(actor);
				}
			}
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
		//LOG_1("{}[Events] [BGSActorCellEvent]");
		EvalProcessingEvent();
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
	/// EventHandler for TESLoadGameEvent. Loads main thread
	/// </summary>
	/// <param name="">unused</param>
	/// <param name="">unused</param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESLoadGameEvent*, RE::BSTEventSource<RE::TESLoadGameEvent>*)
	{
		LOG_1("{}[Events] [LoadGameEvent]");
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
			LOG_1("{}[Events] [LoadGameEvent] Started CheckActors");
		}
		// reset the list of actors that died
		deads.clear();
		// set player to alive
		ReEvalPlayerDeath;

		enableProcessing = true;

		if (Settings::_Test) {
			if (testhandler == nullptr) {
				testhandler = new std::thread(TestAllCells);
				LOG_1("{}[Events] [LoadGameEvent] Started TestHandler");
			}
		}

		if (Settings::_CompatibilityRemoveItemsStartup) {
			if (removeitemshandler == nullptr) {
				removeitemshandler = new std::thread(RemoveItemsOnStartup);
				LOG_1("{}[Events] [LoadGameEvent] Started RemoveItemsHandler");
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
		LOG1_1("{}[Events] [LoadGameEvent] found {} cells", std::to_string(gamecells.size()));
		for (int i = 0; i < (int)gamecells.size(); i++) {
			if (gamecells[i]->IsAttached())
			{
				auto itr = gamecells[i]->references.begin();
				while (itr != gamecells[i]->references.end())
				{
					if (itr->get())
					{
						RE::Actor* actor = itr->get()->As<RE::Actor>();
						if (actor && deads.find(actor->GetFormID()) == deads.end() && !actor->IsDead() && !actor->IsPlayerRef())
						{
							RegisterNPC(actor);
						}
					}
					itr++;
				}
			}
		}

		InitializeCompatibilityObjects();

		LOG_1("{}[Events] [LoadGameEvent] end");

		return EventResult::kContinue;
	}

	void SaveGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [SaveGameCallback]");

	}

	void LoadGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [LoadGameCallback]");
	}

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
		scriptEventSourceHolder->GetEventSource<RE::TESLoadGameEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESLoadGameEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESEquipEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESEquipEvent).name());
		if (Settings::_featRemoveItemsOnDeath) {
			scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(EventHandler::GetSingleton());
			LOG1_1("{}Registered {}", typeid(RE::TESDeathEvent).name());
		}
		if (Settings::_CalculateCellRules) {
			RE::PlayerCharacter::GetSingleton()->GetEventSource<RE::BGSActorCellEvent>()->AddEventSink(EventHandler::GetSingleton());
			LOG1_1("{}Registered {}", typeid(RE::BGSActorCellEvent).name());
		}
		scriptEventSourceHolder->GetEventSource<RE::TESCellAttachDetachEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESCellAttachDetachEvent).name());
		Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000001, LoadGameCallback);
		LOG1_1("{}Registered {}", typeid(LoadGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000002, RevertGameCallback);
		LOG1_1("{}Registered {}", typeid(RevertGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000003, SaveGameCallback);
		LOG1_1("{}Registered {}", typeid(SaveGameCallback).name());
		data = Data::GetSingleton();
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
