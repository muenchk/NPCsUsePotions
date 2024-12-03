#include "AlchemyEffect.h"
#include "Compatibility.h"
#include "Settings.h"
#include "Distribution.h"
#include "Utility.h"
#include <string.h>

AlchemicEffect ConvertToAlchemyEffect(RE::EffectSetting* effect)
{
	return ConvertToAlchemyEffectPrimary(effect);
}

AlchemicEffect ConvertToAlchemyEffectPrimary(RE::EffectSetting* effect)
{
	if (effect) {
		AlchemicEffect eff = ConvertToAlchemyEffect(effect->data.primaryAV);
		auto eff2 = ConvertToAlchemyEffectIDs(effect);
		if (eff2 != AlchemicEffect::kNone)
			return eff2;
		else
			return eff;
	}
	return AlchemicEffect::kNone;
}

AlchemicEffect ConvertToAlchemyEffectSecondary(RE::EffectSetting* effect)
{
	if (effect) {
		AlchemicEffect eff = ConvertToAlchemyEffect(effect->data.secondaryAV);
		return eff;
	}
	return AlchemicEffect::kNone;
}

AlchemicEffect ConvertToAlchemyEffectIDs(RE::EffectSetting* effect)
{
	if (effect) {
		AlchemicEffect eff = AlchemicEffect::kNone;
		RE::FormID id = effect->GetFormID();
		if (id == 0x3EAF3)  // FortifyHealth
			eff = AlchemicEffect::kFortifyHealth;
		if (id == 0x3EAF8)  // FortifyMagicka
			eff = AlchemicEffect::kFortifyMagicka;
		if (id == 0x3EAF9)  // FortifyStamina
			eff = AlchemicEffect::kFortifyStamina;
		if (id == 0x73F30)  // Paralysis
			eff = AlchemicEffect::kParalysis;
		if (id == 0xAE722)  // CureDisease
			eff = AlchemicEffect::kCureDisease;
		if (id == 0x109ADD)  // CurePoison
			eff = AlchemicEffect::kCurePoison;
		if (id == 0x3AC2D)  // Waterbreathing
			eff = AlchemicEffect::kWaterbreathing;
		if (id == 0xD6947)  // Persuasion
			eff = AlchemicEffect::kPersuasion;
		// COMPATIBILITY FOR CACO
		if (Compatibility::GetSingleton()->LoadedCACO()) {
			// DamageStaminaRavage
			if (id == 0x73F23)
				eff = AlchemicEffect::kStamina;
			// DamageMagickaRavage
			if (id == 0x73F27)
				eff = AlchemicEffect::kMagicka;
		}
		// COMPATIBILITY FOR APOTHECARY
		if (Compatibility::GetSingleton()->LoadedApothecary()) {
			// DamageWeapon
			if (id == 0x73F26)
				eff = AlchemicEffect::kAttackDamageMult;
			// Silence
			if (id == 0x73F2B)
				eff = AlchemicEffect::kMagickaRate;
		}
		auto itr = Distribution::magicEffectAlchMap()->find(id);
		if (itr != Distribution::magicEffectAlchMap()->end()) {
			eff = itr->second;
		}
		return eff;
	}
	return AlchemicEffect::kNone;
}

