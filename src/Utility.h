#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "Settings.h"
#include <tuple>
#include <utility>

#define Base(x) static_cast<uint64_t>(x)

/// <summary>
/// Provides generic functions
/// </summary>
class Utility
{
private:
	// FROM WIKIPEDIA
	const uint64_t m1 = 0x5555555555555555;   //binary: 0101...
	const uint64_t m2 = 0x3333333333333333;   //binary: 00110011..
	const uint64_t m4 = 0x0f0f0f0f0f0f0f0f;   //binary:  4 zeros,  4 ones ...
	const uint64_t m8 = 0x00ff00ff00ff00ff;   //binary:  8 zeros,  8 ones ...
	const uint64_t m16 = 0x0000ffff0000ffff;  //binary: 16 zeros, 16 ones ...
	const uint64_t m32 = 0x00000000ffffffff;  //binary: 32 zeros, 32 ones
	const uint64_t h01 = 0x0101010101010101;

	int Bits_1(uint64_t x)
	{
		x -= (x >> 1) & m1;              //put count of each 2 bits into those 2 bits
		x = (x & m2) + ((x >> 2) & m2);  //put count of each 4 bits into those 4 bits
		x = (x + (x >> 4)) & m4;         //put count of each 8 bits into those 8 bits
		return (x * h01) >> 56;          //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
	}
	// END


public:

	#pragma region SortingFunctions
	// comparator used to sort magnitude - duration - AlchemyItem* lists for maximum magnitude descending
	static bool SortMagnitude(std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> first, std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> second)
	{
		return (std::get<0>(first) * (std::get<1>(first) == 0 ? 1 : std::get<1>(first))) > (std::get<0>(second) * (std::get<1>(second) == 0 ? 1 : std::get<1>(second)));
	}
	#pragma endregion

	/// <summary>
	/// Returns a string showing [val] as Hexadecimal number
	/// </summary>
	/// <param name="val"></param>
	/// <returns></returns>
	static std::string GetHex(long val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}
	/// <summary>
	/// Returns a string showing [val] as Hexadecimal number
	/// </summary>
	/// <param name="val"></param>
	/// <returns></returns>
	static std::string GetHex(uint64_t val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}
	/// <summary>
	/// Returns a string showing [val] as Hexadecimal number
	/// </summary>
	/// <param name="val"></param>
	/// <returns></returns>
	static std::string GetHex(uint32_t val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}
	/// <summary>
	/// Returns a string showing [val] as Hexadecimal number
	/// </summary>
	/// <param name="val"></param>
	/// <returns></returns>
	static std::string GetHex(int val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}

