#pragma once

class Statistics
{
public:
#pragma region Events
	/// <summary>
	/// Number of times the TESHitEvent has been fired
	/// </summary>
	static inline long Events_TESHitEvent = 0;
	/// <summary>
	/// Number of times the TESCombatEvent has been fired
	/// </summary>
	static inline long Events_TESCombatEvent = 0;
	/// <summary>
	/// Number of times the TESDeathEvent has been fired
	/// </summary>
	static inline long Events_TESDeathEvent = 0;
	/// <summary>
	/// Number of times the BGSActorCellEvent has been fired
	/// </summary>
	static inline long Events_BGSActorCellEvent = 0;
	/// <summary>
	/// Number of times the TESCellAttachDetachEvent has been fired
	/// </summary>
	static inline long Events_TESCellAttachDetachEvent = 0;
	/// <summary>
	/// Number of times the TESEquipEvent has been fired
	/// </summary>
	static inline long Events_TESEquipEvent = 0;
	/// <summary>
	/// Number of times the TESFormDeleteEvent has been fired
	/// </summary>
	static inline long Events_TESFormDeleteEvent = 0;
	/// <summary>
	/// Number of times the TESContainerChangedEvent has been fired
	/// </summary>
	static inline long Events_TESContainerChangedEvent = 0;
#pragma endregion

#pragma region System
	/// <summary>
	/// Number of bytes written during the last save
	/// </summary>
	static inline long Storage_BytesWrittenLast = 0;
	/// <summary>
	/// Number of bytes read during the last load
	/// </summary>
	static inline long Storage_BytesReadLast = 0;
	/// <summary>
	/// Number of actors save during the last save
	/// </summary>
	static inline long Storage_ActorsSavedLast = 0;
	/// <summary>
	/// Number of actors read during the last load
	/// </summary>
	static inline long Storage_ActorsReadLast = 0;
#pragma endregion

#pragma region Misc
	/// <summary>
	/// Number of potions administered during this game session
	/// </summary>
	static inline long Misc_PotionsAdministered = 0;
	/// <summary>
	/// Number of poisons used during this game session
	/// </summary>
	static inline long Misc_PoisonsUsed = 0;
	/// <summary>
	/// Number of food items eaten during this game session
	/// </summary>
	static inline long Misc_FoodEaten = 0;
#pragma endregion
};
