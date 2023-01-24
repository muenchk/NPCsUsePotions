#include <Utility.h>
#include <AlchemyEffect.h>
#include <Distribution.h>
#include "Compatibility.h"

bool Utility::SortMagnitude(std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> first, std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> second)
{
	return (std::get<0>(first) * (std::get<1>(first) == 0 ? 1 : std::get<1>(first))) > (std::get<0>(second) * (std::get<1>(second) == 0 ? 1 : std::get<1>(second)));
}

std::string Utility::ToString(ActorStrength acs)
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

std::string Utility::ToString(ItemStrength is)
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

std::string Utility::ToString(AlchemyEffect ae)
{
	switch (ae) {
	case AlchemyEffect::kAlteration:
		return "Alteration";
	case AlchemyEffect::kAnyFood:
		return "AnyFood";
	case AlchemyEffect::kAnyFortify:
		return "AnyFortify";
	case AlchemyEffect::kAnyPoison:
		return "AnyPoison";
	case AlchemyEffect::kAnyPotion:
		return "AnyPotion";
	case AlchemyEffect::kArchery:
		return "Archery";
	case AlchemyEffect::kAttackDamageMult:
		return "AttackDamageMult";
	case AlchemyEffect::kBlock:
		return "Block";
	case AlchemyEffect::kBlood:
		return "Blood";
	case AlchemyEffect::kBowSpeed:
		return "BowSpeed";
	case AlchemyEffect::kConjuration:
		return "Conjuration";
	case AlchemyEffect::kCriticalChance:
		return "CriticalChance";
	case AlchemyEffect::kDamageResist:
		return "DamageResist";
	case AlchemyEffect::kDestruction:
		return "Destruction";
	case AlchemyEffect::kFear:
		return "Fear";
	case AlchemyEffect::kFrenzy:
		return "Frenzy";
	case AlchemyEffect::kHealRate:
		return "HealRate";
	case AlchemyEffect::kHealRateMult:
		return "HealRateMult";
	case AlchemyEffect::kHealth:
		return "Health";
	case AlchemyEffect::kHeavyArmor:
		return "HeavyArmor";
	case AlchemyEffect::kIllusion:
		return "Illusion";
	case AlchemyEffect::kInvisibility:
		return "Invisibility";
	case AlchemyEffect::kLightArmor:
		return "LightArmor";
	case AlchemyEffect::kLockpicking:
		return "Lockpicking";
	case AlchemyEffect::kMagicka:
		return "Magicka";
	case AlchemyEffect::kMagickaRate:
		return "MagickaRate";
	case AlchemyEffect::kMagickaRateMult:
		return "MagickaRateMult";
	case AlchemyEffect::kMeleeDamage:
		return "MeleeDamage";
	case AlchemyEffect::kNone:
		return "None";
	case AlchemyEffect::kOneHanded:
		return "OneHanded";
	case AlchemyEffect::kParalysis:
		return "Paralysis";
	case AlchemyEffect::kPickpocket:
		return "Pickpocket";
	case AlchemyEffect::kPoisonResist:
		return "PoisonResist";
	case AlchemyEffect::kReflectDamage:
		return "ReflectDamage";
	case AlchemyEffect::kResistDisease:
		return "ResistDisease";
	case AlchemyEffect::kResistFire:
		return "ResistFire";
	case AlchemyEffect::kResistFrost:
		return "ResistFrost";
	case AlchemyEffect::kResistMagic:
		return "ResistMagic";
	case AlchemyEffect::kResistShock:
		return "ResistShock";
	case AlchemyEffect::kRestoration:
		return "Restoration";
	case AlchemyEffect::kSneak:
		return "Sneak";
	case AlchemyEffect::kSpeedMult:
		return "SpeedMult";
	case AlchemyEffect::kStamina:
		return "Stamina";
	case AlchemyEffect::kStaminaRate:
		return "StaminaRate";
	case AlchemyEffect::kStaminaRateMult:
		return "StaminaRateMult";
	case AlchemyEffect::kTwoHanded:
		return "TwoHanded";
	case AlchemyEffect::kUnarmedDamage:
		return "UnarmedDamage";
	case AlchemyEffect::kWeaponSpeedMult:
		return "WeapenSpeedMult";
	case AlchemyEffect::kCureDisease:
		return "CureDisease";
	case AlchemyEffect::kCurePoison:
		return "CurePoison";
	case AlchemyEffect::kEnchanting:
		return "Enchanting";
	case AlchemyEffect::kWaterbreathing:
		return "Waterbreathing";
	case AlchemyEffect::kSmithing:
		return "Smithing";
	case AlchemyEffect::kSpeech:
		return "Speech";
	case AlchemyEffect::kCarryWeight:
		return "CarryWeight";
	case AlchemyEffect::kAlchemy:
		return "Alchemy";
	case AlchemyEffect::kPersuasion:
		return "Persuasion";
	case AlchemyEffect::kFortifyHealth:
		return "FortifyHealth";
	case AlchemyEffect::kFortifyMagicka:
		return "FortifyMagicka";
	case AlchemyEffect::kFortifyStamina:
		return "FortifyStamina";
	case AlchemyEffect::kCustom:
		return "Custom";
	default:
		return "Unknown";
	}
}

