#include "NUPInterface.h"
#include "CustomItem.h"
#include "AlchemyEffect.h"
#include "ID.h"

#include <mutex>
#include <memory>

#define aclock ((void)0);  //std::lock_guard<std::mutex> guard(mutex);

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

enum class CombatState
{
	OutOfCombat = 0,
	InCombat = 1 << 0,
	Searching = 1 << 1,
};

/// <summary>
/// Stores information about an actor.
/// </summary>
class ActorInfo
{

#pragma region static

private:
	/// <summary>
	/// current position of the player character, for faster access
	/// </summary>
	static inline RE::NiPoint3 playerPosition;
	/// <summary>
	/// PlayerRef
	/// </summary>
	static inline RE::Actor* playerRef;

public:
	/// <summary>
	/// Sets the current position of the player character
	/// </summary>
	/// <param name="position"></param>
	static void SetPlayerPosition(RE::NiPoint3 position) { playerPosition = position; }
	/// <summary>
	/// Inits static class data
	/// </summary>
	static void Init();
#pragma endregion

#pragma region runtime
public:
	std::vector<NPCsUsePotions::NUPActorInfoHandle*> handles;

	struct CustomItems
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
	CustomItems citems;

private:
	/// <summary>
	/// central lock coordinating all function accesses
	/// </summary>
	std::mutex mutex;

	/// <summary>
	/// The actor
	/// </summary>
	RE::ActorHandle actor;
	/// <summary>
	/// form id of the actor
	/// </summary>
	ID formid;
	/// <summary>
	/// pluginname the actor is defined in
	/// </summary>
	std::string pluginname = "";
	/// <summary>
	/// ID of the plugin in the current loadorder [runtime]
	/// </summary>
	uint32_t pluginID = 0;
	/// <summary>
	/// name of the actor
	/// </summary>
	std::string name = "";
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
	/// if the actor is a vampire
	/// </summary>
	bool _vampire = false;

	/// <summary>
	/// while the actor is busy with one animation, no other animation should be prepared / played
	/// </summary>
	bool Animation_busy = false;

	/// <summary>
	/// cooldown until next potion / poison / food can be used
	/// </summary>
	int globalCooldownTimer = 0;

	/// <summary>
	/// Whether the NPC has been whitelisted
	/// </summary>
	bool whitelisted = false;
	/// <summary>
	/// Whether it has been calculated whether the npc is whitelisted
	/// </summary>
	bool whitelistedcalculated = false;

	/// <summary>
	/// Combat state of the actor
	/// </summary>
	CombatState combatstate = CombatState::OutOfCombat;

	/// <summary>
	/// whether the actor has a lefthand slot
	/// </summary>
	bool _haslefthand = false;

	// temporary targeting variables
	
	// own combat data
	uint32_t combatdata = 0;
	// target combat data
	uint32_t tcombatdata = 0;
	// current target
	std::weak_ptr<ActorInfo> target;
	// whether to process the actor
	bool handleactor = true;
	// distance to player
	float playerDistance = 0;
	// hostile to player
	bool playerHostile = false;
	// whether the weapons are drawn
	bool weaponsDrawn = false;

	
	/// <summary>
	/// string that represents this object (for fast usage)
	/// </summary>
	std::string _formstring;


public:
	/// <summary>
	/// version of class [used for save and load]
	/// </summary>
	static inline const uint32_t version = 0x00000004;

	ActorInfo(RE::Actor* _actor);
	ActorInfo(bool blockedReset);
	ActorInfo();

	/// <summary>
	/// returns a string that represents the actor
	/// </summary>
	/// <returns></returns>
	std::string ToString();
	

