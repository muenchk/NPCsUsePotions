#pragma once

#include <vector>

namespace Console
{
	class CalcRule
	{
	private:
		static constexpr char LONG_NAME[] = "NPCsUsePotionStats";
		static constexpr char SHORT_NAME[] = "nupstats";

		CalcRule() = default;
		CalcRule(const CalcRule&) = delete;
		CalcRule(CalcRule&&) = delete;
		virtual ~CalcRule() = default;

		CalcRule& operator=(const CalcRule&) = delete;
		CalcRule& operator=(CalcRule&&) = delete;

	public:
		static bool Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&);
		static void Register()
		{
			auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand("DumpNiUpdates");  // unused
			if (info) {
				info->functionName = LONG_NAME;
				info->shortName = SHORT_NAME;
				info->helpString = "Displays NPCUsePotions rule and strength stats for the selected npc";
				info->referenceFunction = true;
				RE::SCRIPT_PARAMETER* params = new RE::SCRIPT_PARAMETER;
				params->paramName = "Actor";
				params->paramType = RE::SCRIPT_PARAM_TYPE::kActor;
				info->params = nullptr;
				info->numParams = 1;
				info->executeFunction = &Process;
				info->conditionFunction = nullptr;

				logger::info("Registered console command: {} ({})", LONG_NAME, SHORT_NAME);
			} else {
				logger::critical("Failed to register console command: {} ({})", LONG_NAME, SHORT_NAME);
			}
		}
	};
}
