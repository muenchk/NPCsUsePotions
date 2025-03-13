#pragma once

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/unordered_map.hpp>

typedef uint64_t AlchemyBaseEffect;


/// <summary>
/// Supported AlchemyEffects for first identifier
/// </summary>
enum class AlchemyBaseEffectFirst : AlchemyBaseEffect
{
	kNone = 0,										// 0
};

/// <summary>
/// Supported AlchemyEffects for second identifier
/// </summary>
enum class AlchemyBaseEffectSecond : AlchemyBaseEffect
{
	kNone = 0,                                      // 0
	kHealth = 1 << 0,                               // 1
	kMagicka = 1 << 1,                              // 2
	kStamina = 1 << 2,                              // 4
	kOneHanded = 1 << 3,                            // 8
	kTwoHanded = 1 << 4,                            // 10
	kArchery = 1 << 5,                              // 20
	kBlock = 1 << 6,                                // 40
	kHeavyArmor = 1 << 7,                           // 80
	kLightArmor = 1 << 8,                           // 100
	kAlteration = 1 << 9,                           // 200
	kConjuration = 1 << 10,                         // 400
	kDestruction = 1 << 11,                         // 800
	kIllusion = 1 << 12,                            // 1000
	kRestoration = 1 << 13,                         // 2000
	kHealRate = 1 << 14,                            // 4000
	kMagickaRate = 1 << 15,                         // 8000
	kStaminaRate = 1 << 16,                         // 10000
	kSpeedMult = 1 << 17,                           // 20000
	kCriticalChance = 1 << 18,                      // 40000
	kMeleeDamage = 1 << 19,                         // 80000
	kUnarmedDamage = 1 << 20,                       // 100000
	kDamageResist = 1 << 21,                        // 200000
	kPoisonResist = 1 << 22,                        // 400000
	kResistFire = 1 << 23,                          // 800000
	kResistShock = 1 << 24,                         // 1000000
	kResistFrost = 1 << 25,                         // 2000000
	kResistMagic = 1 << 26,                         // 4000000
	kResistDisease = 1 << 27,                       // 8000000
	kParalysis = 1 << 28,                           // 10000000
	kInvisibility = 1 << 29,                        // 20000000
	kWeaponSpeedMult = 1 << 30,                     // 40000000
	kAttackDamageMult = (unsigned __int64)1 << 31,  // 80000000
	kHealRateMult = (unsigned __int64)1 << 32,      // 100000000
	kMagickaRateMult = (unsigned __int64)1 << 33,   // 200000000
	kStaminaRateMult = (unsigned __int64)1 << 34,   // 400000000
	kBlood = (unsigned __int64)1 << 35,             // 800000000
	kPickpocket = (unsigned __int64)1 << 36,        // 1000000000
	kLockpicking = (unsigned __int64)1 << 37,       // 2000000000
	kSneak = (unsigned __int64)1 << 38,             // 4000000000
	kFrenzy = (unsigned __int64)1 << 39,            // 8000000000
	kFear = (unsigned __int64)1 << 40,              // 10000000000
	kBowSpeed = (unsigned __int64)1 << 41,          // 20000000000
	kReflectDamage = (unsigned __int64)1 << 42,     // 40000000000
	kCureDisease = (unsigned __int64)1 << 43,       // 80000000000
	kCurePoison = (unsigned __int64)1 << 44,        // 100000000000
	kEnchanting = (unsigned __int64)1 << 45,        // 200000000000
	kWaterbreathing = (unsigned __int64)1 << 46,    // 400000000000
	kSmithing = (unsigned __int64)1 << 47,          // 800000000000
	kSpeech = (unsigned __int64)1 << 48,            // 1000000000000
	kCarryWeight = (unsigned __int64)1 << 49,       // 2000000000000
	kPersuasion = (unsigned __int64)1 << 50,        // 4000000000000
	kAlchemy = (unsigned __int64)1 << 51,           // 8000000000000
	kFortifyHealth = (unsigned __int64)1 << 52,     // 10000000000000
	kFortifyMagicka = (unsigned __int64)1 << 53,    // 20000000000000
	kFortifyStamina = (unsigned __int64)1 << 54,    // 40000000000000
	kShield = (unsigned __int64)1 << 55,            // 80000000000000
	kUnused2 = (unsigned __int64)1 << 56,           // 100000000000000
	kUnused3 = (unsigned __int64)1 << 57,           // 200000000000000
	kUnused4 = (unsigned __int64)1 << 58,           // 300000000000000
	kUnused5 = (unsigned __int64)1 << 59,           // 400000000000000
	kUnused6 = (unsigned __int64)1 << 60,           // 800000000000000
	kUnused7 = (unsigned __int64)1 << 61,           // 1000000000000000
	kUnused8 = (unsigned __int64)1 << 62,           // 2000000000000000
	kCustom = (unsigned __int64)1 << 63,            // 4000000000000000

