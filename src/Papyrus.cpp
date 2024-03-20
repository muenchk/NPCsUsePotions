

#include "Papyrus.h"
#include "Logging.h"
#include "Papyrus/NPCsUsePotions_Poisons.h"
#include "Papyrus/NPCsUsePotions_Potions.h"
#include "Papyrus/NPCsUsePotions_Settings.h"

namespace Papyrus
{
	bool Register(RE::BSScript::Internal::VirtualMachine* a_vm)
	{
		if (!a_vm) {
			logwarn("[Papyrus] [Register] VM not available");
			return false;
		}

		Poisons::Register(a_vm);
		Potions::Register(a_vm);
		SettingsAPI::Register(a_vm);

		return true;
	}
}
