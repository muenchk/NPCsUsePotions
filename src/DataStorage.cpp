#include "DataStorage.h"
#include "Logging.h"
#include "Game.h"
#include "Events.h"
#include "Data.h"
#include "Utility.h"
#include "Statistics.h"

namespace Storage
{
	/// <summary>
	/// whether processing is enabled
	/// </summary>
	bool processing;
	bool CanProcess() { return processing; }
#define EvalProcessing \
	if (processing == false) \
		return;
#define EvalProcessingBool       \
	if (processing == false) \
		return false;
	

	/// <summary>
	/// Pointer to the singleton of the data class
	/// </summary>
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
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESHitEvents registered               {}", Statistics::Events_TESHitEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESCombatEvents registered            {}", Statistics::Events_TESCombatEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESDeathEvents registered             {}", Statistics::Events_TESDeathEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] BGSActorCellEvents registered         {}", Statistics::Events_BGSActorCellEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESCellAttachDetachEvents registered  {}", Statistics::Events_TESCellAttachDetachEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESEquipEvents registered             {}", Statistics::Events_TESEquipEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESFormDeleteEvent registereds        {}", Statistics::Events_TESHitEvent);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] TESContainerChangedEvent registereds  {}", Statistics::Events_TESContainerChangedEvent);

		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] Bytes Written To Last Savegame        {}", Statistics::Storage_BytesWrittenLast);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] Bytes Read From Last Savegame         {}", Statistics::Storage_BytesReadLast);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] Actors Saved To Last Savegame         {}", Statistics::Storage_ActorsSavedLast);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] Actors Read From Last Savegame        {}", Statistics::Storage_ActorsReadLast);

		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] potions administered                  {}", Statistics::Misc_PotionsAdministered);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] poisons used                          {}", Statistics::Misc_PoisonsUsed);
		LOG1_1("{}[DataStorage] [SaveGameCallback] [Statistics] food eaten                            {}", Statistics::Misc_FoodEaten);

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
		LOG_1("{}[DataStorage] [Register] Registered for LoadGameCallback");
		Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000020, RevertGameCallback);
		LOG_1("{}[DataStorage] [Register] Registered for RevertGameCallback");
		Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000030, SaveGameCallback);
		LOG_1("{}[DataStorage] [Register] Registered for SaveGameCallback");
		data = Data::GetSingleton();
		if (data == nullptr)
			logcritical("[DataStorage] [Register] Cannot access data storage");
	}

	/// <summary>
	/// Reads data from savegame
	/// </summary>
	/// <param name="a_intfc"></param>
	void ReadData(SKSE::SerializationInterface* a_intfc)
	{
		bool preproc = Events::Main::LockProcessing();

		// total number of bytes read
		long size = 0;

		LOG_1("{}[DataStorage] [ReadData] Beginning data load...");

		uint32_t type = 0;
		uint32_t version = 0;
		uint32_t length = 0;

		// for actor info map
		int accounter = 0;
		int acfcounter = 0;
		int acdcounter = 0;

		
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			LOG2_1("{}[DataStorage] [ReadData] found record with type {} and length {}", type, length);
			size += length;
			switch (type) {
			case 'ACIF':  // ActorInfo
				size += data->ReadActorInfoMap(a_intfc, length, accounter, acdcounter, acfcounter);
				break;
			case 'DAID':  // Deleted Actor
				size += data->ReadDeletedActors(a_intfc, length);
				break; 
			case 'EDID':  // Dead Actor
				size += Events::Main::ReadDeadActors(a_intfc, length);
				break;
			}
		}

		Statistics::Storage_ActorsReadLast = accounter;
		LOG1_1("{}[Data] [ReadActorInfoMap] Read {} ActorInfos", accounter);
		LOG1_1("{}[Data] [ReadActorInfoMap] Read {} dead, deleted or invalid ActorInfos", acdcounter);
		LOG1_1("{}[Data] [ReadActorInfoMap] Failed to read {} ActorInfos", acfcounter);

		LOG_1("{}[DataStorage] [ReadData] Finished loading data");
		if (preproc) {  // if processing was enabled before locking
			Events::Main::UnlockProcessing();
			LOG_1("{}[DataStorage] [ReadData] Enable processing");
		}
		Statistics::Storage_BytesReadLast = size;
	}

	/// <summary>
	/// Writes data to savegame
	/// </summary>
	/// <param name="a_intfc"></param>
	void WriteData(SKSE::SerializationInterface* a_intfc)
	{
		bool preproc = Events::Main::LockProcessing();

		// total number of bytes written
		long size = 0;

		LOG_1("{}[DataStorage] [WriteData] Beginning to write data...");
		
		data->CleanActorInfos();
		size += data->SaveActorInfoMap(a_intfc);
		size += data->SaveDeletedActors(a_intfc);
		size += Events::Main::SaveDeadActors(a_intfc);

		LOG_1("{}[DataStorage] [WriteData] Finished writing data");

		if (preproc) {  // if processing was enabled before locking
			Events::Main::UnlockProcessing();
			LOG_1("{}[DataStorage] [WriteData] Enable processing");
		}
		Statistics::Storage_BytesWrittenLast = size;
	}

	/// <summary>
	/// Reverts data during game reset
	/// </summary>
	void RevertData()
	{
		bool preproc = Events::Main::LockProcessing();

		LOG_1("{}[DataStorage] [RevertData] Reverting ActorInfo");
		data->DeleteActorInfoMap();

		LOG_1("{}[DataStorage] [RevertData] Finished reverting");

		if (preproc) // if processing was enabled before locking
			Events::Main::UnlockProcessing();
	}
}
