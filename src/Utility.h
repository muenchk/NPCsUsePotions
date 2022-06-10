#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include "Settings.h"
#include <tuple>
#include <utility>

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
	static bool SortMagnitude(std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect> first, std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect> second)
	{
		return std::get<0>(first) > std::get<0>(second);
	}
	#pragma endregion

	static std::string GetHex(long val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}
	static std::string GetHex(uint64_t val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}
	static std::string GetHex(uint32_t val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}
	static std::string GetHex(int val)
	{
		std::stringstream ss;
		ss << std::hex << val;
		return ss.str();
	}

	static std::string ToLower(std::string s)
	{
		std::transform(s.begin(), s.end(), s.begin(),
			[](unsigned char c) { return (unsigned char)std::tolower(c); }  // correct
		);
		return s;
	}

	static std::string ToString(Settings::ActorStrength acs)
	{
		switch (acs) {
		case Settings::ActorStrength::Weak:
			return "Weak";
		case Settings::ActorStrength::Normal:
			return "Normal";
		case Settings::ActorStrength::Powerful:
			return "Powerful";
		case Settings::ActorStrength::Insane:
			return "Insane";
		case Settings::ActorStrength::Boss:
			return "Boss";
		default:
			return "Not Applicable";
		}
	}

	static std::string ToString(Settings::ItemStrength is)
	{
		switch (is) {
		case Settings::ItemStrength::kWeak:
			return "Weak";
		case Settings::ItemStrength::kStandard:
			return "Standard";
		case Settings::ItemStrength::kPotent:
			return "Potent";
		case Settings::ItemStrength::kInsane:
			return "Insane";
		default:
			return "Not Applicable";
		}
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

	#pragma region Parsing

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
		default:
			valid = false;
			return Settings::Distribution::AssocType::kKeyword;
		}
	}

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

	static std::vector<std::tuple<int, Settings::AlchemyEffect>> GetDistribution(std::vector<std::tuple<uint64_t, float>> effectmap, int range)
	{
		std::vector<std::tuple<int, Settings::AlchemyEffect>> ret;
		uint64_t tmp = 0;
		std::map<Settings::AlchemyEffect, float> map;
		// iterate over all effects in effect map
		for (int i = 0; i < effectmap.size(); i++) {
			// iterate over all effects that could be mashed up in the effect map we can only iterate until c << 62 so as to avoid
			// an overflow error
			for (uint64_t c = 1; c < 4611686018427387903; c = c << 1) {
				if ((tmp = (std::get<0>(effectmap[i]) & c)) > 0) {
					map.insert_or_assign(static_cast<Settings::AlchemyEffect>(tmp), std::get<1>(effectmap[i]));
				}
			}
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

	static uint64_t SumAlchemyEffects(std::vector<std::tuple<int, Settings::AlchemyEffect>> list)
	{
		uint64_t ret = 0;
		for (int i = 0; i < list.size(); i++) {
			LOG1_4("{}[SumAlchemyEffects] effect to sum: {}", GetHex(static_cast<uint64_t>(std::get<1>(list[i]))));
			ret |= static_cast<uint64_t>(std::get<1>(list[i]));
		}
		LOG1_4("{}[SumAlchemyEffects] summed effects: {}", GetHex(ret));
		return ret;
	}

	static bool CanApplyPoison(RE::Actor* actor)
	{
		auto ied = actor->GetEquippedEntryData(false);
		if (ied == nullptr)
			return false;
		RE::ExtraPoison* pois = nullptr;
		if (ied->extraLists) {
			for (const auto& extraL : *(ied->extraLists)) {
				pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
				if (pois)
					break;
			}
		}
		//logger::info("poison check. Actor:\t{}\tpoison:\t{}\t count:\t{}", actor->GetName(), pois && pois->poison ? pois->poison->GetName() : "not found", pois ? pois->count :  -1);

		if (pois && pois->count > 0)
			return false;
		return true;
	}
	#pragma endregion

};
