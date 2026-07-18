#include <Utility.h>
#include <Distribution.h>
#include "Compatibility.h"


#define TRANSLATE(in, out) if (SKSE::Translation::Translate(in, out); out.empty()) out = in;

bool Utility::SortMagnitude(std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> first, std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> second)
{
	return (std::get<0>(first) * (std::get<1>(first) == 0 ? 1 : std::get<1>(first))) > (std::get<0>(second) * (std::get<1>(second) == 0 ? 1 : std::get<1>(second)));
}

bool Utility::SortPotion(std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> first, std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> second)
{
	// the goal is to sort item so, that items with fewer effects are prioritized, and they are still sorted for their magnitude
	// first sort for number of magic effects attached to the item
	if (std::get<2>(first)->effects.size() != std::get<2>(second)->effects.size())
		return std::get<2>(first)->effects.size() > std::get<2>(second)->effects.size();
	// then sort for the magnitude of the most potent effect
	else
		return (std::get<0>(first) * (std::get<1>(first) == 0 ? 1 : std::get<1>(first))) > (std::get<0>(second) * (std::get<1>(second) == 0 ? 1 : std::get<1>(second)));
}

bool Utility::SortFortify(std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> first, std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> second)
{
	// the goal is to sort items so, that items with fewer effects are prioritized, and they are still sorted for their magnitude
	// additionally, items that have restorative effects have a lower priority, so as not to waste any effects.
	// 
	// first check whether there are restorative options attached
	if (std::get<3>(first).HasRestorativeEffect() && std::get<3>(second).HasRestorativeEffect() == false)
		return false;
	else if (std::get<3>(first).HasRestorativeEffect() == false && std::get<3>(second).HasRestorativeEffect())
		return true;
	else {
		// first sort for number of magic effects attached to the item
		if (std::get<2>(first)->effects.size() != std::get<2>(second)->effects.size())
			return std::get<2>(first)->effects.size() > std::get<2>(second)->effects.size();
		// then sort for the magnitude of the most potent effect
		else
			return (std::get<0>(first) * (std::get<1>(first) == 0 ? 1 : std::get<1>(first))) > (std::get<0>(second) * (std::get<1>(second) == 0 ? 1 : std::get<1>(second)));
	}
}

std::string Utility::PrintForm(ActorInfo* acinfo)
{
	if (acinfo == nullptr)
		return "None";
	return std::string("[") + typeid(ActorInfo).name() + "<" + Utility::GetHex(acinfo->GetFormID()) + "><" + acinfo->GetName() + "><" + acinfo->GetPluginname() + ">]";
}

std::string Utility::PrintForm(std::shared_ptr<ActorInfo> const& acinfo)
{
	if (acinfo == nullptr || acinfo->IsValid() == false || Logging::EnableGenericLogging == false)
		return "None";
	return std::string("[") + typeid(ActorInfo).name() + "<" + Utility::GetHex(acinfo->GetFormID()) + "><" + acinfo->GetName() + "><" + acinfo->GetPluginname() + ">]";
}