AlchemicEffect ConvertToAlchemyEffect(RE::ActorValue val)
{
	switch (val) {
	case RE::ActorValue::kHealth:
		return (AlchemicEffect::kHealth);
		break;
	case RE::ActorValue::kMagicka:
		return (AlchemicEffect::kMagicka);
		break;
	case RE::ActorValue::kStamina:
		return (AlchemicEffect::kStamina);
		break;
	case RE::ActorValue::kOneHanded:
	case RE::ActorValue::kOneHandedModifier:
	case RE::ActorValue::kOneHandedPowerModifier:
		return (AlchemicEffect::kOneHanded);
		break;
	case RE::ActorValue::kTwoHanded:
	case RE::ActorValue::kTwoHandedModifier:
	case RE::ActorValue::kTwoHandedPowerModifier:
		return (AlchemicEffect::kTwoHanded);
		break;
	case RE::ActorValue::kArchery:
	case RE::ActorValue::kMarksmanModifier:
	case RE::ActorValue::kMarksmanPowerModifier:
		return (AlchemicEffect::kArchery);
		break;
	case RE::ActorValue::kBlock:
	case RE::ActorValue::kBlockModifier:
	case RE::ActorValue::kBlockPowerModifier:
		return (AlchemicEffect::kBlock);
		break;
	case RE::ActorValue::kSmithing:
	case RE::ActorValue::kSmithingModifier:
	case RE::ActorValue::kSmithingPowerModifier:
		return (AlchemicEffect::kSmithing);
		break;
	case RE::ActorValue::kHeavyArmor:
	case RE::ActorValue::kHeavyArmorModifier:
	case RE::ActorValue::kHeavyArmorPowerModifier:
		return (AlchemicEffect::kHeavyArmor);
		break;
	case RE::ActorValue::kLightArmor:
	case RE::ActorValue::kLightArmorModifier:
	case RE::ActorValue::kLightArmorSkillAdvance:
		return (AlchemicEffect::kLightArmor);
		break;
	case RE::ActorValue::kPickpocket:
	case RE::ActorValue::kPickpocketModifier:
	case RE::ActorValue::kPickpocketPowerModifier:
		return (AlchemicEffect::kPickpocket);
		break;
	case RE::ActorValue::kLockpicking:
	case RE::ActorValue::kLockpickingModifier:
	case RE::ActorValue::kLockpickingPowerModifier:
		return (AlchemicEffect::kLockpicking);
		break;
	case RE::ActorValue::kSneak:
	case RE::ActorValue::kSneakingModifier:
	case RE::ActorValue::kSneakingPowerModifier:
		return (AlchemicEffect::kSneak);
		break;
	case RE::ActorValue::kAlchemy:
	case RE::ActorValue::kAlchemyModifier:
	case RE::ActorValue::kAlchemyPowerModifier:
		return (AlchemicEffect::kAlchemy);
		break;
	case RE::ActorValue::kSpeech:
	case RE::ActorValue::kSpeechcraftModifier:
	case RE::ActorValue::kSpeechcraftPowerModifier:
		return (AlchemicEffect::kSpeech);
		break;
	case RE::ActorValue::kAlteration:
	case RE::ActorValue::kAlterationModifier:
	case RE::ActorValue::kAlterationPowerModifier:
		return (AlchemicEffect::kAlteration);
		break;
	case RE::ActorValue::kConjuration:
	case RE::ActorValue::kConjurationModifier:
	case RE::ActorValue::kConjurationPowerModifier:
		return (AlchemicEffect::kConjuration);
		break;
	case RE::ActorValue::kDestruction:
	case RE::ActorValue::kDestructionModifier:
	case RE::ActorValue::kDestructionPowerModifier:
		return (AlchemicEffect::kDestruction);
		break;
	case RE::ActorValue::kIllusion:
	case RE::ActorValue::kIllusionModifier:
	case RE::ActorValue::kIllusionPowerModifier:
		return (AlchemicEffect::kIllusion);
		break;
	case RE::ActorValue::kRestoration:
	case RE::ActorValue::kRestorationModifier:
	case RE::ActorValue::kRestorationPowerModifier:
		return (AlchemicEffect::kRestoration);
		break;
	case RE::ActorValue::kEnchanting:
	case RE::ActorValue::kEnchantingModifier:
	case RE::ActorValue::kEnchantingPowerModifier:
		return (AlchemicEffect::kEnchanting);
		break;
	case RE::ActorValue::kHealRate:
		return (AlchemicEffect::kHealRate);
		break;
	case RE::ActorValue::kMagickaRate:
		return (AlchemicEffect::kMagickaRate);
		break;
	case RE::ActorValue::kStaminaRate:
		return (AlchemicEffect::kStaminaRate);
		break;
	case RE::ActorValue::kSpeedMult:
		return (AlchemicEffect::kSpeedMult);
		break;
	//case RE::ActorValue::kInventoryWeight:
	//	break;
	case RE::ActorValue::kCarryWeight:
		return (AlchemicEffect::kCarryWeight);
		break;
	case RE::ActorValue::kCriticalChance:
		return (AlchemicEffect::kCriticalChance);
		break;
	case RE::ActorValue::kMeleeDamage:
		return (AlchemicEffect::kMeleeDamage);
		break;
	case RE::ActorValue::kUnarmedDamage:
		return (AlchemicEffect::kUnarmedDamage);
		break;
	case RE::ActorValue::kDamageResist:
		return (AlchemicEffect::kDamageResist);
		break;
	case RE::ActorValue::kPoisonResist:
		return (AlchemicEffect::kPoisonResist);
		break;
	case RE::ActorValue::kResistFire:
		return (AlchemicEffect::kResistFire);
		break;
	case RE::ActorValue::kResistShock:
		return (AlchemicEffect::kResistShock);
		break;
	case RE::ActorValue::kResistFrost:
		return (AlchemicEffect::kResistFrost);
		break;
	case RE::ActorValue::kResistMagic:
		return (AlchemicEffect::kResistMagic);
		break;
	case RE::ActorValue::kResistDisease:
		return (AlchemicEffect::kResistDisease);
		break;
	case RE::ActorValue::kParalysis:
		return (AlchemicEffect::kParalysis);
		break;
	case RE::ActorValue::kInvisibility:
		return (AlchemicEffect::kInvisibility);
		break;
	case RE::ActorValue::kWeaponSpeedMult:
	case RE::ActorValue::kLeftWeaponSpeedMultiply:
		return (AlchemicEffect::kWeaponSpeedMult);
		break;
	case RE::ActorValue::kBowSpeedBonus:
		return (AlchemicEffect::kBowSpeed);
		break;
	case RE::ActorValue::kAttackDamageMult:
		return (AlchemicEffect::kAttackDamageMult);
		break;
	case RE::ActorValue::kHealRateMult:
		return (AlchemicEffect::kHealRateMult);
		break;
	case RE::ActorValue::kMagickaRateMult:
		return (AlchemicEffect::kMagickaRateMult);
		break;
	case RE::ActorValue::kStaminaRateMult:
		return (AlchemicEffect::kStaminaRateMult);
		break;
	case RE::ActorValue::kAggression:
		return (AlchemicEffect::kFrenzy);
		break;
	case RE::ActorValue::kConfidence:
		return (AlchemicEffect::kFear);
		break;
	case RE::ActorValue::kReflectDamage:
		return (AlchemicEffect::kReflectDamage);
		break;
	case RE::ActorValue::kWaterBreathing:
		return (AlchemicEffect::kWaterbreathing);
		break;
	default:
		return AlchemicEffect::kNone;
		break;
	}
}

