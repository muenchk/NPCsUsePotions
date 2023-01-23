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
	/// Current time spent in combat
	/// </summary>
	int durCombat = 0;

	/// <summary>
	/// whether custom items have already been given to the npc
	/// </summary>
	bool _distributedCustomItems = false;

	ActorStrength actorStrength = ActorStrength::Weak;
	ItemStrength itemStrength = ItemStrength::kWeak;

	/// <summary>
	/// if the actor is considered boss level
	/// </summary>
	bool _boss = false;
	/// <summary>
	/// if the actor is a automaton
	/// </summary>
	bool _automaton = false;

	/// <summary>
	/// while the actor is busy with one animation, no other animation should be prepared / played
	/// </summary>
	bool Animation_busy = false;

	/// <summary>
	/// cooldown until next potion / poison / food can be used
	/// </summary>
	int globalCooldownTimer = 0;

	/// <summary>
	/// version of class [used for save and load]
	/// </summary>
	static inline const uint32_t version = 0x00000002;

	ActorInfo(RE::Actor* _actor, int _durHealth, int _durMagicka, int _durStamina, int _durFortify, int _durRegeneration);
	ActorInfo();

	/// <summary>
	/// returns a string that represents the actor
	/// </summary>
	/// <returns></returns>
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

	/// <summary>
	/// custom potion distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemyEffect>> potionDistr;
	/// <summary>
	/// custom poison distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemyEffect>> poisonDistr;
	/// <summary>
	/// custom food distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemyEffect>> foodDistr;
	/// <summary>
	/// custom fortify distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemyEffect>> fortifyDistf;

public:
	/// <summary>
	/// calculates the custom items available
	/// </summary>
	void CalcCustomItems();
	/// <summary>
	/// Returns whether the actor is a boss
	/// </summary>
	/// <returns></returns>
	bool IsBoss() { return _boss; }

	/// <summary>
	/// Filters a list of custom items and returns only those that can be distributed
	/// </summary>
	/// <param name="itms">list of custom items</param>
	/// <returns>list of distributable items</returns>
	std::vector<CustomItemAlch*> FilterCustomConditionsDistr(std::vector<CustomItemAlch*> itms);
	/// <summary>
	/// Returns whether at least one item in a list of custom items can be distributed
	/// </summary>
	/// <param name="itms"></param>
	/// <returns></returns>
	bool CheckCustomConditionsDistr(std::vector<CustomItemAlch*> itms);
	/// <summary>
	/// Filters a list of custom items and returns only those that can be used
	/// </summary>
	/// <param name="itms">list of custom items</param>
	/// <returns>list of usable items</returns>
	std::vector<CustomItemAlch*> FilterCustomConditionsUsage(std::vector<CustomItemAlch*> itms);
	/// <summary>
	/// Filters a list of custom items and returns only those that can be distributed
	/// </summary>
	/// <param name="itms">list of custom items</param>
	/// <returns>list of distributable items</returns>
	std::vector<CustomItem*> FilterCustomConditionsDistrItems(std::vector<CustomItem*> itms);
	/// <summary>
	/// Returns whether at least one item in a list of custom items can be distributed
	/// </summary>
	/// <param name="itms"></param>
	/// <returns></returns>
	bool CheckCustomConditionsDistrItems(std::vector<CustomItem*> itms);
	/// <summary>
	/// Returns whether the actor may use the given item
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool CanUseItem(RE::FormID item);
	/// <summary>
	/// Returns whether the actor may use the given potion or fortify potion
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool CanUsePot(RE::FormID item);
	/// <summary>
	/// Returns whether the actor may use the given potion
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool CanUsePotion(RE::FormID item);
	/// <summary>
	/// Returns whether the actor may use the given poison
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool CanUsePoison(RE::FormID item);
	/// <summary>
	/// Returns whether the actor may use the given fortify potion
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool CanUseFortify(RE::FormID item);
	/// <summary>
	/// Returns whether the actor may use the given food
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool CanUseFood(RE::FormID item);
	/// <summary>
	/// Returns whether the given item is a custom alchemy item
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool IsCustomAlchItem(RE::AlchemyItem* item);
	/// <summary>
	/// Returns whether the given item is a custom potion
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool IsCustomPotion(RE::AlchemyItem* item);
	/// <summary>
	/// Returns whether the given item is a custom poison
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool IsCustomPoison(RE::AlchemyItem* item);
	/// <summary>
	/// Returns whether the given item is a custom food
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool IsCustomFood(RE::AlchemyItem* item);
	/// <summary>
	/// Returns whether the given item is a custom item
	/// </summary>
	/// <param name="item"></param>
	/// <returns></returns>
	bool IsCustomItem(RE::TESBoundObject* item);

	/// <summary>
	/// Returns whether the given custom item can be used by the actor
	/// </summary>
	/// <param name="">item to check</param>
	/// <returns>[true] if usage is possible, [false] otherwise</returns>
	bool CalcUsageConditions(CustomItem* item);
	/// <summary>
	/// Returns whether the given custom item may be distributed to the actor
	/// </summary>
	/// <param name="item">item to check</param>
	/// <returns>[true] if distribution is allowed, [false] otherwise</returns>
	bool CalcDistrConditions(CustomItem* item);

	/// <summary>
	/// Returns whether the actor is follower
	/// </summary>
	/// <returns></returns>
	bool IsFollower();

	/// <summary>
	/// Returns the version of the class
	/// </summary>
	/// <returns></returns>
	static uint32_t GetVersion();

	/// <summary>
	/// Returns the save size of the object in bytes
	/// </summary>
	/// <returns></returns>
	int32_t GetDataSize();
	/// <summary>
	/// Returns the minimal save size of the object in bytes
	/// </summary>
	/// <param name="version"></param>
	/// <returns></returns>
	int32_t GetMinDataSize(int32_t version);
	/// <summary>
	/// Writes the object information to the given buffer
	/// </summary>
	/// <param name="buffer">buffer to write to</param>
	/// <param name="offset">offset at which writing will begin</param>
	/// <returns>Whether the data was successfully written</returns>
	bool WriteData(unsigned char* buffer, int offset);
	/// <summary>
	/// Reads the object information from the given data
	/// </summary>
	/// <param name="buffer">buffer to read from</param>
	/// <param name="offset">offset in the buffer where the read operation will begin</param>
	/// <param name="length">maximal length to read</param>
	/// <returns>Whether the read operation was successful</returns>
	bool ReadData(unsigned char* buffer, int offset, int length);
};
