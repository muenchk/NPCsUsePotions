
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
		if (acinfo->GetDurHealth() >= 0)
			acinfo->DecDurHealth(Settings::System::_cycletime);
		if (acinfo->GetDurMagicka() >= 0)
			acinfo->DecDurMagicka(Settings::System::_cycletime);
		if (acinfo->GetDurStamina() >= 0)
			acinfo->DecDurStamina(Settings::System::_cycletime);
		if (acinfo->GetDurFortify() >= 0)
			acinfo->DecDurFortify(Settings::System::_cycletime);
		if (acinfo->GetDurRegeneration() >= 0)
			acinfo->DecDurRegeneration(Settings::System::_cycletime);
		if (acinfo->GetGlobalCooldownTimer() >= 0)
			acinfo->DecGlobalCooldownTimer(Settings::System::_cycletime);
	}

	void Main::HandleActorPotions(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->GetHandleActor() == false)
			return;
		if (Settings::Potions::_HandleWeaponSheathedAsOutOfCombat && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorPotions] {}", Utility::PrintForm(acinfo));
		AlchemyEffectBase alch = 0;
		AlchemyEffectBase alch2 = 0;
		AlchemyEffectBase alch3 = 0;
		if (acinfo->GetGlobalCooldownTimer() <= tolerance && (!acinfo->IsPlayer() || Settings::Player::_playerPotions)) {
			LOG_2("{}[Events] [CheckActors] [potions]")
			// get combined effect for magicka, health, and stamina
			if (Settings::Potions::_enableHealthRestoration && acinfo->GetDurHealth() < tolerance && ACM::GetAVPercentage(acinfo->GetActor(), RE::ActorValue::kHealth) < Settings::Potions::_healthThreshold)
				alch = static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth);
			else
				alch = 0;
			if (Settings::Potions::_enableMagickaRestoration && acinfo->GetDurMagicka() < tolerance && ACM::GetAVPercentage(acinfo->GetActor(), RE::ActorValue::kMagicka) < Settings::Potions::_magickaThreshold)
				alch2 = static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka);
			else
				alch2 = 0;
			if (Settings::Potions::_enableStaminaRestoration && acinfo->GetDurMagicka() < tolerance && ACM::GetAVPercentage(acinfo->GetActor(), RE::ActorValue::kStamina) < Settings::Potions::_staminaThreshold)
				alch3 = static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina);
			else
				alch3 = 0;
			// construct combined effect
			if (alch && acinfo->IsVampire())
				alch |= static_cast<AlchemyEffectBase>(AlchemyEffect::kBlood);
			alch |= alch2 | alch3;
			LOG4_4("{}[Events] [CheckActors] check for alchemyeffect {} with current dur health {} dur mag {} dur stam {} ", alch, acinfo->GetDurHealth(), acinfo->GetDurMagicka(), acinfo->GetDurStamina());
			// use potions
			// do the first round
			if (alch != 0 && (Settings::Potions::_UsePotionChance == 100 || rand100(rand) < Settings::Potions::_UsePotionChance)) {
				auto tup = ACM::ActorUsePotion(acinfo, alch, Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation, false);
				LOG1_2("{}[Events] [CheckActors] found potion has Alchemy Effect {}", static_cast<uint64_t>(std::get<1>(tup)));
				if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
					acinfo->SetDurHealth(std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);  // convert to milliseconds
					LOG2_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {}", acinfo->GetDurHealth(), std::get<0>(tup));
				}
				if (static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka) & std::get<1>(tup)) {
					acinfo->SetDurMagicka(std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);
					LOG2_4("{}[Events] [CheckActors] use magicka pot with duration {} and magnitude {}", acinfo->GetDurMagicka(), std::get<0>(tup));
				}
				if (static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina) & std::get<1>(tup)) {
					acinfo->SetDurStamina(std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);
					LOG2_4("{}[Events] [CheckActors] use stamina pot with duration {} and magnitude {}", acinfo->GetDurStamina(), std::get<0>(tup));
				}
				// check if we have a valid duration
				if (std::get<1>(tup) != 0) {
					acinfo->SetGlobalCooldownTimer(comp->GetGlobalCooldown());
				}
			}
		}
	}

	void Main::HandleActorFortifyPotions(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->GetHandleActor() == false)
			return;
		if (Settings::FortifyPotions::_DontUseWithWeaponsSheathed && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorFortifyPotions] {}", Utility::PrintForm(acinfo));
		if (acinfo->GetGlobalCooldownTimer() <= tolerance &&
			Settings::FortifyPotions::_enableFortifyPotions &&
			(!(acinfo->IsPlayer()) || Settings::Player::_playerFortifyPotions)) {
			//LOG_2("{}[Events] [CheckActors] [fortify]");

			if ((acinfo->IsFollower() || acinfo->IsPlayer()) && !(Settings::FortifyPotions::_EnemyNumberThresholdFortify < hostileactors || (acinfo->GetTargetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::FortifyPotions::_EnemyLevelScalePlayerLevelFortify))) {
				return;
			}
			// handle fortify potions
			if ((Settings::FortifyPotions::_UseFortifyPotionChance == 100 || rand100(rand) < Settings::FortifyPotions::_UseFortifyPotionChance)) {
				// general stuff
				uint64_t effects = 0;

				// determine valid regeneration effects
				if (acinfo->GetDurRegeneration() < tolerance) {
					effects |= CalcRegenEffects(acinfo, acinfo->GetCombatData());
				}
				// determine valid fortify effects
				if (acinfo->GetDurFortify() < tolerance) {
					effects |= CalcFortifyEffects(acinfo, acinfo->GetCombatData(), acinfo->GetCombatDataTarget());
				}

				// std::tuple<int, AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffect>>>
				//loginfo("take fortify with effects: {}", Utility::GetHex(effects));
				LOG1_4("{}[Events] [CheckActors] check for fortify potion with effect {}", Utility::GetHex(effects));
				auto tup = ACM::ActorUsePotion(acinfo, effects, Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify, true);
				if (std::get<0>(tup) != -1) {
					acinfo->SetGlobalCooldownTimer(comp->GetGlobalCooldown());
					AlchemyEffectBase eff = std::get<1>(tup);
					if (eff & Base(AlchemyEffect::kAnyRegen)) {
						CalcActorCooldowns(acinfo, eff, std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000);
						LOG3_4("{}[Events] [CheckActors] used regeneration potion with tracked duration {} {} and effect {}", acinfo->GetDurRegeneration(), std::get<0>(tup) * 1000, Utility::ToString(std::get<1>(tup)));
					}
					if (eff & Base(AlchemyEffect::kAnyFortify)) {
						CalcActorCooldowns(acinfo, eff, std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000);
						LOG3_4("{}[Events] [CheckActors] used fortify av potion with tracked duration {} {} and effect {}", acinfo->GetDurFortify(), std::get<0>(tup) * 1000, Utility::ToString(std::get<1>(tup)));
					}
				}
			}
		}
	}

	void Main::HandleActorPoisons(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->GetHandleActor() == false)
			return;
		if (Settings::Poisons::_DontUseWithWeaponsSheathed && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorPoisons] {}", Utility::PrintForm(acinfo));
		if (acinfo->GetDurCombat() > 1000 &&
			acinfo->GetGlobalCooldownTimer() <= tolerance &&
			Settings::Poisons::_enablePoisons &&
			(Settings::Poisons::_UsePoisonChance == 100 || rand100(rand) < Settings::Poisons::_UsePoisonChance) &&
			(!acinfo->IsPlayer() || Settings::Player::_playerPoisons)) {
			LOG_2("{}[Events] [CheckActors] [poisons]");
			// handle poisons
			if (acinfo->GetCombatData() != 0 && (acinfo->GetCombatData() & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
				(acinfo->GetCombatData() & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0 /* &&
									comp->AnPois_FindActorPoison(curr->actor->GetFormID()) == nullptr*/
				&&
				Utility::CanApplyPoison(acinfo)) {
				// handle followers
				// they only use poisons if there are many npcs in the fight, or if the enemies they are targetting
				// have a high enough level, like starting at PlayerLevel*0.8 or so
				if (((acinfo->IsFollower() || acinfo->IsPlayer()) &&
						(Settings::Poisons::_EnemyNumberThreshold < hostileactors || (acinfo->GetTargetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::Poisons::_EnemyLevelScalePlayerLevel))) ||
					acinfo->IsFollower() == false && acinfo->IsPlayer() == false) {
					// time to use some poisons
					uint64_t effects = 0;
					// kResistMagic, kResistFire, kResistFrost, kResistMagic should only be used if the follower is a spellblade
					if (acinfo->GetCombatData() & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) ||
						acinfo->GetCombatData() & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword))) {
						effects |= static_cast<uint64_t>(AlchemyEffect::kResistMagic);
						if (acinfo->GetCombatData() & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
							if (acinfo->GetCombatData() & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire))
								effects |= static_cast<uint64_t>(AlchemyEffect::kResistFire);
							if (acinfo->GetCombatData() & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost))
								effects |= static_cast<uint64_t>(AlchemyEffect::kResistFrost);
							if (acinfo->GetCombatData() & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock))
								effects |= static_cast<uint64_t>(AlchemyEffect::kResistShock);
						}
					}
					// incorporate enemy specific data, player is recognized here
					RE::Actor* target = nullptr;
					if (std::shared_ptr<ActorInfo> tar = acinfo->GetTarget().lock())
						target = tar->GetActor();
					if (target) {
						// we can make the usage dependent on the target
						if (target->GetRace() && target->GetRace()->HasKeyword(Settings::ActorTypeDwarven) || target->GetActorBase() && target->GetActorBase()->HasKeyword(Settings::ActorTypeDwarven))
							return;
						effects |= CalcRegenEffects(acinfo->GetCombatDataTarget());
						effects |= CalcPoisonEffects(acinfo->GetCombatData(), target, acinfo->GetCombatDataTarget());
					} else {
						// we dont have a target so just use any poison
						effects |= static_cast<uint64_t>(AlchemyEffect::kAnyPoison);
					}
					LOG1_4("{}[Events] [CheckActors] check for poison with effect {}", effects);
					auto tup = ACM::ActorUsePoison(acinfo, effects);
					if (std::get<1>(tup) != 0)  // check whether an effect was applied
						acinfo->SetGlobalCooldownTimer(comp->GetGlobalCooldown());
				}
			}
			if (acinfo->GetCombatData() == 0)
				LOG1_2("{}[Events] [CheckActors] couldn't determine combatdata for npc {}", Utility::PrintForm(acinfo));
			// else Mage or Hand to Hand which cannot use poisons
		}
	}

	void Main::HandleActorFood(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == false || acinfo->GetHandleActor() == false)
			return;
		if (Settings::Food::_DisableFollowers && acinfo->IsFollower())
			return;
		if (Settings::Food::_DontUseWithWeaponsSheathed && !acinfo->IsWeaponDrawn())
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorFood] {}", Utility::PrintForm(acinfo));
		if (acinfo->GetGlobalCooldownTimer() <= tolerance &&
			Settings::Food::_enableFood &&
			RE::Calendar::GetSingleton()->GetDaysPassed() >= acinfo->GetNextFoodTime() &&
			(!acinfo->IsPlayer() || Settings::Player::_playerFood) &&
			(Settings::Food::_RestrictFoodToCombatStart == false || acinfo->GetDurCombat() < 2000)) {
			// use food at the beginning of the fight to simulate acinfo npc having eaten
			// calc effects that we want to be applied
			AlchemyEffectBase effects = 0;
			effects |= CalcFortifyEffects(acinfo, acinfo->GetCombatData(), acinfo->GetCombatDataTarget());
			effects |= CalcRegenEffects(acinfo, acinfo->GetCombatData());
			auto [dur, effect] = ACM::ActorUseFood(acinfo, effects, false);
			if (effect == 0) {  // nothing found
				auto tup = acinfo->IsPlayer() && Settings::Player::_DontEatRawFood ? ACM::ActorUseFood(acinfo, false) : ACM::ActorUseFood(acinfo, true);
				dur = std::get<0>(tup);
				effect = std::get<1>(tup);
			}
			if (effect != 0) {
				// if duration is zero (vanilla food) set cooldown to at least 3 minutes
				if (dur == 0)
					acinfo->SetNextFoodTime(RE::Calendar::GetSingleton()->GetDaysPassed() + 180 * RE::Calendar::GetSingleton()->GetTimescale() / 60 / 60 / 24);
				else
					acinfo->SetNextFoodTime(RE::Calendar::GetSingleton()->GetDaysPassed() + dur * RE::Calendar::GetSingleton()->GetTimescale() / 60 / 60 / 24);
			}
			LOG2_2("{}[Events] [CheckActors] current days passed: {}, next food time: {}", std::to_string(RE::Calendar::GetSingleton()->GetDaysPassed()), std::to_string(acinfo->GetNextFoodTime()));
		}
	}

	void Main::HandleActorOOCPotions(std::shared_ptr<ActorInfo> acinfo)
	{
		if (!acinfo->IsValid())
			return;
		if (acinfo->IsInCombat() == true &&
				(Settings::Potions::_HandleWeaponSheathedAsOutOfCombat == false /*if disabled we always use the combat handler*/ ||
					Settings::Potions::_HandleWeaponSheathedAsOutOfCombat == true && acinfo->IsWeaponDrawn() == true /*if weapons are drawn we use the combat handler*/) ||
			acinfo->GetHandleActor() == false)
			return;
		LOG1_1("{}[Events] [CheckActors] [HandleActorOOCPotions] {}", Utility::PrintForm(acinfo));
		// we are only checking for health here
		if (Settings::Potions::_enableHealthRestoration && acinfo->GetGlobalCooldownTimer() <= tolerance && acinfo->GetDurHealth() < tolerance &&
			ACM::GetAVPercentage(acinfo->GetActor(), RE::ActorValue::kHealth) < Settings::Potions::_healthThreshold) {
			auto tup = ACM::ActorUsePotion(acinfo, static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth), Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation, false);
			if (static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth) & std::get<1>(tup)) {
				acinfo->SetDurHealth(std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000);  // convert to milliseconds
				// update global cooldown
				acinfo->SetGlobalCooldownTimer(comp->GetGlobalCooldown());
				LOG2_4("{}[Events] [CheckActors] use health pot with duration {} and magnitude {}", acinfo->GetDurHealth(), std::get<0>(tup));
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
		if (acinfo->GetGlobalCooldownTimer() > tolerance) {
			acinfo->SetHandleActor(false);
			return;
		}
		LOG1_1("{}[Events] [CheckActors] [HandleActorRuntimeData] {}", Utility::PrintForm(acinfo));
		LOG5_2("{}[Events] [CheckActors] [HandleActorRuntimeData] cooldowns: durHealth:{}\tdurMagicka:{}\tdurStamina:{}\tdurFortify:{}\tdurRegen:{}", acinfo->GetDurHealth(), acinfo->GetDurMagicka(), acinfo->GetDurStamina(), acinfo->GetDurFortify(), acinfo->GetDurRegeneration());
		// check for staggered option
		// check for paralyzed
		if (comp->DisableItemUsageWhileParalyzed()) {
			if (acinfo->IsParalyzed() ||
				acinfo->IsFlying() ||
				acinfo->IsInKillMove() ||
				acinfo->IsInMidair() ||
				acinfo->IsInRagdollState() ||
				acinfo->IsUnconscious() ||
				acinfo->IsStaggered() ||
				acinfo->IsBleedingOut()) {
				LOG_1("{}[Events] [CheckActors] [Actor] Actor is unable to use items");
				acinfo->SetHandleActor(false);
				return;
			}
		}
		// check for non-follower option
		if (Settings::Usage::_DisableNonFollowerNPCs && acinfo->IsFollower() == false && acinfo->IsPlayer() == false) {
			LOG_2("{}[Events] [CheckActors] [Actor] Actor is not a follower, and non-follower processing has been disabled");
			acinfo->SetHandleActor(false);
			return;
		}

		auto CheckHandle = [](RE::ActorHandle handle) {
			if (handle && handle.get() && handle.get().get())
				return data->FindActor(handle.get().get());
			else
				return std::shared_ptr<ActorInfo>{};
		};

		// reset target
		acinfo->ResetTarget();
		acinfo->SetCombatDataTarget(0);

		// only try to get combat target, if the actor is actually in combat
		if (acinfo->IsInCombat()) {
			// get combatdata of current actor
			acinfo->SetCombatData(Utility::GetCombatData(acinfo->GetActor()));
			LOG1_2("{}[Events] [HandleActorRuntimeData] CombatData: {}", Utility::GetHex(acinfo->GetCombatData()));
			RE::ActorHandle handle;
			if (acinfo->IsPlayer() == false && acinfo->GetActor() != nullptr) {
				// retrieve target of current actor if present
				acinfo->SetTarget(CheckHandle(acinfo->GetActor()->currentCombatTarget));
				if (std::shared_ptr<ActorInfo> tar = acinfo->GetTarget().lock()) {
					// we can make the usage dependent on the target
					acinfo->SetCombatDataTarget(Utility::GetCombatData(tar->GetActor()));
				}
			} else {
				// try to find out the players combat target, since we cannot get it the normal way

				// if we have access to the True Directional Movement API and target lock is activated
				// try to get the actor from there
				if (Settings::Interfaces::tdm_api != nullptr && Settings::Interfaces::tdm_api->GetTargetLockState() == true) {
					acinfo->SetTarget(CheckHandle(Settings::Interfaces::tdm_api->GetCurrentTarget()));
				}
				if (std::shared_ptr<ActorInfo> tar = acinfo->GetTarget().lock()) {
				} else {
					// try to infer the target from the npcs that are in combat
					// get the combatant with the shortest range to player, which is hostile to the player
					auto GetClosestEnemy = []() {
						std::shared_ptr<ActorInfo> current = nullptr;
						for (auto aci : combatants) {
							if (current == nullptr || (aci != nullptr && aci->GetPlayerHostile() && aci->GetPlayerDistance() < current->GetPlayerDistance()))
								current = aci;
						}
						return current;
					};
					acinfo->SetTarget(GetClosestEnemy());
				}
			}
		}

		// if actor is valid and not dead
		if (acinfo->IsValid() && !(acinfo->IsDead()) && acinfo->GetActor() && acinfo->GetActor()->GetActorValue(RE::ActorValue::kHealth) > 0) {
			acinfo->SetHandleActor(true);
		} else
			acinfo->SetHandleActor(false);

		if (acinfo->IsInCombat()) {
			// increase time spent in combat
			acinfo->IncDurCombat(1000);
		} else {
			// reset time spent in combat
			acinfo->SetDurCombat(0);
		}

		// get whether weapons are drawn
		acinfo->UpdateWeaponsDrawn();
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

		std::weak_ptr<ActorInfo> playerweak = data->FindActor(RE::PlayerCharacter::GetSingleton());

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

			PlayerDied((bool)(RE::PlayerCharacter::GetSingleton()->boolBits & RE::Actor::BOOL_BITS::kDead));
			
			// if we are in a paused menu (SoulsRE unpauses menus, which is supported by this)
			// do not compute, since nobody can actually take potions.
			if (!ui->GameIsPaused() && initialized && !playerdied) {
				// get starttime of iteration
				begin = std::chrono::steady_clock::now();

				LOG1_1("{}[Events] [CheckActors] Handling {} registered Actors", std::to_string(acset.size()));

				if (!CanProcess())
					goto CheckActorsSkipIteration;

				if (std::shared_ptr<ActorInfo> playerinfo = playerweak.lock()) {
					if (playerinfo->GetActor()) {
						// store position of player character
						ActorInfo::SetPlayerPosition(playerinfo->GetActor()->GetPosition());
						// reset player combat state, we don't want to include them in our checks
						playerinfo->SetCombatState(CombatState::OutOfCombat);
					}
				}

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
					actorsincombat = 0;
					hostileactors = 0;

					// update combat status of player
					if (std::shared_ptr<ActorInfo> playerinfo = playerweak.lock()) {
						// the player should always be valid. If they don't the game doesn't work either anyway
						if (playerinfo->GetActor()->IsInCombat()) {
							playerinfo->SetCombatState(CombatState::InCombat);
							// if player is in combat, decrease actorsincombat by 1 to ensure that the player won't affect
							// the statistics
							actorsincombat--;
						} else
							playerinfo->SetCombatState(CombatState::OutOfCombat);
					}

					// first decrease all cooldowns for all registered actors
					// decreasing durations
					//
					// calc actors in combat
					// number of actors currently in combat, does not account for multiple combats taking place that are not related to each other
					std::for_each(actors.begin(), actors.end(), [](std::weak_ptr<ActorInfo> acweak) {
						if (std::shared_ptr<ActorInfo> acinfo = acweak.lock()) {
							DecreaseActorCooldown(acinfo);
							// retrieve runtime data
							HandleActorRuntimeData(acinfo);
							if (acinfo->IsInCombat()) {
								actorsincombat++;
								combatants.push_front(acinfo);
								if (acinfo->GetPlayerHostile())
									hostileactors++;
							}
						}
					});

					if (!CanProcess())
						goto CheckActorsSkipIteration;
					if (IsPlayerDead())
						break;

					// collect actor runtime data
					std::for_each(actors.begin(), actors.end(), [](std::weak_ptr<ActorInfo> acweak) {
						if (std::shared_ptr<ActorInfo> acinfo = acweak.lock()) {
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
		PlayerDied((bool)(RE::PlayerCharacter::GetSingleton()->boolBits & RE::Actor::BOOL_BITS::kDead));

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
	/// Callback on reverting the game. Disables processing and stops all handlers
	/// </summary>
	/// <param name=""></param>
	void Main::RevertGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
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
}
