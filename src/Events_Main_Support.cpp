
#include "ActorManipulation.h"
#include "Events.h"
#include "Utility.h"

namespace Events
{
#define EvalProcessing()   \
	if (!CanProcess()) \
		return;

	/// <summary>
	/// initializes important variables, which need to be initialized every time a game is loaded
	/// </summary>
	void Main::InitializeCompatibilityObjects()
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
	void Main::CalcActorCooldowns(std::shared_ptr<ActorInfo> acinfo, AlchemyEffectBase effect, int dur)
	{
		EvalProcessing();
		if (effect & static_cast<uint64_t>(AlchemyEffect::kHealth)) {
			acinfo->SetDurHealth(dur);
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kMagicka)) {
			acinfo->SetDurMagicka(dur);
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kStamina)) {
			acinfo->SetDurStamina(dur);
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kAnyRegen)) {
			acinfo->SetDurRegeneration(dur);
		}
		if (effect & static_cast<uint64_t>(AlchemyEffect::kAnyFortify)) {
			acinfo->SetDurFortify(dur);
		}
	}

	/// <summary>
	/// Calculates poison effects based on [combatdata], [target], and [tcombatdata]
	/// </summary>
	/// <param name="combatdata">combatdata of the actor using poison</param>
	/// <param name="target">target</param>
	/// <param name="tcombatdata">combatdata of the target</param>
	/// <returns>valid poison effects</returns>
	uint64_t Main::CalcPoisonEffects(uint32_t combatdata, RE::Actor* target, uint32_t tcombatdata)
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
	uint64_t Main::CalcFortifyEffects(std::shared_ptr<ActorInfo> acinfo, uint32_t combatdata, uint32_t tcombatdata)
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
		if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHanded)) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kOneHanded) |
			           static_cast<uint64_t>(AlchemyEffect::kBlock) |
			           static_cast<uint64_t>(AlchemyEffect::kMeleeDamage) |
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
		uint32_t armordata = Utility::GetArmorData(acinfo->GetActor());
		if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::LightArmor))
			effects |= static_cast<uint64_t>(AlchemyEffect::kLightArmor);
		if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::HeavyArmor))
			effects |= static_cast<uint64_t>(AlchemyEffect::kHeavyArmor);

		// shield potions
		effects |= static_cast<uint64_t>(AlchemyEffect::kShield);
		return effects;
	}

	
	/// <summary>
	/// Calculates all regeneration effects that an actor is equitable for, based on their combat data
	/// </summary>
	/// <param name="combatdata">combatdata of the actor</param>
	/// <returns>valid regeneration effects</returns>
	uint64_t Main::CalcRegenEffects(uint32_t combatdata)
	{
		LOG_4("{}[Events] [CalcRegenEffects]");
		uint64_t effects = 0;
		effects |= static_cast<uint64_t>(AlchemyEffect::kHealRate) |
		           static_cast<uint64_t>(AlchemyEffect::kHealRateMult);

		if (combatdata &
			(static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword))) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kMagickaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kMagickaRateMult) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
		}
		if (combatdata &
			(static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield) |
				static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand))) {
			effects |= static_cast<uint64_t>(AlchemyEffect::kStaminaRate) |
			           static_cast<uint64_t>(AlchemyEffect::kStaminaRateMult);
		}
		if (combatdata &
			(static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage) |
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
	uint64_t Main::CalcRegenEffects(std::shared_ptr<ActorInfo> /*acinfo*/, uint32_t combatdata)
	{
		return CalcRegenEffects(combatdata);
	}

	/// <summary>
	/// Processes the item distribution for an actor
	/// </summary>
	/// <param name="acinfo"></param>
	void Main::ProcessDistribution(std::shared_ptr<ActorInfo> acinfo)
	{
		// check wether this charackter maybe a follower
		if (acinfo->GetLastDistrTime() == 0.0f || RE::Calendar::GetSingleton()->GetDaysPassed() - acinfo->GetLastDistrTime() > 1) {
			if (!Distribution::ExcludedNPC(acinfo)) {
				// begin with compatibility mode removing items before distributing new ones
				if (Settings::Debug::_CompatibilityRemoveItemsBeforeDist) {
					auto items = ACM::GetAllPotions(acinfo);
					auto it = items.begin();
					while (it != items.end()) {
						acinfo->RemoveItem(*it, 1);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", Utility::PrintForm(*it));
						it++;
					}
					items = ACM::GetAllPoisons(acinfo);
					it = items.begin();
					while (it != items.end()) {
						acinfo->RemoveItem(*it, 1);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", Utility::PrintForm(*it));
						it++;
					}
					items = ACM::GetAllFood(acinfo);
					it = items.begin();
					while (it != items.end()) {
						acinfo->RemoveItem(*it, 1);
						LOG1_1("{}[Events] [ProcessDistribution] Removed item {}", Utility::PrintForm(*it));
						it++;
					}
				}

				// if we have characters that should not get items, the function
				// just won't return anything, but we have to check for standard factions like CurrentFollowerFaction
				auto items = Distribution::GetDistrItems(acinfo);
				if (acinfo->IsDead()) {
					return;
				}
				if (items.size() > 0) {
					for (int i = 0; i < items.size(); i++) {
						if (items[i] == nullptr) {
							continue;
						}
						acinfo->AddItem(items[i], 1);
						LOG2_4("{}[Events] [ProcessDistribution] added item {} to actor {}", Utility::PrintForm(items[i]), Utility::PrintForm(acinfo));
					}
					acinfo->SetLastDistrTime(RE::Calendar::GetSingleton()->GetDaysPassed());
				}
			}
		}
	}

	/// <summary>
	/// Registers an NPC for handling
	/// </summary>
	/// <param name="actor"></param>
	void Main::RegisterNPC(RE::Actor* actor)
	{
		EvalProcessing();
		// exit if the actor is unsafe / not valid
		if (Utility::ValidateActor(actor) == false)
			return;
		LOG1_1("{}[Events] [RegisterNPC] Trying to register new actor for potion tracking: {}", Utility::PrintForm(actor));
		std::shared_ptr<ActorInfo> acinfo = data->FindActor(actor);
		LOG1_1("{}[Events] [RegisterNPC] Found: {}", Utility::PrintForm(acinfo));
		// if actor was deleted, exit
		if (acinfo->GetDeleted()) {
			LOG_1("{}[Events] [RegisterNPC] Actor already deleted");
			return;
		}
		// reset object to account for changes to underlying objects
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
			LOG_1("{}[Events] [RegisterNPC] Actor already registered");
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
	void Main::UnregisterNPC(RE::Actor* actor)
	{
		EvalProcessing();
		// exit if actor is unsafe / not valid
		if (Utility::ValidateActor(actor) == false)
			return;
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", Utility::PrintForm(actor));
		std::shared_ptr<ActorInfo> acinfo = data->FindActor(actor);
		sem.acquire();
		acset.erase(acinfo);
		acinfo->SetDurHealth(0);
		acinfo->SetDurMagicka(0);
		acinfo->SetDurStamina(0);
		acinfo->SetDurFortify(0);
		acinfo->SetDurRegeneration(0);
		sem.release();
		LOG_1("{}[Events] [UnregisterNPC] Unregistered NPC");
	}

	/// <summary>
	/// Unregisters an NPC from handling
	/// </summary>
	/// <param name="acinfo"></param>
	void Main::UnregisterNPC(std::shared_ptr<ActorInfo> acinfo)
	{
		EvalProcessing();
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", acinfo->GetName());
		sem.acquire();
		acset.erase(acinfo);
		acinfo->SetDurHealth(0);
		acinfo->SetDurMagicka(0);
		acinfo->SetDurStamina(0);
		acinfo->SetDurFortify(0);
		acinfo->SetDurRegeneration(0);
		sem.release();
	}

	/// <summary>
	/// Unregisters an NPC from handling
	/// </summary>
	/// <param name="acinfo"></param>
	void Main::UnregisterNPC(RE::FormID formid)
	{
		EvalProcessing();
		LOG1_1("{}[Events] [UnregisterNPC] Unregister NPC from potion tracking: {}", Utility::GetHex(formid));
		sem.acquire();
		auto itr = acset.begin();
		while (itr != acset.end()) {
			if (std::shared_ptr<ActorInfo> acinfo = itr->lock()) {
				if (acinfo->GetFormIDBlank() == formid) {
					acset.erase(itr);
					break;
				}
			} else {
				// weak pointer is expired, so remove it while we are on it
				acset.erase(itr);
			}
			itr++;
		}
		sem.release();
	}

	bool Main::IsDead(RE::Actor* actor)
	{
		return actor == nullptr || deads.contains(actor->GetFormID()) || actor->boolBits & RE::Actor::BOOL_BITS::kDead;
	}

	bool Main::IsDeadEventFired(RE::Actor* actor)
	{
		return actor == nullptr || deads.contains(actor->GetFormID());
	}

	void Main::SetDead(RE::Actor* actor)
	{
		if (actor != nullptr)
			deads.insert(actor->GetFormID());
	}

	/// <summary>
	/// Removes all distributable alchemy items from all actors in the game on loading a game
	/// </summary>
	void Main::RemoveItemsOnStartup()
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
						std::shared_ptr<ActorInfo> acinfo = data->FindActor(actor);
						auto items = ACM::GetAllPotions(acinfo);
						auto it = items.begin();
						while (it != items.end()) {
							if (Settings::Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded && !(Distribution::excludedItems()->contains((*it)->GetFormID()))) {
								it++;
								continue;
							}
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
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
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
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
							actor->RemoveItem(*it, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
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

}
