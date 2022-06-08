#include<Console.h>
#include<Settings.h>
#include<Utility.h>

bool Console::CalcRule::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
{
	auto console = RE::ConsoleLog::GetSingleton();
	logger::info("console 1");
	if (a_thisObj == nullptr) {
		console->Print("No target selected");
		return false;
	}
	logger::info("console 2");
	RE::Actor* actor = a_thisObj->As<RE::Actor>();
	if (actor == nullptr) {
		console->Print("No valid target selected");
		return false;
	}
	logger::info("console 3");
	Settings::ActorStrength acs = Settings::ActorStrength::Weak;
	Settings::ItemStrength is = Settings::ItemStrength::kWeak;
	std::vector<Settings::Distribution::Rule*> rls = Settings::Distribution::CalcAllRules(actor, acs, is);
	logger::info("console 4");

	std::string tmp = "Displaying stats for Actor:\t" + std::string(actor->GetName()) + "\tFormID:\t" + Utility::GetHex(actor->GetFormID());
	logger::info("console 5");
	console->Print(tmp.c_str());
	logger::info("console 6");
	tmp = "Strength of Actor:\t\t\t" + Utility::ToString(acs);
	logger::info("console 7");
	console->Print(tmp.c_str());
	logger::info("console 8");
	tmp = "Strength of given Items:\t" + Utility::ToString(is);
	logger::info("console 9");
	console->Print(tmp.c_str());
	logger::info("console 10");
	tmp = "Applied Rule:\t\t\t\t" + (rls[0] ? std::string(rls[0]->ruleName) : "");
	logger::info("console 11");
	console->Print(tmp.c_str());
	logger::info("console 12");
	tmp = "Number Of Rules considered:\t" + std::to_string((rls.size() - 1));
	logger::info("console 12-1");
	console->Print(tmp.c_str());
	for (int i = 1; i < rls.size(); i++) {
		logger::info("console 13");
		if (rls[i]) {
			logger::info("console 14.0");
			tmp = "Found Rule:\t\t\t\t" + (rls[i] ? rls[i]->ruleName : "");
		} else {
			logger::info("console 14.1");
			tmp = "ERROR";
		}
		logger::info("console 14.2");
		console->Print(tmp.c_str());
		logger::info("console 14.3");
	}
	logger::info("console 15");
	console->Print("");
	logger::info("console 16");

	return true;
}