RE::ActorValue ConvertAlchemyEffect(AlchemicEffect eff)
{
	// check whether there are higher order effects, and otherwise evaluate lower order effects
	if (eff.AlchemyBaseEffectFirst() != AlchemyBaseEffectFirst::kNone)
	{
		switch (eff.AlchemyBaseEffectFirst())
		{
		case AlchemyBaseEffectFirst::kNone:
			return RE::ActorValue::kNone;
		default:
			return RE::ActorValue::kNone;
		}
	} else {
		switch (eff.AlchemyBaseEffectSecond()) {
		case AlchemyBaseEffectSecond::kNone:
			return RE::ActorValue::kNone;
			break;
		case AlchemyBaseEffectSecond::kHealth:
			return RE::ActorValue::kHealth;
			break;
		case AlchemyBaseEffectSecond::kMagicka:
			return RE::ActorValue::kMagicka;
			break;
		case AlchemyBaseEffectSecond::kStamina:
			return RE::ActorValue::kStamina;
			break;
		case AlchemyBaseEffectSecond::kOneHanded:
			return RE::ActorValue::kOneHanded;
			break;
		case AlchemyBaseEffectSecond::kTwoHanded:
			return RE::ActorValue::kTwoHanded;
			break;
		case AlchemyBaseEffectSecond::kArchery:
			return RE::ActorValue::kArchery;
			break;
		case AlchemyBaseEffectSecond::kHeavyArmor:
			return RE::ActorValue::kHeavyArmor;
			break;
		case AlchemyBaseEffectSecond::kLightArmor:
			return RE::ActorValue::kLightArmor;
			break;
		case AlchemyBaseEffectSecond::kAlteration:
			return RE::ActorValue::kAlteration;
			break;
		case AlchemyBaseEffectSecond::kConjuration:
			return RE::ActorValue::kConjuration;
			break;
		case AlchemyBaseEffectSecond::kDestruction:
			return RE::ActorValue::kDestruction;
			break;
		case AlchemyBaseEffectSecond::kIllusion:
			return RE::ActorValue::kIllusion;
			break;
		case AlchemyBaseEffectSecond::kRestoration:
			return RE::ActorValue::kRestoration;
			break;
		case AlchemyBaseEffectSecond::kHealRate:
			return RE::ActorValue::kHealRate;
			break;
		case AlchemyBaseEffectSecond::kMagickaRate:
			return RE::ActorValue::kMagickaRate;
			break;
		case AlchemyBaseEffectSecond::kStaminaRate:
			return RE::ActorValue::kStaminaRate;
			break;
		case AlchemyBaseEffectSecond::kSpeedMult:
			return RE::ActorValue::kSpeedMult;
			break;
		case AlchemyBaseEffectSecond::kCriticalChance:
			return RE::ActorValue::kCriticalChance;
			break;
		case AlchemyBaseEffectSecond::kMeleeDamage:
			return RE::ActorValue::kMeleeDamage;
			break;
		case AlchemyBaseEffectSecond::kUnarmedDamage:
			return RE::ActorValue::kUnarmedDamage;
			break;
		case AlchemyBaseEffectSecond::kDamageResist:
			return RE::ActorValue::kDamageResist;
			break;
		case AlchemyBaseEffectSecond::kPoisonResist:
			return RE::ActorValue::kPoisonResist;
			break;
		case AlchemyBaseEffectSecond::kResistFire:
			return RE::ActorValue::kResistFire;
			break;
		case AlchemyBaseEffectSecond::kResistShock:
			return RE::ActorValue::kResistShock;
			break;
		case AlchemyBaseEffectSecond::kResistFrost:
			return RE::ActorValue::kResistFrost;
			break;
		case AlchemyBaseEffectSecond::kResistMagic:
			return RE::ActorValue::kResistMagic;
			break;
		case AlchemyBaseEffectSecond::kResistDisease:
			return RE::ActorValue::kResistDisease;
			break;
		case AlchemyBaseEffectSecond::kParalysis:
			return RE::ActorValue::kParalysis;
			break;
		case AlchemyBaseEffectSecond::kInvisibility:
			return RE::ActorValue::kInvisibility;
			break;
		case AlchemyBaseEffectSecond::kWeaponSpeedMult:
			return RE::ActorValue::kWeaponSpeedMult;
			break;
		case AlchemyBaseEffectSecond::kAttackDamageMult:
			return RE::ActorValue::kAttackDamageMult;
			break;
		case AlchemyBaseEffectSecond::kBowSpeed:
			return RE::ActorValue::kBowSpeedBonus;
			break;
		case AlchemyBaseEffectSecond::kHealRateMult:
			return RE::ActorValue::kHealRateMult;
			break;
		case AlchemyBaseEffectSecond::kMagickaRateMult:
			return RE::ActorValue::kMagickaRateMult;
			break;
		case AlchemyBaseEffectSecond::kStaminaRateMult:
			return RE::ActorValue::kStaminaRateMult;
			break;
		case AlchemyBaseEffectSecond::kPickpocket:
			return RE::ActorValue::kPickpocket;
			break;
		case AlchemyBaseEffectSecond::kLockpicking:
			return RE::ActorValue::kLockpicking;
			break;
		case AlchemyBaseEffectSecond::kSneak:
			return RE::ActorValue::kSneak;
			break;
		case AlchemyBaseEffectSecond::kFear:
			return RE::ActorValue::kConfidence;
			break;
		case AlchemyBaseEffectSecond::kFrenzy:
			return RE::ActorValue::kAggression;
			break;
		case AlchemyBaseEffectSecond::kReflectDamage:
			return RE::ActorValue::kReflectDamage;
			break;
		case AlchemyBaseEffectSecond::kEnchanting:
			return RE::ActorValue::kEnchanting;
			break;
		case AlchemyBaseEffectSecond::kSmithing:
			return RE::ActorValue::kSmithing;
			break;
		case AlchemyBaseEffectSecond::kSpeech:
			return RE::ActorValue::kSpeech;
		case AlchemyBaseEffectSecond::kCarryWeight:
			return RE::ActorValue::kCarryWeight;
			break;
		case AlchemyBaseEffectSecond::kAlchemy:
			return RE::ActorValue::kAlchemy;
			break;
		//case AlchemyBaseEffectSecond::kCureDisease:
		//case AlchemyBaseEffectSecond::kCurePoison:
		//case AlchemyBaseEffectSecond::kWaterbreathing:
		//case AlchemyBaseEffectSecond::kPersuasion:
		default:
			return RE::ActorValue::kNone;
			break;
		}
	}
}