std::string Utility::ToString(AlchemyEffectBase ae)
{
	std::string ret = "|";
	if (ae & Base(AlchemyEffect::kAlteration))
		ret += "Alteration|";
	if (ae & Base(AlchemyEffect::kArchery))
		ret += "Archery|";
	if (ae & Base(AlchemyEffect::kAttackDamageMult))
		ret += "AttackDamageMult|";
	if (ae & Base(AlchemyEffect::kBlock))
		ret += "Block|";
	if (ae & Base(AlchemyEffect::kBlood))
		ret += "Blood|";
	if (ae & Base(AlchemyEffect::kBowSpeed))
		ret += "BowSpeed|";
	if (ae & Base(AlchemyEffect::kConjuration))
		ret += "Conjuration|";
	if (ae & Base(AlchemyEffect::kCriticalChance))
		ret += "CriticalChance|";
	if (ae & Base(AlchemyEffect::kDamageResist))
		ret += "DamageResist|";
	if (ae & Base(AlchemyEffect::kDestruction))
		ret += "Destruction|";
	if (ae & Base(AlchemyEffect::kFear))
		ret += "Fear|";
	if (ae & Base(AlchemyEffect::kFrenzy))
		ret += "Frenzy|";
	if (ae & Base(AlchemyEffect::kHealRate))
		ret += "HealRate|";
	if (ae & Base(AlchemyEffect::kHealRateMult))
		ret += "HealRateMult|";
	if (ae & Base(AlchemyEffect::kHealth))
		ret += "Health|";
	if (ae & Base(AlchemyEffect::kHeavyArmor))
		ret += "HeavyArmor|";
	if (ae & Base(AlchemyEffect::kIllusion))
		ret += "Illusion|";
	if (ae & Base(AlchemyEffect::kInvisibility))
		ret += "Invisibility|";
	if (ae & Base(AlchemyEffect::kLightArmor))
		ret += "LightArmor|";
	if (ae & Base(AlchemyEffect::kLockpicking))
		ret += "Lockpicking|";
	if (ae & Base(AlchemyEffect::kMagicka))
		ret += "Magicka|";
	if (ae & Base(AlchemyEffect::kMagickaRate))
		ret += "MagickaRate|";
	if (ae & Base(AlchemyEffect::kMagickaRateMult))
		ret += "MagickaRateMult|";
	if (ae & Base(AlchemyEffect::kMeleeDamage))
		ret += "MeleeDamage|";
	if (ae & Base(AlchemyEffect::kNone))
		ret += "None|";
	if (ae & Base(AlchemyEffect::kOneHanded))
		ret += "OneHanded|";
	if (ae & Base(AlchemyEffect::kParalysis))
		ret += "Paralysis|";
	if (ae & Base(AlchemyEffect::kPickpocket))
		ret += "Pickpocket|";
	if (ae & Base(AlchemyEffect::kPoisonResist))
		ret += "PoisonResist|";
	if (ae & Base(AlchemyEffect::kReflectDamage))
		ret += "ReflectDamage|";
	if (ae & Base(AlchemyEffect::kResistDisease))
		ret += "ResistDisease|";
	if (ae & Base(AlchemyEffect::kResistFire))
		ret += "ResistFire|";
	if (ae & Base(AlchemyEffect::kResistFrost))
		ret += "ResistFrost|";
	if (ae & Base(AlchemyEffect::kResistMagic))
		ret += "ResistMagic|";
	if (ae & Base(AlchemyEffect::kResistShock))
		ret += "ResistShock|";
	if (ae & Base(AlchemyEffect::kRestoration))
		ret += "Restoration|";
	if (ae & Base(AlchemyEffect::kSneak))
		ret += "Sneak|";
	if (ae & Base(AlchemyEffect::kSpeedMult))
		ret += "SpeedMult|";
	if (ae & Base(AlchemyEffect::kStamina))
		ret += "Stamina|";
	if (ae & Base(AlchemyEffect::kStaminaRate))
		ret += "StaminaRate|";
	if (ae & Base(AlchemyEffect::kStaminaRateMult))
		ret += "StaminaRateMult|";
	if (ae & Base(AlchemyEffect::kTwoHanded))
		ret += "TwoHanded|";
	if (ae & Base(AlchemyEffect::kUnarmedDamage))
		ret += "UnarmedDamage|";
	if (ae & Base(AlchemyEffect::kWeaponSpeedMult))
		ret += "WeapenSpeedMult|";
	if (ae & Base(AlchemyEffect::kCureDisease))
		ret += "CureDisease|";
	if (ae & Base(AlchemyEffect::kCurePoison))
		ret += "CurePoison|";
	if (ae & Base(AlchemyEffect::kEnchanting))
		ret += "Enchanting|";
	if (ae & Base(AlchemyEffect::kWaterbreathing))
		ret += "Waterbreathing|";
	if (ae & Base(AlchemyEffect::kSmithing))
		ret += "Smithing|";
	if (ae & Base(AlchemyEffect::kSpeech))
		ret += "Speech|";
	if (ae & Base(AlchemyEffect::kCarryWeight))
		ret += "CarryWeight|";
	if (ae & Base(AlchemyEffect::kAlchemy))
		ret += "Alchemy|";
	if (ae & Base(AlchemyEffect::kPersuasion))
		ret += "Persuasion|";
	if (ae & Base(AlchemyEffect::kFortifyHealth))
		ret += "FortifyHealth|";
	if (ae & Base(AlchemyEffect::kFortifyMagicka))
		ret += "FortifyMagicka|";
	if (ae & Base(AlchemyEffect::kFortifyStamina))
		ret += "FortifyStamina|";
	if (ae & Base(AlchemyEffect::kCustom))
		ret += "Custom|";

	if (ret == "|")
		return "|Unknown|";
	return ret;
}

