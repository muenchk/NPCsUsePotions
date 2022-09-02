#pragma once

#include <set>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <tuple>

namespace Game
{
	class SaveLoad
	{
	public:
		using EventCallback = void(SKSE::SerializationInterface* a_intfc);

		enum class CallbackType
		{
			Load = 1,
			Save = 2,
			Revert = 3,
		};
	private:
		std::unordered_map<uint32_t, std::pair<EventCallback*, CallbackType>> callbacks;

		static void LoadCallback(SKSE::SerializationInterface* a_intfc);

		static void SaveCallback(SKSE::SerializationInterface* a_intfc);

		static void RevertCallback(SKSE::SerializationInterface* a_intfc);

	public:
		static SaveLoad* GetSingleton();

		void Register(SKSE::SerializationInterface*, uint32_t uniqueid);

		void RegisterForLoadCallback(uint32_t id, EventCallback* callback);

		void UnregisterForLoadCallback(uint32_t id);

		void RegisterForSaveCallback(uint32_t id, EventCallback* callback);

		void UnregisterForSaveCallback(uint32_t id);

		void RegisterForRevertCallback(uint32_t id, EventCallback* callback);

		void UnregisterForRevertCallback(uint32_t id);
	};
}