namespace AlchEff
{

	bool IsPotion(AlchemicEffect effects)
	{
		return (effects & AlchemicEffect::kAnyPotion).IsValid();
	}
	bool IsFortify(AlchemicEffect effects)
	{
		return (effects & AlchemicEffect::kAnyFortify).IsValid();
	}
	std::vector<AlchemicEffect> GetAlchemyEffects(AlchemicEffect effects)
	{
		std::vector<AlchemicEffect> effvec;
		AlchemicEffect base = 1;
		for (int i = 0; i < 64; i++) {
			if ((effects & (base << i)).IsValid())
				effvec.push_back(static_cast<AlchemicEffect>(base << i));
		}
		return effvec;
	}
}

static ::Compatibility* comp;

void AlchemicEffect::Init()
{
	comp = Compatibility::GetSingleton();
}

bool AlchemicEffect::operator==(const AlchemicEffect& rhs) const
{
	// return true if all values are equal
	return first == rhs.first && second == rhs.second;
}

bool AlchemicEffect::operator==(const int& rhs) const
{
	
	return first == 0 && second == rhs;
}

bool AlchemicEffect::operator<(const AlchemicEffect& rhs) const
{
	return first ? (first < rhs.first) : (rhs.first > 0 ? true : second < rhs.second);
}

