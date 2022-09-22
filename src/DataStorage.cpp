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

		int accounter = 0;
		int acfcounter = 0;

		loginfo("[DataStorage] [ReadData] Beginning data load...");
		while (a_intfc->GetNextRecordInfo(type, version, length)) {
			loginfo("[DataStorage] found record with type {} and length {}", type, length);
			switch (type) {
			case 'ACIF':  // ActorInfo
				unsigned char* buffer = new unsigned char[length];
				a_intfc->ReadRecordData(buffer, length);
				ActorInfo* acinfo = new ActorInfo();
				if (acinfo->ReadData(buffer, 0, length) == false) {
					acfcounter++;
					logwarn("[DataStorage] Couldn't read ActorInfo");
				} else {
					accounter++;
					data->ActorInfoMap()->insert_or_assign(acinfo->actor->GetFormID(), acinfo);
					loginfo("[DataStorage] read ActorInfo. id: {}, name: {}", Utility::GetHex(acinfo->actor->GetFormID()), acinfo->actor->GetName());
				}
				break;
			}
		}

		loginfo("[DataStorage] [ReadData] Read {} ActorInfos", accounter);
		loginfo("[DataStorage] [ReadData] Failed to read {} ActorInfos", acfcounter);

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
		
		loginfo("[DataStorage] [WriteData] Writing ActorInfo");
		auto itr = data->ActorInfoMap()->begin();
		while (itr != data->ActorInfoMap()->end()) {
			if (itr->second && itr->second->actor && itr->second->actor->IsDead() == false) {
				// open skse record
				if (a_intfc->OpenRecord('ACIF', 0)) {
					// get entry length
					int length = itr->second->GetDataSize();
					// create buffer
					unsigned char* buffer = new unsigned char[length];
					// fill buffer
					itr->second->WriteData(buffer, 0);
					// write record
					a_intfc->WriteRecordData(buffer, length);
				} else
					logwarn("[DataStorage] [WriteData] failed to write ActorInfo record for {}", Utility::GetHex(itr->second->actor->GetFormID()));
			}
			itr++;
		}

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
		auto itr = data->ActorInfoMap()->begin();
		while (itr != data->ActorInfoMap()->end()) {
			if (itr->second)
				try {
					delete itr->second;
				} catch (std::exception&) {}
			itr++;
		}
		data->ActorInfoMap()->clear();

		loginfo("[DataStorage] [RevertData] Finished reverting");

		if (preproc) // if processing was enabled before locking
			Events::UnlockProcessing();
	}
}
