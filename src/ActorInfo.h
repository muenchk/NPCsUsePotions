#pragma once
/// <summary>
/// Determines the strength of an Item
/// </summary>
enum class ItemStrength
{
	kWeak = 1,
	kStandard = 2,
	kPotent = 3,
	kInsane = 4
};
/// <summary>
/// Determines the strength of an Actor
/// </summary>
enum class ActorStrength
{
	Weak = 0,
	Normal = 1,
	Powerful = 2,
	Insane = 3,
	Boss = 4,
};

/// <summary>
/// Flags that specify what type a custom item is
/// </summary>
enum CustomItemFlag
{
	Potion = 0,
	Poison = 1 << 0,
	Food = 1 << 1,
	Fortify = 1 << 2,
	DeathObject = 1 << 29,
	Object = 1 << 30,
};

/// <summary>
/// Specifies conditions for distribution and usage for custom items
/// </summary>
enum class CustomItemConditions : unsigned __int64
{
	None = 0,
	IsBoss = 1, // distribution condition
};

enum class CustomItemConditions2 : unsigned __int64
{

};

/// <summary>
/// Stores information about an actor.
/// </summary>
class ActorInfo
{
public:

	class CustomItems
	{
	public:
		/// <summary>
		/// contains custom items that may be distributed to the actor on combat enter
		/// </summary>
		std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t>> items;
		std::unordered_map<uint32_t, int> itemsset;
		/// <summary>
		/// contains custom items that may be distributed to the actor upon their death
		/// </summary>
		std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t>> death;
		std::unordered_map<uint32_t, int> deathset;
		/// <summary>
		/// contains custom potions that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> potions;
		std::unordered_map<uint32_t, int> potionsset;
		/// <summary>
		/// contains custom fortify potions that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> fortify;
		std::unordered_map<uint32_t, int> fortifyset;
		/// <summary>
		/// contains custom poisons that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> poisons;
		std::unordered_map<uint32_t, int> poisonsset;
		/// <summary>
		/// contains custom food that may be distributed to the actor
		/// </summary>
		std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> food;
		std::unordered_map<uint32_t, int> foodset;
		/// <summary>
		/// whether the custom items have been calculated once
		/// </summary>
		bool calculated = false;

		void CreateMaps();
		void Reset();
	};
	/// <summary>
	/// custom items that may be distributed to an actor
	/// </summary>
	CustomItems* citems;
	/// <summary>
	/// The actor
	/// </summary>
	RE::Actor* actor;
	/// <summary>
	/// Current remaining cooldown on health potions
	/// </summary>
	int durHealth = 0;
	/// <summary>
	/// Current remaining cooldown on magicka potions
	/// </summary>
	int durMagicka = 0;
	/// <summary>
	/// Current remaining cooldown on stamina potions
	/// </summary>
	int durStamina = 0;
	/// <summary>
	/// Current remaining cooldown on fortify potions
	/// </summary>
	int durFortify = 0;
	/// <summary>
	/// Current remaining cooldown on regeneration potions
	/// </summary>
	int durRegeneration = 0;

	/// <summary>
	/// Time the npc was last given items
	/// </summary>
	float lastDistrTime = 0.0f;

	ActorStrength actorStrength;
	ItemStrength itemStrength;

	bool _boss;

	ActorInfo(RE::Actor* _actor, int _durHealth, int _durMagicka, int _durStamina, int _durFortify, int _durRegeneration);
	

	~ActorInfo()
	{
		delete citems;
	}

public:
	void CalcCustomItems();
	bool IsBoss() { return _boss; }

	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> FilterCustomConditionsDistr(std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> itms);
	std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t>> FilterCustomConditionsDistrItems(std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t>> itms);
	bool CanUseItem(RE::FormID item);
	bool CanUsePot(RE::FormID item);
	bool CanUsePotion(RE::FormID item);
	bool CanUsePoison(RE::FormID item);
	bool CanUseFortify(RE::FormID item);
	bool CanUseFood(RE::FormID item);
	bool IsCustomAlchItem(RE::AlchemyItem* item);
	bool IsCustomItem(RE::TESBoundObject* item);
};
