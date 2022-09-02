#pragma once
namespace Hooks
{
	class LoadSaveHook
	{
	public:
		static void InstallHook() {
			REL::Relocation<std::uintptr_t> LG{ RE::Offset::BGSSaveLoadManager::Load };
			auto& trampoline = SKSE::GetTrampoline();

			_LoadGame = trampoline.write_call<5>(LG.address(), LoadGame);
		}

	private:
		static bool LoadGame(uint64_t ptr, const char* a_fileName, std::int32_t a_deviceID, std::uint32_t a_outputStats, bool a_checkForMods);
		static inline REL::Relocation<decltype(LoadGame)> _LoadGame;
	};

	void InstallHooks();
}