std::string Utility::ToStringCombatStyle(uint32_t style)
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

uint32_t Utility::GetCombatData(RE::Actor* actor)
{
	LOG_3("{}[Utility] [GetCombatData]");
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
	} else if (rightweap && (rightweap->GetWeaponType() == RE::WEAPON_TYPE::kBow || rightweap->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow)) {
		// ranged
		combatdata |= static_cast<uint32_t>(CurrentCombatStyle::Ranged);
	} else if (rightweap && leftweap) {
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
							} catch (std::exception&) {
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

int Utility::GetItemType(RE::TESForm* form)
{
	if (form == nullptr)
		return 0; // HandtoHand
	RE::TESObjectWEAP* weap = form->As<RE::TESObjectWEAP>();
	if (weap) {
		if (weap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandSword)
			return 1;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandDagger)
			return 2;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandAxe)
			return 3;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kOneHandMace)
			return 4;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kTwoHandSword)
			return 5;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kTwoHandAxe)
			return 6;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kBow)
			return 7;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kStaff)
			return 8;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kCrossbow)
			return 12;
		else if (weap->GetWeaponType() == RE::WEAPON_TYPE::kHandToHandMelee)
			return 0;
	}
	RE::TESObjectARMO* armo = form->As<RE::TESObjectARMO>();
	if (armo)
		return 10;
	RE::SpellItem* spell = form->As<RE::SpellItem>();
	if (spell)
		return 9; // magic spell

	RE::TESObjectMISC* misc = form->As<RE::TESObjectMISC>();
	if (misc && misc->GetFormID() == 0x0001D4EC)
		return 11;

	return 0; // fallback
}

