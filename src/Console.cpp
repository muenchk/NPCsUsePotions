#include<Console.h>
#include<Settings.h>
#include<Utility.h>
#include<Events.h>

bool Console::CalcRule::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
{
	auto console = RE::ConsoleLog::GetSingleton();
	//logger::info("console 1");
	if (a_thisObj == nullptr) {
		console->Print("No target selected");
		return false;
	}
	//logger::info("console 2");
	RE::Actor* actor = a_thisObj->As<RE::Actor>();
	if (actor == nullptr) {
		console->Print("No valid target selected");
		return false;
	}
	//logger::info("console 3");
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	std::vector<std::tuple<int, Settings::Distribution::Rule*, std::string>> rls = Settings::Distribution::CalcAllRules(actor, acs, is);
	//logger::info("console 4");

	std::string tmp = "Displaying stats for Actor:\t" + std::string(actor->GetName()) + "\tFormID:\t" + Utility::GetHex(actor->GetFormID());
	//logger::info("console 5");
	console->Print(tmp.c_str());
	tmp = "Race:\t\t\t\t\t" + Utility::GetHex(actor->GetRace()->GetFormID()) + "\t" + std::string(actor->GetRace()->GetFormEditorID());
	console->Print(tmp.c_str());
	tmp = "ActorBase:\t\t\t\t" + Utility::GetHex(actor->GetActorBase()->GetFormID()) + "\t" + std::string(actor->GetActorBase()->GetName());
	console->Print(tmp.c_str());
	tmp = "Race:\t\t\t\t\t" + Utility::GetHex(actor->GetActorBase()->GetRace()->GetFormID()) + "\t" + std::string(actor->GetActorBase()->GetRace()->GetFormEditorID());
	console->Print(tmp.c_str());
	tmp = "Excluded:\t\t\t\t" + std::to_string(Settings::Distribution::ExcludedNPC(actor));
	console->Print(tmp.c_str());
	//logger::info("console 6");
	tmp = "Strength of Actor:\t\t\t" + Utility::ToString(acs);
	//logger::info("console 7");
	console->Print(tmp.c_str());
	//logger::info("console 8");
	tmp = "Strength of given Items:\t\t" + Utility::ToString(is);
	//logger::info("console 9");
	console->Print(tmp.c_str());
	tmp = "Combat data:\t\t\t\t" + Utility::ToStringCombatStyle(Utility::GetCombatData(actor));
	console->Print(tmp.c_str());
	//logger::info("console 10");
	tmp = "Applied Rule:\t\t\t\t" + (std::get<1>(rls[0]) ? std::string(std::get<1>(rls[0])->ruleName) : "");
	//logger::info("console 11");
	console->Print(tmp.c_str());
	//logger::info("console 12");
	tmp = "Number Of Rules considered:\t" + std::to_string((rls.size() - 1));
	//logger::info("console 12-1");
	console->Print(tmp.c_str());
	for (int i = 1; i < rls.size(); i++) {
		//logger::info("console 13");
		if (std::get<1>(rls[i])) {
			//logger::info("console 14.0");
			tmp = "Found Rule:\t\t\t\t" + (std::get<1>(rls[i]) ? std::get<1>(rls[i])->ruleName : "") 
				+ "\tPrio:\t" 
				+ std::to_string(std::get<0>(rls[i])) 
				+ "\tInfo:\t" 
				+ std::get<2>(rls[i]);
		} else {
			//logger::info("console 14.1");
			tmp = "ERROR";
		}
		//logger::info("console 14.2");
		console->Print(tmp.c_str());
		//logger::info("console 14.3");
	}
	//logger::info("console 15");
	console->Print("");
	//logger::info("console 16");


	return true;
}


bool Console::ReloadDist::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR*, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&) 
{
	/*{
		std::ofstream outcons("Data\\SKSE\\Plugins\\NPCsUsePotions\\ConsoleCommands.txt");
		std::ofstream outscript("Data\\SKSE\\Plugins\\NPCsUsePotions\\ScriptCommands.txt");

		RE::SCRIPT_FUNCTION* info = RE::SCRIPT_FUNCTION::GetFirstConsoleCommand();
		outcons << info->functionName << ";"
				<< info->shortName << ";"
				<< "\n";
		int iter = 0;
		for (; static_cast<int>(info->output) < RE::SCRIPT_FUNCTION::Commands::kConsoleCommandsEnd + RE::SCRIPT_FUNCTION::Commands::kConsoleOpBase; iter++) {
			info++;
			outcons << info->functionName << ";"
					<< info->shortName << ";"
					<< "\n";
		}

		info = RE::SCRIPT_FUNCTION::GetFirstScriptCommand();
		outscript << info->functionName << ";"
				  << info->shortName << ";"
				  << "\n";
		iter = 0;
		for (; static_cast<int>(info->output) < RE::SCRIPT_FUNCTION::Commands::kScriptCommandsEnd + RE::SCRIPT_FUNCTION::Commands::kScriptOpBase; iter++) {
			info++;
			outscript << info->functionName << ";"
					  << info->shortName << ";"
					  << "\n";
		}
		outcons.flush();
		outcons.close();
		outscript.flush();
		outscript.close();
	}*/
	auto console = RE::ConsoleLog::GetSingleton();
	console->Print("Resetting information about actors...");
	Events::ResetActorInfoMap();
	console->Print("Reloading Settings...");
	Settings::Load();
	console->Print("Reloading Distribution rules...");
	Settings::LoadDistrConfig();
	if (Settings::_CheckActorsWithoutRules) {
		console->Print("Calculating Actors without rules...");
		Settings::CheckActorsForRules();
	}
	console->Print("Classifying Items...");
	Settings::ClassifyItems();
	console->Print("Finished");
	console->Print("");

	return true;
}


void Console::RegisterConsoleCommands()
{
	CalcRule::Register();
	ReloadDist::Register();
}
