#include <Game.h>


namespace Game
{
	void SaveLoad::Register(SKSE::SerializationInterface* a_intfc, uint32_t uniqueid)
	{
		a_intfc->SetUniqueID(uniqueid);
		a_intfc->SetLoadCallback(LoadCallback);
		a_intfc->SetSaveCallback(SaveCallback);
	}

	void SaveLoad::LoadCallback(SKSE::SerializationInterface* a_intfc)
	{
		logger::info("[LoadCallback] Load game callback");
		auto it = GetSingleton()->callbacks.begin();
		while (it != GetSingleton()->callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Load)
				(std::get<0>(it->second))(a_intfc);
			it++;
		}
	}

	void SaveLoad::SaveCallback(SKSE::SerializationInterface* a_intfc)
	{
		logger::info("[SaveCallback] Saving game");
		auto it = GetSingleton()->callbacks.begin();
		while (it != GetSingleton()->callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Save)
				(std::get<0>(it->second))(a_intfc);
			it++;
		}
	}

	void SaveLoad::RegisterForLoadCallback(uint32_t id, EventCallback* callback)
	{
		std::pair<EventCallback*, CallbackType> pa{ callback, CallbackType::Load };
		callbacks.insert_or_assign(id, pa);
	}

	void SaveLoad::UnregisterForLoadCallback(uint32_t id)
	{
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Load)
				callbacks.erase(it);
		}
	}

	void SaveLoad::RegisterForSaveCallback(uint32_t id, EventCallback* callback)
	{
		std::pair<EventCallback*, CallbackType> pa{ callback, CallbackType::Save };
		callbacks.insert_or_assign(id, pa);
	}

	void SaveLoad::UnregisterForSaveCallback(uint32_t id)
	{
		auto it = callbacks.find(id);
		if (it != callbacks.end()) {
			if (std::get<1>(it->second) == CallbackType::Save)
				callbacks.erase(it);
		}
	}

	SaveLoad* SaveLoad::GetSingleton()
	{
		static SaveLoad singleton;
		return std::addressof(singleton);
	}
}
