#pragma once

/// <summary>
/// Specifies conditions for distribution and usage for custom items
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
		kActorTypeDwarven = 1 << 4,  // 10	// usage
		kHasMagicEffect = 1 << 5,	 // 20  // distribution & usage
		kHasPerk = 1 << 6,			 // 40  // distribution & usage

		kAllUsage = kIsBoss | kHealthThreshold | kMagickaThreshold | kStaminaThreshold | kActorTypeDwarven,
		kAllDistr = kIsBoss,
	};
};

struct CustomItemConditionsAny
{
	enum CustomItemConditionsAnyEnum : unsigned __int64
	{
		kNone = 0,
		kIsBoss = 1 << 0,            // 1	// distribution & usage  condition
		kHealthThreshold = 1 << 1,   // 2	// usage
		kMagickaThreshold = 1 << 2,  // 4	// usage
		kStaminaThreshold = 1 << 3,  // 8	// usage
		kActorTypeDwarven = 1 << 4,  // 10	// usage
		kHasMagicEffect = 1 << 5,    // 20  // distribution & usage
		kHasPerk = 1 << 6,           // 40  // distribution & usage

		kAllUsage = kIsBoss | kHealthThreshold | kMagickaThreshold | kStaminaThreshold | kActorTypeDwarven,
		kAllDistr = kIsBoss,
	};
};

struct CustomItem
{
	RE::TESBoundObject* object;
	int num;
	int8_t chance;
	std::vector<std::tuple<uint64_t, uint32_t, std::string>> conditionsall;
	std::vector<std::tuple<uint64_t, uint32_t, std::string>> conditionsany;
	bool giveonce = false;
};

struct CustomItemAlch : CustomItem
{
};