	/// <summary>
	/// Converts all symbols in a string into lower case.
	/// </summary>
	/// <param name="s"></param>
	/// <returns></returns>
	static std::string ToLower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return (unsigned char)std::tolower(c); }  // correct
		);
		return s;
	}

	/// <summary>
	/// converts an ActorStrength into a string
	/// </summary>
	/// <param name="acs"></param>
	/// <returns></returns>
	static std::string ToString(ActorStrength acs)
	{
		switch (acs) {
		case ActorStrength::Weak:
			return "Weak";
		case ActorStrength::Normal:
			return "Normal";
		case ActorStrength::Powerful:
			return "Powerful";
		case ActorStrength::Insane:
			return "Insane";
		case ActorStrength::Boss:
			return "Boss";
		default:
			return "Not Applicable";
		}
	}

	/// <summary>
	/// Converts an ItemStrength into a string
	/// </summary>
	/// <param name="is"></param>
	/// <returns></returns>
	static std::string ToString(ItemStrength is)
	{
		switch (is) {
		case ItemStrength::kWeak:
			return "Weak";
		case ItemStrength::kStandard:
			return "Standard";
		case ItemStrength::kPotent:
			return "Potent";
		case ItemStrength::kInsane:
			return "Insane";
		default:
			return "Not Applicable";
		}
	}

	static std::string ToString(Settings::AlchemyEffect ae)
	{
		switch (ae) {
		case Settings::AlchemyEffect::kAlteration:
			return "Alteration";
		case Settings::AlchemyEffect::kAnyFood:
			return "AnyFood";
		case Settings::AlchemyEffect::kAnyFortify:
			return "AnyFortify";
		case Settings::AlchemyEffect::kAnyPoison:
			return "AnyPoison";
		case Settings::AlchemyEffect::kAnyPotion:
			return "AnyPotion";
		case Settings::AlchemyEffect::kArchery:
			return "Archery";
		case Settings::AlchemyEffect::kAttackDamageMult:
			return "AttackDamageMult";
		case Settings::AlchemyEffect::kBlock:
			return "Block";
		case Settings::AlchemyEffect::kBlood:
			return "Blood";
		case Settings::AlchemyEffect::kBowSpeed:
			return "BowSpeed";
		case Settings::AlchemyEffect::kConjuration:
			return "Conjuration";
		case Settings::AlchemyEffect::kCriticalChance:
			return "CriticalChance";
		case Settings::AlchemyEffect::kDamageResist:
			return "DamageResist";
		case Settings::AlchemyEffect::kDestruction:
			return "Destruction";
		case Settings::AlchemyEffect::kFear:
			return "Fear";
		case Settings::AlchemyEffect::kFrenzy:
			return "Frenzy";
		case Settings::AlchemyEffect::kHealRate:
			return "HealRate";
		case Settings::AlchemyEffect::kHealRateMult:
			return "HealRateMult";
		case Settings::AlchemyEffect::kHealth:
			return "Health";
		case Settings::AlchemyEffect::kHeavyArmor:
			return "HeavyArmor";
		case Settings::AlchemyEffect::kIllusion:
			return "Illusion";
		case Settings::AlchemyEffect::kInvisibility:
			return "Invisibility";
		case Settings::AlchemyEffect::kLightArmor:
			return "LightArmor";
		case Settings::AlchemyEffect::kLockpicking:
			return "Lockpicking";
		case Settings::AlchemyEffect::kMagicka:
			return "Magicka";
		case Settings::AlchemyEffect::kMagickaRate:
			return "MagickaRate";
		case Settings::AlchemyEffect::kMagickaRateMult:
			return "MagickaRateMult";
		case Settings::AlchemyEffect::kMeleeDamage:
			return "MeleeDamage";
		case Settings::AlchemyEffect::kNone:
			return "None";
		case Settings::AlchemyEffect::kOneHanded:
			return "OneHanded";
		case Settings::AlchemyEffect::kParalysis:
			return "Paralysis";
		case Settings::AlchemyEffect::kPickpocket:
			return "Pickpocket";
		case Settings::AlchemyEffect::kPoisonResist:
			return "PoisonResist";
		case Settings::AlchemyEffect::kReflectDamage:
			return "ReflectDamage";
		case Settings::AlchemyEffect::kResistDisease:
			return "ResistDisease";
		case Settings::AlchemyEffect::kResistFire:
			return "ResistFire";
		case Settings::AlchemyEffect::kResistFrost:
			return "ResistFrost";
		case Settings::AlchemyEffect::kResistMagic:
			return "ResistMagic";
		case Settings::AlchemyEffect::kResistShock:
			return "ResistShock";
		case Settings::AlchemyEffect::kRestoration:
			return "Restoration";
		case Settings::AlchemyEffect::kSneak:
			return "Sneak";
		case Settings::AlchemyEffect::kSpeedMult:
			return "SpeedMult";
		case Settings::AlchemyEffect::kStamina:
			return "Stamina";
		case Settings::AlchemyEffect::kStaminaRate:
			return "StaminaRate";
		case Settings::AlchemyEffect::kStaminaRateMult:
			return "StaminaRateMult";
		case Settings::AlchemyEffect::kTwoHanded:
			return "TwoHanded";
		case Settings::AlchemyEffect::kUnarmedDamage:
			return "UnarmedDamage";
		case Settings::AlchemyEffect::kWeaponSpeedMult:
			return "WeapenSpeedMult";
		case Settings::AlchemyEffect::kCureDisease:
			return "CureDisease";
		case Settings::AlchemyEffect::kCurePoison:
			return "CurePoison";
		case Settings::AlchemyEffect::kEnchanting:
			return "Enchanting";
		case Settings::AlchemyEffect::kWaterbreathing:
			return "Waterbreathing";
		case Settings::AlchemyEffect::kSmithing:
			return "Smithing";
		case Settings::AlchemyEffect::kSpeech:
			return "Speech";
		case Settings::AlchemyEffect::kCarryWeight:
			return "CarryWeight";
		case Settings::AlchemyEffect::kAlchemy:
			return "Alchemy";
		case Settings::AlchemyEffect::kPersuasion:
			return "Persuasion";
		case Settings::AlchemyEffect::kCustom:
			return "Custom";
		default:
			return "Unknown";
		}
	}

	static std::string ToString(AlchemyEffectBase ae)
	{
		std::string ret = "|";
		if (ae & Base(Settings::AlchemyEffect::kAlteration))
			ret += "Alteration|";
		if (ae & Base(Settings::AlchemyEffect::kArchery))
			ret += "Archery|";
		if (ae & Base(Settings::AlchemyEffect::kAttackDamageMult))
			ret += "AttackDamageMult|";
		if (ae & Base(Settings::AlchemyEffect::kBlock))
			ret += "Block|";
		if (ae & Base(Settings::AlchemyEffect::kBlood))
			ret += "Blood|";
		if (ae & Base(Settings::AlchemyEffect::kBowSpeed))
			ret += "BowSpeed|";
		if (ae & Base(Settings::AlchemyEffect::kConjuration))
			ret += "Conjuration|";
		if (ae & Base(Settings::AlchemyEffect::kCriticalChance))
			ret += "CriticalChance|";
		if (ae & Base(Settings::AlchemyEffect::kDamageResist))
			ret += "DamageResist|";
		if (ae & Base(Settings::AlchemyEffect::kDestruction))
			ret += "Destruction|";
		if (ae & Base(Settings::AlchemyEffect::kFear))
			ret += "Fear|";
		if (ae & Base(Settings::AlchemyEffect::kFrenzy))
			ret += "Frenzy|";
		if (ae & Base(Settings::AlchemyEffect::kHealRate))
			ret += "HealRate|";
		if (ae & Base(Settings::AlchemyEffect::kHealRateMult))
			ret += "HealRateMult|";
		if (ae & Base(Settings::AlchemyEffect::kHealth))
			ret += "Health|";
		if (ae & Base(Settings::AlchemyEffect::kHeavyArmor))
			ret += "HeavyArmor|";
		if (ae & Base(Settings::AlchemyEffect::kIllusion))
			ret += "Illusion|";
		if (ae & Base(Settings::AlchemyEffect::kInvisibility))
			ret += "Invisibility|";
		if (ae & Base(Settings::AlchemyEffect::kLightArmor))
			ret += "LightArmor|";
		if (ae & Base(Settings::AlchemyEffect::kLockpicking))
			ret += "Lockpicking|";
		if (ae & Base(Settings::AlchemyEffect::kMagicka))
			ret += "Magicka|";
		if (ae & Base(Settings::AlchemyEffect::kMagickaRate))
			ret += "MagickaRate|";
		if (ae & Base(Settings::AlchemyEffect::kMagickaRateMult))
			ret += "MagickaRateMult|";
		if (ae & Base(Settings::AlchemyEffect::kMeleeDamage))
			ret += "MeleeDamage|";
		if (ae & Base(Settings::AlchemyEffect::kNone))
			ret += "None|";
		if (ae & Base(Settings::AlchemyEffect::kOneHanded))
			ret += "OneHanded|";
		if (ae & Base(Settings::AlchemyEffect::kParalysis))
			ret += "Paralysis|";
		if (ae & Base(Settings::AlchemyEffect::kPickpocket))
			ret += "Pickpocket|";
		if (ae & Base(Settings::AlchemyEffect::kPoisonResist))
			ret += "PoisonResist|";
		if (ae & Base(Settings::AlchemyEffect::kReflectDamage))
			ret += "ReflectDamage|";
		if (ae & Base(Settings::AlchemyEffect::kResistDisease))
			ret += "ResistDisease|";
		if (ae & Base(Settings::AlchemyEffect::kResistFire))
			ret += "ResistFire|";
		if (ae & Base(Settings::AlchemyEffect::kResistFrost))
			ret += "ResistFrost|";
		if (ae & Base(Settings::AlchemyEffect::kResistMagic))
			ret += "ResistMagic|";
		if (ae & Base(Settings::AlchemyEffect::kResistShock))
			ret += "ResistShock|";
		if (ae & Base(Settings::AlchemyEffect::kRestoration))
			ret += "Restoration|";
		if (ae & Base(Settings::AlchemyEffect::kSneak))
			ret += "Sneak|";
		if (ae & Base(Settings::AlchemyEffect::kSpeedMult))
			ret += "SpeedMult|";
		if (ae & Base(Settings::AlchemyEffect::kStamina))
			ret += "Stamina|";
		if (ae & Base(Settings::AlchemyEffect::kStaminaRate))
			ret += "StaminaRate|";
		if (ae & Base(Settings::AlchemyEffect::kStaminaRateMult))
			ret += "StaminaRateMult|";
		if (ae & Base(Settings::AlchemyEffect::kTwoHanded))
			ret += "TwoHanded|";
		if (ae & Base(Settings::AlchemyEffect::kUnarmedDamage))
			ret += "UnarmedDamage|";
		if (ae & Base(Settings::AlchemyEffect::kWeaponSpeedMult))
			ret += "WeapenSpeedMult|";
		if (ae & Base(Settings::AlchemyEffect::kCureDisease))
			ret += "CureDisease|";
		if (ae & Base(Settings::AlchemyEffect::kCurePoison))
			ret += "CurePoison|";
		if (ae & Base(Settings::AlchemyEffect::kEnchanting))
			ret += "Enchanting|";
		if (ae & Base(Settings::AlchemyEffect::kWaterbreathing))
			ret += "Waterbreathing|";
		if (ae & Base(Settings::AlchemyEffect::kSmithing))
			ret += "Smithing|";
		if (ae & Base(Settings::AlchemyEffect::kSpeech))
			ret += "Speech|";
		if (ae & Base(Settings::AlchemyEffect::kCarryWeight))
			ret += "CarryWeight|";
		if (ae & Base(Settings::AlchemyEffect::kAlchemy))
			ret += "Alchemy|";
		if (ae & Base(Settings::AlchemyEffect::kPersuasion))
			ret += "Persuasion|";
		if (ae & Base(Settings::AlchemyEffect::kCustom))
			ret += "Custom|";
		
		if (ret == "|")
			return "|Unknown|";
		return ret;
	}

	enum class CurrentCombatStyle
	{
		Spellsword = 0x1, // combination spell and onehanded
		OneHandedShield = 0x2,
		TwoHanded = 0x4,
		MagicDestruction = 0x8,
		MagicConjuration = 0x10,
		MagicAlteration = 0x20,
		MagicIllusion = 0x40,
		MagicRestoration = 0x80,
		OneHanded = 0x100,
		Ranged = 0x200,
		DualWield = 0x400,
		Staffsword = 0x800,  // combination staff and onehanded
		HandToHand = 0x1000,
		Mage = 0x2000,
		DualStaff = 0x4000,
		Staff = 0x8000,
		MagicDamageFire = 0x100000,
		MagicDamageShock = 0x200000,
		MagicDamageFrost = 0x400000,
	};

	/// <summary>
	/// converts a CurrentCombatStyle value into a string
	/// </summary>
	/// <param name="style">CurrentCombatStyle value to convert</param>
	/// <returns>String representing [style]</returns>
	static std::string ToStringCombatStyle(uint32_t style)
	{
		std::string flags = "|";
		if (style & static_cast<int>(CurrentCombatStyle::Spellsword))
			flags += "Spellsword|";
		if (style & static_cast<int>(CurrentCombatStyle::OneHandedShield))
			flags += "OneHandedShield|";
		if (style & static_cast<int>(CurrentCombatStyle::TwoHanded))
			flags += "TwoHanded|";
		if (style & static_cast<int>(CurrentCombatStyle::OneHanded))
			flags += "OneHanded|";
		if (style & static_cast<int>(CurrentCombatStyle::Ranged))
			flags += "Ranged|";
		if (style & static_cast<int>(CurrentCombatStyle::DualWield))
			flags += "DualWield|";
		if (style & static_cast<int>(CurrentCombatStyle::HandToHand))
			flags += "HandToHand|";
		if (style & static_cast<int>(CurrentCombatStyle::Staffsword))
			flags += "Staffsword|";
		if (style & static_cast<int>(CurrentCombatStyle::DualStaff))
			flags += "DualStaff|";
		if (style & static_cast<int>(CurrentCombatStyle::Staff))
			flags += "Staff|";
		if (style & static_cast<int>(CurrentCombatStyle::Mage))
			flags += "Mage|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicDestruction))
			flags += "MagicDestruction|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicConjuration))
			flags += "MagicConjuration|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicAlteration))
			flags += "MagicAlteration|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicIllusion))
			flags += "MagicIllusion|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicRestoration))
			flags += "MagicRestoration|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicDamageFire))
			flags += "MagicDamageFire|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicDamageShock))
			flags += "MagicDamageShock|";
		if (style & static_cast<int>(CurrentCombatStyle::MagicDamageFrost))
			flags += "MagicDamageFrost|";
		return flags;
	}

	/// <summary>
	/// Retrieves data about the current equiped items and spells of an actor
	/// </summary>
	/// <param name="actor">the actor to check</param>
	/// <returns>A linear combination of CurrentCombatStyle, representing the combat data of an actor</returns>
	static uint32_t GetCombatData(RE::Actor* actor)
	{
		uint32_t combatdata = 0;

		RE::TESForm* lefthand = actor->GetEquippedObject(true);
		RE::TESForm* righthand = actor->GetEquippedObject(false);
		RE::TESObjectWEAP* leftweap = lefthand != nullptr ? lefthand->As<RE::TESObjectWEAP>() : nullptr;
		RE::TESObjectWEAP* rightweap = righthand != nullptr ? righthand->As<RE::TESObjectWEAP>() : nullptr;
		RE::SpellItem* leftspell = lefthand != nullptr ? lefthand->As<RE::SpellItem>() : nullptr;
		RE::SpellItem* rightspell = righthand != nullptr ? righthand->As<RE::SpellItem>() : nullptr;
		if (rightweap && (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kTwoHandAxe || rightweap->GetWeaponType() == RE::WEAPON_TYPE::kTwoHandSword)) {
			// twohanded
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::TwoHanded);
		}
		else if (rightweap && (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kBow || rightweap->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow)) {
			// ranged
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Ranged);
		}
		else if (rightweap && leftweap) {
			if ((rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
					rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
					rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
					rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace) &&
				(leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
					leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
					leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
					leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace)) {
				// dual wield
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::DualWield);
			} else if ((rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee) &&
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff ||
					   (leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee) &&
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
				// spellstaff
				if (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
					// right staff
					if (rightweap->amountofEnchantment > 0) {
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Staffsword);
						RE::EnchantmentItem* ench = rightweap->formEnchanting;
						if (ench) {
							for (uint32_t i = 0; i < ench->effects.size(); i++) {
								try {
									switch (ench->effects[i]->baseEffect->data.associatedSkill) {
									case RE::ActorValue::kAlteration:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
										break;
									case RE::ActorValue::kDestruction:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
										if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
											combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
										if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
											combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
										if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
											combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
										break;
									case RE::ActorValue::kConjuration:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
										break;
									case RE::ActorValue::kIllusion:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
										break;
									case RE::ActorValue::kRestoration:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
										break;
									}
								}
								catch (std::exception&) {

								}
							}
						}
					} else {
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::OneHanded);
					}

				} else {
					// left staff
					if (leftweap->amountofEnchantment > 0) {
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Staffsword);
						RE::EnchantmentItem* ench = leftweap->formEnchanting;
						if (ench) {
							for (uint32_t i = 0; i < ench->effects.size(); i++) {
								try {
									switch (ench->effects[i]->baseEffect->data.associatedSkill) {
									case RE::ActorValue::kAlteration:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
										break;
									case RE::ActorValue::kDestruction:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
										if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
											combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
										if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
											combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
										if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
											combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
										break;
									case RE::ActorValue::kConjuration:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
										break;
									case RE::ActorValue::kIllusion:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
										break;
									case RE::ActorValue::kRestoration:
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
										break;
									}
								} catch (std::exception&) {
								}
							}
						}
					} else {
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::OneHanded);
					}
					if (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee ||
						leftweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee) {
						// if one hand is not a sword, but a hand to hand weapon fix that shit
						if (combatdata & static_cast<uint32_t>(CurrentCombatStyle::OneHanded))
							combatdata = (combatdata & (0xffffffff ^ static_cast<uint32_t>(CurrentCombatStyle::OneHanded))) | static_cast<uint32_t>(CurrentCombatStyle::Staff);
						else if (combatdata & static_cast<uint32_t>(CurrentCombatStyle::Staffsword))
							combatdata = (combatdata & (0xffffffff ^ static_cast<uint32_t>(CurrentCombatStyle::Staffsword))) | static_cast<uint32_t>(CurrentCombatStyle::Staff);
					}
				}
			} else if (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff &&
					   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff) {
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::DualStaff);
				if (leftweap->amountofEnchantment > 0) {
					RE::EnchantmentItem* ench = leftweap->formEnchanting;
					if (ench) {
						for (uint32_t i = 0; i < ench->effects.size(); i++) {
							try {
								switch (ench->effects[i]->baseEffect->data.associatedSkill) {
								case RE::ActorValue::kAlteration:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
									break;
								case RE::ActorValue::kDestruction:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
									if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
									if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
									if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
									break;
								case RE::ActorValue::kConjuration:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
									break;
								case RE::ActorValue::kIllusion:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
									break;
								case RE::ActorValue::kRestoration:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
									break;
								}
							} catch (std::exception&) {
							}
						}
					}
				}
				if (rightweap->amountofEnchantment > 0) {
					RE::EnchantmentItem* ench = rightweap->formEnchanting;
					if (ench) {
						for (uint32_t i = 0; i < ench->effects.size(); i++) {
							try {
								switch (ench->effects[i]->baseEffect->data.associatedSkill) {
								case RE::ActorValue::kAlteration:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
									break;
								case RE::ActorValue::kDestruction:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
									if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
									if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
									if (ench->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
										combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
									break;
								case RE::ActorValue::kConjuration:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
									break;
								case RE::ActorValue::kIllusion:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
									break;
								case RE::ActorValue::kRestoration:
									combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
									break;
								}
							} catch (std::exception&) {
							}
						}
					}
				}
			} else if (leftweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee &&
					   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee) {
				// fix for weapons that use hand to hand animations
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::HandToHand);
			} else if ((rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace) &&
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee ||
					   (leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
						   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace) &&
						   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee) {
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::OneHanded);
			}
		} else if (rightweap && leftspell &&
				   (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
					   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
					   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
					   rightweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace)) {
			// spellsrod
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Spellsword);
			for (uint32_t i = 0; i < leftspell->effects.size(); i++) {
				try {
					switch (leftspell->effects[i]->baseEffect->data.associatedSkill) {
					case RE::ActorValue::kAlteration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
						break;
					case RE::ActorValue::kDestruction:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
						break;
					case RE::ActorValue::kConjuration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
						break;
					case RE::ActorValue::kIllusion:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
						break;
					case RE::ActorValue::kRestoration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
						break;
					}
				} catch (std::exception&) {
				}
			}
		} else if (leftweap && rightspell &&
				   (leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword ||
					   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger ||
					   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe ||
					   leftweap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace)) {
			// spellsword
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Spellsword);
			for (uint32_t i = 0; i < rightspell->effects.size(); i++) {
				try {
					switch (rightspell->effects[i]->baseEffect->data.associatedSkill) {
					case RE::ActorValue::kAlteration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
						break;
					case RE::ActorValue::kDestruction:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
						break;
					case RE::ActorValue::kConjuration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
						break;
					case RE::ActorValue::kIllusion:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
						break;
					case RE::ActorValue::kRestoration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
						break;
					}
				} catch (std::exception&) {
				}
			}
		} else if (rightweap && lefthand && lefthand->As<RE::TESObjectARMO>()) {
			if (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee)
				// fix for weapons that use hand to hand animations
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::HandToHand);
			else
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::OneHandedShield);
		} else if (leftspell && rightspell) {
			for (uint32_t i = 0; i < rightspell->effects.size(); i++) {
				try {
					switch (rightspell->effects[i]->baseEffect->data.associatedSkill) {
					case RE::ActorValue::kAlteration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
						break;
					case RE::ActorValue::kDestruction:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
						break;
					case RE::ActorValue::kConjuration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
						break;
					case RE::ActorValue::kIllusion:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
						break;
					case RE::ActorValue::kRestoration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
						break;
					}
				} catch (std::exception&) {
				}
			}
			for (uint32_t i = 0; i < leftspell->effects.size(); i++) {
				try {
					switch (leftspell->effects[i]->baseEffect->data.associatedSkill) {
					case RE::ActorValue::kAlteration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
						break;
					case RE::ActorValue::kDestruction:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
						break;
					case RE::ActorValue::kConjuration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
						break;
					case RE::ActorValue::kIllusion:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
						break;
					case RE::ActorValue::kRestoration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
						break;
					}
				} catch (std::exception&) {
				}
			}
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Mage);
		} else if (leftspell) {
			for (uint32_t i = 0; i < leftspell->effects.size(); i++) {
				try {
					switch (leftspell->effects[i]->baseEffect->data.associatedSkill) {
					case RE::ActorValue::kAlteration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
						break;
					case RE::ActorValue::kDestruction:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
						if (leftspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
						break;
					case RE::ActorValue::kConjuration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
						break;
					case RE::ActorValue::kIllusion:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
						break;
					case RE::ActorValue::kRestoration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
						break;
					}
				} catch (std::exception&) {
				}
			}
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Mage);
		} else if (rightspell) {
			for (uint32_t i = 0; i < rightspell->effects.size(); i++) {
				try {
					switch (rightspell->effects[i]->baseEffect->data.associatedSkill) {
					case RE::ActorValue::kAlteration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicAlteration);
						break;
					case RE::ActorValue::kDestruction:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDestruction);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFrost" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFrost);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageFire" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageFire);
						if (rightspell->effects[i]->baseEffect->HasKeyword(std::string_view{ "MagicDamageShock" }))
							combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicDamageShock);
						break;
					case RE::ActorValue::kConjuration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicConjuration);
						break;
					case RE::ActorValue::kIllusion:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicIllusion);
						break;
					case RE::ActorValue::kRestoration:
						combatdata |= static_cast<uint32_t>(CurrentCombatStyle::MagicRestoration);
						break;
					}
				} catch (std::exception&) {
				}
			}
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Mage);
		} else if (leftweap || rightweap) {
			if (leftweap && leftweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff ||
				rightweap && rightweap->GetWeaponType() == RE::WEAPON_TYPE::kStaff)
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Mage);
			else if (leftweap && leftweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee ||
					 rightweap && rightweap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee)
				// fix for weapons that use hand to hand animations
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::HandToHand);
			else
				combatdata |= static_cast<uint32_t>(CurrentCombatStyle::OneHanded);
		} else {
			combatdata |= static_cast<uint32_t>(CurrentCombatStyle::HandToHand);
		}

		return combatdata;
	}

	enum class CurrentArmor
	{
		None = 0,
		HeavyArmor = 1 << 0,
		LightArmor = 1 << 1,
		Clothing = 1 << 2,
	};

	/// <summary>
	/// Returns which armor types are worn by a given npc.
	/// </summary>
	/// <param name="actor">The NPC to check.</param>
	/// <returns>Linear combination of values of CurrentArmor</returns>
	static uint32_t GetArmorData(RE::Actor* actor)
	{
		static std::vector<RE::BGSBipedObjectForm::BipedObjectSlot> armorSlots{
			RE::BGSBipedObjectForm::BipedObjectSlot::kHead,
			RE::BGSBipedObjectForm::BipedObjectSlot::kBody,
			RE::BGSBipedObjectForm::BipedObjectSlot::kHands,
			RE::BGSBipedObjectForm::BipedObjectSlot::kForearms,
			RE::BGSBipedObjectForm::BipedObjectSlot::kFeet,
			RE::BGSBipedObjectForm::BipedObjectSlot::kCalves,
			RE::BGSBipedObjectForm::BipedObjectSlot::kShield,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModChestPrimary,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModBack,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisPrimary,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModPelvisSecondary,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModLegRight,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModLegLeft,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModChestSecondary,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModShoulder,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModArmLeft,
			RE::BGSBipedObjectForm::BipedObjectSlot::kModArmRight
		};

		std::set<RE::FormID> visited{};
		unsigned char armorHeavy = 0;
		unsigned char armorLight = 0;
		unsigned char clothing = 0;
		RE::TESObjectARMO* item = nullptr;
		for (int i = 0; i < armorSlots.size(); i++) {
			item = actor->GetWornArmor(armorSlots[i]);
			if (item) {
				if (visited.contains(item->GetFormID()))
					continue;
				visited.insert(item->GetFormID());
				for (uint32_t c = 0; c < item->numKeywords; c++) {
					if (item->keywords[c]) {
						if (item->keywords[c]->GetFormID() == 0x6BBE8) {  // ArmorClothing
							clothing++;
							// every item may only be either clothing, light or heavy armor
							continue;
						} else if (item->keywords[c]->GetFormID() == 0x6BBD2) {  // ArmorHeavy
							armorHeavy++;
							continue;
						} else if (item->keywords[c]->GetFormID() == 0x6BBD3) {  // ArmorLight
							armorLight++;
							continue;
						}
					}
				}
			}
		}
		// we finished every word item in the possible armor slots.
		// armor is recognised as worn, if two or more pieces of the same type are worn.
		uint32_t ret = 0; // also CurrentArmor::Nothing in case nothing below fires
		if (armorHeavy >= 2)
			ret |= static_cast<uint32_t>(CurrentArmor::HeavyArmor);
		if (armorLight >= 2)
			ret |= static_cast<uint32_t>(CurrentArmor::LightArmor);
		if (clothing >= 2)
			ret |= static_cast<uint32_t>(CurrentArmor::Clothing);
		return ret;
	}

	#pragma region Parsing

	/// <summary>
	/// Parses a string into a vector of int (array of int)
	/// </summary>
	/// <param name="line">string to parse</param>
	/// <returns>vector of int (array of int)</returns>
	static std::vector<int> ParseIntArray(std::string line)
	{
		std::vector<int> ret;
		size_t pos = line.find(';');
		while (pos != std::string::npos) {
			try {
				ret.push_back(std::stoi(line.substr(0, pos)));
			} catch (std::out_of_range&) {
				return std::vector<int>();
			} catch (std::invalid_argument&) {
				return std::vector<int>();
			}
			line.erase(0, pos+1);
			pos = line.find(';');
		}
		if (line.length() != 0) {
			try {
				ret.push_back(std::stoi(line));
			} catch (std::out_of_range&) {
				return std::vector<int>();
			} catch (std::invalid_argument&) {
				return std::vector<int>();
			}
		}
		return ret;
	}

	/// <summary>
	/// returns a TESForm* from various inputs
	/// </summary>
	/// <param name="datahandler">datahandler to get data from</param>
	/// <param name="formid">id or partial id of item (may be 0, if editorid is set)</param>
	/// <param name="pluginname">name of the plugin the item is included (may be the empty string, if item is in the basegame, or editorid is given)</param>
	/// <param name="editorid">editorid of the item, defaults to empty string</param>
	/// <returns></returns>
	static RE::TESForm* GetTESForm(RE::TESDataHandler* datahandler, RE::FormID formid, std::string pluginname, std::string editorid = "")
	{
		RE::TESForm* tmp = nullptr;
		if (pluginname.size() != 0) {
			if (formid != 0) {
				tmp = datahandler->LookupForm(formid, std::string_view{ pluginname });
				if (tmp == nullptr && editorid.size() > 0) {
					tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
				}
			} else if (editorid.size() > 0) {
				tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
			}
			// else we cannot find what we were lookin for
		} else if (formid > 0) {
			// pluginname is not given, so try to find the form by the id itself
			tmp = RE::TESForm::LookupByID(formid);
		} else if (editorid.size() > 0) {
			tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
		}
		return tmp;
	}

	/// <summary>
	/// Parses objects for distribution rules from a string input
	/// </summary>
	/// <param name="input">the string to parse</param>
	/// <param name="error">will be overwritten with [true] if an error occurs</param>
	/// <param name="file">the relative path of the file that contains the string</param>
	/// <param name="line">the line in the file that contains the string</param>
	/// <returns>a vector of parsed and validated objects</returns>
	static std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> ParseAssocObjects(std::string input, bool &error, std::string file, std::string line)
	{
		LOG_4("{}[ParseAssocObjects]");
		std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> ret;
		try {
			auto datahandler = RE::TESDataHandler::GetSingleton();
			size_t pos;
			Settings::Distribution::AssocType type = Settings::Distribution::AssocType::kActor;
			uint32_t formid = 0;
			bool form = false;
			bool valid = false;
			std::string editorid;
			RE::TESForm* tmp = nullptr;
			std::string pluginname;
			while (input.empty() == false) {
				form = false;
				valid = false;
				pluginname = "";
				input.erase(0, input.find('<')+1);
				if ((pos = input.find('>')) != std::string::npos) {
					// we have a valid entry, probably
					std::string entry = input.substr(0, pos);
					input.erase(0, pos+1);
					// parse form or editor id
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					try {
						formid = static_cast<uint32_t>(std::stol(entry.substr(0, pos), nullptr, 16));
						form = true;
					} catch (std::exception&) {

					}
					editorid = entry.substr(0, pos);
					entry.erase(0, pos+1);
					pluginname = entry;
					if (pluginname.size() != 0) {
						if (form) {
							tmp = datahandler->LookupForm(formid, std::string_view{ pluginname });
							if (tmp == nullptr) {
								tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
							}
						} else {
							tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
						}
						// else we cannot find what we were lookin for
					} else {
						// pluginname is not given, so try to find the form by the id itself
						tmp = RE::TESForm::LookupByID(formid);
					}
					// check wether form has a correct type
					if (tmp != nullptr) {
						type = MatchValidFormType(tmp->GetFormType(), valid);
						if (valid) {
							ret.push_back({ type, tmp->GetFormID() });
						} else {
							logger::warn("[Settings] [LoadDistrRules] Form {} has an unsupported FormType. file: \"{}\" Rule: \"{}\"", GetHex(tmp->GetFormID()), file, line);
						}
					} else {
						if (form)
							logger::warn("[Settings] [LoadDistrRules] FormID {} couldn't be found. file: \"{}\" Rule: \"\"", GetHex(formid), file, line);
						else
							logger::warn("[Settings] [LoadDistrRules] EditorID {} couldn't be found. file: \"{}\" Rule: \"\"", editorid, file, line);
					}
				} else {
					// invalid input return what we parsed so far and set error
					error = true;
					return ret;
				}
			}
		} catch (std::exception&) {
			// we have a malformed input so return
			error = true;
			return ret;
		}
		return ret;
	}

	/// <summary>
	/// Parses objects for distribution rules from a string input with an optional chance for items
	/// </summary>
	/// <param name="input">the string to parse</param>
	/// <param name="error">will be overwritten with [true] if an error occurs</param>
	/// <param name="file">the relative path of the file that contains the string</param>
	/// <param name="line">the line in the file that contains the string</param>
	/// <returns>a vector of parsed and validated objects and their chances</returns>
	static std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, uint64_t, uint64_t, bool>> ParseCustomObjects(std::string input, bool& error, std::string file, std::string line)
	{
		LOG_4("{}[ParseAssocObjectsChance]");
		std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, uint64_t, uint64_t, bool>> ret;
		try {
			auto datahandler = RE::TESDataHandler::GetSingleton();
			size_t pos;
			Settings::Distribution::AssocType type = Settings::Distribution::AssocType::kActor;
			uint32_t formid = 0;
			bool form = false;
			bool valid = false;
			std::string editorid;
			RE::TESForm* tmp = nullptr;
			std::string pluginname;
			std::string chances;
			std::string flags;
			std::string numitems;
			bool exclude = false;
			bool giveonce = false;
			uint64_t conditions1;
			uint64_t conditions2;
			int8_t num;
			CustomItemFlag flag;
			int32_t chance = 100;
			while (input.empty() == false) {
				form = false;
				valid = false;
				exclude = false;
				giveonce = false;
				pluginname = "";
				chances = "";
				chance = 100;
				input.erase(0, input.find('<') + 1);
				if ((pos = input.find('>')) != std::string::npos) {
					// we have a valid entry, probably
					// copy the current object to [entry]
					std::string entry = input.substr(0, pos);
					input.erase(0, pos + 1);
					// parse form or editor id
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					try {
						formid = static_cast<uint32_t>(std::stol(entry.substr(0, pos), nullptr, 16));
						form = true;
					} catch (std::exception&) {
					}
					// assign [editorid] the first entry
					editorid = entry.substr(0, pos);
					// assign [entry] the rest
					entry.erase(0, pos + 1);
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					pluginname = entry.substr(0, pos);
					entry.erase(0, pos + 1);
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					numitems = entry.substr(0, pos);
					entry.erase(0, pos + 1);
					try {
						num = (int8_t)(std::stol(numitems));
					} catch (std::exception&) {
					}
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					if (entry.substr(0, pos) == "1")
						exclude = true;
					entry.erase(0, pos + 1);
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					if (entry.substr(0, pos) == "1")
						giveonce = true;
					entry.erase(0, pos + 1);
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					try {
						conditions1 = static_cast<uint32_t>(std::stol(entry.substr(0, pos), nullptr, 16));
					} catch (std::exception&) {
					}
					entry.erase(0, pos + 1);
					if ((pos = entry.find(',')) == std::string::npos) {
						error = true;
						return ret;
					}
					try {
						conditions2 = static_cast<uint32_t>(std::stol(entry.substr(0, pos), nullptr, 16));
					} catch (std::exception&) {
					}
					entry.erase(0, pos + 1);
					if ((pos = entry.find(',')) == std::string::npos) {
						// the rest is the item flag
						flags = entry;
					} else {
						// the rest consists of the item flag and the chance
						flags = entry.substr(0, pos);
						entry.erase(0, pos + 1);
						chances = entry;
						try {
							chance = static_cast<uint32_t>(std::stol(chances, nullptr));
							if (chance < 0)
								chance = 0;
							if (chance > 100)
								chance = 100;
						} catch (std::exception&) {
							chance = 100;
						}
					}
					if (flags == "" || ToLower(flags) == "object") {
						flag = CustomItemFlag::Object;
					} else if (ToLower(flags) == "potion") {
						flag = CustomItemFlag::Potion;
					} else if (ToLower(flags) == "poison") {
						flag = CustomItemFlag::Poison;
					} else if (ToLower(flags) == "food") {
						flag = CustomItemFlag::Food;
					} else if (ToLower(flags) == "fortify") {
						flag = CustomItemFlag::Fortify;
					} else if (ToLower(flags) == "death") {
						flag = CustomItemFlag::DeathObject;
					} else {
						flag = CustomItemFlag::Object;
					}
					if (pluginname.size() != 0) {
						if (form) {
							tmp = datahandler->LookupForm(formid, std::string_view{ pluginname });
							if (tmp == nullptr) {
								tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
							}
						} else {
							tmp = RE::TESForm::LookupByEditorID(std::string_view{ editorid });
						}
						// else we cannot find what we were lookin for
					} else {
						// pluginname is not given, so try to find the form by the id itself
						tmp = RE::TESForm::LookupByID(formid);
					}
					// check wether form has a correct type
					if (tmp != nullptr) {
						type = MatchValidFormType(tmp->GetFormType(), valid);
						if (valid) {
							ret.push_back({ type, tmp->GetFormID(), chance, flag, num, exclude, conditions1, conditions2, giveonce });
						} else {
							logger::warn("[Settings] [LoadDistrRules] Form {} has an unsupported FormType. file: \"{}\" Rule: \"{}\"", GetHex(tmp->GetFormID()), file, line);
						}
					} else {
						if (form)
							logger::warn("[Settings] [LoadDistrRules] FormID {} couldn't be found. file: \"{}\" Rule: \"\"", GetHex(formid), file, line);
						else
							logger::warn("[Settings] [LoadDistrRules] EditorID {} couldn't be found. file: \"{}\" Rule: \"\"", editorid, file, line);
					}
				} else {
					// invalid input return what we parsed so far and set error
					error = true;
					return ret;
				}
			}
		} catch (std::exception&) {
			// we have a malformed input so return
			error = true;
			return ret;
		}
		return ret;
	}

	/// <summary>
	/// Returns an AssocType for the given RE::FormType. If the RE::FormType is not supported, [valid] is set to true
	/// </summary>
	/// <param name="type">RE::FormType to convert</param>
	/// <param name="valid">Overridable value, which is set to true if [type] is not supported.</param>
	/// <returns>AssocType associated with [type]</returns>
	static Settings::Distribution::AssocType MatchValidFormType(RE::FormType type, bool &valid)
	{
		switch (type) {
		case RE::FormType::Keyword:
			valid = true;
			return Settings::Distribution::AssocType::kKeyword;
		case RE::FormType::Faction:
			valid = true;
			return Settings::Distribution::AssocType::kFaction;
		case RE::FormType::Race:
			valid = true;
			return Settings::Distribution::AssocType::kRace;
		case RE::FormType::NPC:
			valid = true;
			return Settings::Distribution::AssocType::kNPC;
		case RE::FormType::ActorCharacter:
			valid = true;
			return Settings::Distribution::AssocType::kActor;
		case RE::FormType::Reference:
			valid = true;
			return Settings::Distribution::AssocType::kItem;
		case RE::FormType::Class:
			valid = true;
			return Settings::Distribution::AssocType::kClass;
		case RE::FormType::CombatStyle:
			valid = true;
			return Settings::Distribution::AssocType::kCombatStyle;
		case RE::FormType::AlchemyItem:
		case RE::FormType::Spell:
		case RE::FormType::Scroll:
		case RE::FormType::Armor:
		case RE::FormType::Book:
		case RE::FormType::Ingredient:
		case RE::FormType::Misc:
		case RE::FormType::Weapon:
		case RE::FormType::Ammo:
		case RE::FormType::SoulGem:
			valid = true;
			return Settings::Distribution::AssocType::kItem;
		default:
			valid = false;
			return Settings::Distribution::AssocType::kKeyword;
		}
	}

	/// <summary>
	/// Parses AlchemyEffects from an input string.
	/// </summary>
	/// <param name="input">string to parse</param>
	/// <param name="error">Overrisable value, which is set to true if there is an error during parsing.</param>
	/// <returns>A vector of AlchemyEffects and Weights</returns>
	static std::vector<std::tuple<uint64_t, float>> ParseAlchemyEffects(std::string input, bool& error)
	{
		std::vector<std::tuple<uint64_t, float>> ret;
		try {
			float modifier = 1.0f;
			size_t pos = 0;
			uint64_t effect = 0;
			while (input.empty() == false) {
				effect = 0;
				modifier = 1.0f;
				input.erase(0, input.find('<') + 1);
				if ((pos = input.find('>')) != std::string::npos) {
					// we have a valid entry, probably
					std::string entry = input.substr(0, pos);
					input.erase(0, pos + 1);

					if ((pos = entry.find(',')) != std::string::npos) {
						auto tmp = entry;
						entry = entry.substr(0, pos);
						tmp = tmp.erase(0, pos+1);
						try {
							modifier = std::stof(tmp);
						} catch (std::exception&) {
						}
					}
					try {
						// read the effectmapping in hex
						effect = std::stoull(entry, nullptr, 16);
					} catch (std::exception&) {
					}
					if (!(effect == 0 || modifier == 0.0f)) {
						ret.push_back({ effect, modifier });
					}

				} else {
					// invalid input return what we parsed so far and set error
					error = true;
					return ret;
				}
			}
		} catch (std::exception&) {
			error = true;
			return ret;
		}
		LOG1_4("{}[ParseAlchemyEffect] found effects: {}", ret.size());
		return ret;
	}

	/// <summary>
	/// Computes a distribution from an effectmap.
	/// </summary>
	/// <param name="effectmap">effectmap containing effects and weights which will be translated into the distribution</param>
	/// <param name="range">range the distribution chances are computed for</param>
	/// <returns>Weighted Distribution</returns>
	static std::vector<std::tuple<int, Settings::AlchemyEffect>> GetDistribution(std::vector<std::tuple<uint64_t, float>> effectmap, int range, bool chance = false)
	{
		std::vector<std::tuple<int, Settings::AlchemyEffect>> ret;
		uint64_t tmp = 0;
		std::map<Settings::AlchemyEffect, float> map;
		// iterate over all effects in effect map
		for (int i = 0; i < effectmap.size(); i++) {
			// iterate over all effects that could be mashed up in the effect map we can only iterate until c 62 so as to avoid
			// an overflow error
			for (uint64_t c = 1; c < 4611686018427387905; c = c << 1) {
				if ((tmp = (std::get<0>(effectmap[i]) & c)) > 0) {
					map.insert_or_assign(static_cast<Settings::AlchemyEffect>(tmp), std::get<1>(effectmap[i]));
				}
			}
		}
		if (chance) {
			map.insert_or_assign(static_cast<Settings::AlchemyEffect>(Settings::AlchemyEffect::kCustom), 1.0f);
		}
		// get the weighted sum of all modifiers over all effects and do multiply mashed up effects with the number of contained effects
		float sum = 0.0f;
		for (auto entry : map) {
			sum = sum + entry.second;
		}
		// get the slice size for each individual effect
		float slicesize = range / sum;
		// value for current effect
		float currval = 0.0f;
		for (auto entry : map) {
			// compute current upper bound from slicesize and our modifier
			currval += slicesize * entry.second;
			// insert bound effect mapping
			ret.push_back({ (int)std::ceil(currval), entry.first });
		}
		return ret;
	}

	/// <summary>
	/// Sums the Alchemyeffects in [list]
	/// </summary>
	/// <param name="list">list with AlchemyEffects to sum</param>
	/// <returns>Combined value with all Alchemyeffects</returns>
	static uint64_t SumAlchemyEffects(std::vector<std::tuple<int, Settings::AlchemyEffect>> list, bool chance = false)
	{
		uint64_t ret = 0;
		for (int i = 0; i < list.size(); i++) {
			LOG1_4("{}[SumAlchemyEffects] effect to sum: {}", GetHex(static_cast<uint64_t>(std::get<1>(list[i]))));
			ret |= static_cast<uint64_t>(std::get<1>(list[i]));
		}
		if (chance)
			ret |= static_cast<uint64_t>(Settings::AlchemyEffect::kCustom);
		LOG1_4("{}[SumAlchemyEffects] summed effects: {}", GetHex(ret));
		return ret;
	}

	/// <summary>
	/// Checks whether poison can be applied to the weapons of an actor
	/// </summary>
	/// <param name="actor">Actor to check</param>
	/// <returns>Whether poison can be applied to the actors weapons</returns>
	static bool CanApplyPoison(RE::Actor* actor)
	{
		auto ied = actor->GetEquippedEntryData(false);
		RE::ExtraPoison* pois = nullptr;
		if (ied) {
			if (ied->extraLists) {
				for (const auto& extraL : *(ied->extraLists)) {
					pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
					if (pois)
						break;
				}
			}
		}
		if (pois == nullptr) {
			ied = actor->GetEquippedEntryData(true);
			if (ied && ied->extraLists) {
				for (const auto& extraL : *(ied->extraLists)) {
					pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
					if (pois)
						break;
				}
			}
		}
		//logger::info("poison check. Actor:\t{}\tpoison:\t{}\t count:\t{}", actor->GetName(), pois && pois->poison ? pois->poison->GetName() : "not found", pois ? pois->count :  -1);

		if (pois && pois->count > 0)
			return false;
		return true;
	}
	#pragma endregion

};