	// 2000007
	kAnyPotion = static_cast<uint64_t>(kHealth) |
	             static_cast<uint64_t>(kMagicka) |
	             static_cast<uint64_t>(kStamina) |
	             static_cast<uint64_t>(kInvisibility),

	// 720387DFFBFFFF
	kAnyPoison = static_cast<uint64_t>(kHealth) |
	             static_cast<uint64_t>(kMagicka) |
	             static_cast<uint64_t>(kStamina) |
	             static_cast<uint64_t>(kOneHanded) |
	             static_cast<uint64_t>(kTwoHanded) |
	             static_cast<uint64_t>(kArchery) |
	             static_cast<uint64_t>(kBlock) |
	             static_cast<uint64_t>(kHeavyArmor) |
	             static_cast<uint64_t>(kLightArmor) |
	             static_cast<uint64_t>(kAlteration) |
	             static_cast<uint64_t>(kConjuration) | 
	             static_cast<uint64_t>(kDestruction) |
	             static_cast<uint64_t>(kIllusion) |
	             static_cast<uint64_t>(kRestoration) |
	             static_cast<uint64_t>(kHealRate) |
	             static_cast<uint64_t>(kMagickaRate) |
	             static_cast<uint64_t>(kStaminaRate) |
	             static_cast<uint64_t>(kSpeedMult) |
	             static_cast<uint64_t>(kMeleeDamage) |
	             static_cast<uint64_t>(kUnarmedDamage) |
	             static_cast<uint64_t>(kDamageResist) |
	             static_cast<uint64_t>(kPoisonResist) |
	             static_cast<uint64_t>(kResistFire) |
	             static_cast<uint64_t>(kResistShock) |
	             static_cast<uint64_t>(kResistFrost) |
	             static_cast<uint64_t>(kResistMagic) |
	             static_cast<uint64_t>(kResistDisease) |
	             static_cast<uint64_t>(kParalysis) |
	             static_cast<uint64_t>(kWeaponSpeedMult) |
	             static_cast<uint64_t>(kAttackDamageMult) |
	             static_cast<uint64_t>(kHealRateMult) |
	             static_cast<uint64_t>(kMagickaRateMult) |
	             static_cast<uint64_t>(kStaminaRateMult) |
	             static_cast<uint64_t>(kFrenzy) |
	             static_cast<uint64_t>(kFear) |
	             static_cast<uint64_t>(kBowSpeed) |
	             static_cast<uint64_t>(kCarryWeight) |
	             static_cast<uint64_t>(kFortifyHealth) |
	             static_cast<uint64_t>(kFortifyMagicka) |
	             static_cast<uint64_t>(kFortifyStamina),

	// 70001C000
	kAnyRegen = static_cast<uint64_t>(kHealRate) |
	            static_cast<uint64_t>(kMagickaRate) |
	            static_cast<uint64_t>(kStaminaRate) |
	            static_cast<uint64_t>(kHealRateMult) |
	            static_cast<uint64_t>(kMagickaRateMult) |
	            static_cast<uint64_t>(kStaminaRateMult),