std::string Utility::PrintForm(std::weak_ptr<ActorInfo> acweak)
{
	if (std::shared_ptr<ActorInfo> acinfo = acweak.lock()) {
		if (acinfo == nullptr || acinfo->IsValid() == false || Logging::EnableGenericLogging == false)
			return "None";
		return std::string("[") + typeid(ActorInfo).name() + "<" + Utility::GetHex(acinfo->GetFormID()) + "><" + acinfo->GetName() + "><" + acinfo->GetPluginname() + ">]";
	} else {
		return "None";
	}
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

std::string Utility::ToStringLocalized(AlchemicEffect ae)
{
	std::string result = "";
	if (ae.IsEffect()) {
		switch (ae.AlchemyBaseEffectFirst()) {
		case AlchemyBaseEffectFirst::kNone:
			break;
		default:
			TRANSLATE("$NUP_kUnknown", result);
			return result;
		}
		switch (ae.AlchemyBaseEffectSecond()) {
		case AlchemyBaseEffectSecond::kAlteration:
			TRANSLATE("$NUP_kAlteration", result);
			return result;
		case AlchemyBaseEffectSecond::kAnyFood:
			TRANSLATE("$NUP_kAnyFood", result);
			return result;
		case AlchemyBaseEffectSecond::kAnyFortify:
			TRANSLATE("$NUP_kAnyFortify", result);
			return result;
		case AlchemyBaseEffectSecond::kAnyPoison:
			TRANSLATE("$NUP_kAnyPoison", result);
			return result;
		case AlchemyBaseEffectSecond::kAnyPotion:
			TRANSLATE("$NUP_kAnyPotion", result);
			return result;
		case AlchemyBaseEffectSecond::kArchery:
			TRANSLATE("$NUP_kArchery", result);
			return result;
		case AlchemyBaseEffectSecond::kAttackDamageMult:
			TRANSLATE("$NUP_kAttackDamageMult", result);
			return result;
		case AlchemyBaseEffectSecond::kBlock:
			TRANSLATE("$NUP_kBlock", result);
			return result;
		case AlchemyBaseEffectSecond::kBlood:
			TRANSLATE("$NUP_kBlood", result);
			return result;
		case AlchemyBaseEffectSecond::kBowSpeed:
			TRANSLATE("$NUP_kBowSpeed", result);
			return result;
		case AlchemyBaseEffectSecond::kConjuration:
			TRANSLATE("$NUP_kConjuration", result);
			return result;
		case AlchemyBaseEffectSecond::kCriticalChance:
			TRANSLATE("$NUP_kCriticalChance", result);
			return result;
		case AlchemyBaseEffectSecond::kDamageResist:
			TRANSLATE("$NUP_kDamageResist", result);
			return result;
		case AlchemyBaseEffectSecond::kDestruction:
			TRANSLATE("$NUP_kDestruction", result);
			return result;
		case AlchemyBaseEffectSecond::kFear:
			TRANSLATE("$NUP_kFear", result);
			return result;
		case AlchemyBaseEffectSecond::kFrenzy:
			TRANSLATE("$NUP_kFrenzy", result);
			return result;
		case AlchemyBaseEffectSecond::kHealRate:
			TRANSLATE("$NUP_kHealRate", result);
			return result;
		case AlchemyBaseEffectSecond::kHealRateMult:
			TRANSLATE("$NUP_kHealRateMult", result);
			return result;
		case AlchemyBaseEffectSecond::kHealth:
			TRANSLATE("$NUP_kHealth", result);
			return result;
		case AlchemyBaseEffectSecond::kHeavyArmor:
			TRANSLATE("$NUP_kHeavyArmor", result);
			return result;
		case AlchemyBaseEffectSecond::kIllusion:
			TRANSLATE("$NUP_kIllusion", result);
			return result;
		case AlchemyBaseEffectSecond::kInvisibility:
			TRANSLATE("$NUP_kInvisibility", result);
			return result;
		case AlchemyBaseEffectSecond::kLightArmor:
			TRANSLATE("$NUP_kLightArmor", result);
			return result;
		case AlchemyBaseEffectSecond::kLockpicking:
			TRANSLATE("$NUP_kLockpicking", result);
			return result;
		case AlchemyBaseEffectSecond::kMagicka:
			TRANSLATE("$NUP_kMagicka", result);
			return result;
		case AlchemyBaseEffectSecond::kMagickaRate:
			TRANSLATE("$NUP_kMagickaRate", result);
			return result;
		case AlchemyBaseEffectSecond::kMagickaRateMult:
			TRANSLATE("$NUP_kMagickaRateMult", result);
			return result;
		case AlchemyBaseEffectSecond::kMeleeDamage:
			TRANSLATE("$NUP_kMeleeDamage", result);
			return result;
		case AlchemyBaseEffectSecond::kNone:
			TRANSLATE("$NUP_kNone", result);
			return result;
		case AlchemyBaseEffectSecond::kOneHanded:
			TRANSLATE("$NUP_kOneHanded", result);
			return result;
		case AlchemyBaseEffectSecond::kParalysis:
			TRANSLATE("$NUP_kParalysis", result);
			return result;
		case AlchemyBaseEffectSecond::kPickpocket:
			TRANSLATE("$NUP_kPickpocket", result);
			return result;
		case AlchemyBaseEffectSecond::kPoisonResist:
			TRANSLATE("$NUP_kPoisonResist", result);
			return result;
		case AlchemyBaseEffectSecond::kReflectDamage:
			TRANSLATE("$NUP_kReflectDamage", result);
			return result;
		case AlchemyBaseEffectSecond::kResistDisease:
			TRANSLATE("$NUP_kResistDisease", result);
			return result;
		case AlchemyBaseEffectSecond::kResistFire:
			TRANSLATE("$NUP_kResistFire", result);
			return result;
		case AlchemyBaseEffectSecond::kResistFrost:
			TRANSLATE("$NUP_kResistFrost", result);
			return result;
		case AlchemyBaseEffectSecond::kResistMagic:
			TRANSLATE("$NUP_kResistMagic", result);
			return result;
		case AlchemyBaseEffectSecond::kResistShock:
			TRANSLATE("$NUP_kResistShock", result);
			return result;
		case AlchemyBaseEffectSecond::kRestoration:
			TRANSLATE("$NUP_kRestoration", result);
			return result;
		case AlchemyBaseEffectSecond::kSneak:
			TRANSLATE("$NUP_kSneak", result);
			return result;
		case AlchemyBaseEffectSecond::kSpeedMult:
			TRANSLATE("$NUP_kSpeedMult", result);
			return result;
		case AlchemyBaseEffectSecond::kStamina:
			TRANSLATE("$NUP_kStamina", result);
			return result;
		case AlchemyBaseEffectSecond::kStaminaRate:
			TRANSLATE("$NUP_kStaminaRate", result);
			return result;
		case AlchemyBaseEffectSecond::kStaminaRateMult:
			TRANSLATE("$NUP_kStaminaRateMult", result);
			return result;
		case AlchemyBaseEffectSecond::kTwoHanded:
			TRANSLATE("$NUP_kTwoHanded", result);
			return result;
		case AlchemyBaseEffectSecond::kUnarmedDamage:
			TRANSLATE("$NUP_kUnarmedDamage", result);
			return result;
		case AlchemyBaseEffectSecond::kWeaponSpeedMult:
			TRANSLATE("$NUP_kWeaponSpeedMult", result);
			return result;
		case AlchemyBaseEffectSecond::kCureDisease:
			TRANSLATE("$NUP_kCureDisease", result);
			return result;
		case AlchemyBaseEffectSecond::kCurePoison:
			TRANSLATE("$NUP_kCurePoison", result);
			return result;
		case AlchemyBaseEffectSecond::kEnchanting:
			TRANSLATE("$NUP_kEnchanting", result);
			return result;
		case AlchemyBaseEffectSecond::kWaterbreathing:
			TRANSLATE("$NUP_kWaterbreathing", result);
			return result;
		case AlchemyBaseEffectSecond::kSmithing:
			TRANSLATE("$NUP_kSmithing", result);
			return result;
		case AlchemyBaseEffectSecond::kSpeech:
			TRANSLATE("$NUP_kSpeech", result);
			return result;
		case AlchemyBaseEffectSecond::kCarryWeight:
			TRANSLATE("$NUP_kCarryWeight", result);
			return result;
		case AlchemyBaseEffectSecond::kAlchemy:
			TRANSLATE("$NUP_kAlchemy", result);
			return result;
		case AlchemyBaseEffectSecond::kPersuasion:
			TRANSLATE("$NUP_kPersuasion", result);
			return result;
		case AlchemyBaseEffectSecond::kFortifyHealth:
			TRANSLATE("$NUP_kFortifyHealth", result);
			return result;
		case AlchemyBaseEffectSecond::kFortifyMagicka:
			TRANSLATE("$NUP_kFortifyMagicka", result);
			return result;
		case AlchemyBaseEffectSecond::kFortifyStamina:
			TRANSLATE("$NUP_kFortifyStamina", result);
			return result;
		case AlchemyBaseEffectSecond::kCustom:
			TRANSLATE("$NUP_kCustom", result);
			return result;
		case AlchemyBaseEffectSecond::kShield:
			TRANSLATE("$NUP_kShield", result);
			return result;
		case AlchemyBaseEffectSecond::kDamageUndead:
			TRANSLATE("$NUP_kDamageUndead", result);
			return result;
		default:
			TRANSLATE("$NUP_kUnknown", result);
			return result;
		}
	} else {
		std::string ret = "|";
		if ((ae & AlchemicEffect::kAlteration).IsValid())
			ret += "Alteration|";
		if ((ae & AlchemicEffect::kArchery).IsValid())
			ret += "Archery|";
		if ((ae & AlchemicEffect::kAttackDamageMult).IsValid())
			ret += "AttackDamageMult|";
		if ((ae & AlchemicEffect::kBlock).IsValid())
			ret += "Block|";
		if ((ae & AlchemicEffect::kBlood).IsValid())
			ret += "Blood|";
		if ((ae & AlchemicEffect::kBowSpeed).IsValid())
			ret += "BowSpeed|";
		if ((ae & AlchemicEffect::kConjuration).IsValid())
			ret += "Conjuration|";
		if ((ae & AlchemicEffect::kCriticalChance).IsValid())
			ret += "CriticalChance|";
		if ((ae & AlchemicEffect::kDamageResist).IsValid())
			ret += "DamageResist|";
		if ((ae & AlchemicEffect::kDestruction).IsValid())
			ret += "Destruction|";
		if ((ae & AlchemicEffect::kFear).IsValid())
			ret += "Fear|";
		if ((ae & AlchemicEffect::kFrenzy).IsValid())
			ret += "Frenzy|";
		if ((ae & AlchemicEffect::kHealRate).IsValid())
			ret += "HealRate|";
		if ((ae & AlchemicEffect::kHealRateMult).IsValid())
			ret += "HealRateMult|";
		if ((ae & AlchemicEffect::kHealth).IsValid())
			ret += "Health|";
		if ((ae & AlchemicEffect::kHeavyArmor).IsValid())
			ret += "HeavyArmor|";
		if ((ae & AlchemicEffect::kIllusion).IsValid())
			ret += "Illusion|";
		if ((ae & AlchemicEffect::kInvisibility).IsValid())
			ret += "Invisibility|";
		if ((ae & AlchemicEffect::kLightArmor).IsValid())
			ret += "LightArmor|";
		if ((ae & AlchemicEffect::kLockpicking).IsValid())
			ret += "Lockpicking|";
		if ((ae & AlchemicEffect::kMagicka).IsValid())
			ret += "Magicka|";
		if ((ae & AlchemicEffect::kMagickaRate).IsValid())
			ret += "MagickaRate|";
		if ((ae & AlchemicEffect::kMagickaRateMult).IsValid())
			ret += "MagickaRateMult|";
		if ((ae & AlchemicEffect::kMeleeDamage).IsValid())
			ret += "MeleeDamage|";
		if ((ae & AlchemicEffect::kNone).IsValid())
			ret += "None|";
		if ((ae & AlchemicEffect::kOneHanded).IsValid())
			ret += "OneHanded|";
		if ((ae & AlchemicEffect::kParalysis).IsValid())
			ret += "Paralysis|";
		if ((ae & AlchemicEffect::kPickpocket).IsValid())
			ret += "Pickpocket|";
		if ((ae & AlchemicEffect::kPoisonResist).IsValid())
			ret += "PoisonResist|";
		if ((ae & AlchemicEffect::kReflectDamage).IsValid())
			ret += "ReflectDamage|";
		if ((ae & AlchemicEffect::kResistDisease).IsValid())
			ret += "ResistDisease|";
		if ((ae & AlchemicEffect::kResistFire).IsValid())
			ret += "ResistFire|";
		if ((ae & AlchemicEffect::kResistFrost).IsValid())
			ret += "ResistFrost|";
		if ((ae & AlchemicEffect::kResistMagic).IsValid())
			ret += "ResistMagic|";
		if ((ae & AlchemicEffect::kResistShock).IsValid())
			ret += "ResistShock|";
		if ((ae & AlchemicEffect::kRestoration).IsValid())
			ret += "Restoration|";
		if ((ae & AlchemicEffect::kSneak).IsValid())
			ret += "Sneak|";
		if ((ae & AlchemicEffect::kSpeedMult).IsValid())
			ret += "SpeedMult|";
		if ((ae & AlchemicEffect::kStamina).IsValid())
			ret += "Stamina|";
		if ((ae & AlchemicEffect::kStaminaRate).IsValid())
			ret += "StaminaRate|";
		if ((ae & AlchemicEffect::kStaminaRateMult).IsValid())
			ret += "StaminaRateMult|";
		if ((ae & AlchemicEffect::kTwoHanded).IsValid())
			ret += "TwoHanded|";
		if ((ae & AlchemicEffect::kUnarmedDamage).IsValid())
			ret += "UnarmedDamage|";
		if ((ae & AlchemicEffect::kWeaponSpeedMult).IsValid())
			ret += "WeapenSpeedMult|";
		if ((ae & AlchemicEffect::kCureDisease).IsValid())
			ret += "CureDisease|";
		if ((ae & AlchemicEffect::kCurePoison).IsValid())
			ret += "CurePoison|";
		if ((ae & AlchemicEffect::kEnchanting).IsValid())
			ret += "Enchanting|";
		if ((ae & AlchemicEffect::kWaterbreathing).IsValid())
			ret += "Waterbreathing|";
		if ((ae & AlchemicEffect::kSmithing).IsValid())
			ret += "Smithing|";
		if ((ae & AlchemicEffect::kSpeech).IsValid())
			ret += "Speech|";
		if ((ae & AlchemicEffect::kCarryWeight).IsValid())
			ret += "CarryWeight|";
		if ((ae & AlchemicEffect::kAlchemy).IsValid())
			ret += "Alchemy|";
		if ((ae & AlchemicEffect::kPersuasion).IsValid())
			ret += "Persuasion|";
		if ((ae & AlchemicEffect::kFortifyHealth).IsValid())
			ret += "FortifyHealth|";
		if ((ae & AlchemicEffect::kFortifyMagicka).IsValid())
			ret += "FortifyMagicka|";
		if ((ae & AlchemicEffect::kFortifyStamina).IsValid())
			ret += "FortifyStamina|";
		if ((ae & AlchemicEffect::kCustom).IsValid())
			ret += "Custom|";
		if ((ae & AlchemicEffect::kShield).IsValid())
			ret += "Shield|";

		if (ret == "|")
			return "|Unknown|";
		return ret;
	}
}


std::string Utility::PrintDistribution(std::vector<std::tuple<int, AlchemicEffect>> distribution)
{
	std::string ret = "|";
	for (int i = 0; i < distribution.size(); i++) {
		ret += ToString(std::get<1>(distribution[i])) + ":" + std::to_string(std::get<0>(distribution[i])) + "|";
	}
	return ret;
}

std::string Utility::PrintEffectMap(std::map<AlchemicEffect, float> effectMap)
{
	std::string ret = "|";
	for (auto& [key, value] : effectMap) {
		ret += ToString(key) + ":" + std::to_string(value) + "|";
	}
	return ret;
}

std::string Utility::PrintEffectMap(std::unordered_map<AlchemicEffect, Distribution::Effect>& effectMap)
{
	std::string ret = "|";
	for (auto& [key, value] : effectMap) {
		ret += ToString(key) + ":" + std::to_string(value.weight) + "|";
	}
	return ret;
}

void Utility::ReadAdjusters(std::string input, std::unordered_map<AlchemicEffect, float>* adjusterMap, bool& error)
{
	if (!adjusterMap)
		return;
	while (input.empty() == false) {
		size_t pos;
		AlchemicEffect effect;
		float adjuster = 0;
		input.erase(0, input.find('<') + 1);
		while (input.empty() == false) {
			input.erase(0, input.find('<') + 1);
			if ((pos = input.find('>')) != std::string::npos) {
				// we have a valid entry, probably
				std::string entry = input.substr(0, pos);
				input.erase(0, pos + 1);
				// parse form or editor id
				if ((pos = entry.find(',')) == std::string::npos) {
					error = true;
				}
				effect = entry.substr(0, pos);
				entry.erase(0, pos + 1);
				try {
					adjuster = static_cast<float>(std::stof(entry.substr(0, pos)));
				} catch (std::exception&) {
					continue;
				}

				adjusterMap->insert_or_assign(effect, adjuster);
			} else {
				// invalid input return what we parsed so far and set error
				error = true;
			}
		}
	}
}

std::string Utility::WriteAdjusters(std::unordered_map<AlchemicEffect, float>* adjusterMap)
{
	if (!adjusterMap)
		return "";
	std::string res = "";
	for (auto& [effect, adjuster] : *adjusterMap) {
		res += "<" + effect.string() + "," + std::to_string(adjuster) + ">";
	}
	return res;
}

std::vector<std::tuple<AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> Utility::ParseCustomObjects(std::string input, bool& error, std::string file, std::string line)
{
	LOG_3("");
	std::vector<std::tuple<AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> ret;
	try {
		auto datahandler = RE::TESDataHandler::GetSingleton();
		size_t pos;
		AssocType type = AssocType::kActor;
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
				LOG_3("Flag: \t\t{}", flags);
				LOG_3("Flag converted: {}", static_cast<uint64_t>(flag));

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
						logwarn("Form {} has an unsupported FormType. file: \"{}\" Rule: \"{}\"", PrintForm(tmp), file, line);
					}
				} else {
					if (form) {
						logwarn("FormID {} couldn't be found. file: \"{}\" Rule: \"\"", GetHex(formid), file, line);
					} else {
						logwarn("EditorID {} couldn't be found. file: \"{}\" Rule: \"\"", editorid, file, line);
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

std::vector<std::tuple<AlchemicEffect, float, int>> Utility::ParseAlchemyEffects(std::string input, bool& error)
{
	LOG_3("");
	std::vector<std::tuple<AlchemicEffect, float, int>> ret;
	try {
		float modifier = 1.0f;
		int max = 0;
		size_t pos = 0;
		AlchemicEffect effect = 0;
		while (input.empty() == false) {
			effect = 0;
			modifier = 1.0f;
			max = 0;
			input.erase(0, input.find('<') + 1);
			if ((pos = input.find('>')) != std::string::npos) {
				// we have a valid entry, probably
				std::string entry = input.substr(0, pos);
				input.erase(0, pos + 1);
				// find weight if present
				if ((pos = entry.find(',')) != std::string::npos) {
					auto tmp = entry;
					entry = entry.substr(0, pos);
					tmp = tmp.erase(0, pos + 1);
					// if weight is present find max value
					if ((pos = tmp.find(',')) != std::string::npos) {
						auto tmp2 = tmp;
						tmp = tmp.substr(0, pos);
						tmp2 = tmp2.erase(0, pos + 1);
						try {
							max = std::stoi(tmp2);
						} catch (std::exception&) {
						}
					}
					try {
						modifier = std::stof(tmp);
					} catch (std::exception&) {
					}
				}
				try {
					// read the effectmapping in hex
					effect = entry;
				} catch (std::exception&) {
				}
				if (!(effect == 0 || modifier == 0.0f)) {
					ret.push_back({ effect, modifier, max });
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
	LOG_4("found effects: {}", ret.size());
	return ret;
}

bool Utility::ParseAlchemyEffects(std::string input, Distribution::EffectPreset* preset)
{
	LOG_3("");
	if (preset == nullptr)
		return false;
	try {
		float modifier = 1.0f;
		int max = 0;
		size_t pos = 0;
		int32_t op = 0;
		AlchemicEffect effect= 0;
		while (input.empty() == false)
		{
			effect = 0;
			modifier = 1.0f;
			max = 0;
			op = 0;
			input.erase(0, input.find('<') + 1);
			if ((pos = input.find('>')) != std::string::npos) {
				std::string entry = input.substr(0, pos);
				input.erase(0, pos + 1);
				// find weight
				if ((pos = entry.find(',')) != std::string::npos) {
					auto tmp = entry;
					entry = entry.substr(0, pos);
					tmp = tmp.erase(0, pos + 1);
					// find max value
					if ((pos = tmp.find(',')) != std::string::npos)
					{
						auto tmp2 = tmp;
						tmp = tmp.substr(0, pos);
						tmp2 = tmp2.erase(0, pos + 1);
						// find operation
						if ((pos = tmp2.find(',')) != std::string::npos) {
							auto tmp3 = tmp2;
							tmp2 = tmp2.substr(0, pos);
							tmp3 = tmp3.erase(0, pos + 1);
							try
							{
								op = std::stoi(tmp3);
							}
							catch (std::exception&)
							{

							}
						}
						try
						{
							max = std::stoi(tmp2);
						}
						catch (std::exception&) {

						}
					}
					try {
						modifier = std::stof(tmp);
					}
					catch (std::exception&)
					{

					}
				}
				try {
					effect = entry;
				}
				catch (std::exception&) {

				}
				if (!(effect == 0)) {
					std::vector<AlchemicEffect> effects;
					if (effect.IsEffect())
						effects.push_back(effect);
					else
						effects = AlchEff::GetAlchemyEffects(effect);

					if (modifier == 0.0f) {
						for (auto eff : effects)
							preset->effects.erase(eff);
					} else {
						for (auto eff : effects) {
							auto itr = preset->effects.find(eff);
							if (itr != preset->effects.end()) {
								Distribution::Effect e;
								switch (op) {
								case 1:  // add
									e = itr->second;
									e.max += max;
									e.weight += modifier;
									preset->effects.insert_or_assign(eff, e);
									break;
								case 2:  // sub
									e = itr->second;
									e.max -= max;
									if (e.max < 0)
										e.max = 0;
									e.weight -= modifier;
									if (e.weight <= 0)
										preset->effects.erase(eff);
									else
										preset->effects.insert_or_assign(eff, e);
									break;
								case 3:   // override
									e = itr->second;
									e.max = max;
									e.weight = modifier;
									preset->effects.insert_or_assign(eff, e);
									break;
								default:  // override
									break;
								}
							}
							else
							{
								// not found -> just set
								Distribution::Effect e;
								e.effect = eff;
								e.weight = modifier;
								e.max = max;
								e.current = 0;
								preset->effects.insert_or_assign(eff, e);
							}
						}
					}

				} else {
					LOG_5("warning: found empty effect, effectstring: {}", entry);
				}

			} else {
				// invalid input return what we parsed so far and set error
				return false;
			}
		}
	}
	catch (std::exception&) {
		return false;
	}
	LOG_3("found effects: {}", preset->effects.size());
	return true;
}

std::vector<std::tuple<int, AlchemicEffect>> Utility::GetDistribution(std::vector<std::tuple<AlchemicEffect, float, int>> effectmap, int range, bool chance)
{
	std::vector<std::tuple<int, AlchemicEffect>> ret;
	if (effectmap.size() == 0)
		return ret;
	AlchemicEffect tmp = 0;
	std::map<AlchemicEffect, float> map;
	// iterate over all effects in effect map
	for (int i = 0; i < effectmap.size(); i++) {
		// iterate over all effects that could be mashed up in the effect map we can only iterate until c 62 so as to avoid
		// an overflow error
		for (AlchemicEffect c = 1; c < AlchemicEffect(4611686018427387904, 1); c = c << 1) {
			if ((tmp = (std::get<0>(effectmap[i]) & c)) > 0) {
				map.insert_or_assign(tmp, std::get<1>(effectmap[i]));
			}
		}
	}
	if (chance) {
		map.insert_or_assign(AlchemicEffect::kCustom, 1.0f);
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

std::vector<std::tuple<int, AlchemicEffect>> Utility::GetDistribution(std::map<AlchemicEffect, float> map, int range, bool chance)
{
	if (chance) {
		map.insert_or_assign(AlchemicEffect::kCustom, 1.0f);
	}
	std::vector<std::tuple<int, AlchemicEffect>> ret;
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

std::map<AlchemicEffect, float> Utility::UnifyEffectMap(std::vector<std::tuple<AlchemicEffect, float, int>> effectmap)
{
	AlchemicEffect tmp = 0;
	std::map<AlchemicEffect, float> map;
	// iterate over all effects in effect map
	for (int i = 0; i < effectmap.size(); i++) {
		// iterate over all effects that could be mashed up in the effect map we can only iterate until c 62 so as to avoid
		// an overflow error
		for (AlchemicEffect c = 1; c < AlchemicEffect(4611686018427387904, 1); c = c << 1) {
			if ((tmp = (std::get<0>(effectmap[i]) & c)) > 0) {
				map.insert_or_assign(tmp, std::get<1>(effectmap[i]));
			}
		}
	}
	return map;
}

AlchemicEffect Utility::SumAlchemyEffects(std::vector<std::tuple<int, AlchemicEffect>> list, bool chance)
{
	AlchemicEffect ret = 0;
	for (int i = 0; i < list.size(); i++) {
		ret |= std::get<1>(list[i]);
	}
	if (chance)
		ret |= AlchemicEffect::kCustom;
	return ret;
}

bool Utility::CanApplyPoison(std::shared_ptr<ActorInfo> const& actor)
{
	LOG_3("");
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
	LOG_4("poison check. Actor:\t{}\tpoison:\t{}\t count:\t{}", Utility::PrintForm(actor), pois && pois->poison ? pois->poison->GetName() : "not found", std::to_string(pois ? pois->count : -1));

	if (pois && pois->count > 0)
		return false;
	return true;
}

bool Utility::GetAppliedPoison(RE::Actor* actor, RE::ExtraPoison* &pois)
{
	LOG_3("");
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
	LOG_4("poison check. Actor:\t{}\tpoison:\t{}\t count:\t{}", Utility::PrintForm(actor), pois && pois->poison ? pois->poison->GetName() : "not found", std::to_string(pois ? pois->count : -1));

	if (pois && pois->count > 0)
		return true;
	return false;
}

bool Utility::VerifyActorInfo(std::shared_ptr<ActorInfo> const& acinfo)
{
	if (acinfo->IsValid() == false || acinfo->GetActor() == nullptr || acinfo->GetActor()->GetFormID() == 0) {
		LOG_1("actor info damaged");
		return false;
	}
	return true;
}
