#include "Console.h"
#include "Settings.h"
#include "Utility.h"
#include "Events.h"
#include "Distribution.h"
#include "Data.h"

bool Console::CalcRule::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR* a_thisObj, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&)
{
	LOG_3("[CalcRule] [Process]");
	auto console = RE::ConsoleLog::GetSingleton();
	//loginfo("console 1");
	if (a_thisObj == nullptr) {
		console->Print("No target selected");
		return false;
	}
	//loginfo("console 2");
	RE::Actor* actor = a_thisObj->As<RE::Actor>();
	if (actor == nullptr) {
		console->Print("No valid target selected");
		return false;
	}
	//loginfo("console 3");
	std::shared_ptr<ActorInfo> acinfo = Data::GetSingleton()->FindActor(actor);
	if (actor->IsPlayerRef())
		acinfo = Data::GetSingleton()->FindActor(RE::PlayerCharacter::GetSingleton());
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	std::vector<std::tuple<int, Distribution::Rule*, std::string>> rls = Distribution::CalcAllRules(actor, acs, is);
	//loginfo("console 4");

	std::string tmp = "Displaying stats for Actor:\t\t" + std::string(actor->GetName()) + "\tFormID:\t" + Utility::PrintForm(actor) + "\tOriginalID:\t" + Utility::GetHex(acinfo->GetFormIDOriginal());
	//loginfo("console 5");
	console->Print(tmp.c_str());
	tmp = "Templates:\t\t\t\t\t|";
	for (auto& id : acinfo->GetTemplateIDs())
		tmp += Utility::GetHex(id) + "|";
	console->Print(tmp.c_str());
	tmp = "Race:\t\t\t\t\t\t" + Utility::PrintForm(actor->GetRace()) + "\t" + std::string(actor->GetRace()->GetFormEditorID());
	console->Print(tmp.c_str());
	tmp = "ActorBase:\t\t\t\t\t" + Utility::PrintForm(actor->GetActorBase()) + "\t" + std::string(actor->GetActorBase()->GetName());
	console->Print(tmp.c_str());
	tmp = "Valid:\t\t\t\t\t\t" + std::to_string(acinfo->IsValid());
	console->Print(tmp.c_str());
	tmp = "Excluded:\t\t\t\t\t\t" + std::to_string(Distribution::ExcludedNPC(acinfo));
	console->Print(tmp.c_str());
	if (acinfo->GetCombatState() == CombatState::OutOfCombat) {
		tmp = "CombatState:\t\t\t\t\t" + std::string("Out of Combat");
		console->Print(tmp.c_str());
	} else if (acinfo->GetCombatState() == CombatState::InCombat) {
		tmp = "CombatState:\t\t\t\t\t" + std::string("In Combat");
		console->Print(tmp.c_str());
		tmp = "CombatTarget:\t\t\t\t\t" + Utility::PrintForm(acinfo->GetTarget());
		console->Print(tmp.c_str());
	} else if (acinfo->GetCombatState() == CombatState::Searching) {
		tmp = "CombatState:\t\t\t\t\t" + std::string("Searching");
		console->Print(tmp.c_str());
		tmp = "CombatTarget:\t\t\t\t\t" + Utility::PrintForm(acinfo->GetTarget());
		console->Print(tmp.c_str());
	}
	tmp = "Whitelisted:\t\t\t\t\t" + std::to_string(acinfo->IsWhitelisted());
	console->Print(tmp.c_str());
	tmp = "Whitelist calculated:\t\t\t" + std::to_string(acinfo->IsWhitelistCalculated());
	console->Print(tmp.c_str());
	tmp = "Combat data:\t\t\t\t\t" + Utility::ToStringCombatStyle(Utility::GetCombatData(actor));
	console->Print(tmp.c_str());
	//loginfo("console 10");
	tmp = "Applied Rule:\t\t\t\t\t" + (std::get<1>(rls[0]) ? std::string(std::get<1>(rls[0])->ruleName) : "");
	//loginfo("console 11");
	console->Print(tmp.c_str());
	//loginfo("console 12");
	tmp = "Number Of Rules considered:\t\t" + std::to_string((rls.size() - 1));
	//loginfo("console 12-1");
	console->Print(tmp.c_str());
	for (int i = 1; i < rls.size(); i++) {
		//loginfo("console 13");
		if (std::get<1>(rls[i])) {
			//loginfo("console 14.0");
			tmp = "Found Rule:\t\t\t\t\t" + (std::get<1>(rls[i]) ? std::get<1>(rls[i])->ruleName : "") 
				+ "\tPrio:\t" 
				+ std::to_string(std::get<0>(rls[i])) 
				+ "\tInfo:\t" 
				+ std::get<2>(rls[i]);
		} else {
			//loginfo("console 14.1");
			tmp = "ERROR";
		}
		//loginfo("console 14.2");
		console->Print(tmp.c_str());
		//loginfo("console 14.3");
	}
	//loginfo("console 15");
	console->Print("");
	//loginfo("console 16");

	console->Print("");
	console->Print("Current Actor Info");
	Distribution::CalcRule(acinfo);
	// durHealth
	tmp = "Duration Health:\t\t\t\t" + std::to_string(acinfo->GetDurHealth());
	console->Print(tmp.c_str());
	// durMagicka
	tmp = "Duration Magicka:\t\t\t\t" + std::to_string(acinfo->GetDurMagicka());
	console->Print(tmp.c_str());
	// durStamina
	tmp = "Duration Stamina:\t\t\t\t" + std::to_string(acinfo->GetDurStamina());
	console->Print(tmp.c_str());
	// durFortify
	tmp = "Duration Fortify:\t\t\t\t" + std::to_string(acinfo->GetDurFortify());
	console->Print(tmp.c_str());
	// durRegeneration
	tmp = "Duration Regen:\t\t\t\t" + std::to_string(acinfo->GetDurRegeneration());
	console->Print(tmp.c_str());
	// globalCooldownTimer
	tmp = "Global Cooldown:\t\t\t\t" + std::to_string(acinfo->GetGlobalCooldownTimer());
	console->Print(tmp.c_str());
	// nextFoodTime
	tmp = "Next Food Time:\t\t\t\t" + std::to_string(acinfo->GetNextFoodTime());
	console->Print(tmp.c_str());
	// lastDistrTime
	tmp = "Last Distribution Time:\t\t" + std::to_string(acinfo->GetLastDistrTime());
	console->Print(tmp.c_str());
	// distributedCustomItems
	tmp = "Distributed Custom Items:\t\t" + std::to_string(acinfo->DistributedItems());
	console->Print(tmp.c_str());
	// actorStrength
	tmp = "Actor Strength:\t\t\t\t" + Utility::ToString(acinfo->GetActorStrength());
	console->Print(tmp.c_str());
	// itemStrength
	tmp = "Item Strength:\t\t\t\t" + Utility::ToString(acinfo->GetItemStrength());
	console->Print(tmp.c_str());
	// boss
	tmp = "Boss:\t\t\t\t\t\t" + std::to_string(acinfo->IsBoss());
	console->Print(tmp.c_str());
	// IsFollower
	tmp = "IsFollower:\t\t\t\t\t" + std::to_string(acinfo->IsFollower());
	console->Print(tmp.c_str());
	// AnimationBusy
	tmp = "Animation Busy:\t\t\t\t" + std::to_string(acinfo->IsAnimationBusy());
	console->Print(tmp.c_str());
	tmp = "pluginID:\t\t\t\t\t\t" + Utility::GetHex(acinfo->GetPluginID());
	console->Print(tmp.c_str());

	console->Print("CustomItems");
	console->Print("\titems");
	for (int i = 0; i < acinfo->citems.items.size(); i++) {
		auto cit = acinfo->citems.items[i];
		tmp = "\t\t" + std::string(cit->object->GetName()) + "\tchance: " + std::to_string(cit->chance) + "\t\tdistribution possible:\t" + std::to_string(acinfo->CalcDistrConditions(cit));
		console->Print(tmp.c_str());
	}
	console->Print("\tdeath items");
	for (int i = 0; i < acinfo->citems.death.size(); i++) {
		auto cit = acinfo->citems.death[i];
		tmp = "\t\t" + std::string(cit->object->GetName()) + "\tchance: " + std::to_string(cit->chance) + "\t\tdistribution possible:\t" + std::to_string(acinfo->CalcDistrConditions(cit));
		console->Print(tmp.c_str());
	}
	console->Print("\tpoisons");
	for (int i = 0; i < acinfo->citems.poisons.size(); i++) {
		auto cit = acinfo->citems.poisons[i];
		tmp = "\t\t" + std::string(cit->object->GetName()) + "\tchance: " + std::to_string(cit->chance) + "\t\tdistribution possible:\t" + std::to_string(acinfo->CalcDistrConditions(cit));
		console->Print(tmp.c_str());
	}
	console->Print("\tpotions");
	for (int i = 0; i < acinfo->citems.potions.size(); i++) {
		auto cit = acinfo->citems.potions[i];
		tmp = "\t\t" + std::string(cit->object->GetName()) + "\tchance: " + std::to_string(cit->chance) + "\t\tdistribution possible:\t" + std::to_string(acinfo->CalcDistrConditions(cit));
		console->Print(tmp.c_str());
	}
	console->Print("\tfortify");
	for (int i = 0; i < acinfo->citems.fortify.size(); i++) {
		auto cit = acinfo->citems.fortify[i];
		tmp = "\t\t" + std::string(cit->object->GetName()) + "\tchance: " + std::to_string(cit->chance) + "\t\tdistribution possible:\t" + std::to_string(acinfo->CalcDistrConditions(cit));
		console->Print(tmp.c_str());
	}
	console->Print("\tfood");
	for (int i = 0; i < acinfo->citems.food.size(); i++) {
		auto cit = acinfo->citems.food[i];
		tmp = "\t\t" + std::string(cit->object->GetName()) + "\tchance: " + std::to_string(cit->chance) + "\t\tdistribution possible:\t" + std::to_string(acinfo->CalcDistrConditions(cit));
		console->Print(tmp.c_str());
	}
	console->Print("");

	return true;
}


bool Console::ReloadDist::Process(const RE::SCRIPT_PARAMETER*, RE::SCRIPT_FUNCTION::ScriptData*, RE::TESObjectREFR*, RE::TESObjectREFR* /*a_containingObj*/, RE::Script*, RE::ScriptLocals*, double&, std::uint32_t&) 
{
	LOG_3("[ReloadDist] [Process]");
	auto console = RE::ConsoleLog::GetSingleton();
	console->Print("Resetting information about actors...");
	bool preproc = Events::Main::LockProcessing();
	Data::GetSingleton()->ResetActorInfoMap();
	console->Print("Reloading Settings...");
	Settings::Load();
	console->Print("Reloading Distribution rules...");
	Settings::LoadDistrConfig();
	if (preproc)
		Events::Main::UnlockProcessing();
	if (Settings::Debug::_CheckActorsWithoutRules) {
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