	// 7FE670CFFE3FF8
	kAnyFortify = static_cast<uint64_t>(kOneHanded) |
	              static_cast<uint64_t>(kTwoHanded) |
	              static_cast<uint64_t>(kArchery) |
	              static_cast<uint64_t>(kBlock) |
	              static_cast<uint64_t>(kHeavyArmor) |
	              static_cast<uint64_t>(kLightArmor) |
	              static_cast<uint64_t>(kAlteration) |
	              static_cast<uint64_t>(kConjuration) |
	              static_cast<uint64_t>(kDestruction) |
	              static_cast<uint64_t>(kIllusion) |
	              static_cast<uint64_t>(kRestoration) |
	              static_cast<uint64_t>(kSpeedMult) |
	              static_cast<uint64_t>(kCriticalChance) |
	              static_cast<uint64_t>(kMeleeDamage) |
	              static_cast<uint64_t>(kUnarmedDamage) |
	              static_cast<uint64_t>(kDamageResist) |
	              static_cast<uint64_t>(kPoisonResist) |
	              static_cast<uint64_t>(kResistFire) |
	              static_cast<uint64_t>(kResistShock) |
	              static_cast<uint64_t>(kResistFrost) |
	              static_cast<uint64_t>(kResistMagic) |
	              static_cast<uint64_t>(kResistDisease) |
	              static_cast<uint64_t>(kWeaponSpeedMult) |
	              static_cast<uint64_t>(kAttackDamageMult) |
	              static_cast<uint64_t>(kPickpocket) |
	              static_cast<uint64_t>(kLockpicking) |
	              static_cast<uint64_t>(kSneak) |
	              static_cast<uint64_t>(kBowSpeed) |
	              static_cast<uint64_t>(kReflectDamage) |
	              static_cast<uint64_t>(kEnchanting) |
	              static_cast<uint64_t>(kWaterbreathing) |
	              static_cast<uint64_t>(kSmithing) |
	              static_cast<uint64_t>(kSpeech) |
	              static_cast<uint64_t>(kCarryWeight) |
	              static_cast<uint64_t>(kPersuasion) |
	              static_cast<uint64_t>(kAlchemy) |
	              static_cast<uint64_t>(kFortifyHealth) |
	              static_cast<uint64_t>(kFortifyMagicka) |
	              static_cast<uint64_t>(kFortifyStamina) |
	              static_cast<uint64_t>(kShield),

	// 7FE677EFFFFFFF
	kAnyFood = static_cast<uint64_t>(kAnyPotion) |
	           static_cast<uint64_t>(kAnyRegen) |
	           static_cast<uint64_t>(kAnyFortify),

					 
	kAllPotions = static_cast<uint64_t>(kHealth) |
	              static_cast<uint64_t>(kMagicka) |
	              static_cast<uint64_t>(kStamina) |
	              static_cast<uint64_t>(kInvisibility) |
				  static_cast<uint64_t>(kOneHanded) |
	              static_cast<uint64_t>(kTwoHanded) |
	              static_cast<uint64_t>(kArchery) |
	              static_cast<uint64_t>(kBlock) |
	              static_cast<uint64_t>(kHeavyArmor) |
	              static_cast<uint64_t>(kLightArmor) |
	              static_cast<uint64_t>(kAlteration) |
	              static_cast<uint64_t>(kConjuration) |
	              static_cast<uint64_t>(kDestruction) |
	              static_cast<uint64_t>(kIllusion) |
	              static_cast<uint64_t>(kRestoration) |
	              static_cast<uint64_t>(kSpeedMult) |
	              static_cast<uint64_t>(kCriticalChance) |
	              static_cast<uint64_t>(kMeleeDamage) |
	              static_cast<uint64_t>(kUnarmedDamage) |
	              static_cast<uint64_t>(kDamageResist) |
	              static_cast<uint64_t>(kPoisonResist) |
	              static_cast<uint64_t>(kResistFire) |
	              static_cast<uint64_t>(kResistShock) |
	              static_cast<uint64_t>(kResistFrost) |
	              static_cast<uint64_t>(kResistMagic) |
	              static_cast<uint64_t>(kResistDisease) |
	              static_cast<uint64_t>(kWeaponSpeedMult) |
	              static_cast<uint64_t>(kAttackDamageMult) |
	              static_cast<uint64_t>(kPickpocket) |
	              static_cast<uint64_t>(kLockpicking) |
	              static_cast<uint64_t>(kSneak) |
	              static_cast<uint64_t>(kBowSpeed) |
	              static_cast<uint64_t>(kReflectDamage) |
	              static_cast<uint64_t>(kEnchanting) |
	              static_cast<uint64_t>(kWaterbreathing) |
	              static_cast<uint64_t>(kSmithing) |
	              static_cast<uint64_t>(kSpeech) |
	              static_cast<uint64_t>(kCarryWeight) |
	              static_cast<uint64_t>(kPersuasion) |
	              static_cast<uint64_t>(kAlchemy) |
	              static_cast<uint64_t>(kFortifyHealth) |
	              static_cast<uint64_t>(kFortifyMagicka) |
	              static_cast<uint64_t>(kFortifyStamina) |
	              static_cast<uint64_t>(kShield),

};

