#pragma once




typedef uint64_t AlchemyEffectBase;

	/// <summary>
/// Supported AlchemyEffects
/// </summary>
enum class AlchemyEffect : unsigned __int64
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
	kCustom = (unsigned __int64)1 << 63,            // 4000000000000000
	// 2000007
	kAnyPotion = static_cast<uint64_t>(kHealth) | static_cast<uint64_t>(kMagicka) | static_cast<uint64_t>(kStamina) | static_cast<uint64_t>(kInvisibility),
	// 180D7E3C007
	kAnyPoison = static_cast<uint64_t>(kHealth) | static_cast<uint64_t>(kMagicka) | static_cast<uint64_t>(kStamina) | static_cast<uint64_t>(kMagickaRate) | static_cast<uint64_t>(kStaminaRate) | static_cast<uint64_t>(kHealRate) | static_cast<uint64_t>(kParalysis) | static_cast<uint64_t>(kSpeedMult) | static_cast<uint64_t>(kDamageResist) | static_cast<uint64_t>(kPoisonResist) | static_cast<uint64_t>(kWeaponSpeedMult) | static_cast<uint64_t>(kAttackDamageMult) | static_cast<uint64_t>(kResistFire) | static_cast<uint64_t>(kResistFrost) | static_cast<uint64_t>(kResistMagic) | static_cast<uint64_t>(kResistShock) | static_cast<uint64_t>(kFrenzy) | static_cast<uint64_t>(kFear),
	kAnyRegen = static_cast<uint64_t>(kHealRate) | static_cast<uint64_t>(kMagickaRate) | static_cast<uint64_t>(kStaminaRate) | static_cast<uint64_t>(kHealRateMult) | static_cast<uint64_t>(kMagickaRateMult) | static_cast<uint64_t>(kStaminaRateMult),
	// CBFA3FF8
	kAnyFortify = static_cast<uint64_t>(kOneHanded) | static_cast<uint64_t>(kTwoHanded) | static_cast<uint64_t>(kArchery) | static_cast<uint64_t>(kBlock) | static_cast<uint64_t>(kHeavyArmor) | static_cast<uint64_t>(kLightArmor) | static_cast<uint64_t>(kAlteration) | static_cast<uint64_t>(kConjuration) | static_cast<uint64_t>(kDestruction) | static_cast<uint64_t>(kIllusion) | static_cast<uint64_t>(kRestoration) | static_cast<uint64_t>(kSpeedMult) | static_cast<uint64_t>(kMeleeDamage) | static_cast<uint64_t>(kUnarmedDamage) | static_cast<uint64_t>(kDamageResist) | static_cast<uint64_t>(kPoisonResist) | static_cast<uint64_t>(kResistFire) | static_cast<uint64_t>(kResistFrost) | static_cast<uint64_t>(kResistShock) | static_cast<uint64_t>(kResistDisease) | static_cast<uint64_t>(kWeaponSpeedMult) | static_cast<uint64_t>(kAttackDamageMult) | static_cast<uint64_t>(kPickpocket) | static_cast<uint64_t>(kLockpicking) | static_cast<uint64_t>(kEnchanting) | static_cast<uint64_t>(kWaterbreathing) | static_cast<uint64_t>(kSmithing) | static_cast<uint64_t>(kSpeech) | static_cast<uint64_t>(kCarryWeight) | static_cast<uint64_t>(kPersuasion) | static_cast<uint64_t>(kAlchemy),
	// 1C007
	kAnyFood = static_cast<uint64_t>(kHealth) | static_cast<uint64_t>(kMagicka) | static_cast<uint64_t>(kStamina) | static_cast<uint64_t>(kHealRate) | static_cast<uint64_t>(kMagickaRate) | static_cast<uint64_t>(kStaminaRate),

};

AlchemyEffect ConvertToAlchemyEffect(RE::EffectSetting* effect);

AlchemyEffect ConvertToAlchemyEffectPrimary(RE::EffectSetting* effect);

AlchemyEffect ConvertToAlchemyEffectSecondary(RE::EffectSetting* effect);

AlchemyEffect ConvertToAlchemyEffectIDs(RE::EffectSetting* effect);

AlchemyEffect ConvertToAlchemyEffect(RE::ActorValue val);

/// <summary>
/// converts an AlchemyEffect into RE::ActorValue
/// </summary>
/// <param name="eff"></param>
/// <returns></returns>
RE::ActorValue ConvertAlchemyEffect(AlchemyEffect eff);
