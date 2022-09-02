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
#include "ActorManipulation.h"
#include <limits>
#include <filesystem>
#include <deque>
#include "ActorInfo.h"
#include <Game.h>
#include <Distribution.h>
		
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

	// Actor, health cooldown, magicka cooldown, stamina cooldown, other cooldown, reg cooldown
	// static std::vector<std::tuple<RE::Actor*, int, int, int, int, int>> aclist{};

	/// <summary>
	/// list that holds currently handled actors
	/// </summary>
	static std::vector<ActorInfo*> aclist{};
	/// <summary>
	/// semaphore used to sync access to actor handling, to prevent list changes while operations are done
	/// </summary>
	static std::binary_semaphore sem(1);

	/// <summary>
	/// since the TESDeathEvent seems to be able to fire more than once for an actor we need to track the deaths
	/// </summary>
	static std::unordered_set<RE::FormID> deads;

	/// <summary>
	/// map that contains information about any npc that has entered combat during runtime
	/// </summary>
	static std::unordered_map<uint32_t, ActorInfo*> actorinfoMap;

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
	}                                                             \
	LOG1_1("{}[ReevalPlayerDeath] {}", playerdied);

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
					logger::info("[Events] [INIT] Some Forms from PotionAnimatedfx.esp seem to be missing. Forcefully deactivated compatibility mode");
				}
			}
			initialized = true;
		}
	}

	ActorInfo* FindActor(RE::Actor* actor)
	{
		ActorInfo* acinfo = nullptr;
		logger::info("1");
		auto itr = actorinfoMap.find(actor->GetFormID());
		logger::info("2");
		if (itr == actorinfoMap.end()) {
			logger::info("3");
			acinfo = new ActorInfo(actor, 0, 0, 0, 0, 0);
			actorinfoMap.insert_or_assign(actor->GetFormID(), acinfo);
		} else if (itr->second == nullptr || itr->second->actor == nullptr || itr->second->actor->GetFormID() == 0 || itr->second->actor->GetFormID() != actor->GetFormID()) {
			// either delete acinfo, deleted actor, actor fid 0 or acinfo belongs to wrong actor
			actorinfoMap.erase(actor->GetFormID());
			logger::info("3.1");
			acinfo = new ActorInfo(actor, 0, 0, 0, 0, 0);
			actorinfoMap.insert_or_assign(actor->GetFormID(), acinfo);
		} else {
			logger::info("4");
			acinfo = itr->second;
			if (acinfo->citems == nullptr)
				acinfo->citems = new ActorInfo::CustomItems();
		}
		logger::info("5");
		return acinfo;
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

		ActorInfo* playerinfo = new ActorInfo(RE::PlayerCharacter::GetSingleton(), 0, 0, 0, 0, 0);

		/// temp section
		uint64_t alch = 0;
		uint64_t alch2 = 0;
		uint64_t alch3 = 0;
		bool player = false;  // wether player was inserted into list

		auto datahandler = RE::TESDataHandler::GetSingleton();
		const RE::TESFile* file = nullptr;
		std::string_view name = std::string_view{ "" };

		// main loop, if the thread should be stopped, exit the loop
		while (!stopactorhandler) {
			EvalProcessing();
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

				sem.acquire();
				// checking if player should be handled
				if ((Settings::_playerRestorationEnabled || Settings::_playerUseFortifyPotions || Settings::_playerUsePoisons) && RE::PlayerCharacter::GetSingleton()->IsInCombat()) {
					// inject player into the list and remove him later
					aclist.insert(aclist.end(), playerinfo);
					LOG_3("{}[Events] [CheckActors] Adding player to the list");
					player = true;
				}
				LOG1_1("{}[Events] [CheckActors] Handling all registered Actors {}", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count()));
				// handle all registered actors
				// the list does not change while doing this
				for (int i = 0; i < aclist.size(); i++) {
					CheckDeadCheckHandlerLoop;
					curr = aclist[i];
					if (curr == nullptr || curr->actor == nullptr || curr->actor->GetFormID() == 0) {
						auto itr = aclist.begin();
						for (int x = 0; x < i; x++) {
							itr++;
						}
						aclist.erase(itr);
						i--;
					}
					if (curr->citems == nullptr)
						curr->citems = new ActorInfo::CustomItems();
					if (Settings::_featDisableItemUsageWhileStaggered && curr->actor->actorState2.staggered) {
						LOG_1("{}[Events] [CheckActors] [Actor] Actor is staggered, abort round");
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
						// handle potions
						// get current duration
						curr->durHealth -= Settings::_cycletime;
						curr->durMagicka -= Settings::_cycletime;
						curr->durStamina -= Settings::_cycletime;
						curr->durFortify -= Settings::_cycletime;
						curr->durRegeneration -= Settings::_cycletime;
						LOG5_1("{}[Events] [CheckActors] [Actor] cooldown: {} {} {} {} {}", curr->durHealth, curr->durMagicka, curr->durStamina, curr->durFortify, curr->durRegeneration);

						// potions used this cycle
						int counter = 0;

						if (!curr->actor->IsPlayerRef() || Settings::_playerRestorationEnabled) {
							LOG_2("{}[Events] [CheckActors] [potions]")
							// get combined effect for magicka, health, and stamina
							if (Settings::_featHealthRestoration && curr->durHealth < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kHealth) < Settings::_healthThreshold)
								alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
							else
								alch = 0;
							if (Settings::_featMagickaRestoration && curr->durMagicka < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kMagicka) < Settings::_magickaThreshold)
								alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
							else
								alch2 = 0;
							if (Settings::_featStaminaRestoration && curr->durStamina < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kStamina) < Settings::_staminaThreshold)
								alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
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
										alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
									else
										alch = 0;
									if (Settings::_featMagickaRestoration && curr->durMagicka < tolerance && ACM::GetAVPercentageFromValue(curr->actor, RE::ActorValue::kMagicka, avmag) < Settings::_magickaThreshold)
										alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
									else
										alch2 = 0;
									if (Settings::_featStaminaRestoration && curr->durStamina < tolerance && ACM::GetAVPercentageFromValue(curr->actor, RE::ActorValue::kStamina, avstam) < Settings::_staminaThreshold)
										alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
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
							if (curr->actor->IsInFaction(Settings::CurrentFollowerFaction) || curr->actor->IsPlayerRef()) {
								if (combatdata != 0 && (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
									(combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0 && Utility::CanApplyPoison(curr->actor)) {
									// handle followers
									// they only use poisons if there are many npcs in the fight, or if the enemies they are targetting
									// have a high enough level, like starting at PlayerLevel*0.8 or so
									if (Settings::_EnemyNumberThreshold < aclist.size() || (handle && handle.get() && handle.get().get() && handle.get().get()->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::_EnemyLevelScalePlayerLevel)) {
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
										//	logger::info("Used poison on actor:\t{}", curr->actor->GetName());
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

						if (Settings::_featUseFortifyPotions && counter < Settings::_maxPotionsPerCycle && (!(curr->actor->IsPlayerRef()) || Settings::_playerUseFortifyPotions)) {
							//logger::info("fortify potions stuff");
							LOG_2("{}[Events] [CheckActors] [fortify]");

							if (curr->actor->IsInFaction(Settings::CurrentFollowerFaction) || curr->actor->IsPlayerRef() && !(Settings::_EnemyNumberThreshold < aclist.size() || (handle && handle.get() && handle.get().get() && handle.get().get()->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::_EnemyLevelScalePlayerLevel))) {
								goto SkipFortify;
							}
							// handle fortify potions
							if ((Settings::_UseFortifyPotionChance == 100 || rand100(rand) < Settings::_UseFortifyPotionChance)) {
								// general stuff
								uint64_t effects = 0;

								// determine valid regeneration effects
								if (curr->durRegeneration < tolerance) {
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
											static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield))) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
										           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
										           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult);
									}
								}
								// determine valid fortify effects
								if (curr->durFortify < tolerance) {
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
										           static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword)) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
										           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
										           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield)) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
										           static_cast<uint64_t>(AlchemyEffect::kBlock) |
										           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded)) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kTwoHanded) |
										           static_cast<uint64_t>(AlchemyEffect::kBlock) |
										           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
										           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged)) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kArchery) |
										           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(AlchemyEffect::kBowSpeed) |
										           static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield)) {
										effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
										           static_cast<uint64_t>(AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualStaff)) {
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
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
									uint32_t armordata = Utility::GetArmorData(curr->actor);
									if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::LightArmor))
										effects |= static_cast<uint64_t>(AlchemyEffect::kLightArmor);
									if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::HeavyArmor))
										effects |= static_cast<uint64_t>(AlchemyEffect::kHeavyArmor);
								}

								// std::tuple<int, AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffect>>>
								//logger::info("take fortify with effects: {}", Utility::GetHex(effects));
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

						// end all values are up to date
					} else {
						// actor dead or invalid
						// dont remove it, since we would need an iterator for that ... which we don't have
						// the list doesn't persist between game starts, so it doesn't hurt leaving it
						// and an actor is removed from combat once they die anyway, so this case shouldn't happen
					}
				}

				// if we inserted the player, remove them and get their applied values
				if (player) {
					LOG_3("{}[Events] [CheckActors] Removing Player from the list.");
					aclist.pop_back();
					// we do not need to retrieve values, since wthey are written to the reference directly
				}
				// write execution time of iteration
				PROF1_1("{}[PROF] [Events]  [CheckActors] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("{}[Events] [CheckActors] checked {} actors", std::to_string(aclist.size()));
				// release lock.
				sem.release();
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
					//logger::info("tryna fail");
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
						//logger::info("tryna fail fail");
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

		//logger::info("tryna start");
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
		auto console = RE::ConsoleLog::GetSingleton();
		console->Print("Start Test");
		logger::info("Start Test");
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
						console->Print(buff);
						logger::info("Moving to cell:\t{}", cell->GetFormEditorID());
						RE::PlayerCharacter::GetSingleton()->MoveTo((*(cell->references.begin())).get());
					}
					std::this_thread::sleep_for(7s);
				}
		//	}
		//	iter++;
		}
		console->Print("Finished Test");
		logger::info("Finished Test");
		//logger::info("tryna end");
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
		//logger::info("tryna start");
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
		auto console = RE::ConsoleLog::GetSingleton();
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
						ActorInfo* acinfo = FindActor(actor);
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
		console->Print("Finished Thread RemoveItemsOnStartup");
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
				ActorInfo* acinfo = FindActor(actor);
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
					if (actor->IsDead()) {
						return EventResult::kContinue;
					}
					//logger::info("[Events] [TESDeathEvent] 3");
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
					if (rand100(rand) <= std::get<1>(ditems[i])) {
						// distr item
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(actor);
						actor->AddObjectToContainer(std::get<0>(ditems[i]), extra, std::get<2>(ditems[i]), nullptr);
					}
				}
			}
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
		LOG_1("{}[Events] [TESCombatEvent]");
		EvalProcessingEvent();
		InitializeCompatibilityObjects();
		auto actor = a_event->actor->As<RE::Actor>();
		if (actor && !actor->IsDead() && actor != RE::PlayerCharacter::GetSingleton() && actor->IsChild() == false) {
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				LOG_1("{}[Events] [TesCombatEnterEvent] Trying to register new actor for potion tracking");

				ActorInfo* acinfo = FindActor(actor);
				// first insert the actor into the list
				EvalProcessingEvent();
				sem.acquire();
				logger::info("x1");
				auto itra = aclist.begin();
				auto end = aclist.end();
				bool cont = false;
				logger::info("x2");
				while (itra != end) {
					if ((*itra)->actor == actor) {
						cont = true;
						break;
					}
					itra++;
				}
				logger::info("x3");
				if (!cont) {
					logger::info("x4");
					aclist.insert(aclist.begin(), acinfo);
				} else {
					logger::info("x5");
					sem.release();
					return EventResult::kContinue;
				}
				sem.release();
				logger::info("x6\t{}", acinfo->ToString());

				// check wether this charackter maybe a follower
				if (acinfo->lastDistrTime == 0.0f || RE::Calendar::GetSingleton()->GetDaysPassed() - acinfo->lastDistrTime > 1) {
					logger::info("x^7");
					if (!Distribution::ExcludedNPC(actor)) {
						logger::info("x8");
						// begin with compatibility mode removing items before distributing new ones
						if (Settings::_CompatibilityRemoveItemsBeforeDist) {
							auto items = ACM::GetAllPotions(acinfo);
							auto it = items.begin();
							while (it != items.end()) {
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
								LOG1_1("{}[Events] [TESCombatEventEnter] [Compatibility] Removed item {}", (*it)->GetName());
								it++;
							}
							items = ACM::GetAllPoisons(acinfo);
							it = items.begin();
							while (it != items.end()) {
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
								LOG1_1("{}[Events] [TESCombatEventEnter] [Compatibility] Removed item {}", (*it)->GetName());
								it++;
							}
							items = ACM::GetAllFood(acinfo);
							it = items.begin();
							while (it != items.end()) {
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
								LOG1_1("{}[Events] [TESCombatEventEnter] [Compatibility] Removed item {}", (*it)->GetName());
								it++;
							}
						}

						logger::info("x9");
						// if we have characters that should not get items, the function
						// just won't return anything, but we have to check for standard factions like CurrentFollowerFaction
						auto items = Distribution::GetDistrItems(acinfo);
						logger::info("x10");
						if (actor->IsDead()) {
							return EventResult::kContinue;
						}
						if (items.size() > 0) {
							for (int i = 0; i < items.size(); i++) {
								if (items[i] == nullptr) {
									//logger::info("[Events] [TESCombatEvent] Item: null");
									continue;
								}
								std::string name = items[i]->GetName();
								std::string id = Utility::GetHex(items[i]->GetFormID());
								//logger::info("[Events] [TESCombatEvent] Item: {} {}", id, name);
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->AddObjectToContainer(items[i], extra, 1, nullptr);
								LOG2_4("{}[Events] [TESCombatEventEnter] added item {} to actor {}", Utility::GetHex(items[i]->GetFormID()), Utility::GetHex(actor->GetFormID()));
							}
							acinfo->lastDistrTime = RE::Calendar::GetSingleton()->GetDaysPassed();
						}
					}
				}
				logger::info("x20");
				EvalProcessingEvent();
				if (actor->IsDead())
					return EventResult::kContinue;

				if (Settings::_featUseFood) {
					// use food at the beginning of the fight to simulate the npc having eaten
					ACM::ActorUseFood(acinfo);
				}
				LOG_1("{}[Events] [TesCombatEnterEvent] finished registering NPC");
			} else {
				LOG_1("{}[Events] [TesCombatLeaveEvent] Unregister NPC from potion tracking")
				EvalProcessingEvent();
				sem.acquire();
				auto it = aclist.begin();
				auto end = aclist.end();
				while (it != end) {
					if ((*it)->actor == actor) {
						// just remove object, do not delete it
						(*it)->durHealth = 0;
						(*it)->durMagicka = 0;
						(*it)->durStamina = 0;
						(*it)->durFortify = 0;
						(*it)->durRegeneration = 0;
						aclist.erase(it);
						break;
					}
					it++;
				}
				sem.release();
				LOG_1("{}[Events] [TesCombatLeaveEvent] Unregistered NPC");
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
					if (obj->data.consumptionSound &&
						(obj->data.consumptionSound->GetFormID() == Settings::PotionUse->GetFormID() && Settings::FixedPotionUse ||
							obj->data.consumptionSound->GetFormID() == Settings::PoisonUse->GetFormID() && Settings::FixedPoisonUse ||
							obj->data.consumptionSound->GetFormID() == Settings::FoodEat->GetFormID() && Settings::FixedFoodEat)) {
						RE::BSSoundHandle handle;
						audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
						handle.SetObjectToFollow(a_event->actor->Get3D());
						handle.SetVolume(1.0);
						handle.Play();
					}
					/* if ((obj->IsFood() || obj->HasKeyword(Settings::VendorItemFood)) && Settings::FixedFoodEat) {
						RE::BSSoundHandle handle;
						if (obj->data.consumptionSound) {
							audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
							handle.SetObjectToFollow(a_event->actor->Get3D());
							handle.SetVolume(1.0);
							handle.Play();
						} else
							;  //audiomanager->BuildSoundDataFromDescriptor(handle, Settings::FoodEat->soundDescriptor);
					} else if ((obj->IsPoison() || obj->HasKeyword(Settings::VendorItemPoison)) && Settings::FixedPoisonUse) {
						RE::BSSoundHandle handle;
						if (obj->data.consumptionSound) {
							audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
							handle.SetObjectToFollow(a_event->actor->Get3D());
							handle.SetVolume(1.0);
							handle.Play()
						} else
							;  //audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
					} else if ((obj->IsMedicine() || obj->HasKeyword(Settings::VendorItemPotion)) && Settings::FixedPotionUse) {
						RE::BSSoundHandle handle;
						if (obj->data.consumptionSound) {
							audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
							handle.SetObjectToFollow(a_event->actor->Get3D());
							handle.SetVolume(1.0);
							handle.Play();
						} else
							;  //audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PotionUse->soundDescriptor);
					}*/
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
				if (actorhandler->joinable())
					actorhandler->join();
				actorhandler->~thread();
				delete actorhandler;
				actorhandler = nullptr;
			}
			actorhandler = new std::thread(CheckActors);
			LOG_1("{}[Events] [LoadGameEvent] Started CheckActors");
		}
		LOG_1("{}[Events] [LoadGameEvent] 1");
		LOG_1("{}[Events] [LoadGameEvent] 2");
		// reset the list of actors that died
		deads.clear();
		// set player to alive
		ReEvalPlayerDeath;
		LOG_1("{}[Events] [LoadGameEvent] 3");

		enableProcessing = true;
		LOG_1("{}[Events] [LoadGameEvent] 4");

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
		LOG1_1("{}[PlayerDead] {}", playerdied);
		auto itr = actorinfoMap.begin();
		while (itr != actorinfoMap.end()) {
			if (itr->second != nullptr)
				try {
					delete itr->second;
				} catch (std::exception&) {}
			itr++;
		}
		// reset actor information
		actorinfoMap.clear();
		// reset actor processing list
		aclist.clear();
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
		Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000001, LoadGameCallback);
		LOG1_1("{}Registered {}", typeid(LoadGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000003, RevertGameCallback);
		LOG1_1("{}Registered {}", typeid(RevertGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000003, SaveGameCallback);
		LOG1_1("{}Registered {}", typeid(SaveGameCallback).name());
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

	/// <summary>
	/// Resets information about actors
	/// </summary>
	void ResetActorInfoMap()
	{
		sem.acquire();
		auto itr = actorinfoMap.begin();
		while (itr != actorinfoMap.end()) {
			if (itr->second )
			itr->second->_boss = false;
			itr->second->citems->Reset();
		}
		sem.release();
	}

}