class AlchemicEffect;

template <>
struct std::hash<AlchemicEffect>
{
	std::size_t operator()(AlchemicEffect const& effect) const noexcept;
};

class AlchemicEffect
{
public:
	class InitializationError : public std::exception
	{
	private:
		std::string _newmessage;

	public:
		InitializationError(const char* const _Message, const char* const _OrigMessage)
		{
			exception::exception(_OrigMessage);
			_newmessage = _Message;
		}

		inline std::string what()
		{
			return _newmessage + "\t|\tOriginal Error Message: " + exception::what();
		}
	};

	static void Init();

private:

	/// <summary>
	/// first identifier
	/// </summary>
	uint64_t first;
	/// <summary>
	/// second identifier
	/// </summary>
	uint64_t second;

	AlchemicEffect ShiftLeft(const uint64_t& shift) const;

	//typedef boost::bimap<boost::bimaps::unordered_set_of<std::string>, boost::bimaps::unordered_set_of<AlchemicEffect>> StringEffectMap;

	/// <summary>
	/// string registry
	/// </summary>
	//StringEffectMap _stringEffectMap;
	std::unordered_map<std::string, AlchemicEffect> _stringEffectMap;

	void InitStringConversion();

public:
	AlchemicEffect() = default;
	virtual ~AlchemicEffect() = default;
	AlchemicEffect(uint64_t ident_1, uint64_t ident_2) :
		first(ident_1),
		second(ident_2)
	{
	}
	AlchemicEffect(const std::string& rhs);
	AlchemicEffect(const uint64_t& rhs);
	AlchemicEffect(const int& rhs);
	AlchemicEffect(const AlchemyBaseEffectSecond& rhs);

	// comparison
	bool operator==(const AlchemicEffect& rhs) const;
	bool operator==(const int& rhs) const;
	bool operator<(const AlchemicEffect& rhs) const;
	bool operator>(const int& rhs) const;
	// logical
	bool operator!() const;
	bool operator&&(const AlchemicEffect& rhs) const;
	bool operator&&(const bool& rhs) const;
	bool operator||(const AlchemicEffect& rhs) const;
	// arithmetic
	AlchemicEffect operator~() const;
	AlchemicEffect operator&(const AlchemicEffect& rhs) const;
	AlchemicEffect operator|(const AlchemicEffect& rhs) const;
	AlchemicEffect operator^(const AlchemicEffect& rhs) const;
	AlchemicEffect operator<<(const int& rhs) const;
	AlchemicEffect operator<<(const uint64_t& rhs) const;
	//friend AlchemicEffect operator|(const AlchemicEffect& lhs, const AlchemicEffect& rhs);
	//friend AlchemicEffect operator&(const AlchemicEffect& lhs, const AlchemicEffect& rhs);
	//friend AlchemicEffect operator^(const AlchemicEffect& lhs, const AlchemicEffect& rhs);
	// assignment
	AlchemicEffect& operator=(const AlchemicEffect& rhs);
	AlchemicEffect& operator&=(const AlchemicEffect& rhs);
	AlchemicEffect& operator|=(const AlchemicEffect& rhs);
	AlchemicEffect& operator^=(const AlchemicEffect& rhs);

