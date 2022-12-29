#include "DataStorage.h"
#include "Logging.h"
#include "Game.h"
#include "Events.h"
#include "Data.h"
#include "Utility.h"
#include "Statistics.h"

namespace Storage
{
	bool processing;
	bool CanProcess() { return processing; }
#define EvalProcessing \
	if (processing == false) \
		return;
#define EvalProcessingBool       \
	if (processing == false) \
		return false;
	

	static Data* data = nullptr;

	void ReadData(SKSE::SerializationInterface* a_intfc);
	void WriteData(SKSE::SerializationInterface* a_intfc);
	void RevertData();

	/// <summary>
	/// Callback executed on saving
	/// saves all global data
	/// </summary>
	/// <param name=""></param>
	void SaveGameCallback(SKSE::SerializationInterface* a_intfc)
	{
		LOG_1("{}[DataStorage] [SaveGameCallback]");
		WriteData(a_intfc);

		// print statistics to logfile
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESHitEvents registered               {}", Statistics::Events_TESHitEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESCombatEvents registered            {}", Statistics::Events_TESCombatEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESLoadGameEvents registered          {}", Statistics::Events_TESLoadGameEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESDeathEvents registered             {}", Statistics::Events_TESDeathEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] BGSActorCellEvents registered         {}", Statistics::Events_BGSActorCellEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESCellAttachDetachEvents registered  {}", Statistics::Events_TESCellAttachDetachEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESEquipEvents registered             {}", Statistics::Events_TESEquipEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESFormDeleteEvent registereds        {}", Statistics::Events_TESHitEvent);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] TESContainerChangedEvent registereds  {}", Statistics::Events_TESContainerChangedEvent);

		loginfo("[DataStorage] [SaveGameCallback] [Statistics] Bytes Written To Last Savegame        {}", Statistics::Storage_BytesWrittenLast);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] Bytes Read From Last Savegame         {}", Statistics::Storage_BytesReadLast);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] Actors Saved To Last Savegame         {}", Statistics::Storage_ActorsSavedLast);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] Actors Read From Last Savegame        {}", Statistics::Storage_ActorsReadLast);

		loginfo("[DataStorage] [SaveGameCallback] [Statistics] potions administered                  {}", Statistics::Misc_PotionsAdministered);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] poisons used                          {}", Statistics::Misc_PoisonsUsed);
		loginfo("[DataStorage] [SaveGameCallback] [Statistics] food eaten                            {}", Statistics::Misc_FoodEaten);

		LOG_1("{}[DataStorage] [SaveGameCallback] end");
	}

	/// <summary>
	/// Callback executed on loading
	/// loads all global data
	/// </summary>
	/// <param name=""></param>
	void LoadGameCallback(SKSE::SerializationInterface* a_intfc)
	{
		LOG_1("{}[DataStorage] [LoadGameCallback]");
		ReadData(a_intfc);

		processing = true;
		LOG_1("{}[DataStorage] [LoadGameCallback] end");
	}

	/// <summary>
	/// Callback executed on reverting to older savegame
	/// deletes all active data and disables processing until load event
	/// </summary>
	/// <param name=""></param>
	void RevertGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
	{
		LOG_1("{}[DataStorage] [RevertGameCallback]");
		processing = false;
		RevertData();
		LOG_1("{}[DataStorage] [RevertGameCallback] end");
	}

	void Register()
	{
		Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000010, LoadGameCallback);
		loginfo("[DataStorage] [Register] Registered for LoadGameCallback");
		Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000020, RevertGameCallback);
		loginfo("[DataStorage] [Register] Registered for RevertGameCallback");
		Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000030, SaveGameCallback);
		loginfo("[DataStorage] [Register] Registered for SaveGameCallback");
		data = Data::GetSingleton();
		if (data == nullptr)
			logcritical("[DataStorage] [Register] Cannot access data storage");
	}

	void ReadData(SKSE::SerializationInterface* a_intfc)
	{
		bool preproc = Events::LockProcessing();

		// total number of bytes read
		long size = 0;

		loginfo("[DataStorage] [ReadData] Beginning data load...");
		
		size += data->ReadActorInfoMap(a_intfc);

		loginfo("[DataStorage] [ReadData] Finished loading data");
		if (preproc) {  // if processing was enabled before locking
			Events::UnlockProcessing();
			loginfo("[DataStorage] [ReadData] Enable processing");
		}
		Statistics::Storage_BytesReadLast = size;
	}

	void WriteData(SKSE::SerializationInterface* a_intfc)
	{
		bool preproc = Events::LockProcessing();

		// total number of bytes written
		long size = 0;

		loginfo("[DataStorage] [WriteData] Beginning to write data...");
		
		size +=data->SaveActorInfoMap(a_intfc);

		loginfo("[DataStorage] [WriteData] Finished writing data");

		if (preproc) {  // if processing was enabled before locking
			Events::UnlockProcessing();
			loginfo("[DataStorage] [WriteData] Enable processing");
		}
		Statistics::Storage_BytesWrittenLast = size;
	}

	void RevertData()
	{
		bool preproc = Events::LockProcessing();

		loginfo("[DataStorage] [RevertData] Reverting ActorInfo");
		data->DeleteActorInfoMap();

		loginfo("[DataStorage] [RevertData] Finished reverting");

		if (preproc) // if processing was enabled before locking
			Events::UnlockProcessing();
	}
}
