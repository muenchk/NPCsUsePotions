#include "AlchemyEffect.h"
#include "Settings.h"

AlchemyEffect ConvertToAlchemyEffect(RE::EffectSetting* effect)
{
	return ConvertToAlchemyEffectPrimary(effect);
}

AlchemyEffect ConvertToAlchemyEffectPrimary(RE::EffectSetting* effect)
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

AlchemyEffect ConvertToAlchemyEffectSecondary(RE::EffectSetting* effect)
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

AlchemyEffect ConvertToAlchemyEffectIDs(RE::EffectSetting* effect)
{
	if (effect) {
		AlchemyEffect eff = AlchemyEffect::kNone;
		RE::FormID id = effect->GetFormID();
		if (id == 0x3EAF3)  // FortifyHealth
			eff = AlchemyEffect::kFortifyHealth;
		if (id == 0x3EAF8)  // FortifyMagicka
			eff = AlchemyEffect::kFortifyMagicka;
		if (id == 0x3EAF9)  // FortifyStamina
			eff = AlchemyEffect::kFortifyStamina;
		if (id == 0x73F30)  // Paralysis
			eff = AlchemyEffect::kParalysis;
		if (id == 0xAE722)  // CureDisease
			eff = AlchemyEffect::kCureDisease;
		if (id == 0x109ADD)  // CurePoison
			eff = AlchemyEffect::kCurePoison;
		if (id == 0x3AC2D)  // Waterbreathing
			eff = AlchemyEffect::kWaterbreathing;
		if (id == 0xD6947)  // Persuasion
			eff = AlchemyEffect::kPersuasion;
		// COMPATIBILITY FOR CACO
		if (Settings::Compatibility::CACO::_CompatibilityCACO) {
			// DamageStaminaRavage
			if (id == 0x73F23)
				eff = AlchemyEffect::kStamina;
			// DamageMagickaRavage
			if (id == 0x73F27)
				eff = AlchemyEffect::kMagicka;
		}
		// COMPATIBILITY FOR APOTHECARY
		if (Settings::Compatibility::Apothecary::_CompatibilityApothecary) {
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

AlchemyEffect ConvertToAlchemyEffect(RE::ActorValue val)
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
	case RE::ActorValue::kStaminaRate:
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
	case RE::ActorValue::kAggression:
		return (AlchemyEffect::kFrenzy);
		break;
	case RE::ActorValue::kConfidence:
		return (AlchemyEffect::kFear);
		break;
	case RE::ActorValue::kReflectDamage:
		return (AlchemyEffect::kReflectDamage);
		break;
	case RE::ActorValue::kWaterBreathing:
		return (AlchemyEffect::kWaterbreathing);
		break;
	default:
		return AlchemyEffect::kNone;
		break;
	}
}

RE::ActorValue ConvertAlchemyEffect(AlchemyEffect eff)
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
		return RE::ActorValue::kStaminaRate;
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
		return RE::ActorValue::kAggression;
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


namespace AlchEff
{

	bool IsPotion(AlchemyEffectBase effects)
	{
		return effects & static_cast<AlchemyEffectBase>(AlchemyEffect::kAnyPotion);
	}
	bool IsFortify(AlchemyEffectBase effects)
	{
		return effects & static_cast<AlchemyEffectBase>(AlchemyEffect::kAnyFortify);
	}
	std::vector<AlchemyEffect> GetAlchemyEffects(AlchemyEffectBase effects)
	{
		std::vector<AlchemyEffect> effvec;
		AlchemyEffectBase base = 1;
		for (int i = 0; i < 64; i++) {
			if (effects & (base << i))
				effvec.push_back(static_cast<AlchemyEffect>(base << i));
		}
		return effvec;
	}
}