bool AlchemicEffect::operator>(const int& rhs) const
{
	return first ? true : (second > rhs);
}


bool AlchemicEffect::operator!() const
{
	// return true if both value are zero
	return first == 0 && second == 0;
}

bool AlchemicEffect::operator&&(const AlchemicEffect& rhs) const
{
	// return true if both objects have a value != 0
	return (first != 0 || second != 0) && (rhs.first != 0 || rhs.second != 0);
}

bool AlchemicEffect::operator&&(const bool& rhs) const
{
	// return true if both objects have a value != 0
	return (first != 0 || second != 0) && rhs;
}

bool AlchemicEffect::operator||(const AlchemicEffect& rhs) const
{
	// return true one of the objects has a value != 0
	return (first != 0 || second != 0) || (rhs.first != 0 || rhs.second != 0);
}

AlchemicEffect AlchemicEffect::operator~() const
{
	return AlchemicEffect(~first, ~second);
}

AlchemicEffect AlchemicEffect::operator&(const AlchemicEffect& rhs) const
{
	return AlchemicEffect(first & rhs.first, second & rhs.second);
}

/* AlchemicEffect operator&(const AlchemicEffect& lhs, const AlchemicEffect& rhs)
{
	return AlchemicEffect(lhs.first & rhs.first, lhs.second & rhs.second);
}*/

AlchemicEffect AlchemicEffect::operator|(const AlchemicEffect& rhs) const
{
	return AlchemicEffect(first | rhs.first, second | rhs.second);
}

/* AlchemicEffect operator|(const AlchemicEffect& lhs, const AlchemicEffect& rhs)
{
	return AlchemicEffect(lhs.first | rhs.first, lhs.second | rhs.second);
}*/

AlchemicEffect AlchemicEffect::operator^(const AlchemicEffect& rhs) const
{
	return AlchemicEffect(first ^ rhs.first, second ^ rhs.second);
}

/* AlchemicEffect operator^(const AlchemicEffect& lhs, const AlchemicEffect& rhs)
{
	return AlchemicEffect(lhs.first ^ rhs.first, lhs.second ^ rhs.second);
}*/

AlchemicEffect AlchemicEffect::ShiftLeft(const uint64_t& shift) const
{
	AlchemicEffect alc;
	if (shift > 64) {
		// we are shifting second into first completely and more
		// just replace first by second and shift less
		alc.first = second << (shift - 64);
		alc.second = 0;
	} else if (shift == 64) {
		// we are shifting second into first
		alc.first = second;
		alc.second = 0;
	} else if (shift == 0) {
		alc.first = first;
		alc.second = second;
	} else {
		// we shift less than 64 bytes, thus we need an intermediary
		// save bytes shifted out
		uint64_t tmp = second >> (64 - shift);
		// shift second
		alc.second = second << (shift);
		// shift first
		alc.first = first << shift;
		// merge tmp into first
		alc.first |= tmp;
	}
	return alc;
}

AlchemicEffect AlchemicEffect::operator<<(const uint64_t& shift) const
{
	return ShiftLeft(shift);
}

AlchemicEffect AlchemicEffect::operator<<(const int& shift) const
{
	return ShiftLeft(shift);
}

AlchemicEffect& AlchemicEffect::operator=(const AlchemicEffect& rhs)
{
	first = rhs.first;
	second = rhs.second;
	return *this;
}

AlchemicEffect::AlchemicEffect(const std::string& rhs)
{
	if (rhs.size() > 16) {
		int length = (int)rhs.size() - 16;
		try {
			first = std::stoull(rhs.substr(0, length), nullptr, 16);
			second = std::stoull(rhs.substr(length, 16), nullptr, 16);
		} catch (std::exception& e) {
			//throw(InitializationError("Cannot convert string to AlchemicEffect", e.what()));
		}
	} else {
		first = 0;
		try {
			second = std::stoull(rhs, nullptr, 16);
		} catch (std::exception& e) {
			//throw(InitializationError("Cannot convert string to AlchemicEffect", e.what()));
		}
	}
}

