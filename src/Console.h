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
		/// <summary>
		/// Processes the nupstats console command. Prints information about rules and combatdata about an actor.
		/// </summary>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name="a_thisObj"></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <returns></returns>
		static bool Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&);
		/// <summary>
		/// Registers the nupstats console command
		/// </summary>
		static void Register()
		{
			auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand("AcceptLegalDoc");  // unused
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

	class ReloadDist
	{
	private:
		static constexpr char LONG_NAME[] = "NPCsUsePotionsReset";
		static constexpr char SHORT_NAME[] = "nupreset";

		ReloadDist() = default;
		ReloadDist(const ReloadDist&) = delete;
		ReloadDist(ReloadDist&&) = delete;
		virtual ~ReloadDist() = default;

		ReloadDist& operator=(const ReloadDist&) = delete;
		ReloadDist& operator=(ReloadDist&&) = delete;

	public:
		/// <summary>
		/// Processes the nupreset console command. Reloads entire plugin configuration.
		/// </summary>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name="a_thisObj"></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <returns></returns>
		static bool Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&);
		/// <summary>
		/// Registers the nupstats console command
		/// </summary>
		static void Register()
		{
			auto info = RE::SCRIPT_FUNCTION::LocateConsoleCommand("GetLegalDocs");  // unused
			if (info) {
				info->functionName = LONG_NAME;
				info->shortName = SHORT_NAME;
				info->helpString = "Reloads all distribution rules from settings files";
				info->referenceFunction = false;
				info->params = nullptr;
				info->numParams = 0;
				info->executeFunction = &Process;
				info->conditionFunction = nullptr;

				logger::info("Registered console command: {} ({})", LONG_NAME, SHORT_NAME);
			} else {
				logger::critical("Failed to register console command: {} ({})", LONG_NAME, SHORT_NAME);
			}
		}
	};

	/// <summary>
	/// Registers all console commands
	/// </summary>
	void RegisterConsoleCommands();
}