	// conversion
	operator std::string();
	/// <summary>
	/// Returns the hexadecimal representation of the object as string
	/// </summary>
	/// <returns></returns>
	std::string string();
	/// <summary>
	/// Returns the singular effect represented by the object as AlchemyBaseEffectSecond
	/// </summary>
	/// <returns></returns>
	AlchemyBaseEffectSecond AlchemyBaseEffectSecond();
	/// <summary>
	/// Returns the singular effect represented by the object as AlchemyBaseEffectFirst
	/// </summary>
	/// <returns></returns>
	AlchemyBaseEffectFirst AlchemyBaseEffectFirst();
	/// <summary>
	/// Returns the basevalue for an effect
	/// </summary>
	/// <returns></returns>
	unsigned long GetBaseValue();

	/// <summary>
	/// Returns an AlchemicEffect from the given base value
	/// </summary>
	/// <param name="basevalue"></param>
	/// <returns></returns>
	static AlchemicEffect GetFromBaseValue(unsigned long basevalue);

	// functions
	
	/// <summary>
	/// Returns whether the object represents a single effect
	/// </summary>
	/// <returns></returns>
	bool IsEffect();
	/// <summary>
	/// Returns whether the object represents multiple effects
	/// </summary>
	/// <returns></returns>
	bool IsEffectMap();
	/// <summary>
	/// Returns whether the AlchemicEffect represented by [basevalue] is contained in this AlchemicEffect
	/// </summary>
	/// <param name="basevalue"></param>
	/// <returns></returns>
	bool HasEffect(unsigned long basevalue);

	/// <summary>
	/// Whether the effect contains attribute restoring effects, e.g. kHealth
	/// </summary>
	/// <returns></returns>
	bool HasRestorativeEffect();

	/// <summary>
	/// Returns whether the object contains any effects
	/// </summary>
	/// <returns></returns>
	bool IsValid();

	friend std::hash<AlchemicEffect>;

public:
	// Constants
	static const AlchemicEffect kNone;
	static const AlchemicEffect kHealth;
	static const AlchemicEffect kMagicka;
	static const AlchemicEffect kStamina;
	static const AlchemicEffect kOneHanded;
	static const AlchemicEffect kTwoHanded;
	static const AlchemicEffect kArchery;
	static const AlchemicEffect kBlock;
	static const AlchemicEffect kHeavyArmor;
	static const AlchemicEffect kLightArmor;
	static const AlchemicEffect kAlteration;
	static const AlchemicEffect kConjuration;
	static const AlchemicEffect kDestruction;
	static const AlchemicEffect kIllusion;
	static const AlchemicEffect kRestoration;
	static const AlchemicEffect kHealRate;
	static const AlchemicEffect kMagickaRate;
	static const AlchemicEffect kStaminaRate;
	static const AlchemicEffect kSpeedMult;
	static const AlchemicEffect kCriticalChance;
	static const AlchemicEffect kMeleeDamage;
	static const AlchemicEffect kUnarmedDamage;
	static const AlchemicEffect kDamageResist;
	static const AlchemicEffect kPoisonResist;
	static const AlchemicEffect kResistFire;
	static const AlchemicEffect kResistShock;
	static const AlchemicEffect kResistFrost;
	static const AlchemicEffect kResistMagic;
	static const AlchemicEffect kResistDisease;
	static const AlchemicEffect kParalysis;
	static const AlchemicEffect kInvisibility;
	static const AlchemicEffect kWeaponSpeedMult;
	static const AlchemicEffect kAttackDamageMult;
	static const AlchemicEffect kHealRateMult;
	static const AlchemicEffect kMagickaRateMult;
	static const AlchemicEffect kStaminaRateMult;
	static const AlchemicEffect kBlood;
	static const AlchemicEffect kPickpocket;
	static const AlchemicEffect kLockpicking;
	static const AlchemicEffect kSneak;
	static const AlchemicEffect kFrenzy;
	static const AlchemicEffect kFear;
	static const AlchemicEffect kBowSpeed;
	static const AlchemicEffect kReflectDamage;
	static const AlchemicEffect kCureDisease;
	static const AlchemicEffect kCurePoison;
	static const AlchemicEffect kEnchanting;
	static const AlchemicEffect kWaterbreathing;
	static const AlchemicEffect kSmithing;
	static const AlchemicEffect kSpeech;
	static const AlchemicEffect kCarryWeight;
	static const AlchemicEffect kPersuasion;
	static const AlchemicEffect kAlchemy;
	static const AlchemicEffect kFortifyHealth;
	static const AlchemicEffect kFortifyMagicka;
	static const AlchemicEffect kFortifyStamina;
	static const AlchemicEffect kShield;
	static const AlchemicEffect kUnused2;
	static const AlchemicEffect kUnused3;
	static const AlchemicEffect kUnused4;
	static const AlchemicEffect kUnused5;
	static const AlchemicEffect kUnused6;
	static const AlchemicEffect kUnused7;
	static const AlchemicEffect kUnused8;
	static const AlchemicEffect kCustom;

