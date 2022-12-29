#pragma once

class Statistics
{
public:
#pragma region Events
	static inline long Events_TESHitEvent = 0;
	static inline long Events_TESCombatEvent = 0;
	static inline long Events_TESLoadGameEvent = 0;
	static inline long Events_TESDeathEvent = 0;
	static inline long Events_BGSActorCellEvent = 0;
	static inline long Events_TESCellAttachDetachEvent = 0;
	static inline long Events_TESEquipEvent = 0;
	static inline long Events_TESFormDeleteEvent = 0;
	static inline long Events_TESContainerChangedEvent = 0;
#pragma endregion

#pragma region System
	static inline long Storage_BytesWrittenLast = 0;
	static inline long Storage_BytesReadLast = 0;
	static inline long Storage_ActorsSavedLast = 0;
	static inline long Storage_ActorsReadLast = 0;
#pragma endregion

#pragma region Misc
	static inline long Misc_PotionsAdministered = 0;
	static inline long Misc_PoisonsUsed = 0;
	static inline long Misc_FoodEaten = 0;
#pragma endregion
};
