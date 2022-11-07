#include "NUPInterface.h"
#include "CustomItem.h"
#include "AlchemyEffect.h"

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
/// Stores information about an actor.
/// </summary>
class ActorInfo
{
public:

	std::vector<NPCsUsePotions::NUPActorInfoHandle*> handles;

	class CustomItems
	{
	public:
		/// <summary>
		/// contains custom items that may be distributed to the actor on combat enter
		/// </summary>
		std::vector<CustomItem*> items;
		std::unordered_map<uint32_t, int> itemsset;
		/// <summary>
		/// contains custom items that may be distributed to the actor upon their death
		/// </summary>
		std::vector<CustomItem*> death;
		std::unordered_map<uint32_t, int> deathset;
		/// <summary>
		/// contains custom potions that may be distributed to the actor
		/// </summary>
		std::vector<CustomItemAlch*> potions;
		std::unordered_map<uint32_t, int> potionsset;
		/// <summary>
		/// contains custom fortify potions that may be distributed to the actor
		/// </summary>
		std::vector<CustomItemAlch*> fortify;
		std::unordered_map<uint32_t, int> fortifyset;
		/// <summary>
		/// contains custom poisons that may be distributed to the actor
		/// </summary>
		std::vector<CustomItemAlch*> poisons;
		std::unordered_map<uint32_t, int> poisonsset;
		/// <summary>
		/// contains custom food that may be distributed to the actor
		/// </summary>
		std::vector<CustomItemAlch*> food;
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
	/// form id of the actor
	/// </summary>
	RE::FormID formid;
	/// <summary>
	/// pluginname the actor is defined in
	/// </summary>
	std::string pluginname;
	/// <summary>
	/// name of the actor
	/// </summary>
	std::string name;
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
	/// time when the actor may use the next food item -> compare with RE::Calendar::GetSingleton()->GetDaysPassed();
	/// </summary>
	float nextFoodTime = 0.0f;
	/// <summary>
	/// Time the npc was last given items
	/// </summary>
	float lastDistrTime = 0.0f;

	/// <summary>
	/// whether custom items have already been given to the npc
	/// </summary>
	bool _distributedCustomItems = false;

	ActorStrength actorStrength = ActorStrength::Weak;
	ItemStrength itemStrength = ItemStrength::kWeak;

	bool _boss = false;
	bool _automaton = false;

	static inline const uint32_t version = 0x00000001;

	ActorInfo(RE::Actor* _actor, int _durHealth, int _durMagicka, int _durStamina, int _durFortify, int _durRegeneration);
	ActorInfo();

	std::string ToString();
	

	~ActorInfo()
	{
		try {
			delete citems;
		} catch (std::exception&) {}

		for (int i = 0; i < handles.size(); i++) {
			try {
				if (handles[i] != nullptr)
					handles[i]->Invalidate();
			} catch (std::exception&) {}
		}
	}

	std::vector<std::tuple<int, AlchemyEffect>> potionDistr;
	std::vector<std::tuple<int, AlchemyEffect>> poisonDistr;
	std::vector<std::tuple<int, AlchemyEffect>> foodDistr;
	std::vector<std::tuple<int, AlchemyEffect>> fortifyDistf;

public:
	void CalcCustomItems();
	bool IsBoss() { return _boss; }

	std::vector<CustomItemAlch*> FilterCustomConditionsDistr(std::vector<CustomItemAlch*> itms);
	std::vector<CustomItemAlch*> FilterCustomConditionsUsage(std::vector<CustomItemAlch*> itms);
	std::vector<CustomItem*> FilterCustomConditionsDistrItems(std::vector<CustomItem*> itms);
	bool CanUseItem(RE::FormID item);
	bool CanUsePot(RE::FormID item);
	bool CanUsePotion(RE::FormID item);
	bool CanUsePoison(RE::FormID item);
	bool CanUseFortify(RE::FormID item);
	bool CanUseFood(RE::FormID item);
	bool IsCustomAlchItem(RE::AlchemyItem* item);
	bool IsCustomPotion(RE::AlchemyItem* item);
	bool IsCustomPoison(RE::AlchemyItem* item);
	bool IsCustomFood(RE::AlchemyItem* item);
	bool IsCustomItem(RE::TESBoundObject* item);

	bool CalcUsageConditions(CustomItem*);
	bool CalcDistrConditions(CustomItem*);

	bool IsFollower();

	static uint32_t GetVersion();

	int32_t GetDataSize();
	int32_t GetMinDataSize(int32_t version);
	bool WriteData(unsigned char* buffer, int offset);
	bool ReadData(unsigned char* buffer, int offset, int length);
};
