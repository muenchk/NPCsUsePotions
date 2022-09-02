#include <NUPInterface.h>
#include "Settings.h"

namespace NPCsUsePotions
{
	NUPInterface::NUPInterface() noexcept
	{
		apiTID = GetCurrentThreadId();
	}

	NUPInterface::~NUPInterface() noexcept
	{

	}

	void NUPInterface::EnableCureDiseasePotionUsage(bool enable)
	{
		logger::info("[NUPInterface] [EnableCureDiseasePotionUsage]");
		Settings::Internal::_useCureDiseaseEffect = enable;
	}
}
