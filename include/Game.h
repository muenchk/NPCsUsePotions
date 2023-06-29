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

		/// <summary>
		/// Type of callback
		/// </summary>
		enum class CallbackType
		{
			Load = 1,
			Save = 2,
			Revert = 3,
		};
	private:
		/// <summary>
		/// map of registered callback by registration id
		/// </summary>
		std::unordered_map<uint32_t, std::pair<EventCallback*, CallbackType>> callbacks;

		/// <summary>
		/// Processes the Load Game Callback and executes registered callbacks
		/// </summary>
		/// <param name="a_intfc"></param>
		static void LoadCallback(SKSE::SerializationInterface* a_intfc);

		/// <summary>
		/// Processes the Save Game Callback and executes registered callbacks
		/// </summary>
		/// <param name="a_intfc"></param>
		static void SaveCallback(SKSE::SerializationInterface* a_intfc);

		/// <summary>
		/// Processes the Revert Game Callback and executes registered callbacks
		/// </summary>
		/// <param name="a_intfc"></param>
		static void RevertCallback(SKSE::SerializationInterface* a_intfc);

	public:
		/// <summary>
		/// Returns the singleton of the class
		/// </summary>
		/// <returns></returns>
		static SaveLoad* GetSingleton();

		/// <summary>
		/// Registers the class for game callbacks
		/// </summary>
		/// <param name=""></param>
		/// <param name="uniqueid"></param>
		void Register(SKSE::SerializationInterface*, uint32_t uniqueid);

		/// <summary>
		/// Registers a Load Game Callback [callback] with the given [id]
		/// </summary>
		/// <param name="id">unique registration id</param>
		/// <param name="callback">the callback function</param>
		void RegisterForLoadCallback(uint32_t id, EventCallback* callback);

		/// <summary>
		/// Unregisters the Load Game Callback with [id]
		/// </summary>
		/// <param name="id">unique registration id</param>
		void UnregisterForLoadCallback(uint32_t id);

		/// <summary>
		/// Registers a Save Game Callback [callback] with the given [id]
		/// </summary>
		/// <param name="id">unique registration id</param>
		/// <param name="callback">the callback function</param>
		void RegisterForSaveCallback(uint32_t id, EventCallback* callback);

		/// <summary>
		/// Unregisters the Save Game Callback with [id]
		/// </summary>
		/// <param name="id">unique registration id</param>
		void UnregisterForSaveCallback(uint32_t id);

		/// <summary>
		/// Registers a Revert Game Callback [callback] with the given [id]
		/// </summary>
		/// <param name="id">unqiue registration id</param>
		/// <param name="callback">the callback function</param>
		void RegisterForRevertCallback(uint32_t id, EventCallback* callback);

		/// <summary>
		/// Unregisters the Revert Game Callback with [id]
		/// </summary>
		/// <param name="id">unique registration id</param>
		void UnregisterForRevertCallback(uint32_t id);
	};

	bool IsFastTravelling();

	void SetFastTraveling(bool travelling = true);
}
