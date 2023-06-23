#include "Hooks.h"
#include "Events.h"
#include "Logging.h"

namespace Hooks
{

	bool FastTravelConfirmHook::FastTravelConfirm(uint64_t self, uint64_t menu)
	{
		LOG_1("{}[Hooks] [FastTravelConfirm] executed hook");
		return _FastTravelConfirm(self, menu);
	}

	uint64_t FadeThenFastTravelHook::FadeThenFastTravel(uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4)
	{
		LOG_1("{}[Hooks] [FadeThenFastTravel] executed hook");
		return _FadeThenFastTravel(arg_1, arg_2, arg_3, arg_4);
	}

	uint64_t TryFast::FadeThenFastTravel(uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4)
	{
		LOG_1("{}[Hooks] [TryFast] executed hook");
		return _FadeThenFastTravel(arg_1, arg_2, arg_3, arg_4);
	}

	void InstallHooks()
	{
		FastTravelConfirmHook::InstallHook();
		FadeThenFastTravelHook::InstallHook();
		TryFast::InstallHook();
	}
}