uint32_t Utility::GetArmorData(RE::Actor* actor)
{
	LOG_3("{}[Utility] [GetArmorData]");
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
	uint32_t ret = 0;  // also CurrentArmor::Nothing in case nothing below fires
	if (armorHeavy >= 2)
		ret |= static_cast<uint32_t>(CurrentArmor::HeavyArmor);
	if (armorLight >= 2)
		ret |= static_cast<uint32_t>(CurrentArmor::LightArmor);
	if (clothing >= 2)
		ret |= static_cast<uint32_t>(CurrentArmor::Clothing);
	return ret;
}

std::vector<int> Utility::ParseIntArray(std::string line)
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
		line.erase(0, pos + 1);
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

RE::TESForm* Utility::GetTESForm(RE::TESDataHandler* datahandler, RE::FormID formid, std::string pluginname, std::string editorid)
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

std::vector<std::tuple<Distribution::AssocType, RE::FormID>> Utility::ParseAssocObjects(std::string input, bool& error, std::string file, std::string line, int& totalobjects)
{
	LOG_3("{}[Utility] [ParseAssocObjects]");
	std::vector<std::tuple<Distribution::AssocType, RE::FormID>> ret;
	try {
		auto datahandler = RE::TESDataHandler::GetSingleton();
		size_t pos;
		Distribution::AssocType type = Distribution::AssocType::kActor;
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
			input.erase(0, input.find('<') + 1);
			if ((pos = input.find('>')) != std::string::npos) {
				// we have a valid entry, probably
				totalobjects++;
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
				editorid = entry.substr(0, pos);
				entry.erase(0, pos + 1);
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
						logwarn("[Utility] [ParseAssocObjects] Form {} has an unsupported FormType. file: \"{}\" Rule: \"{}\"", PrintForm(tmp), file, line);
					}
				} else {
					if (form) {
						logwarn("[Utility] [ParseAssocObjects] FormID {} couldn't be found. file: \"{}\" Rule: \"\"", GetHex(formid), file, line);
					} else {
						logwarn("[Utility] [ParseAssocObjects] EditorID {} couldn't be found. file: \"{}\" Rule: \"\"", editorid, file, line);
					}
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

std::vector<std::tuple<Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> Utility::ParseCustomObjects(std::string input, bool& error, std::string file, std::string line)
{
	LOG_3("{}[Utility] [ParseAssocObjectsChance]");
	std::vector<std::tuple<Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> ret;
	try {
		auto datahandler = RE::TESDataHandler::GetSingleton();
		size_t pos;
		Distribution::AssocType type = Distribution::AssocType::kActor;
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
		uint64_t tmp1;
		uint64_t tmp2;
		std::string tmp3;
		bool giveonce = false;
		std::vector<std::tuple<uint64_t, uint32_t, std::string>> conditionsall;
		std::vector<std::tuple<uint64_t, uint32_t, std::string>> conditionsany;
		uint32_t num;
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
				// num items
				if ((pos = entry.find(',')) == std::string::npos) {
					error = true;
					return ret;
				}
				numitems = entry.substr(0, pos);
				entry.erase(0, pos + 1);
				try {
					num = (std::stol(numitems));
				} catch (std::exception&) {
				}
				// exclude
				if ((pos = entry.find(',')) == std::string::npos) {
					error = true;
					return ret;
				}
				if (entry.substr(0, pos) == "1")
					exclude = true;
				entry.erase(0, pos + 1);
				// give once
				if ((pos = entry.find(',')) == std::string::npos) {
					error = true;
					return ret;
				}
				if (entry.substr(0, pos) == "1")
					giveonce = true;
				entry.erase(0, pos + 1);

				// conditionsall
				if ((pos = entry.find(',')) == std::string::npos) {
					error = true;
					return ret;
				}
				std::string condall = entry.substr(0, pos);
				entry.erase(0, pos + 1);
				// we have to parse a substr of variable length
				// ; - separates entries
				// : - separates first and second argument
				std::vector<std::string> splits;
				while ((pos = condall.find(';')) != std::string::npos) {
					splits.push_back(condall.substr(0, pos));
					condall.erase(0, pos + 1);
				}
				splits.push_back(condall);
				for (int x = 0; x < splits.size(); x++) {
					tmp3 = splits[x];
					if ((pos = tmp3.find(':')) != std::string::npos) {
						try {
							tmp1 = std::stoull(tmp3.substr(0, pos), nullptr, 16);
						} catch (std::exception&) {
							continue;
						}
						tmp3.erase(0, pos+1);
						if ((pos = tmp3.find(':')) != std::string::npos) {
							try {
								tmp2 = std::stoull(tmp3.substr(0, pos), nullptr, 16);
							} catch (std::exception&) {
								continue;
							}
							tmp3.erase(0, pos + 1);
						}
						else
						{
							try {
								tmp2 = std::stoull(tmp3, nullptr, 16);
							} catch (std::exception&) {
								continue;
							}
							tmp3 = "";
						}
						conditionsall.push_back({ tmp1, (uint32_t)tmp2, tmp3 });
					}
				}
				splits.clear();
				// conditionsany
				if ((pos = entry.find(',')) == std::string::npos) {
					error = true;
					return ret;
				}
				std::string condany = entry.substr(0, pos);
				entry.erase(0, pos + 1);// we have to parse a substr of variable length
				// ; - separates entries
				// : - separates first and second argument
				while ((pos = condany.find(';')) != std::string::npos) {
					splits.push_back(condany.substr(0, pos));
					condany.erase(0, pos + 1);
				}
				splits.push_back(condany);
				for (int x = 0; x < splits.size(); x++) {
					tmp3 = splits[x];
					if ((pos = tmp3.find(':')) != std::string::npos) {
						try {
							tmp1 = std::stoull(tmp3.substr(0, pos), nullptr, 16);
						} catch (std::exception&) {
							continue;
						}
						tmp3.erase(0, pos + 1);
						if ((pos = tmp3.find(':')) != std::string::npos) {
							try {
								tmp2 = std::stoull(tmp3.substr(0, pos), nullptr, 16);
							} catch (std::exception&) {
								continue;
							}
							tmp3.erase(0, pos + 1);
						} else {
							try {
								tmp2 = std::stoull(tmp3, nullptr, 16);
							} catch (std::exception&) {
								continue;
							}
							tmp3 = "";
						}
						conditionsall.push_back({ tmp1, (uint32_t)tmp2, tmp3 });
					}
				}
				splits.clear();

				// flags
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
				if (flags == std::string("") || ToLower(flags) == std::string("object")) {
					flag = CustomItemFlag::Object;
				} else if (ToLower(flags) == std::string("potion")) {
					flag = CustomItemFlag::Potion;
				} else if (ToLower(flags) == std::string("poison")) {
					flag = CustomItemFlag::Poison;
				} else if (ToLower(flags) == std::string("food")) {
					flag = CustomItemFlag::Food;
				} else if (ToLower(flags) == std::string("fortify")) {
					flag = CustomItemFlag::Fortify;
				} else if (ToLower(flags) == std::string("death")) {
					flag = CustomItemFlag::DeathObject;
				} else {
					flag = CustomItemFlag::Object;
				}
				LOG1_3("{}[Utility] [ParseAssocObjectsChance] Flag: \t\t{}", flags);
				LOG1_3("{}[Utility] [ParseAssocObjectsChance] Flag converted: {}", static_cast<uint64_t>(flag));

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
						ret.push_back({ type, tmp->GetFormID(), num, flag, (uint8_t)chance, exclude, conditionsall, conditionsany, giveonce });
					} else {
						logwarn("[Utility] [ParseAssocObjectsChance] Form {} has an unsupported FormType. file: \"{}\" Rule: \"{}\"", PrintForm(tmp), file, line);
					}
				} else {
					if (form) {
						logwarn("[Utility] [ParseAssocObjectsChance] FormID {} couldn't be found. file: \"{}\" Rule: \"\"", GetHex(formid), file, line);
					} else {
						logwarn("[Utility] [ParseAssocObjectsChance] EditorID {} couldn't be found. file: \"{}\" Rule: \"\"", editorid, file, line);
					}
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

Distribution::AssocType Utility::MatchValidFormType(RE::FormType type, bool& valid)
{
	switch (type) {
	case RE::FormType::Keyword:
		valid = true;
		return Distribution::AssocType::kKeyword;
	case RE::FormType::Faction:
		valid = true;
		return Distribution::AssocType::kFaction;
	case RE::FormType::Race:
		valid = true;
		return Distribution::AssocType::kRace;
	case RE::FormType::NPC:
		valid = true;
		return Distribution::AssocType::kNPC;
	case RE::FormType::ActorCharacter:
		valid = true;
		return Distribution::AssocType::kActor;
	case RE::FormType::Reference:
		valid = true;
		return Distribution::AssocType::kItem;
	case RE::FormType::Class:
		valid = true;
		return Distribution::AssocType::kClass;
	case RE::FormType::CombatStyle:
		valid = true;
		return Distribution::AssocType::kCombatStyle;
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
		return Distribution::AssocType::kItem;
	default:
		valid = false;
		return Distribution::AssocType::kKeyword;
	}
}

std::vector<std::tuple<uint64_t, float>> Utility::ParseAlchemyEffects(std::string input, bool& error)
{
	LOG_3("{}[Utility] [ParseAlchemyEffect]");
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
					tmp = tmp.erase(0, pos + 1);
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
	LOG1_4("{}[Uility] [ParseAlchemyEffect] found effects: {}", ret.size());
	return ret;
}

std::vector<std::tuple<int, AlchemyEffect>> Utility::GetDistribution(std::vector<std::tuple<uint64_t, float>> effectmap, int range, bool chance)
{
	LOG_3("{}[Utility] [GetDistribution]");
	std::vector<std::tuple<int, AlchemyEffect>> ret;
	if (effectmap.size() == 0)
		return ret;
	uint64_t tmp = 0;
	std::map<AlchemyEffect, float> map;
	// iterate over all effects in effect map
	for (int i = 0; i < effectmap.size(); i++) {
		// iterate over all effects that could be mashed up in the effect map we can only iterate until c 62 so as to avoid
		// an overflow error
		for (uint64_t c = 1; c < 4611686018427387905; c = c << 1) {
			if ((tmp = (std::get<0>(effectmap[i]) & c)) > 0) {
				map.insert_or_assign(static_cast<AlchemyEffect>(tmp), std::get<1>(effectmap[i]));
			}
		}
	}
	if (chance) {
		map.insert_or_assign(static_cast<AlchemyEffect>(AlchemyEffect::kCustom), 1.0f);
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

std::vector<std::tuple<int, AlchemyEffect>> Utility::GetDistribution(std::map<AlchemyEffect, float> map, int range, bool chance)
{
	LOG_3("{}[Utility] [GetDistributionMap]");
	if (chance) {
		map.insert_or_assign(static_cast<AlchemyEffect>(AlchemyEffect::kCustom), 1.0f);
	}
	std::vector<std::tuple<int, AlchemyEffect>> ret;
	if (map.size() == 0)
		return ret;
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

std::map<AlchemyEffect, float> Utility::UnifyEffectMap(std::vector<std::tuple<uint64_t, float>> effectmap)
{
	LOG_3("{}[Utility] [UnifyEffectMap]");
	uint64_t tmp = 0;
	std::map<AlchemyEffect, float> map;
	// iterate over all effects in effect map
	for (int i = 0; i < effectmap.size(); i++) {
		// iterate over all effects that could be mashed up in the effect map we can only iterate until c 62 so as to avoid
		// an overflow error
		for (uint64_t c = 1; c < 4611686018427387905; c = c << 1) {
			if ((tmp = (std::get<0>(effectmap[i]) & c)) > 0) {
				map.insert_or_assign(static_cast<AlchemyEffect>(tmp), std::get<1>(effectmap[i]));
			}
		}
	}
	return map;
}

uint64_t Utility::SumAlchemyEffects(std::vector<std::tuple<int, AlchemyEffect>> list, bool chance)
{
	LOG_3("{}[Utility] [SumAlchemyEffects]");
	uint64_t ret = 0;
	for (int i = 0; i < list.size(); i++) {
		LOG1_4("{}[Utility] [SumAlchemyEffects] effect to sum: {}", GetHex(static_cast<uint64_t>(std::get<1>(list[i]))));
		ret |= static_cast<uint64_t>(std::get<1>(list[i]));
	}
	if (chance)
		ret |= static_cast<uint64_t>(AlchemyEffect::kCustom);
	LOG1_4("{}[SumAlchemyEffects] summed effects: {}", GetHex(ret));
	return ret;
}

bool Utility::CanApplyPoison(RE::Actor* actor)
{
	LOG_3("{}[Utility] [CanApplyPoison]");
	auto ied = actor->GetEquippedEntryData(false);
	RE::ExtraPoison* pois = nullptr;
	if (ied) {
		if (ied->extraLists && ied->IsPoisoned()) {
			for (const auto& extraL : *(ied->extraLists)) {
				pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
				if (pois)
					break;
			}
		}
	}
	if (pois == nullptr && Compatibility::GetSingleton()->CanApplyPoisonToLeftHand()) {
		ied = actor->GetEquippedEntryData(true);
		if (ied && ied->extraLists) {
			if (ied->IsPoisoned()) {
				for (const auto& extraL : *(ied->extraLists)) {
					pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
					if (pois)
						break;
				}
			}
		}
	}
	LOG3_4("{}[Utility] [CanApplyPoison] poison check. Actor:\t{}\tpoison:\t{}\t count:\t{}", Utility::PrintForm(actor), pois && pois->poison ? pois->poison->GetName() : "not found", std::to_string(pois ? pois->count : -1));

	if (pois && pois->count > 0)
		return false;
	return true;
}

bool Utility::GetAppliedPoison(RE::Actor* actor, RE::ExtraPoison* &pois)
{
	auto ied = actor->GetEquippedEntryData(false);
	if (ied) {
		if (ied->extraLists && ied->IsPoisoned()) {
			for (const auto& extraL : *(ied->extraLists)) {
				pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
				if (pois)
					break;
			}
		}
	}
	if (pois == nullptr && Compatibility::GetSingleton()->CanApplyPoisonToLeftHand()) {
		ied = actor->GetEquippedEntryData(true);
		if (ied && ied->extraLists && ied->IsPoisoned()) {
			for (const auto& extraL : *(ied->extraLists)) {
				pois = (RE::ExtraPoison*)extraL->GetByType<RE::ExtraPoison>();
				if (pois)
					break;
			}
		}
	}
	LOG3_4("[Utility] [GetAppliedPoison] poison check. Actor:\t{}\tpoison:\t{}\t count:\t{}", Utility::PrintForm(actor), pois && pois->poison ? pois->poison->GetName() : "not found", std::to_string(pois ? pois->count : -1));

	if (pois && pois->count > 0)
		return true;
	return false;
}

bool Utility::VerifyActorInfo(ActorInfo* acinfo)
{
	if (acinfo == nullptr || acinfo->actor == nullptr || acinfo->actor->GetFormID() == 0) {
		LOG_1("{}[Utility] [VerifyActorInfo] actor info damaged");
		return false;
	}
	if (acinfo->citems == nullptr) {
		acinfo->citems = new ActorInfo::CustomItems();
		acinfo->CalcCustomItems();
	}
	return true;
}

std::string Utility::Mods::GetPluginName(RE::TESForm* form)
{
	auto datahandler = RE::TESDataHandler::GetSingleton();
	const RE::TESFile* file = nullptr;
	std::string_view name = std::string_view{ "" };
	if ((form->GetFormID() >> 24) == 0xFF)
		return "";
	if ((form->GetFormID() >> 24) != 0xFE) {
		auto itr = Settings::pluginIndexMap.find(form->GetFormID() & 0xFF000000);
		if (itr != Settings::pluginIndexMap.end())
			return itr->second;
		return "";
	}
	if ((form->GetFormID() >> 24) == 0x00)
		return "Skyrim.esm";
	// light mod
	auto itr = Settings::pluginIndexMap.find(form->GetFormID() & 0xFFFFF000);
	if (itr != Settings::pluginIndexMap.end())
		return itr->second;
	return "";
}

std::string Utility::Mods::GetPluginName(uint32_t pluginIndex)
{
	auto itr = Settings::pluginIndexMap.find(pluginIndex);
	if (itr != Settings::pluginIndexMap.end())
		return itr->second;
	else
		return "";
}

uint32_t Utility::Mods::GetPluginIndex(std::string pluginname)
{
	auto itr = Settings::pluginNameMap.find(pluginname);
	if (itr != Settings::pluginNameMap.end()) {
		return itr->second;
	} else
		return 0x1;
}

uint32_t Utility::Mods::GetPluginIndex(RE::TESForm* form)
{
	return GetPluginIndex(GetPluginName(form));
}

bool Utility::ValidateActor(RE::Actor* actor)
{
	if (actor == nullptr || actor->IsDeleted() || actor->IsMarkedForDeletion() || actor->GetFormID() == 0 || actor->IsDisabled())
		return false; 

	return true;
}

Misc::NPCTPLTInfo Utility::ExtractTemplateInfo(RE::TESLevCharacter* lvl)
{
	if (lvl == nullptr)
		return Misc::NPCTPLTInfo{};
	// just try to grab the first entry of the leveled list, since they should all share
	// factions 'n stuff
	if (lvl->entries.size() > 0) {
		uint32_t plugID = Utility::Mods::GetPluginIndex(lvl);
		RE::TESForm* entry = lvl->entries[0].form;
		RE::TESNPC* tplt = entry->As<RE::TESNPC>();
		RE::TESLevCharacter* lev = entry->As<RE::TESLevCharacter>();
		if (tplt)
			return [&tplt, &plugID]() { auto info = ExtractTemplateInfo(tplt); if (plugID != 0x1) {info.pluginID = plugID;}return info; }();

		else if (lev)
			return [&lev, &plugID]() { auto info = ExtractTemplateInfo(lev); if (plugID != 0x1) {info.pluginID = plugID;} return info; }();
		else
			;  //loginfo("template invalid");
	}
	return Misc::NPCTPLTInfo{};
}

Misc::NPCTPLTInfo Utility::ExtractTemplateInfo(RE::TESNPC* npc)
{
	Misc::NPCTPLTInfo info;
	if (npc == nullptr)
		return info;
	if (npc->baseTemplateForm == nullptr) {
		// we are at the base, so do the main work
		info.tpltrace = npc->GetRace();
		info.tpltstyle = npc->combatStyle;
		info.tpltclass = npc->npcClass;
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				info.tpltkeywords.push_back(npc->keywords[i]);
		}
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				info.tpltfactions.push_back(npc->factions[i].faction);
		}

		uint32_t plugID = Utility::Mods::GetPluginIndex(npc);
		if (plugID != 0x1) {
			info.pluginID = plugID;
		}
		return info;
	}
	RE::TESNPC* tplt = npc->baseTemplateForm->As<RE::TESNPC>();
	RE::TESLevCharacter* lev = npc->baseTemplateForm->As<RE::TESLevCharacter>();
	Misc::NPCTPLTInfo tpltinfo;
	if (tplt) {
		// get info about template and then integrate into our local information according to what we use
		tpltinfo = ExtractTemplateInfo(tplt);
	} else if (lev) {
		tpltinfo = ExtractTemplateInfo(lev);
	} else {
		//loginfo("template invalid");
	}

	info.pluginID = tpltinfo.pluginID;

	uint32_t plugID = Utility::Mods::GetPluginIndex(npc);
	if (plugID != 0x1) {
		info.pluginID = plugID;
	}

	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kFactions) {
		info.tpltfactions = tpltinfo.tpltfactions;
	} else {
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				info.tpltfactions.push_back(npc->factions[i].faction);
		}
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kKeywords) {
		info.tpltkeywords = tpltinfo.tpltkeywords;
	} else {
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				info.tpltkeywords.push_back(npc->keywords[i]);
		}
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kTraits) {
		// race
		info.tpltrace = tpltinfo.tpltrace;
	} else {
		info.tpltrace = npc->GetRace();
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kStats) {
		// class
		info.tpltclass = tpltinfo.tpltclass;
	} else {
		info.tpltclass = npc->npcClass;
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kAIData) {
		// combatstyle
		info.tpltstyle = tpltinfo.tpltstyle;
	} else {
		info.tpltstyle = npc->combatStyle;
	}
	return info;
}

Misc::NPCTPLTInfo Utility::ExtractTemplateInfo(RE::Actor* actor)
{
	if (actor != nullptr) {
		Misc::NPCTPLTInfo tpltinfo = ExtractTemplateInfo(actor->GetActorBase());
		uint32_t plugID = Utility::Mods::GetPluginIndex(actor);
		if (plugID != 0x1) {
			tpltinfo.pluginID = plugID;
		}
		return tpltinfo;
	}
	return Misc::NPCTPLTInfo{};
}
