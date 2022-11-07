#include "DataStorage.h"
#include "Logging.h"
#include "Game.h"
#include "Events.h"
#include "Data.h"
#include "Utility.h"

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

		uint32_t type = 0;
		uint32_t version = 0;
		uint32_t length = 0;

		loginfo("[DataStorage] [ReadData] Beginning data load...");
		
		data->ReadActorInfoMap(a_intfc);

		loginfo("[DataStorage] [ReadData] Finished loading data");
		if (preproc) {  // if processing was enabled before locking
			Events::UnlockProcessing();
			loginfo("[DataStorage] [ReadData] Enable processing");
		}
	}

	void WriteData(SKSE::SerializationInterface* a_intfc)
	{
		bool preproc = Events::LockProcessing();

		loginfo("[DataStorage] [WriteData] Beginning to write data...");
		
		data->SaveActorInfoMap(a_intfc);

		loginfo("[DataStorage] [WriteData] Finished writing data");

		if (preproc) {  // if processing was enabled before locking
			Events::UnlockProcessing();
			loginfo("[DataStorage] [WriteData] Enable processing");
		}
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
