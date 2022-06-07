#include "Hooks.h"
#include "Events.h"

namespace Hooks
{

	bool LoadSaveHook::LoadGame(uint64_t ptr, const char* a_fileName, std::int32_t a_deviceID, std::uint32_t a_outputStats, bool a_checkForMods)
	{
		Events::DisableThreads();
		logger::info("executed hook");
		return _LoadGame(ptr, a_fileName, a_deviceID, a_outputStats, a_checkForMods);
	}

	void InstallHooks()
	{
		LoadSaveHook::InstallHook();
	}
}