	~ActorInfo()
	{
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
	std::vector<std::tuple<int, AlchemicEffect>> potionDistr;
	/// <summary>
	/// custom poison distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemicEffect>> poisonDistr;
	/// <summary>
	/// custom food distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemicEffect>> foodDistr;
	/// <summary>
	/// custom fortify distribution to be applied
	/// </summary>
	std::vector<std::tuple<int, AlchemicEffect>> fortifyDistf;

private:
	/// <summary>
	/// if [true] the ActorInfo is valid and can be used, if [false] the ActorInfo is a dummy object
	/// </summary>
	bool valid = false;
	/// <summary>
	/// Whether the actor has been deleted;
	/// </summary>
	bool deleted = false;
	/// <summary>
	/// Blocks resetting this info
	/// </summary>
	bool blockReset = false;

	/// <summary>
	/// Updates certain actor metrics
	/// [Should only be called, directly after updating the actor value]
	/// </summary>
	void UpdateMetrics(RE::Actor* reac);

	/// <summary>
	/// Returns whether the given custom item can be used by the actor
	/// </summary>
	/// <param name="">item to check</param>
	/// <returns>[true] if usage is possible, [false] otherwise</returns>
	bool CalcUsageConditionsIntern(CustomItem* item);
	/// <summary>
	/// Returns whether the given custom item may be distributed to the actor
	/// </summary>
	/// <param name="item">item to check</param>
	/// <returns>[true] if distribution is allowed, [false] otherwise</returns>
	bool CalcDistrConditionsIntern(CustomItem* item);

public:

	std::string GetFormString() { return _formstring; }

	/// <summary>
	/// Returns whether the ActorInfo is valid
	/// </summary>
	bool IsValid();
	/// <summary>
	/// Sets the ActorInfo to valid
	/// </summary>
	void SetValid();
	/// <summary>
	/// Sets the ActorInfo to invalid
	/// </summary>
	void SetInvalid();
	/// <summary>
	/// Sets the ActorInfo to deleted
	/// </summary>
	void SetDeleted();
	/// <summary>
	/// Returns whether the actor has been deleted
	/// </summary>
	bool GetDeleted();

	/// <summary>
	/// Resets the actorinfo to default values
	/// </summary>
	/// <param name="actor"></param>
	void Reset(RE::Actor* _actor);

	/// <summary>
	/// Returns the underlying actor object
	/// </summary>
	/// <returns></returns>
	RE::Actor* GetActor();
	/// <summary>
	/// Returns the formid
	/// </summary>
	/// <returns></returns>
	RE::FormID GetFormID();
	/// <summary>
	/// Returns the formid without checking for validity
	/// </summary>
	/// <returns></returns>
	RE::FormID GetFormIDBlank();
	/// <summary>
	/// Returns the original formid
	/// </summary>
	/// <returns></returns>
	RE::FormID GetFormIDOriginal();
	/// <summary>
	/// Returns all known formids of the actors templates
	/// </summary>
	/// <returns></returns>
	std::vector<RE::FormID> GetTemplateIDs();
	/// <summary>
	/// Returns the name of the plugin the actor is defined in
	/// </summary>
	/// <returns></returns>
	std::string GetPluginname();
	/// <summary>
	/// Returns the ID of the plugin in the current loadorder [runtime]
	/// </summary>
	/// <returns></returns>
	uint32_t GetPluginID();
	/// <summary>
	/// Returns the name of the actor
	/// </summary>
	/// <returns></returns>
	std::string GetName();

	int GetDurHealth() { return durHealth; }
	void SetDurHealth(int value) { durHealth = value; }
	void DecDurHealth(int value) { durHealth -= value; }
	int GetDurMagicka() { return durMagicka; }
	void SetDurMagicka(int value) { durMagicka = value; }
	void DecDurMagicka(int value) { durMagicka -= value; }
	int GetDurStamina() { return durStamina; }
	void SetDurStamina(int value) { durStamina = value; }
	void DecDurStamina(int value) { durStamina -= value; }
	int GetDurFortify() { return durFortify; }
	void SetDurFortify(int value) { durFortify = value; }
	void DecDurFortify(int value) { durFortify -= value; }
	int GetDurRegeneration() { return durRegeneration; }
	void SetDurRegeneration(int value) { durRegeneration = value; }
	void DecDurRegeneration(int value) { durRegeneration -= value; }
	float GetNextFoodTime() { return nextFoodTime; }
	void SetNextFoodTime(float value) { nextFoodTime = value; }
	float GetLastDistrTime() { return lastDistrTime; }
	void SetLastDistrTime(float value) { lastDistrTime = value; }
	int GetDurCombat() { return durCombat; }
	void SetDurCombat(int value) { durCombat = value; }
	void IncDurCombat(int value) { durCombat += value; }

	bool DistributedItems() { return _distributedCustomItems; }
	void DistributedItems(bool distributed) { _distributedCustomItems = distributed; }

	ActorStrength GetActorStrength() { return actorStrength; }
	void SetActorStrength(ActorStrength acs) { actorStrength = acs; }
	ItemStrength GetItemStrength() { return itemStrength; }
	void SetItemStrength(ItemStrength ics) { itemStrength = ics; }

	/// <summary>
	/// Returns whether the actor is currently in an animation
	/// </summary>
	/// <returns></returns>
	bool IsAnimationBusy() { return Animation_busy; }
	/// <summary>
	/// Sets whether the actor is currently in an animation
	/// </summary>
	/// <param name="value"></param>
	void SetAnimationBusy(bool value) { Animation_busy = value; }

	int GetGlobalCooldownTimer() { return globalCooldownTimer; }
	void SetGlobalCooldownTimer(int value) { globalCooldownTimer = value; }
	void DecGlobalCooldownTimer(int value) { globalCooldownTimer -= value; }

	bool IsWhitelisted() { return whitelisted; }
	void SetWhitelisted() { whitelisted = true; }
	bool IsWhitelistCalculated() { return whitelistedcalculated; }
	void SetWhitelistCalculated() { whitelistedcalculated = true; }

	/// <summary>
	/// Whether the NPC is currently in combat
	/// </summary>
	bool IsInCombat();
	/// <summary>
	/// Returns the combat state of the actor
	/// </summary>
	/// <returns></returns>
	CombatState GetCombatState();
	/// <summary>
	/// Sets the combatstate of the npc
	/// </summary>
	/// <returns></returns>
	void SetCombatState(CombatState state);
	/// <summary>
	/// Whether an NPC has drawn their weapons
	/// </summary>
	/// <returns></returns>
	bool IsWeaponDrawn();
	/// <summary>
	/// Returns whether the actor is a boss
	/// </summary>
	/// <returns></returns>
	bool IsBoss() { return _boss; }
	/// <summary>
	/// Sets the actor as boss
	/// </summary>
	void SetBoss(bool boss) { _boss = boss; }
	/// <summary>
	/// Returns whether the actor is a vampire
	/// </summary>
	/// <returns></returns>
	bool IsVampire() { return _vampire; }
	/// <summary>
	/// Returns whether the actor is an automaton
	/// </summary>
	/// <returns></returns>
	bool IsAutomaton() { return _automaton; }

	/// <summary>
	/// Returns the actors current target
	/// </summary>
	/// <returns></returns>
	std::weak_ptr<ActorInfo> GetTarget();
	void ResetTarget();
	void SetTarget(std::weak_ptr<ActorInfo> tar);
	short GetTargetLevel();
	uint32_t GetCombatData();
	void SetCombatData(uint32_t data);
	uint32_t GetCombatDataTarget();
	void SetCombatDataTarget(uint32_t data);
	bool GetHandleActor();
	void SetHandleActor(bool handle);
	float GetPlayerDistance();
	void SetPlayerDistance(float distance);
	bool GetPlayerHostile();
	void SetPlayerHostile(bool hostile);
	bool GetWeaponsDrawn();
	void SetWeaponsDrawn(bool drawn);
	void UpdateWeaponsDrawn();

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

	/// <summary>
	/// Updates the actor and whether the ActorInfo is valid
	/// </summary>
	void Update();

#pragma endregion

#pragma region ActorSpecificFunctions

	/// <summary>
	/// Returns whether the actor is follower
	/// </summary>
	/// <returns></returns>
	bool IsFollower();

	/// <summary>
	/// Returns whether the actor is the player character
	/// </summary>
	/// <returns></returns>
	bool IsPlayer();

	/// <summary>
	/// Returns the inventory of the actor
	/// </summary>
	/// <returns></returns>
	RE::TESObjectREFR::InventoryItemMap GetInventory();

	/// <summary>
	/// Returns the inventory counts of the actor
	/// </summary>
	/// <returns></returns>
	RE::TESObjectREFR::InventoryCountMap GetInventoryCounts();

	/// <summary>
	/// Returns whether the magic effect is applied to the actor
	/// </summary>
	/// <param name="effect"></param>
	/// <returns></returns>
	bool HasMagicEffect(RE::EffectSetting* effect);

	/// <summary>
	/// Makes the actor drink the potion
	/// </summary>
	/// <param name="a_potion"></param>
	/// <param name="a_extralist"></param>
	/// <returns></returns>
	bool DrinkPotion(RE::AlchemyItem* potion, RE::ExtraDataList* extralist);

	/// <summary>
	/// Returns the inventory entry data for the specified hand
	/// </summary>
	/// <param name="leftHand"></param>
	/// <returns></returns>
	RE::InventoryEntryData* GetEquippedEntryData(bool leftHand);

	/// <summary>
	/// Removes an item from the actors inventory
	/// </summary>
	/// <param name="item"></param>
	/// <param name="count"></param>
	void RemoveItem(RE::TESBoundObject* item, int32_t count);

	/// <summary>
	/// Adds an item to the actors inventory
	/// </summary>
	/// <returns></returns>
	void AddItem(RE::TESBoundObject* item, int32_t count);

	/// <summary>
	/// Returns the formflags
	/// </summary>
	/// <returns></returns>
	uint32_t GetFormFlags();

	/// <summary>
	/// Returns whether the actor is dead
	/// </summary>
	/// <returns></returns>
	bool IsDead();

	/// <summary>
	/// Returns the actors base
	/// </summary>
	/// <returns></returns>
	RE::TESNPC* GetActorBase();

	/// <summary>
	/// Returns the FormID of the actorbase
	/// </summary>
	/// <returns></returns>
	RE::FormID GetActorBaseFormID();

	/// <summary>
	/// Returns the EditorID of the actorbase
	/// </summary>
	/// <returns></returns>
	std::string GetActorBaseFormEditorID();

	/// <summary>
	/// Returns the actors combat style
	/// </summary>
	/// <returns></returns>
	RE::TESCombatStyle* GetCombatStyle();

	RE::TESRace* GetRace();

	RE::FormID GetRaceFormID();

	bool IsGhost();

	bool IsSummonable();

	bool Bleeds();

	short GetLevel();

	SKSE::stl::enumeration<RE::Actor::BOOL_BITS, uint32_t> GetBoolBits();

	bool IsFlying();

	bool IsInKillMove();

	bool IsInMidair();

	bool IsInRagdollState();

	bool IsUnconscious();

	bool IsParalyzed();

	bool IsStaggered();

	bool IsBleedingOut();

#pragma endregion
};