	static const AlchemicEffect kAnyPotion;
	static const AlchemicEffect kAnyPoison;
	static const AlchemicEffect kAnyRegen;
	static const AlchemicEffect kAnyFortify;
	static const AlchemicEffect kAnyFood;
	static const AlchemicEffect kAllPotions;
};

//AlchemicEffect operator|(const AlchemicEffect& lhs, const AlchemicEffect& rhs);
//AlchemicEffect operator&(const AlchemicEffect& lhs, const AlchemicEffect& rhs);
//AlchemicEffect operator^(const AlchemicEffect& lhs, const AlchemicEffect& rhs);



/// <summary>
/// Converts the primary effect of [effect] to an AlchemyEffect
/// </summary>
/// <param name="effect"></param>
/// <returns></returns>
AlchemicEffect ConvertToAlchemyEffect(RE::EffectSetting* effect);

/// <summary>
/// Converts the primary effect of [effect] to an AlchemyEffect
/// </summary>
/// <param name="effect"></param>
/// <returns></returns>
AlchemicEffect ConvertToAlchemyEffectPrimary(RE::EffectSetting* effect);

/// <summary>
/// Converts the secondary effect of [effect] to an AlchemyEffect
/// </summary>
/// <param name="effect"></param>
/// <returns></returns>
AlchemicEffect ConvertToAlchemyEffectSecondary(RE::EffectSetting* effect);

/// <summary>
/// Converts [effect] to an AlchemyEffect based on its FormID
/// </summary>
/// <param name="effect"></param>
/// <returns></returns>
AlchemicEffect ConvertToAlchemyEffectIDs(RE::EffectSetting* effect);

/// <summary>
/// Converts an ActorValue to an AlchemyEffect
/// </summary>
/// <param name="val"></param>
/// <returns></returns>
AlchemicEffect ConvertToAlchemyEffect(RE::ActorValue val);

/// <summary>
/// converts an AlchemyEffect into RE::ActorValue
/// </summary>
/// <param name="eff"></param>
/// <returns></returns>
RE::ActorValue ConvertAlchemyEffect(AlchemicEffect eff);

namespace AlchEff
{
	/// <summary>
	/// Returns whether the [effects] are that of a potion
	/// </summary>
	/// <param name="effects"></param>
	/// <returns></returns>
	bool IsPotion(AlchemicEffect effects);
	/// <summary>
	/// Returns whether the [effects] are that of a fortify potion
	/// </summary>
	/// <param name="effects"></param>
	/// <returns></returns>
	bool IsFortify(AlchemicEffect effects);
	/// <summary>
	/// Converts accumulated [effects] into a list of AlchemyEffect
	/// </summary>
	/// <param name="effects">accumulated effects</param>
	/// <returns>list of AlchemyEffect</returns>
	std::vector<AlchemicEffect> GetAlchemyEffects(AlchemicEffect effects);
}

