#include "Game.h"
#include "Hooks.h"
#include "Events.h"
#include "Logging.h"

namespace Hooks
{
	uint64_t OnFrameHook::OnFrame(void* unk)
	{
		Events::Main::OnFrame();
		return _OnFrame(unk);
	}

	bool FastTravelConfirmHook::FastTravelConfirm(uint64_t self, uint64_t menu)
	{
		LOG_1("Begin FastTravel");
		Game::SetFastTraveling(true);
		Events::Main::KillThreads();
		return _FastTravelConfirm(self, menu);
	}

	uint64_t FadeThenFastTravelHook::FadeThenFastTravel(uint64_t arg_1, uint64_t arg_2, uint64_t arg_3, uint64_t arg_4)
	{
		LOG_1("executed hook");
		return _FadeThenFastTravel(arg_1, arg_2, arg_3, arg_4);
	}

	void Papyrus_FastTravelHook::FastTravelBegin()
	{
		LOG_1("Begin Fast Travel");
		Game::SetFastTraveling(true);
		Events::Main::KillThreads();
	}

	void Papyrus_FastTravelHook::FastTravelEnd()
	{
		LOG_1("End Fast Travel");
		Game::SetFastTraveling(false);
	}

	bool PlayerUsePotionHook::PlayerUsePotion(uint64_t self, RE::AlchemyItem* alch, uint64_t extralist)
	{
		LOG_1("PlayerUsePotionHook");
		Events::Main::AdjustPlayerCooldowns(alch);
		return _PlayerUsePotion(self, alch, extralist);
	}

	void InstallHooks()
	{
		if (REL::Module::IsVR()) {
			OnFrameHook::Install();
			PlayerUsePotionHook::InstallHook();
		} else {
			OnFrameHook::Install();
			FastTravelConfirmHook::InstallHook();
			Papyrus_FastTravelHook::InstallHook();
			//FadeThenFastTravelHook::InstallHook();
			PlayerUsePotionHook::InstallHook();
		}
	}
}
