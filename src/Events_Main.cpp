
#include <chrono>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <thread>

#include "ActorManipulation.h"
#include "Distribution.h"
#include "Events.h"
#include "Logging.h"
#include "Settings.h"
#include "Utility.h"
#include "Tests.h"

namespace Events
{

	//-------------------Random-------------------------

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

	void Main::DecreaseActorCooldown(std::shared_ptr<ActorInfo> acinfo)
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

	void Main::HandleActorPotions(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		if (Settings::Potions::_HandleWeaponSheathedAsOutOfCombat && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorPotions] {}", Utility::PrintForm(acinfo));
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
				auto tup = ACM::ActorUsePotion(acinfo, alch, Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation, false);
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
				if (std::get<1>(tup) != 0) {
					acinfo->globalCooldownTimer = comp->GetGlobalCooldown();
				}
			}
		}
	}

	void Main::HandleActorFortifyPotions(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		if (Settings::FortifyPotions::_DontUseWithWeaponsSheathed && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorFortifyPotions] {}", Utility::PrintForm(acinfo));
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
				auto tup = ACM::ActorUsePotion(acinfo, effects, Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify, true);
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

	void Main::HandleActorPoisons(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		if (Settings::Poisons::_DontUseWithWeaponsSheathed && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorPoisons] {}", Utility::PrintForm(acinfo));
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
					if (std::get<1>(tup) != 0)  // check whether an effect was applied
						acinfo->globalCooldownTimer = comp->GetGlobalCooldown();
				}
			}
			if (acinfo->combatdata == 0)
				LOG1_2("{}[Events] [CheckActors] couldn't determine combatdata for npc {}", Utility::PrintForm(acinfo));
			// else Mage or Hand to Hand which cannot use poisons
		}
	}

	void Main::HandleActorFood(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->handleactor == false)
			return;
		if (Settings::Food::_DisableFollowers && acinfo->IsFollower())
			return;
		if (Settings::Food::_DontUseWithWeaponsSheathed && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorFood] {}", Utility::PrintForm(acinfo));
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
				auto tup = acinfo->actor->IsPlayerRef() && Settings::Player::_DontEatRawFood ? ACM::ActorUseFood(acinfo, false) : ACM::ActorUseFood(acinfo, true);
				dur = std::get<0>(tup);
				effect = std::get<1>(tup);
			}
			if (effect != 0)
				acinfo->nextFoodTime = RE::Calendar::GetSingleton()->GetDaysPassed() + dur * RE::Calendar::GetSingleton()->GetTimescale() / 60 / 60 / 24;
			LOG2_1("{}[Events] [CheckActors] current days passed: {}, next food time: {}", std::to_string(RE::Calendar::GetSingleton()->GetDaysPassed()), std::to_string(acinfo->nextFoodTime));
		}
	}

	void Main::HandleActorOOCPotions(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == true &&
				(Settings::Potions::_HandleWeaponSheathedAsOutOfCombat == false /*if disabled we always use the combat handler*/ ||
					Settings::Potions::_HandleWeaponSheathedAsOutOfCombat == true && acinfo->IsWeaponDrawn() == true /*if weapons are drawn we use the combat handler*/) ||
			acinfo->handleactor == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorOOCPotions] {}", Utility::PrintForm(acinfo));
		// we are only checking for health here
		if (Settings::Potions::_enableHealthRestoration && acinfo->durHealth < tolerance &&
			ACM::GetAVPercentage(acinfo->actor, RE::ActorValue::kHealth) < Settings::Potions::_healthThreshold) {
			auto tup = ACM::ActorUsePotion(acinfo, static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth), Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation, false);
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
	void Main::HandleActorRuntimeData(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		// if global cooldown greater zero, we can skip everything
		if (acinfo->globalCooldownTimer > tolerance) {
			acinfo->handleactor = false;
			return;
		}
		LOG1_1("{}[Events] [CheckActors] [HandleActorRuntimeData] {}", Utility::PrintForm(acinfo));
		LOG1_1("{}[Events] [CheckActors] [HandleActorRuntimeData] {}", Utility::PrintForm(acinfo->actor));
		LOG1_1("{}[Events] [CheckActors] [HandleActorRuntimeData] {}", acinfo->IsValid());
		LOG1_1("{}[Events] [CheckActors] [HandleActorRuntimeData] {}", acinfo->GetDeleted());
		// check for staggered option
		// check for paralyzed
		if (comp->DisableItemUsageWhileParalyzed()) {
			if (acinfo->actor->boolBits & RE::Actor::BOOL_BITS::kParalyzed ||
				acinfo->actor->IsFlying() ||
				acinfo->actor->IsInKillMove() ||
				acinfo->actor->IsInMidair() ||
				acinfo->actor->IsInRagdollState() ||
				acinfo->actor->IsUnconscious() ||
				acinfo->actor->actorState2.staggered ||
				acinfo->actor->IsBleedingOut()) {
				LOG_1("{}[Events] [CheckActors] [Actor] Actor is unable to use items");
				acinfo->handleactor = false;
				return;
			}
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
			LOG1_2("{}[Events] [HandleActorRuntimeData] CombatData: {}", Utility::GetHex(acinfo->combatdata));
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
						std::shared_ptr<ActorInfo> current = nullptr;
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
		if (acinfo->actor && !(acinfo->actor->boolBits & RE::Actor::BOOL_BITS::kDead) && acinfo->actor->GetActorValue(RE::ActorValue::kHealth) > 0) {
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

		// get whether weapons are drawn
		acinfo->weaponsDrawn = acinfo->actor->IsWeaponDrawn();
	}

	/// <summary>
	/// Main routine that periodically checks the actors status, and applies items
	/// </summary>
	void Main::CheckActors()
	{
		// wait until processing is allowed, or we should kill ourselves
		while (!CanProcess() && stopactorhandler == false)
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

		std::weak_ptr<ActorInfo> playerweak;

		SKSE::GetTaskInterface()->AddTask([&playerweak]() {
			playerweak = data->FindActor(RE::PlayerCharacter::GetSingleton());
		});

		/// temp section
		AlchemyEffectBase alch = 0;
		AlchemyEffectBase alch2 = 0;
		AlchemyEffectBase alch3 = 0;

		auto datahandler = RE::TESDataHandler::GetSingleton();
		const RE::TESFile* file = nullptr;
		std::string_view name = std::string_view{ "" };

		// main loop, if the thread should be stopped, exit the loop
		while (!stopactorhandler) {
			if (!CanProcess())
				goto CheckActorsSkipIteration;
			// update active actors
			actorhandlerworking = true;

			SKSE::GetTaskInterface()->AddTask([]() {
				PlayerDied((bool)(RE::PlayerCharacter::GetSingleton()->boolBits & RE::Actor::BOOL_BITS::kDead));
			});

			// if we are in a paused menu (SoulsRE unpauses menus, which is supported by this)
			// do not compute, since nobody can actually take potions.
			if (!ui->GameIsPaused() && initialized && !IsPlayerDead()) {
				// get starttime of iteration
				begin = std::chrono::steady_clock::now();

				LOG1_1("{}[Events] [CheckActors] Handling {} registered Actors", std::to_string(acset.size()));

				if (!CanProcess())
					goto CheckActorsSkipIteration;

				SKSE::GetTaskInterface()->AddTask([&playerweak]() {
					if (std::shared_ptr<ActorInfo> playerinfo = playerweak.lock()) {
						// store position of player character
						ActorInfo::SetPlayerPosition(playerinfo->actor->GetPosition());
						// reset player combat state, we don't want to include them in our checks
						playerinfo->combatstate = CombatState::OutOfCombat;
					}
				});

				LOG1_1("{}[Events] [CheckActors] Validate Actors {}", acset.size());

				// validate actorsets
				sem.acquire();
				std::set<ActorInfoPtr, std::owner_less<ActorInfoPtr>> actors;
				auto itr = acset.begin();
				while (itr != acset.end()) {
					if (std::shared_ptr<ActorInfo> acinfo = itr->lock()) {
						if (!data->UpdateActorInfo(acinfo)) {
							LOG_1("{}[Events] [CheckActors] Removed invalid actor");
							acset.erase(itr);
						} else {
							actors.insert(*itr);
						}
					} else {
						LOG_1("{}[Events] [CheckActors] Removed expired actor");
						acset.erase(itr);
					}
					itr++;
				}
				sem.release();

				LOG1_1("{}[Events] [CheckActors] Validated {} Actors", std::to_string(actors.size()));

				if (!CanProcess())
					goto CheckActorsSkipIteration;
				if (IsPlayerDead())
					break;

				try {
					// first decrease all cooldowns for all registered actors
					actorsincombat = 0;
					hostileactors = 0;
					// decreasing durations
					//
					// calc actors in combat
					// number of actors currently in combat, does not account for multiple combats taking place that are not related to each other
					std::for_each(actors.begin(), actors.end(), [](std::weak_ptr<ActorInfo> acweak) {
						if (std::shared_ptr<ActorInfo> acinfo = acweak.lock()) {
							DecreaseActorCooldown(acinfo);
							if (acinfo->IsInCombat()) {
								actorsincombat++;
								combatants.push_front(acinfo);
								if (acinfo->playerHostile)
									hostileactors++;
							}
						}
					});

					
					SKSE::GetTaskInterface()->AddTask([&playerweak]() {
						if (std::shared_ptr<ActorInfo> playerinfo = playerweak.lock()) {
							// the player should always be valid. If they don't the game doesn't work either anyway
							if (playerinfo->actor->IsInCombat())
								playerinfo->combatstate = CombatState::InCombat;
							else
								playerinfo->combatstate = CombatState::OutOfCombat;
						} 
					});

					if (!CanProcess())
						goto CheckActorsSkipIteration;
					if (IsPlayerDead())
						break;

					SKSE::GetTaskInterface()->AddTask([actors]() {
						// collect actor runtime data
						std::for_each(actors.begin(), actors.end(), [](std::weak_ptr<ActorInfo> acweak) {
							if (std::shared_ptr<ActorInfo> acinfo = acweak.lock()) {
								// retrieve runtime data
								HandleActorRuntimeData(acinfo);
								// handle potions out-of-combat
								if (Settings::Usage::_DisableOutOfCombatProcessing == false) {
									HandleActorOOCPotions(acinfo);
								}
								// handle potions
								HandleActorPotions(acinfo);
								// handle fortify potions
								HandleActorFortifyPotions(acinfo);
								// handle poisons
								HandleActorPoisons(acinfo);
								// handle food
								HandleActorFood(acinfo);
							}
						});
					});
				} catch (std::bad_alloc& e) {
					logcritical("[Events] [CheckActors] Failed to execute due to memory allocation issues: {}", std::string(e.what()));
				}
				// write execution time of iteration
				PROF2_1("{}[Events] [CheckActors] execution time for {} actors: {} µs", actors.size(), std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("{}[Events] [CheckActors] checked {} actors", std::to_string(actors.size()));
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
	/// Callback on saving the game
	/// </summary>
	/// <param name=""></param>
	void Main::SaveGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [SaveGameCallback]");
	}

	/// <summary>
	/// Callback on loading a save game, initializes actor processing
	/// </summary>
	/// <param name=""></param>
	void Main::LoadGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [LoadGameCallback]");
		auto begin = std::chrono::steady_clock::now();
		LOG_1("{}[Events] [LoadGameSub]");
		// if we canceled the main thread, reset that
		stopactorhandler = false;
		initialized = false;
		loaded = true;

		// checking if player should be handled
		if ((Settings::Player::_playerPotions ||
				Settings::Player::_playerFortifyPotions ||
				Settings::Player::_playerPoisons ||
				Settings::Player::_playerFood)) {
			// inject player into the list and remove him later
			sem.acquire();
			acset.insert(data->FindActor(RE::PlayerCharacter::GetSingleton()));
			sem.release();
			LOG_3("{}[Events] [CheckActors] Adding player to the list");
		}

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

		SKSE::GetTaskInterface()->AddTask([]() {
			PlayerDied((bool)(RE::PlayerCharacter::GetSingleton()->boolBits & RE::Actor::BOOL_BITS::kDead));
		});

		enableProcessing = true;

		/// <summary>
		/// thread which executes varying test functions
		/// </summary>
		static std::thread* testhandler = nullptr;
		static std::thread* removeitemshandler = nullptr;

		if (Settings::Debug::_Test) {
			if (testhandler == nullptr) {
				testhandler = new std::thread(Tests::TestAllCells);
				testhandler->detach();
				LOG_1("{}[Events] [LoadGameSub] Started TestHandler");
			}
		}

		if (Settings::Debug::_CompatibilityRemoveItemsStartup) {
			if (removeitemshandler == nullptr) {
				removeitemshandler = new std::thread(RemoveItemsOnStartup);
				removeitemshandler->detach();
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
						if (Utility::ValidateActor(actor) && Main::IsDead(actor) && !actor->IsPlayerRef()) {
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
	/// Callback on reverting the game. Disables processing and stops all handlers
	/// </summary>
	/// <param name=""></param>
	void Main::RevertGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[Events] [RevertGameCallback]");
		loaded = false;
		enableProcessing = false;
		stopactorhandler = true;
		std::this_thread::sleep_for(10ms);
		if (actorhandler != nullptr)
			actorhandler->~thread();
		LOG1_1("{}[PlayerDead] {}", playerdied);
		// reset actor processing list
		acset.clear();
	}
}
