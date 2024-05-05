#include "Game.h"
#include "Logging.h"


namespace Game
{
	void SaveLoad::Register(SKSE::SerializationInterface* a_intfc, uint32_t uniqueid)
	{
		a_intfc->SetUniqueID(uniqueid);
		a_intfc->SetLoadCallback(LoadCallback);
		a_intfc->SetSaveCallback(SaveCallback);
		a_intfc->SetRevertCallback(RevertCallback);
	}

	void SaveLoad::LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		loginfo("Load game callback");
		auto it = GetSingleton()->callbacks.begin();
		while (it != GetSingleton()->callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Load)
				(std::get<0>(it->second))(a_intfc);
			it++;
		}
		loginfo("end");
	}

	void SaveLoad::SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		loginfo("Saving game");
		auto it = GetSingleton()->callbacks.begin();
		while (it != GetSingleton()->callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Save)
				(std::get<0>(it->second))(a_intfc);
			it++;
		}
		loginfo("end");
	}

	void SaveLoad::RevertCallback(SKSE::SerializationInterface* a_intfc)
	{
		loginfo("Reverting game");
		auto it = GetSingleton()->callbacks.begin();
		while (it != GetSingleton()->callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Revert)
				(std::get<0>(it->second))(a_intfc);
			it++;
		}
		loginfo("end");
	}

	void SaveLoad::RegisterForLoadCallback(uint32_t id, EventCallback* callback)
	{
		loginfo("");
		std::pair<EventCallback*, CallbackType> pa{ callback, CallbackType::Load };
		callbacks.insert_or_assign(id, pa);
	}

	void SaveLoad::UnregisterForLoadCallback(uint32_t id)
	{
		loginfo("");
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Load)
				it = callbacks.erase(it);
			it++;
		}
	}

	void SaveLoad::RegisterForSaveCallback(uint32_t id, EventCallback* callback)
	{
		loginfo("");
		std::pair<EventCallback*, CallbackType> pa{ callback, CallbackType::Save };
		callbacks.insert_or_assign(id, pa);
	}

	void SaveLoad::UnregisterForSaveCallback(uint32_t id)
	{
		loginfo("");
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Save)
				it = callbacks.erase(it);
			it++;
		}
	}

	void SaveLoad::RegisterForRevertCallback(uint32_t id, EventCallback* callback)
	{
		loginfo("");
		std::pair<EventCallback*, CallbackType> pa{ callback, CallbackType::Revert };
		callbacks.insert_or_assign(id, pa);
	}

	void SaveLoad::UnregisterForRevertCallback(uint32_t id)
	{
		loginfo("");
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Revert)
				it = callbacks.erase(it);
			it++;
		}
	}

	SaveLoad* SaveLoad::GetSingleton()
	{
		static SaveLoad singleton;
		return std::addressof(singleton);
	}


	bool _fasttravelling = false;

	bool IsFastTravelling()
	{
		return _fasttravelling;
	}

	void SetFastTraveling(bool travelling)
	{
		_fasttravelling = travelling;
	}
}
