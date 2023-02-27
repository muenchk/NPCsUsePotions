#pragma once

/// <summary>
/// Specifies conditions for distribution and usage for custom items that must all be fulfilled
/// </summary>
struct CustomItemConditionsAll
{
	enum CustomItemConditionsAllEnum : unsigned __int64
	{
		kNone = 0,
		kIsBoss = 1 << 0,            // 1	// distribution & usage condition
		kHealthThreshold = 1 << 1,   // 2	// usage
		kMagickaThreshold = 1 << 2,  // 4	// usage
		kStaminaThreshold = 1 << 3,  // 8	// usage
		kActorTypeDwarven = 1 << 4,  // 10	// distribution & usage
		kHasMagicEffect = 1 << 5,	 // 20  // distribution & usage
		kHasPerk = 1 << 6,			 // 40  // distribution & usage
		kHasKeyword = 1 << 7,		 // 80  // distribution & usage
		kIsGhost = 1 << 8,			 // 100 // distribution
		kActorStrengthEq = 1 << 9,		 // 200 // distribution
		kActorStrengthGreaterEq = 1 << 10, // 400 // distribution
		kActorStrengthLesserEq = 1 << 11,  // 800 // distribution
		kIsInFaction = 1 << 12,             // 1000  // distribution
		kNoCustomObjectUsage = 1 << 13,		// 2000  // usage		// used to force normal usage by the effect algorithm

		kAllUsage = kIsBoss | kHealthThreshold | kMagickaThreshold | kStaminaThreshold | kActorTypeDwarven | kHasKeyword | kNoCustomObjectUsage,
		kAllDistr = kIsBoss | kActorTypeDwarven | kHasMagicEffect | kHasPerk | kHasKeyword | kIsGhost | kActorStrengthEq | kActorStrengthGreaterEq | kActorStrengthLesserEq | kIsInFaction,
	};
};

/// <summary>
/// Specified conditions for distribution and usage, of which at least one must be fulfilled
/// </summary>
struct CustomItemConditionsAny
{
	enum CustomItemConditionsAnyEnum : unsigned __int64
	{
		kNone = 0,
		kIsBoss = 1 << 0,            // 1	// distribution & usage  condition
		kHealthThreshold = 1 << 1,   // 2	// usage
		kMagickaThreshold = 1 << 2,  // 4	// usage
		kStaminaThreshold = 1 << 3,  // 8	// usage
		kActorTypeDwarven = 1 << 4,  // 10	// distribution & usage
		kHasMagicEffect = 1 << 5,    // 20  // distribution & usage
		kHasPerk = 1 << 6,           // 40  // distribution & usage
		kHasKeyword = 1 << 7,		 // 80  // distribution & usage
		kIsGhost = 1 << 8,           // 100 // distribution
		kActorStrengthEq = 1 << 9,     // 200 // distribution
		kActorStrengthGreaterEq = 1 << 10,  // 400 // distribution
		kActorStrengthLesserEq = 1 << 11,   // 800 // distribution
		kIsInFaction = 1 << 12,              // 1000  // distribution

		kAllUsage = kIsBoss | kHealthThreshold | kMagickaThreshold | kStaminaThreshold | kActorTypeDwarven | kHasKeyword,
		kAllDistr = kIsBoss | kActorTypeDwarven | kHasMagicEffect | kHasPerk | kHasKeyword | kIsGhost | kActorStrengthEq | kActorStrengthGreaterEq | kActorStrengthLesserEq | kIsInFaction,
	};
};

/// <summary>
/// Custom item
/// </summary>
struct CustomItem
{
	/// <summary>
	/// Custom item
	/// </summary>
	RE::TESBoundObject* object;
	/// <summary>
	/// Number to be distributed to an actor
	/// </summary>
	int num;
	/// <summary>
	/// Chance for distribution
	/// </summary>
	int8_t chance;
	/// <summary>
	/// Conditions that must all be fulfilled
	/// </summary>
	std::vector<std::tuple<uint64_t, uint32_t, std::string>> conditionsall;
	/// <summary>
	/// Conditions, of which at least one must be fulfilled
	/// </summary>
	std::vector<std::tuple<uint64_t, uint32_t, std::string>> conditionsany;
	/// <summary>
	/// Whether to give the item only once
	/// </summary>
	bool giveonce = false;
};

/// <summary>
/// Custom alchemy item
/// </summary>
struct CustomItemAlch : CustomItem
{
};