AlchemicEffect::AlchemicEffect(const uint64_t& rhs)
{
	first = 0;
	second = rhs;
}

AlchemicEffect::AlchemicEffect(const int& rhs)
{
	first = 0;
	second = rhs;
}

AlchemicEffect::AlchemicEffect(const ::AlchemyBaseEffectSecond& rhs)
{
	first = 0;
	second = static_cast<::AlchemyBaseEffect>(rhs);
}

AlchemicEffect& AlchemicEffect::operator&=(const AlchemicEffect& rhs)
{
	first &= rhs.first;
	second &= rhs.second;
	return *this;
}

AlchemicEffect& AlchemicEffect::operator|=(const AlchemicEffect& rhs)
{
	first |= rhs.first;
	second |= rhs.second;
	return *this;
}

AlchemicEffect& AlchemicEffect::operator^=(const AlchemicEffect& rhs)
{
	first ^= rhs.first;
	second ^= rhs.second;
	return *this;
}

AlchemicEffect::operator std::string()
{
	return first ? Utility::GetHex(first) + Utility::GetHexFill(second) : Utility::GetHex(second);
}

std::string AlchemicEffect::string()
{
	return first ? Utility::GetHex(first) + Utility::GetHexFill(second) : Utility::GetHex(second);
}

bool AlchemicEffect::IsValid()
{
	return first != 0 || second != 0;
}

AlchemyBaseEffectSecond AlchemicEffect::AlchemyBaseEffectSecond()
{
	if (first > 0)
		return ::AlchemyBaseEffectSecond::kNone;
	else
		return static_cast<::AlchemyBaseEffectSecond>(second);
}

AlchemyBaseEffectFirst AlchemicEffect::AlchemyBaseEffectFirst()
{
	if (second > 0)
		return ::AlchemyBaseEffectFirst::kNone;
	else
		return static_cast<::AlchemyBaseEffectFirst>(first);
}

unsigned long AlchemicEffect::GetBaseValue()
{
	// if this is not a single effect, return 0
	if (!IsEffect())
		return 0;
	static const int MultiplyDeBruijnBitPosition[32] = { // #stackoverflow #Tykhyy
		0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
		31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
	};
	unsigned long index = 0;
	if (first != 0) {
		_BitScanForward64(&index, first);
		return index + 64;
	} else {
		_BitScanForward64(&index, second);
		return index + 1;
	}
}

AlchemicEffect AlchemicEffect::GetFromBaseValue(unsigned long basevalue)
{
	if (basevalue == 0 || basevalue > 128)
		return {};
	basevalue -= 1;
	if (basevalue >= 64) {
		return AlchemicEffect((uint64_t)1 << (basevalue - 64), 0);
	} else
		return AlchemicEffect(0, (uint64_t)1 << basevalue);
}

bool AlchemicEffect::IsEffect()
{
	// we only need to check one of the values. 
	// If first has a value, second cannot have one, so just check this
	if (first)
	{
		if (second)
			return false;
		return (first & (first - 1)) == 0;
	} else {
		// only have second to check
		return (second & (second - 1)) == 0;
	}
}

bool AlchemicEffect::IsEffectMap()
{
	return !IsEffect();
}

bool AlchemicEffect::HasEffect(unsigned long basevalue)
{
	if (basevalue == 0 || basevalue > 128)
		return false;
	basevalue -= 1;
	if (basevalue >= 64)
		return ((first & ((uint64_t)1 << (basevalue - 64))) != 0);
	else
		return ((second & ((uint64_t)1 << basevalue)) != 0);
}

bool AlchemicEffect::HasRestorativeEffect()
{
	if (second & static_cast<AlchemyBaseEffect>(AlchemyBaseEffectSecond::kHealth) ||
		second & static_cast<AlchemyBaseEffect>(AlchemyBaseEffectSecond::kMagicka) ||
		second & static_cast<AlchemyBaseEffect>(AlchemyBaseEffectSecond::kStamina))
		return true;
	return false;
}

std::size_t std::hash<AlchemicEffect>::operator()(AlchemicEffect const& effect) const noexcept
{
	size_t h1 = std::hash<uint64_t>{}(effect.first);
	size_t h2 = std::hash<uint64_t>{}(effect.first);
	return h1 ^ (h2 << 1);
}

