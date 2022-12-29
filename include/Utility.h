#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Settings.h"
#include "Distribution.h"
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
	/// Returns a string representing the given form
	/// </summary>
	/// <param name="form"></param>
	/// <returns></returns>
	template <class T>
	static std::string PrintForm(T* form)
	{
		if (form == nullptr || form->GetFormID() == 0 || Logging::EnableGenericLogging == false)
			return "None";
		std::string plugin = "";
		if ((form->GetFormID() & 0xFF000000) != 0xFE000000) {
			plugin = Settings::pluginnames[(form->GetFormID() >> 24)];
		} else
			plugin = Settings::pluginnames[256 + (((form->GetFormID() & 0x00FFF000)) >> 12)];

		return std::string("[") + typeid(T).name() + "<" + Utility::GetHex(form->GetFormID()) + "><" + form->GetName() + "><" + plugin + ">]";
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

	static std::string ToString(AlchemyEffect ae)
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
		case AlchemyEffect::kCustom:
			return "Custom";
		default:
			return "Unknown";
		}
	}

	static std::string ToString(AlchemyEffectBase ae)
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
		if (ae & Base(AlchemyEffect::kCustom))
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
	static uint32_t GetCombatData(RE::Actor* actor);

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
	static uint32_t GetArmorData(RE::Actor* actor);

	/// <summary>
	/// Returns the type of item, specific weapon type etc
	/// </summary>
	/// <param name="form"></param>
	/// <returns></returns>
	static int GetItemType(RE::TESForm* form);

	#pragma region Parsing

	/// <summary>
	/// Parses a string into a vector of int (array of int)
	/// </summary>
	/// <param name="line">string to parse</param>
	/// <returns>vector of int (array of int)</returns>
	static std::vector<int> ParseIntArray(std::string line);

	/// <summary>
	/// returns a TESForm* from various inputs
	/// </summary>
	/// <param name="datahandler">datahandler to get data from</param>
	/// <param name="formid">id or partial id of item (may be 0, if editorid is set)</param>
	/// <param name="pluginname">name of the plugin the item is included (may be the empty string, if item is in the basegame, or editorid is given)</param>
	/// <param name="editorid">editorid of the item, defaults to empty string</param>
	/// <returns></returns>
	static RE::TESForm* GetTESForm(RE::TESDataHandler* datahandler, RE::FormID formid, std::string pluginname, std::string editorid = "");

	/// <summary>
	/// Parses objects for distribution rules from a string input
	/// </summary>
	/// <param name="input">the string to parse</param>
	/// <param name="error">will be overwritten with [true] if an error occurs</param>
	/// <param name="file">the relative path of the file that contains the string</param>
	/// <param name="line">the line in the file that contains the string</param>
	/// <returns>a vector of parsed and validated objects</returns>
	static std::vector<std::tuple<Distribution::AssocType, RE::FormID>> ParseAssocObjects(std::string input, bool& error, std::string file, std::string line);

	/// <summary>
	/// Parses objects for distribution rules from a string input with an optional chance for items
	/// </summary>
	/// <param name="input">the string to parse</param>
	/// <param name="error">will be overwritten with [true] if an error occurs</param>
	/// <param name="file">the relative path of the file that contains the string</param>
	/// <param name="line">the line in the file that contains the string</param>
	/// <returns>a vector of parsed and validated objects and their chances</returns>
	static std::vector<std::tuple<Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> ParseCustomObjects(std::string input, bool& error, std::string file, std::string line);

	/// <summary>
	/// Returns an AssocType for the given RE::FormType. If the RE::FormType is not supported, [valid] is set to true
	/// </summary>
	/// <param name="type">RE::FormType to convert</param>
	/// <param name="valid">Overridable value, which is set to true if [type] is not supported.</param>
	/// <returns>AssocType associated with [type]</returns>
	static Distribution::AssocType MatchValidFormType(RE::FormType type, bool& valid);

	/// <summary>
	/// Parses AlchemyEffects from an input string.
	/// </summary>
	/// <param name="input">string to parse</param>
	/// <param name="error">Overrisable value, which is set to true if there is an error during parsing.</param>
	/// <returns>A vector of AlchemyEffects and Weights</returns>
	static std::vector<std::tuple<uint64_t, float>> ParseAlchemyEffects(std::string input, bool& error);

	/// <summary>
	/// Computes a distribution from an effectmap.
	/// </summary>
	/// <param name="effectmap">effectmap containing effects and weights which will be translated into the distribution</param>
	/// <param name="range">range the distribution chances are computed for</param>
	/// <returns>Weighted Distribution</returns>
	static std::vector<std::tuple<int, AlchemyEffect>> GetDistribution(std::vector<std::tuple<uint64_t, float>> effectmap, int range, bool chance = false);

	/// <summary>
	/// Computes a distribution from a unified effect map
	/// <param name="map">unified effect map the distribution is calculated from</param>
	/// <param name="range">range the distribution chances are computed for</param>
	/// </summary>
	static std::vector<std::tuple<int, AlchemyEffect>> GetDistribution(std::map<AlchemyEffect, float> map, int range);

	/// <summary>
	/// Calculates a unified effect map, that contains at most one entry per AlchemyEffect present
	/// </summary>
	/// <param name="effectmap">effectmap containing effects and weights that shal be unified</param>
	/// <returns>map with alchemyeffects and their weights</returns>
	static std::map<AlchemyEffect, float> UnifyEffectMap(std::vector<std::tuple<uint64_t, float>> effectmap);

	/// <summary>
	/// Sums the Alchemyeffects in [list]
	/// </summary>
	/// <param name="list">list with AlchemyEffects to sum</param>
	/// <returns>Combined value with all Alchemyeffects</returns>
	static uint64_t SumAlchemyEffects(std::vector<std::tuple<int, AlchemyEffect>> list, bool chance = false);

	/// <summary>
	/// Checks whether poison can be applied to the weapons of an actor
	/// </summary>
	/// <param name="actor">Actor to check</param>
	/// <returns>Whether poison can be applied to the actors weapons</returns>
	static bool CanApplyPoison(RE::Actor* actor);

	/// <summary>
	/// Verifies that acinfo is a valid object
	/// <param name="acinfo">ActorInfo to verify</param>
	/// </summary>
	static bool VerifyActorInfo(ActorInfo* acinfo);

	/// <summary>
	/// Returns the pluginname the form is defined in
	/// </summary>
	static const char* GetPluginName(RE::TESForm* form);

	/// <summary>
	/// Returns the dosage of the given poison that is to be applied
	/// </summary>
	/// <param name="poison"></param>
	/// <returns></returns>
	static int GetPoisonDosage(RE::AlchemyItem* poison);

	/// <summary>
	/// Returns a vector with all forms of the given type in the plugin
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="pluginname"></param>
	/// <returns></returns>
	template <class T>
	static std::vector<T*> GetFormsInPlugin(std::string pluginname)
	{
		auto datahandler = RE::TESDataHandler::GetSingleton();
		const RE::TESFile* file = datahandler->LookupLoadedModByName(pluginname);
		std::vector<T*> ret;
		if (file != nullptr) {
			uint32_t mask = 0;
			uint32_t index = 0;
			if (file->IsLight()) {
				mask = 0xFFFFF000;
				index = file->GetPartialIndex() << 12;
			} else {
				mask = 0xFF000000;
				index = file->GetPartialIndex() << 24;
			}
			auto forms = datahandler->GetFormArray<T>();
			for (int i = 0; i < (int)forms.size(); i++) {
				if ((forms[i]->GetFormID() & mask) == index)
					ret.push_back(forms[i]);
			}
		}
		return ret;
	}

	/// <summary>
	/// Returns whether an actor is valid and safe to work with
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	static bool ValidateActor(RE::Actor* actor);

	/// <summary>
	/// Returns whether a form is valid and safe to work with
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	template<class T>
	static bool ValidateForm(T* form)
	{
		if (form == nullptr || form->GetFormID() == 0 || form->formFlags & RE::TESForm::RecordFlags::kDeleted)
			return false;
		return true;
	}
	#pragma endregion

};