// definition of constants

const AlchemicEffect AlchemicEffect::kNone = AlchemyBaseEffectSecond::kNone;
const AlchemicEffect AlchemicEffect::kHealth = AlchemyBaseEffectSecond::kHealth;
const AlchemicEffect AlchemicEffect::kMagicka = AlchemyBaseEffectSecond::kMagicka;
const AlchemicEffect AlchemicEffect::kStamina = AlchemyBaseEffectSecond::kStamina;
const AlchemicEffect AlchemicEffect::kOneHanded = AlchemyBaseEffectSecond::kOneHanded;
const AlchemicEffect AlchemicEffect::kTwoHanded = AlchemyBaseEffectSecond::kTwoHanded;
const AlchemicEffect AlchemicEffect::kArchery = AlchemyBaseEffectSecond::kArchery;
const AlchemicEffect AlchemicEffect::kBlock = AlchemyBaseEffectSecond::kBlock;
const AlchemicEffect AlchemicEffect::kHeavyArmor = AlchemyBaseEffectSecond::kHeavyArmor;
const AlchemicEffect AlchemicEffect::kLightArmor = AlchemyBaseEffectSecond::kLightArmor;
const AlchemicEffect AlchemicEffect::kAlteration = AlchemyBaseEffectSecond::kAlteration;
const AlchemicEffect AlchemicEffect::kConjuration = AlchemyBaseEffectSecond::kConjuration;
const AlchemicEffect AlchemicEffect::kDestruction = AlchemyBaseEffectSecond::kDestruction;
const AlchemicEffect AlchemicEffect::kIllusion = AlchemyBaseEffectSecond::kIllusion;
const AlchemicEffect AlchemicEffect::kRestoration = AlchemyBaseEffectSecond::kRestoration;
const AlchemicEffect AlchemicEffect::kHealRate = AlchemyBaseEffectSecond::kHealRate;
const AlchemicEffect AlchemicEffect::kMagickaRate = AlchemyBaseEffectSecond::kMagickaRate;
const AlchemicEffect AlchemicEffect::kStaminaRate = AlchemyBaseEffectSecond::kStaminaRate;
const AlchemicEffect AlchemicEffect::kSpeedMult = AlchemyBaseEffectSecond::kSpeedMult;
const AlchemicEffect AlchemicEffect::kCriticalChance = AlchemyBaseEffectSecond::kCriticalChance;
const AlchemicEffect AlchemicEffect::kMeleeDamage = AlchemyBaseEffectSecond::kMeleeDamage;
const AlchemicEffect AlchemicEffect::kUnarmedDamage = AlchemyBaseEffectSecond::kUnarmedDamage;
const AlchemicEffect AlchemicEffect::kDamageResist = AlchemyBaseEffectSecond::kDamageResist;
const AlchemicEffect AlchemicEffect::kPoisonResist = AlchemyBaseEffectSecond::kPoisonResist;
const AlchemicEffect AlchemicEffect::kResistFire = AlchemyBaseEffectSecond::kResistFire;
const AlchemicEffect AlchemicEffect::kResistShock = AlchemyBaseEffectSecond::kResistShock;
const AlchemicEffect AlchemicEffect::kResistFrost = AlchemyBaseEffectSecond::kResistFrost;
const AlchemicEffect AlchemicEffect::kResistMagic = AlchemyBaseEffectSecond::kResistMagic;
const AlchemicEffect AlchemicEffect::kResistDisease = AlchemyBaseEffectSecond::kResistDisease;
const AlchemicEffect AlchemicEffect::kParalysis = AlchemyBaseEffectSecond::kParalysis;
const AlchemicEffect AlchemicEffect::kInvisibility = AlchemyBaseEffectSecond::kInvisibility;
const AlchemicEffect AlchemicEffect::kWeaponSpeedMult = AlchemyBaseEffectSecond::kWeaponSpeedMult;
const AlchemicEffect AlchemicEffect::kAttackDamageMult = AlchemyBaseEffectSecond::kAttackDamageMult;
const AlchemicEffect AlchemicEffect::kHealRateMult = AlchemyBaseEffectSecond::kHealRateMult;
const AlchemicEffect AlchemicEffect::kMagickaRateMult = AlchemyBaseEffectSecond::kMagickaRateMult;
const AlchemicEffect AlchemicEffect::kStaminaRateMult = AlchemyBaseEffectSecond::kStaminaRateMult;
const AlchemicEffect AlchemicEffect::kBlood = AlchemyBaseEffectSecond::kBlood;
const AlchemicEffect AlchemicEffect::kPickpocket = AlchemyBaseEffectSecond::kPickpocket;
const AlchemicEffect AlchemicEffect::kLockpicking = AlchemyBaseEffectSecond::kLockpicking;
const AlchemicEffect AlchemicEffect::kSneak = AlchemyBaseEffectSecond::kSneak;
const AlchemicEffect AlchemicEffect::kFrenzy = AlchemyBaseEffectSecond::kFrenzy;
const AlchemicEffect AlchemicEffect::kFear = AlchemyBaseEffectSecond::kFear;
const AlchemicEffect AlchemicEffect::kBowSpeed = AlchemyBaseEffectSecond::kBowSpeed;
const AlchemicEffect AlchemicEffect::kReflectDamage = AlchemyBaseEffectSecond::kReflectDamage;
const AlchemicEffect AlchemicEffect::kCureDisease = AlchemyBaseEffectSecond::kCureDisease;
const AlchemicEffect AlchemicEffect::kCurePoison = AlchemyBaseEffectSecond::kCurePoison;
const AlchemicEffect AlchemicEffect::kEnchanting = AlchemyBaseEffectSecond::kEnchanting;
const AlchemicEffect AlchemicEffect::kWaterbreathing = AlchemyBaseEffectSecond::kWaterbreathing;
const AlchemicEffect AlchemicEffect::kSmithing = AlchemyBaseEffectSecond::kSmithing;
const AlchemicEffect AlchemicEffect::kSpeech = AlchemyBaseEffectSecond::kSpeech;
const AlchemicEffect AlchemicEffect::kCarryWeight = AlchemyBaseEffectSecond::kCarryWeight;
const AlchemicEffect AlchemicEffect::kPersuasion = AlchemyBaseEffectSecond::kPersuasion;
const AlchemicEffect AlchemicEffect::kAlchemy = AlchemyBaseEffectSecond::kAlchemy;
const AlchemicEffect AlchemicEffect::kFortifyHealth = AlchemyBaseEffectSecond::kFortifyHealth;
const AlchemicEffect AlchemicEffect::kFortifyMagicka = AlchemyBaseEffectSecond::kFortifyMagicka;
const AlchemicEffect AlchemicEffect::kFortifyStamina = AlchemyBaseEffectSecond::kFortifyStamina;
const AlchemicEffect AlchemicEffect::kShield = AlchemyBaseEffectSecond::kShield;
const AlchemicEffect AlchemicEffect::kUnused2 = AlchemyBaseEffectSecond::kUnused2;
const AlchemicEffect AlchemicEffect::kUnused3 = AlchemyBaseEffectSecond::kUnused3;
const AlchemicEffect AlchemicEffect::kUnused4 = AlchemyBaseEffectSecond::kUnused4;
const AlchemicEffect AlchemicEffect::kUnused5 = AlchemyBaseEffectSecond::kUnused5;
const AlchemicEffect AlchemicEffect::kUnused6 = AlchemyBaseEffectSecond::kUnused6;
const AlchemicEffect AlchemicEffect::kUnused7 = AlchemyBaseEffectSecond::kUnused7;
const AlchemicEffect AlchemicEffect::kUnused8 = AlchemyBaseEffectSecond::kUnused8;
const AlchemicEffect AlchemicEffect::kCustom = AlchemyBaseEffectSecond::kCustom;

const AlchemicEffect AlchemicEffect::kAnyPotion = AlchemicEffect(0, static_cast<uint64_t>(AlchemyBaseEffectSecond::kAnyPotion));
const AlchemicEffect AlchemicEffect::kAnyPoison = AlchemicEffect(0, static_cast<uint64_t>(AlchemyBaseEffectSecond::kAnyPoison));
const AlchemicEffect AlchemicEffect::kAnyRegen = AlchemicEffect(0, static_cast<uint64_t>(AlchemyBaseEffectSecond::kAnyRegen));
const AlchemicEffect AlchemicEffect::kAnyFortify = AlchemicEffect(0, static_cast<uint64_t>(AlchemyBaseEffectSecond::kAnyFortify));
const AlchemicEffect AlchemicEffect::kAnyFood = AlchemicEffect(0, static_cast<uint64_t>(AlchemyBaseEffectSecond::kAnyFood));
const AlchemicEffect AlchemicEffect::kAllPotions = AlchemicEffect(0, static_cast<uint64_t>(AlchemyBaseEffectSecond::kAllPotions));
