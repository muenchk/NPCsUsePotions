#include "Utility.h"
#include <fstream>
#include <iostream>
#include <type_traits>
#include <utility>
#include <string_view>
#include <chrono>
#include <set>
#include <time.h>
#include <random>
#include <tuple>
#include <vector>
#include "ActorManipulation.h"
#include "Distribution.h"
#include "Settings.h"

using ItemType = Settings::ItemType;

static std::mt19937 randi((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
/// <summary>
/// trims random numbers to 1 to RR
/// </summary>
static std::uniform_int_distribution<signed> randRR(1, RandomRange);
static std::uniform_int_distribution<signed> rand100(1, 100);

#pragma region Distribution

std::vector<RE::TESBoundObject*> Distribution::GetDistrItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	StartProfiling;
	DistributionRule* rule = CalcRule(acinfo, nullptr);
	std::vector<RE::TESBoundObject*> ret;
	if (rule == Distribution::emptyRule) {
		// if there is no rule, we have to check for cutsom items anyway, so assign a default rule that 
		auto items = ACM::GetCustomAlchItems(acinfo);
		int diff = 0;
		if (Settings::distr._DistributePotions) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.potions);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::distr._ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[1].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
		if (Settings::distr._DistributePoisons) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.poisons);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::distr._ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[2].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
		if (Settings::distr._DistributeFortifyPotions) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.fortify);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::distr._ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[3].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
		if (Settings::distr._DistributeFortifyPotions) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.food);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::distr._ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[4].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
	} else {
		if (Settings::distr._DistributePotions) {
			auto ritems = rule->GetRandomPotions(acinfo);
			acinfo->potionDistr.clear();
			acinfo->potionDistr.shrink_to_fit();
			LOG_4("matching potions");
			auto items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, true);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				ritems.resize(diff);
				LOG_4("potions size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("potions to give:\t{}", ritems.size());
		}
		if (Settings::distr._DistributePoisons) {
			auto ritems = rule->GetRandomPoisons(acinfo);
			acinfo->poisonDistr.clear();
			acinfo->poisonDistr.shrink_to_fit();
			LOG_4("matching poisons");
			auto items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				ritems.resize(diff);
				LOG_4("poisons size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("poisons to give:\t{}", ritems.size());
		}
		if (Settings::distr._DistributeFortifyPotions) {
			auto ritems = rule->GetRandomFortifyPotions(acinfo);
			acinfo->fortifyDistf.clear();
			acinfo->fortifyDistf.shrink_to_fit();
			LOG_4("matching fortify");
			auto items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				ritems.resize(diff);
				LOG_4("fortify size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("fortify potions to give:\t{}", ritems.size());
		}
		if (Settings::distr._DistributeFood) {
			auto ritems = rule->GetRandomFood(acinfo);
			acinfo->foodDistr.clear();
			acinfo->foodDistr.shrink_to_fit();
			LOG_4("matching food");
			auto items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			LOG_4("diff: {}", diff);
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				LOG_4("diff greater 0");
				ritems.resize(diff);
				LOG_4("food size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("food to give:\t{}", ritems.size());
		}
	}
	// custom generic items are distributed whenever one of the options above is used
	if (Settings::distr._DistributeCustomItems) {
		std::unordered_map<uint32_t, int> items = ACM::GetCustomItems(acinfo);
		auto ritems = acinfo->FilterCustomConditionsDistrItems(acinfo->citems.items);
		for (int i = 0; i < ritems.size(); i++) {
			auto item = ritems[i];
			if (item->giveonce == true && acinfo->DistributedItems())  // if item is only given once and we already gave items: skip
				continue;
			if (acinfo->CalcDistrConditions(item) == false)
				continue;
			auto itr = items.find(item->object->GetFormID());
			if (itr == items.end()) {
				// run for number of items to give
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::distr._ProbabilityScaling) {
						ret.push_back(item->object);
					}
				}
			} else {
				for (int x = itr->second; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::distr._ProbabilityScaling) {
						ret.push_back(item->object);
					}
				}
			}
		}
		acinfo->DistributedItems(true);
	}
	if (ret.size() > 0 && ret.back() == nullptr) {
		LOG_4("remove last item");
		ret.pop_back();
	}
	PROF_2(TimeProfiling, "");
	return ret;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	DistributionRule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomPotions(acinfo);
	acinfo->potionDistr.clear();
	acinfo->potionDistr.shrink_to_fit();
	auto items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrPoisons(std::shared_ptr<ActorInfo> const& acinfo)
{
	DistributionRule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomPoisons(acinfo);
	acinfo->poisonDistr.clear();
	acinfo->poisonDistr.shrink_to_fit();
	auto items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrFortifyPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	DistributionRule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomFortifyPotions(acinfo);
	acinfo->fortifyDistf.clear();
	acinfo->fortifyDistf.shrink_to_fit();
	auto items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrFood(std::shared_ptr<ActorInfo> const& acinfo)
{
	DistributionRule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomFood(acinfo);
	acinfo->foodDistr.clear();
	acinfo->foodDistr.shrink_to_fit();
	auto items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}

std::vector<RE::AlchemyItem*> Distribution::GetMatchingInventoryItemsUnique(std::shared_ptr<ActorInfo> const& acinfo)
{
	DistributionRule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::distr._DistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::distr._DistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::distr._DistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::distr._DistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0 && ret.back() == nullptr)
		ret.pop_back();
	return ret;
}

std::vector<RE::AlchemyItem*> Distribution::GetMatchingInventoryItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	//LOG_4("[SettingsDistribution] GetMatchingInventoryItems enter");
	DistributionRule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::distr._DistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, false);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| potions {} | found: {}", Utility::GetHex(rule->validPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::distr._DistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| poisons {} | found: {}", Utility::GetHex(rule->validPoisons), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::distr._DistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| fortify {} | found: {}", Utility::GetHex(rule->validFortifyPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::distr._DistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| food {} | found: {}", Utility::GetHex(rule->validFood), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = acinfo->GetInventoryCounts();
		size_t currsize = ret.size();
		for (int i = 0; i < currsize; i++) {
			if (auto it = map.find(ret[i]); it != map.end()) {
				if (it->second > 1)
					for (int c = 1; c < it->second; c++)
						ret.push_back(ret[i]);
			}
		}
	}
	return ret;
}

std::vector<RE::AlchemyItem*> Distribution::GetAllInventoryItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	//LOG_4("[SettingsDistribution] GetMatchingInventoryItems enter");
	DistributionRule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::distr._DistributePotions || Settings::distr._DistributeFortifyPotions) {
		std::list<RE::AlchemyItem*> items = ACM::GetAllPotions(acinfo);
		LOG_4("Potions: {}", items.size());
		for (auto i : items) {
			ret.push_back(i);
		}
	}
	//if (Settings::distr._DistributePotions) {
	//	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, AlchemicEffect::kAnyPotion, false);
	//	for (auto i : items) {
	//		ret.insert(ret.end(), std::get<2>(i));
	//	}
	//}
	if (Settings::distr._DistributePoisons) {
		std::list<RE::AlchemyItem*> items = ACM::GetAllPoisons(acinfo);
		LOG_4("Poisons: {}", items.size());
		for (auto i : items) {
			ret.push_back(i);
		}
	}
	//if (Settings::distr._DistributeFortifyPotions) {
	//	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, AlchemicEffect::kAnyFortify, true);
	//	for (auto i : items) {
	//		ret.insert(ret.end(), std::get<2>(i));
	//	}
	//}
	if (Settings::distr._DistributeFood) {
		std::list<RE::AlchemyItem*> items = ACM::GetAllFood(acinfo);
		LOG_4("Food: {}", items.size());
		for (auto i : items) {
			ret.push_back(i);
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = acinfo->GetInventoryCounts();
		size_t currsize = ret.size();
		for (int i = 0; i < currsize; i++) {
			if (auto it = map.find(ret[i]); it != map.end()) {
				if (it->second > 1)
					for (int c = 1; c < it->second; c++)
						ret.push_back(ret[i]);
			}
		}
	}
	LOG_4("Total: {}", ret.size());
	return ret;
}

void Distribution::FilterDistributionExcludedItems(std::vector<RE::AlchemyItem*>& items)
{
	auto ritr = items.begin();
	while (ritr != items.end()) {
		if (_excludedDistrItems.contains((*ritr)->GetFormID()))
			ritr = items.erase(ritr);
		else
			ritr++;
	}
}

int Distribution::GetPoisonDosage(RE::AlchemyItem* poison, AlchemicEffect effects, bool forcenonzero)
{
	int dosage = 0;
	auto itr = dosageItemMap()->find(poison->GetFormID());
	if (itr != dosageItemMap()->end()) {
		auto [force, setting, dos] = itr->second;
		if (force && setting && forcenonzero == false)
			dosage = Settings::poisons._Dosage;
		else if (force)
			dosage = dos;
		else if (setting && forcenonzero == false)
			dosage = Settings::poisons._Dosage;
		else if (Settings::poisons._BaseDosage == Settings::poisons._Dosage)
			dosage = dos;
	}
	// we evaluated the item specific dosage
	// if we did not find anything check the effect specific dosages
	if (dosage != 0) {
		// find the minimum explicit value we can apply
		std::vector<AlchemicEffect> effvec = AlchEff::GetAlchemyEffects(effects);
		int min = INT_MAX;
		for (int i = 0; i < effvec.size(); i++) {
			auto itra = dosageEffectMap()->find(effvec[i]);
			if (itra != dosageEffectMap()->end()) {
				auto [force, setting, dos] = itr->second;
				if (force && setting && forcenonzero == false)
					dos = Settings::poisons._Dosage;
				else if (force)
					dos = dos;
				else if (setting && forcenonzero == false)
					dos = Settings::poisons._Dosage;
				else if (Settings::poisons._BaseDosage == Settings::poisons._Dosage)
					dos = dos;
				if (dos < min)
					min = dos;
			}
		}
		if (min < INT_MAX)
			dosage = min;
	}
	// if we did not find anything at all, assign the setting
	if (dosage <= 0 && forcenonzero == false)
		dosage = Settings::poisons._Dosage;
	return dosage;
}

bool Distribution::ExcludedNPC(std::shared_ptr<ActorInfo> const& acinfo)
{
	if (!acinfo->IsValid() || acinfo->GetActor() == nullptr)
		return true;
	if (Settings::whitelist.EnabledNPCs) {
		if (acinfo->IsWhitelistCalculated()) {
			if (!acinfo->IsWhitelisted())
				return true;
		} else {
			CalcRule(acinfo);
			if (!acinfo->IsWhitelisted())
				return true;
		}
	}
	if (Distribution::excludedNPCs()->contains(acinfo->GetFormID()))
		return true;
	if (Distribution::excludedPlugins_NPCs()->contains(acinfo->GetPluginID()))
		return true;
	if (Distribution::excludedAssoc()->contains(acinfo->GetRaceFormID()))
		return true;
	if (acinfo->IsFollower())
		return true;
	if ((Distribution::excludedNPCs()->contains(acinfo->GetFormIDOriginal())))
		return true;
	for (auto& id : acinfo->GetTemplateIDs())
		if (Distribution::excludedNPCs()->contains(id))
			return true;
	if (acinfo->IsGhost())
		return true;
	if (acinfo->IsSummonable())
		return true;
	if (acinfo->Bleeds() == false && Utility::ToLower(acinfo->GetActorBaseFormEditorID()).find("ghost") != std::string::npos) {
		Distribution::ForceExcludeNPC(acinfo->GetFormID());
		return true;
	}
	// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
	if (!Distribution::npcMap()->contains(acinfo->GetFormID()) && !Distribution::npcMap()->contains(acinfo->GetFormIDOriginal())) {
		auto base = acinfo->GetActorBase();
		for (uint32_t i = 0; i < base->numKeywords; i++) {
			if (base->keywords[i])
				if (Distribution::excludedAssoc()->contains(base->keywords[i]->GetFormID()))
					return true;
		}
		for (uint32_t i = 0; i < base->factions.size(); i++) {
			if (base->factions[i].faction)
				if (Distribution::excludedAssoc()->contains(base->factions[i].faction->GetFormID()))
					return true;
		}
		auto race = acinfo->GetRace();
		if (race) {
			if (Distribution::excludedAssoc()->contains(race->GetFormID()))
				return true;
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				if (race->keywords[i] == nullptr)
					continue;
				if (Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID()))
					return true;
			}
		}
	}
	return false;
}

/// <summary>
/// returns whether the npc should no be handled at all
/// </summary>
/// <param name="actor"></param>
/// <returns></returns>
bool Distribution::ExcludedNPCFromHandling(RE::Actor* actor)
{
	if (actor->formFlags & RE::TESForm::RecordFlags::kDeleted)
		return true;
	{
		ActorInfo acinfo(actor);
		if (Distribution::hardExclusions()->contains(acinfo.GetFormID())) {
			LOG_3("Exclude From Handling: Hard Exclusion Actor");
			return true;
		}
		if (Distribution::hardExclusionsPlugins_NPCs()->contains(acinfo.GetPluginID())) {
			LOG_3("Exclude From Handling: Hard Exclusion Plugin");
			return true;
		}
		if (Distribution::hardExclusions()->contains(acinfo.GetRaceFormID())) {
			LOG_3("Exclude From Handling: Hard Exclusion Race");
			return true;
		}
		if ((Distribution::hardExclusions()->contains(acinfo.GetFormIDOriginal()))) {
			LOG_3("Exclude From Handling: Hard Exclusion Orig ID");
			return true;
		}
		for (auto& id : acinfo.GetTemplateIDs()) {
			if (Distribution::hardExclusions()->contains(id)) {
				LOG_3("Exclude From Handling: Hard Exclusion Template");
				return true;
			}
		}
		if (acinfo.IsGhost()) {
			LOG_3("Exclude From Handling: Hard Exclusion Ghost");
			return true;
		}
		if (acinfo.IsSummonable()) {
			LOG_3("Exclude From Handling: Hard Exclusion Summon");
			return true;
		}
		if (acinfo.Bleeds() == false && Utility::ToLower(acinfo.GetActorBaseFormEditorID()).find("ghost") != std::string::npos) {
			LOG_3("Exclude From Handling: Hard Exclusion Ghost String");
			return true;
		}

		// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
		auto base = acinfo.GetActorBase();
		for (uint32_t i = 0; i < base->numKeywords; i++) {
			if (base->keywords[i])
				if (Distribution::hardExclusions()->contains(base->keywords[i]->GetFormID())) {
					LOG_3("Exclude From Handling: Hard Exclusion Keyword NPC");
					return true;
				}
		}
		for (uint32_t i = 0; i < base->factions.size(); i++) {
			if (base->factions[i].faction)
				if (Distribution::hardExclusions()->contains(base->factions[i].faction->GetFormID())) {
					LOG_3("Exclude From Handling: Hard Exclusion Facrion");
					return true;
				}
		}
		auto race = acinfo.GetRace();
		if (race) {
			if (Distribution::hardExclusions()->contains(race->GetFormID()))
				return true;
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				if (race->keywords[i] == nullptr)
					continue;
				if (Distribution::hardExclusions()->contains(race->keywords[i]->GetFormID())) {
					LOG_3("Exclude From Handling: Hard Exclusion Keyword Race");
					return true;
				}
			}
		}
	}
	if (Settings::usage._DisableItemUsageForExcludedNPCs) {
		ID id = ID(actor);
		// only view them as excluded from handling if they are either excluded themselves, or their race is excluded
		if (Distribution::excludedNPCs()->contains(id))
			return true;
		if (Distribution::excludedPlugins_NPCs()->contains(Mods::GetPluginIndex(actor)))
			return true;
		if ((Mods::GetPluginIndex(actor) == MAXUINT32 && Distribution::excludedPlugins_NPCs()->contains(Utility::ExtractTemplateInfo(actor->GetActorBase()).pluginID)))
			return true;
		if (actor->GetActorBase() && Distribution::excludedNPCs()->contains(id.GetOriginalID()))
			return true;
		if (actor->IsGhost())
			return true;
		if (actor->GetActorBase() && actor->GetActorBase()->IsSummonable())
			return true;
		if (!Distribution::npcMap()->contains(id) && !Distribution::npcMap()->contains(id.GetOriginalID())) {
			auto race = actor->GetRace();
			if (race) {
				if (Distribution::excludedPlugins_NPCs()->contains(race->GetFormID()))
					return true;
				if (Distribution::excludedAssoc()->contains(race->GetFormID()))
					return true;
				for (uint32_t i = 0; i < race->numKeywords; i++) {
					if (race->keywords[i] == nullptr)
						continue;
					if (Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID()))
						return true;
				}
			}
		}
	}
	return false;
}

/// <summary>
/// returns wether an npc is excluded from item distribution
/// </summary>
/// <param name="npc">ActorBase to check</param>
/// <returns></returns>
bool Distribution::ExcludedNPC(RE::TESNPC* npc)
{
	// skip fucking deleted references
	if (npc->formFlags & RE::TESForm::RecordFlags::kDeleted)
		return true;
	if ((Distribution::excludedNPCs()->contains(npc->GetFormID())))
		return true;
	if (npc->IsInFaction(Settings::CurrentFollowerFaction))
		return true;
	if (npc->IsInFaction(Settings::CurrentHirelingFaction))
		return true;
	if (npc->IsGhost())
		return true;
	if (npc->IsSummonable())
		return true;
	if (npc->Bleeds() == false && Utility::ToLower(std::string(npc->GetFormEditorID())).find("ghost") != std::string::npos) {
		Distribution::ForceExcludeNPC(npc->GetFormID());
		return true;
	}
	// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
	if (!Distribution::npcMap()->contains(npc->GetFormID())) {
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				if (Distribution::excludedAssoc()->contains(npc->keywords[i]->GetFormID()))
					return true;
		}
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				if (Distribution::excludedAssoc()->contains(npc->factions[i].faction->GetFormID()))
					return true;
		}
		auto race = npc->GetRace();
		if (race) {
			if (Distribution::excludedAssoc()->contains(race->GetFormID()))
				return true;
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				if (race->keywords[i] == nullptr)
					continue;
				if (Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID()))
					return true;
			}
		}
	}

	return false;
}

bool Distribution::ForceExcludeNPC(uint32_t actorid)
{
	_excludedNPCs.insert(actorid);
	return true;
}

DistributionRule* Distribution::CalcRule(RE::TESNPC* npc, ActorStrength& acs, ItemStrength& is, UtilityBase::NPCTPLTInfo* tpltinfo, CustomItemStorage* custItems)
{
	// calc strength section
	if (Settings::distr._GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty;
		if (diff == 0 || diff == 1) {
			acs = ActorStrength::Weak;
			is = ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acs = ActorStrength::Normal;
			is = ItemStrength::kStandard;
		} else if (diff == 4) {
			acs = ActorStrength::Powerful;
			is = ItemStrength::kPotent;
		} else {  // diff == 5
			acs = ActorStrength::Insane;
			is = ItemStrength::kInsane;
		}
	} else {
		// level not available for BaseActors

		/*
		// get level dependencies
		short lvl = actor->GetLevel();
		if (lvl <= _LevelEasy) {
			acs = ActorStrength::Weak;
			is = ItemStrength::kWeak;
			// weak actor
		} else if (lvl <= _LevelNormal) {
			acs = ActorStrength::Normal;
			is = ItemStrength::kStandard;
			// normal actor
		} else if (lvl <= _LevelDifficult) {
			acs = ActorStrength::Powerful;
			is = ItemStrength::kPotent;
			// difficult actor
		} else if (lvl <= _LevelInsane) {
			acs = ActorStrength::Insane;
			is = ItemStrength::kInsane;
			// insane actor
		} else {
			acs = ActorStrength::Boss;
			is = ItemStrength::kInsane;
			// boss actor
		}*/
	}

	// now calculate rule and on top get the boss override

	bool bossoverride = false;
	int acsadj = 0;
	auto adjustacs = [&acsadj](RE::FormID id) {
		try {
			acsadj += actorStrengthMap()->at(id);
		} catch (std::out_of_range&) {
		}
	};

	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	std::vector<CustomItemStorage*>* citems = nullptr;
	std::unordered_set<std::string>* citemsset = nullptr;
	if (custItems) {
		citems = new std::vector<CustomItemStorage*>{};
		citemsset = new std::unordered_set<std::string>{};
	}

	DistributionRule* rule = nullptr;
	// define general stuff
	auto style = npc->combatStyle;
	auto cls = npc->npcClass;
	auto race = npc->GetRace();

	// find rule in npc map
	// npc rules always have the highest priority
	auto itnpc = npcMap()->find(npc->GetFormID());
	if (itnpc != npcMap()->end()) {  // found the right rule!
		rule = itnpc->second;     // this can be null if the specific npc is excluded
		ruleoverride = true;
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(npc->GetFormID());
	adjustacs(npc->GetFormID());
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(npc->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
	}

	if (tpltinfo && tpltinfo->tpltrace)
		race = tpltinfo->tpltrace;
	// now that we didnt't find something so far, check the rest
	// this time all the priorities are the same
	if (!ruleoverride) {
		auto it = assocMap()->find(race->GetFormID());
		if (it != assocMap()->end())
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				prio = std::get<1>(it->second)->rulePriority;
			}
		baseexcluded |= baselineExclusions()->contains(race->GetFormID());
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (race->keywords[i] == nullptr)
				continue;
			auto itr = assocMap()->find(race->keywords[i]->GetFormID());
			if (itr != assocMap()->end())
			{
				if (prio < std::get<0>(itr->second)) {
					rule = std::get<1>(itr->second);
					prio = std::get<0>(itr->second);
				} else if (prio < std::get<1>(itr->second)->rulePriority) {
					rule = std::get<1>(itr->second);
					prio = std::get<1>(itr->second)->rulePriority;
				}
				baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
				bossoverride |= bosses()->contains(race->keywords[i]->GetFormID());
				adjustacs(race->keywords[i]->GetFormID());
			}
		}
	}
	bossoverride |= bosses()->contains(npc->GetRace()->GetFormID());
	adjustacs(npc->GetRace()->GetFormID());
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (race->keywords[i] == nullptr)
				continue;
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}

	// handle keywords
	for (unsigned int i = 0; i < npc->numKeywords; i++) {
		auto key = npc->keywords[i];
		if (key) {
			if (!ruleoverride) {
				auto it = assocMap()->find(key->GetFormID());
				if (it != assocMap()->end())
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						prio = std::get<1>(it->second)->rulePriority;
					}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
			adjustacs(key->GetFormID());
			// get custom items
			if (custItems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					for (int b = 0; b < vec.size(); b++) {
						if (citemsset->contains(vec[b]->id) == false) {
							citems->push_back(vec[b]);
							citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}
	}
	if (tpltinfo) {
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
				if (!ruleoverride) {
					auto it = assocMap()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (it != assocMap()->end())
						if (prio < std::get<0>(it->second)) {
							rule = std::get<1>(it->second);
							prio = std::get<0>(it->second);
						} else if (prio < std::get<1>(it->second)->rulePriority) {
							rule = std::get<1>(it->second);
							prio = std::get<1>(it->second)->rulePriority;
						}
					baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				adjustacs(tpltinfo->tpltkeywords[i]->GetFormID());
				// get custom items
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int b = 0; b < vec.size(); b++) {
							if (citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
			}
		}
	}

	// handle factions
	for (uint32_t i = 0; i < npc->factions.size(); i++) {
		if (!ruleoverride) {
			auto it = assocMap()->find(npc->factions[i].faction->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
			baseexcluded |= baselineExclusions()->contains(npc->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(npc->factions[i].faction->GetFormID());
		adjustacs(npc->factions[i].faction->GetFormID());
		if (custItems) {
			auto itc = customItems()->find(npc->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}
	if (tpltinfo) {
		for (int i = 0; i < tpltinfo->tpltfactions.size(); i++) {
			if (tpltinfo->tpltfactions[i]) {
				if (!ruleoverride) {
					auto it = assocMap()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (it != assocMap()->end()) {
						if (prio < std::get<0>(it->second)) {
							rule = std::get<1>(it->second);
							prio = std::get<0>(it->second);
						} else if (prio < std::get<1>(it->second)->rulePriority) {
							rule = std::get<1>(it->second);
							prio = std::get<1>(it->second)->rulePriority;
						}
					}
					baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				adjustacs(tpltinfo->tpltfactions[i]->GetFormID());
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int b = 0; b < vec.size(); b++) {
							if (citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
			}
		}
	}

	// handle classes
	if (tpltinfo && tpltinfo->tpltclass)
		cls = tpltinfo->tpltclass;
	if (cls) {
		if (!ruleoverride) {
			auto it = assocMap()->find(cls->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
		}
		adjustacs(cls->GetFormID());
		if (custItems) {
			auto itc = customItems()->find(cls->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}
	// handle combat styles
	if (tpltinfo && tpltinfo->tpltstyle)
		style = tpltinfo->tpltstyle;
	if (style) {
		if (!ruleoverride) {
			auto it = assocMap()->find(style->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
		}
		adjustacs(style->GetFormID());
		if (custItems) {
			auto itc = customItems()->find(style->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}

	if (acsadj != 0) {
		int str = static_cast<int>(acs);
		str += acsadj;
		if (str < 0)
			str = 0;
		if (str > static_cast<int>(ActorStrength::Boss))
			str = static_cast<int>(ActorStrength::Boss);
		acs = static_cast<ActorStrength>(str);
	}
	if (bossoverride)
		acs = ActorStrength::Boss;

	if (custItems) {
		auto itc = customItems()->find(0x0);
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
		// work the accumulated items
		for (int b = 0; b < citems->size(); b++) {
			for (int c = 0; c < citems->at(b)->items.size(); c++) {
				custItems->items.push_back(citems->at(b)->items[c]);
			}
			for (int c = 0; c < citems->at(b)->death.size(); c++) {
				custItems->death.push_back(citems->at(b)->death[c]);
			}
			for (int c = 0; c < citems->at(b)->poisons.size(); c++) {
				custItems->poisons.push_back(citems->at(b)->poisons[c]);
			}
			for (int c = 0; c < citems->at(b)->potions.size(); c++) {
				custItems->potions.push_back(citems->at(b)->potions[c]);
			}
			for (int c = 0; c < citems->at(b)->fortify.size(); c++) {
				custItems->fortify.push_back(citems->at(b)->fortify[c]);
			}
		}
		delete citems;
		delete citemsset;
	}

	if (rule) {
		LOG_1("rule found: {}", rule->ruleName);
		return rule;
	} else {
		// there are no rules!!!
		if (baseexcluded)
			return Distribution::emptyRule;
		LOG_1("default rule found: {}", Distribution::defaultRule->ruleName);
		return Distribution::defaultRule;
	}
}

bool CheckDistributability(std::shared_ptr<ActorInfo> const& acinfo, Distribution::CustomItemStorage* cust)
{
	if (cust == nullptr)
		return false;
	bool distr = false;
	distr |= acinfo->CheckCustomConditionsDistr(cust->food);
	distr |= acinfo->CheckCustomConditionsDistr(cust->fortify);
	distr |= acinfo->CheckCustomConditionsDistr(cust->poisons);
	distr |= acinfo->CheckCustomConditionsDistr(cust->potions);
	distr |= acinfo->CheckCustomConditionsDistrItems(cust->death);
	distr |= acinfo->CheckCustomConditionsDistrItems(cust->items);
	return distr;
}

DistributionRule* Distribution::CalcRule(std::shared_ptr<ActorInfo> const& acinfo, UtilityBase::NPCTPLTInfo* tpltinfo)
{
	StartProfiling;
	if (acinfo == nullptr || acinfo->IsValid() == false) {
		acinfo->SetLastRuleCalcTime();
		acinfo->SetDistributionRule(emptyRule);
		return emptyRule;
	}
	DistributionRule* rule = nullptr;
	if (acinfo->GetLastRuleCalcTime() + std::chrono::seconds(60) > std::chrono::steady_clock::now() && (rule = acinfo->GetDistributionRule()) != nullptr)
		return rule;
	// get npc template info
	UtilityBase::NPCTPLTInfo tplt;
	if (tpltinfo == nullptr) {
		tplt = Utility::ExtractTemplateInfo(acinfo->GetActor());
		tpltinfo = &tplt;
	}
	// calc strength section
	if (Settings::distr._GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty;
		if (diff == 0 || diff == 1) {
			acinfo->SetActorStrength(ActorStrength::Weak);
			acinfo->SetItemStrength(ItemStrength::kWeak);
		} else if (diff == 2 || diff == 3) {
			acinfo->SetActorStrength(ActorStrength::Normal);
			acinfo->SetItemStrength(ItemStrength::kStandard);
		} else if (diff == 4) {
			acinfo->SetActorStrength(ActorStrength::Powerful);
			acinfo->SetItemStrength(ItemStrength::kPotent);
		} else {  // diff == 5
			acinfo->SetActorStrength(ActorStrength::Insane);
			acinfo->SetItemStrength(ItemStrength::kInsane);
		}
	} else {
		// get level dependencies
		short lvl = acinfo->GetLevel();
		if (lvl <= Settings::distr._LevelEasy) {
			acinfo->SetActorStrength(ActorStrength::Weak);
			acinfo->SetItemStrength(ItemStrength::kWeak);
			// weak actor
		} else if (lvl <= Settings::distr._LevelNormal) {
			acinfo->SetActorStrength(ActorStrength::Normal);
			acinfo->SetItemStrength(ItemStrength::kStandard);
			// normal actor
		} else if (lvl <= Settings::distr._LevelDifficult) {
			acinfo->SetActorStrength(ActorStrength::Powerful);
			acinfo->SetItemStrength(ItemStrength::kPotent);
			// difficult actor
		} else if (lvl <= Settings::distr._LevelInsane) {
			acinfo->SetActorStrength(ActorStrength::Insane);
			acinfo->SetItemStrength(ItemStrength::kInsane);
			// insane actor
		} else {
			acinfo->SetActorStrength(ActorStrength::Boss);
			acinfo->SetItemStrength(ItemStrength::kInsane);
			// boss actor
		}
	}
	// now calculate rule and on top get the boss override

	auto acsMap = actorStrengthMap();

	bool bossoverride = false;
	int acsadj = 0;

	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	bool calccustitems = !acinfo->citems.calculated;
	bool calcwhite = !acinfo->IsWhitelistCalculated();

	std::vector<CustomItemStorage*>* citems = nullptr;
	std::unordered_set<std::string>* citemsset = nullptr;
	if (calccustitems) {
		citems = new std::vector<CustomItemStorage*>{};
		citemsset = new std::unordered_set<std::string>{};
	}

	auto base = acinfo->GetActorBase();

	// define general stuff
	auto race = acinfo->GetRace();

	// find rule in npc map
	// npc rules always have the highest priority
	auto itnpc = npcMap()->find(acinfo->GetFormID());
	if (itnpc != npcMap()->end()) {  // found the right rule!
		rule = itnpc->second;        // this can be null if the specific npc is excluded
		ruleoverride = true;
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(acinfo->GetFormID());
	if (auto itr = acsMap->find(acinfo->GetFormID()); itr != acsMap->end())
			acsadj += itr->second;
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(acinfo->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
	}
	if (calcwhite && (whitelistNPCs()->contains(acinfo->GetFormID()) || whitelistNPCsPlugin()->contains(acinfo->GetPluginID())))
		acinfo->SetWhitelisted();

	// now also perform a check on the actor base
	if (!ruleoverride) {
		itnpc = npcMap()->find(acinfo->GetActorBaseFormID());
		if (itnpc != npcMap()->end()) {  // found the right rule!
			rule = itnpc->second;        // this can be null if the specific npc is excluded
			//loginfo("assign rule 2");
			ruleoverride = true;
			prio = INT_MAX;
		}
	}
	bossoverride |= bosses()->contains(acinfo->GetActorBaseFormID());
	if (auto itr = acsMap->find(acinfo->GetActorBaseFormID()); itr != acsMap->end())
		acsadj += itr->second;
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(acinfo->GetActorBaseFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
	}
	if (calcwhite && whitelistNPCs()->contains(acinfo->GetActorBaseFormID()))
		acinfo->SetWhitelisted();
	// perform check on tpltactorbaseinformation
	if (tpltinfo->base != nullptr && tpltinfo->base != acinfo->GetActorBase()) {
		if (!ruleoverride) {
			itnpc = npcMap()->find(tpltinfo->base->GetFormID());
			if (itnpc != npcMap()->end()) {  // found the right rule!
				rule = itnpc->second;        // this can be null if the specific npc is excluded
				//loginfo("assign rule 2");
				ruleoverride = true;
				prio = INT_MAX;
			}
		}
		bossoverride |= bosses()->contains(tpltinfo->base->GetFormID());
		if (auto itr = acsMap->find(tpltinfo->base->GetFormID()); itr != acsMap->end())
			acsadj += itr->second;
		// get custom items
		if (calccustitems) {
			auto itc = customItems()->find(tpltinfo->base->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite && whitelistNPCs()->contains(tpltinfo->base->GetFormID()))
			acinfo->SetWhitelisted();
	}

	if (tpltinfo && tpltinfo->tpltrace)
		race = tpltinfo->tpltrace;
	// now that we didnt't find something so far, check the rest
	// this time all the priorities are the same
	if (!ruleoverride) {
		//loginfo("rule 6");
		auto it = assocMap()->find(race->GetFormID());
		if (it != assocMap()->end())
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				//loginfo("assign rule 3");
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				//loginfo("assign rule 4");
				prio = std::get<1>(it->second)->rulePriority;
			}
		baseexcluded |= baselineExclusions()->contains(race->GetFormID());
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (race->keywords[i] == nullptr)
				continue;
			auto itr = assocMap()->find(race->keywords[i]->GetFormID());
			if (itr != assocMap()->end()) {
				if (prio < std::get<0>(itr->second)) {
					rule = std::get<1>(itr->second);
					//loginfo("assign rule 5 {} {} {}", Utility::GetHex((uintptr_t)std::get<1>(itr->second)), race->keywords[i]->GetFormEditorID(), Utility::GetHex(race->keywords[i]->GetFormID()));
					prio = std::get<0>(itr->second);
				} else if (prio < std::get<1>(itr->second)->rulePriority) {
					rule = std::get<1>(itr->second);
					//loginfo("assign rule 6");
					prio = std::get<1>(itr->second)->rulePriority;
				}
				baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
				bossoverride |= bosses()->contains(race->keywords[i]->GetFormID());
				if (auto itr = acsMap->find(race->keywords[i]->GetFormID()); itr != acsMap->end())
					acsadj += itr->second;
			}
		}
	}
	bossoverride |= bosses()->contains(base->GetRace()->GetFormID());
	if (auto itr = acsMap->find(base->GetRace()->GetFormID()); itr != acsMap->end())
		acsadj += itr->second;
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
			}
		}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (race->keywords[i] == nullptr)
				continue;
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}
	if (calcwhite) { 
		if (whitelistNPCs()->contains(acinfo->GetRaceFormID()))
			acinfo->SetWhitelisted();
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (race->keywords[i] == nullptr)
				continue;
			if (whitelistNPCs()->contains(race->keywords[i]->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}

	// handle keywords
	for (unsigned int i = 0; i < base->numKeywords; i++) {
		auto key = base->keywords[i];
		if (key) {
			if (!ruleoverride) {
				auto it = assocMap()->find(key->GetFormID());
				if (it != assocMap()->end())
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						//loginfo("assign rule 7");
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						//loginfo("assign rule 8");
						prio = std::get<1>(it->second)->rulePriority;
					}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
			if (auto itr = acsMap->find(key->GetFormID()); itr != acsMap->end())
				acsadj += itr->second;
			// get custom items
			if (calccustitems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					for (int b = 0; b < vec.size(); b++) {
						if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
							citems->push_back(vec[b]);
							citemsset->insert(vec[b]->id);
						}
					}
				}
			}
			if (calcwhite) {
				if (whitelistNPCs()->contains(key->GetFormID()))
					acinfo->SetWhitelisted();
			}
		}
	}
	if (tpltinfo) {
		//loginfo("rule 10");
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
				if (!ruleoverride) {
					auto it = assocMap()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (it != assocMap()->end())
						if (prio < std::get<0>(it->second)) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 9");
							prio = std::get<0>(it->second);
						} else if (prio < std::get<1>(it->second)->rulePriority) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 10");
							prio = std::get<1>(it->second)->rulePriority;
						}
					baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				if (auto itr = acsMap->find(tpltinfo->tpltkeywords[i]->GetFormID()); itr != acsMap->end())
					acsadj += itr->second;
				// get custom items
				if (calccustitems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int b = 0; b < vec.size(); b++) {
							if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
				if (calcwhite) {
					if (whitelistNPCs()->contains(tpltinfo->tpltkeywords[i]->GetFormID()))
						acinfo->SetWhitelisted();
				}
			}
		}
	}

	// handle factions
	for (uint32_t i = 0; i < base->factions.size(); i++) {
		if (base->factions[i].faction == nullptr)
			continue;
		if (!ruleoverride) {
			auto it = assocMap()->find(base->factions[i].faction->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 11");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 12");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
			baseexcluded |= baselineExclusions()->contains(base->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(base->factions[i].faction->GetFormID());
		if (auto itr = acsMap->find(base->factions[i].faction->GetFormID()); itr != acsMap->end())
			acsadj += itr->second;
		if (calccustitems) {
			auto itc = customItems()->find(base->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite) {
			if (whitelistNPCs()->contains(base->factions[i].faction->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}
	if (tpltinfo) {
		for (int i = 0; i < tpltinfo->tpltfactions.size(); i++) {
			if (tpltinfo->tpltfactions[i]) {
				if (!ruleoverride) {
					auto it = assocMap()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (it != assocMap()->end()) {
						if (prio < std::get<0>(it->second)) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 13");
							prio = std::get<0>(it->second);
						} else if (prio < std::get<1>(it->second)->rulePriority) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 14");
							prio = std::get<1>(it->second)->rulePriority;
						}
					}
					baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				if (auto itr = acsMap->find(tpltinfo->tpltfactions[i]->GetFormID()); itr != acsMap->end())
					acsadj += itr->second;
				if (calccustitems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int b = 0; b < vec.size(); b++) {
							if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
				if (calcwhite) {
					if (whitelistNPCs()->contains(tpltinfo->tpltfactions[i]->GetFormID()))
						acinfo->SetWhitelisted();
				}
			}
		}
	}
	
	// dont use tplt for class and combatstyle, since they may have been modified during runtime

	// handle classes
	if (base->npcClass) {
		if (!ruleoverride) {
			auto it = assocMap()->find(base->npcClass->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 15");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 16");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
		}
		if (auto itr = acsMap->find(base->npcClass->GetFormID()); itr != acsMap->end())
			acsadj += itr->second;
		if (calccustitems) {
			auto itc = customItems()->find(base->npcClass->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite) {
			if (whitelistNPCs()->contains(base->npcClass->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}
	// handle combat styles
	if (base->combatStyle) {
		//loginfo("rule 16");
		if (!ruleoverride) {
			auto it = assocMap()->find(base->combatStyle->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 17");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 18");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
		}
		if (auto itr = acsMap->find(base->combatStyle->GetFormID()); itr != acsMap->end())
			acsadj += itr->second;
		if (calccustitems) {
			auto itc = customItems()->find(base->combatStyle->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite) {
			if (whitelistNPCs()->contains(base->combatStyle->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}

	if (acsadj != 0) {
		int str = static_cast<int>(acinfo->GetActorStrength());
		str += acsadj;
		if (str < 0)
			str = 0;
		if (str > static_cast<int>(ActorStrength::Boss))
			str = static_cast<int>(ActorStrength::Boss);
		acinfo->SetActorStrength(static_cast<ActorStrength>(str));
	}
	if (bossoverride)
		acinfo->SetActorStrength(ActorStrength::Boss);

	if (calccustitems) {
		auto itc = customItems()->find(0x0);
		LOG_1("custom item 0x0");
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
		// work the accumulated items
		for (int b = 0; b < citems->size(); b++) {
			for (int c = 0; c < citems->at(b)->items.size(); c++) {
				acinfo->citems.items.push_back(citems->at(b)->items[c]);
			}
			for (int c = 0; c < citems->at(b)->death.size(); c++) {
				acinfo->citems.death.push_back(citems->at(b)->death[c]);
			}
			for (int c = 0; c < citems->at(b)->poisons.size(); c++) {
				acinfo->citems.poisons.push_back(citems->at(b)->poisons[c]);
			}
			for (int c = 0; c < citems->at(b)->potions.size(); c++) {
				acinfo->citems.potions.push_back(citems->at(b)->potions[c]);
			}
			for (int c = 0; c < citems->at(b)->fortify.size(); c++) {
				acinfo->citems.fortify.push_back(citems->at(b)->fortify[c]);
			}
		}
		delete citems;
		delete citemsset;

		acinfo->citems.calculated = true;
		acinfo->citems.CreateMaps();
	}
	acinfo->SetBoss(bossoverride);

	if (calcwhite) {
		acinfo->SetWhitelistCalculated();
	}

	PROF_1(TimeProfiling, "execution time for {}", Utility::PrintFormNonDebug(acinfo));

	acinfo->SetLastRuleCalcTime();
	if (rule) {
		LOG_1("rule found: {}", rule->ruleName);
		acinfo->SetDistributionRule(rule);
		return rule;
	} else {
		// there are no rules!!!
		if (baseexcluded) {
			acinfo->SetDistributionRule(Distribution::emptyRule);
			return Distribution::emptyRule;
		}
		LOG_1("default rule found: {}", Distribution::defaultRule->ruleName);
		acinfo->SetDistributionRule(Distribution::defaultRule);
		return Distribution::defaultRule;
	}
}
/*
static std::string GetFormEditorID(RE::TESFaction* fid)
{
	const auto& [map, lock] = RE::TESForm::GetAllFormsByEditorID();
	const RE::BSReadLockGuard locker{ lock };
	if (map) {
		for (auto& [id, form] : *map) {
			auto editorID = id.c_str();
			RE::FormID formID = form->GetFormID();
			if (formID == fid->GetFormID())
				return std::string(editorID);
		}
	}
	auto fullName = fid ? fid->As<RE::TESFullName>() : nullptr;
	if (fullName) {
		return std::string(fullName->fullName.c_str());
	}
	return "ERROR";
}
*/
std::vector<std::tuple<int, DistributionRule*, std::string>> Distribution::CalcAllRules(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	// calc strength section
	if (Settings::distr._GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty;
		if (diff == 0 || diff == 1) {
			acs = ActorStrength::Weak;
			is = ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acs = ActorStrength::Normal;
			is = ItemStrength::kStandard;
		} else if (diff == 4) {
			acs = ActorStrength::Powerful;
			is = ItemStrength::kPotent;
		} else {  // diff == 5
			acs = ActorStrength::Insane;
			is = ItemStrength::kInsane;
		}
	} else {
		// get level dependencies
		short lvl = actor->GetLevel();
		if (lvl <= Settings::distr._LevelEasy) {
			acs = ActorStrength::Weak;
			is = ItemStrength::kWeak;
			// weak actor
		} else if (lvl <= Settings::distr._LevelNormal) {
			acs = ActorStrength::Normal;
			is = ItemStrength::kStandard;
			// normal actor
		} else if (lvl <= Settings::distr._LevelDifficult) {
			acs = ActorStrength::Powerful;
			is = ItemStrength::kPotent;
			// difficult actor
		} else if (lvl <= Settings::distr._LevelInsane) {
			acs = ActorStrength::Insane;
			is = ItemStrength::kInsane;
			// insane actor
		} else {
			acs = ActorStrength::Boss;
			is = ItemStrength::kInsane;
			// boss actor
		}
	}

	// now calculate rule and on top get the boss override

	bool bossoverride = false;
	int acsadj = 0;
	auto adjustacs = [&acsadj](RE::FormID id) {
		try {
			acsadj += actorStrengthMap()->at(id);
		} catch (std::out_of_range&) {
		}
	};

	bool baseexcluded = false;
	int prio = INT_MIN;
	const bool ruleoverride = false;

	auto base = actor->GetActorBase();

	std::vector<std::tuple<int, DistributionRule*, std::string>> rls;
	DistributionRule* rule = nullptr;

	//std::vector<Rule*> rls;
	// find rule in npc map
	// npc rules always have the highest priority
	auto itnpc = npcMap()->find(actor->GetFormID());
	if (itnpc != npcMap()->end()) {  // found the right rule!
		rule = itnpc->second;     // this can be null if the specific npc is excluded
		rls.push_back({ INT_MAX, itnpc->second, "NPC Exclusive" });
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(actor->GetFormID());
	adjustacs(actor->GetFormID());

	// now also perform a check on the actor base
	if (!ruleoverride) {
		itnpc = npcMap()->find(actor->GetActorBase()->GetFormID());
		if (itnpc != npcMap()->end()) {  // found the right rule!
			rule = itnpc->second;     // this can be null if the specific npc is excluded
			rls.push_back({ INT_MAX, itnpc->second, "NPC Exclusive" });
			prio = INT_MAX;
		}
	}
	bossoverride |= bosses()->contains(actor->GetActorBase()->GetFormID());
	adjustacs(actor->GetActorBase()->GetFormID());

	// now that we didnt't find something so far, check the rest
	// this time all the priorities are the same
	if (!ruleoverride) {
		auto it = assocMap()->find(base->GetRace()->GetFormID());
		if (it != assocMap()->end()) {
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				prio = std::get<1>(it->second)->rulePriority;
			}
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Race\t" + Utility::PrintForm(base->GetRace()) });
		}
		baseexcluded |= baselineExclusions()->contains(base->GetRace()->GetFormID());
		adjustacs(base->GetRace()->GetFormID());
		auto race = base->GetRace();
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (race->keywords[i] == nullptr)
				continue;
			auto itr = assocMap()->find(race->keywords[i]->GetFormID());
			if (itr != assocMap()->end())
			{
				if (prio < std::get<0>(itr->second)) {
					rule = std::get<1>(itr->second);
					prio = std::get<0>(itr->second);
				} else if (prio < std::get<1>(itr->second)->rulePriority) {
					rule = std::get<1>(itr->second);
					prio = std::get<1>(itr->second)->rulePriority;
				}
				rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Racekwd\t" + Utility::PrintForm(race->keywords[i]) });
			}
			baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
			bossoverride |= bosses()->contains(race->keywords[i]->GetFormID());
			adjustacs(race->keywords[i]->GetFormID());
		}
	}
	bossoverride |= bosses()->contains(base->GetRace()->GetFormID());
	adjustacs(base->GetRace()->GetFormID());

	// handle keywords
	for (unsigned int i = 0; i < base->numKeywords; i++) {
		auto key = base->keywords[i];
		if (key) {
			if (!ruleoverride) {
				auto it = assocMap()->find(key->GetFormID());
				if (it != assocMap()->end()) {
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						prio = std::get<1>(it->second)->rulePriority;
					}
					rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Keyword\t" + Utility::PrintForm(key)  });
				}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
			adjustacs(key->GetFormID());
		}
	}

	for (uint32_t i = 0; i < base->factions.size(); i++) {
		if (!ruleoverride) {
			auto it = assocMap()->find(base->factions[i].faction->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					prio = std::get<1>(it->second)->rulePriority;
				}
				rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Faction\t" + Utility::PrintForm(base->factions[i].faction) });
			}
			baseexcluded |= baselineExclusions()->contains(base->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(base->factions[i].faction->GetFormID());
		adjustacs(base->factions[i].faction->GetFormID());
	}

	// handle classes
	if (base->npcClass) {
		auto it = assocMap()->find(base->npcClass->GetFormID());
		if (it != assocMap()->end()) {
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				prio = std::get<1>(it->second)->rulePriority;
			}
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Class\t" + Utility::PrintForm(base->npcClass) });
		}
		adjustacs(base->npcClass->GetFormID());
	}

	// handle combat styles
	if (base->combatStyle) {
		auto it = assocMap()->find(base->combatStyle->GetFormID());
		if (it != assocMap()->end()) {
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				prio = std::get<1>(it->second)->rulePriority;
			}
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "CombatStyle\t" + Utility::PrintForm(base->combatStyle) });
		}
		adjustacs(base->combatStyle->GetFormID());
	}

	if (acsadj != 0) {
		int str = static_cast<int>(acs);
		str += acsadj;
		if (str < 0)
			str = 0;
		if (str > static_cast<int>(ActorStrength::Boss))
			str = static_cast<int>(ActorStrength::Boss);
		acs = static_cast<ActorStrength>(str);
	}
	if (bossoverride)
		acs = ActorStrength::Boss;

	if (rule) {
		LOG_1("rule found: {}", rule->ruleName);
		rls.insert(rls.begin(), { INT_MIN, rule, "Chosen" });
		return rls;
	} else {
		// there are no rules!!!
		if (baseexcluded) {
			return std::vector<std::tuple<int, DistributionRule*, std::string>>{ { INT_MIN, Distribution::emptyRule, "Empty" } };
		}
		LOG_1("default rule found: {}", Distribution::defaultRule->ruleName);
		return std::vector<std::tuple<int, DistributionRule*, std::string>>{ { INT_MIN, Distribution::defaultRule, "Default" } };
	}
}

void Distribution::ResetCustomItems()
{
	std::unordered_set<Distribution::CustomItemStorage*> set;
	LogConsole("Gathering custom items...");
	for (auto cust : _customItems) {
		for (int i = 0; i < cust.second.size(); i++) {
			if (cust.second[i] != nullptr)
				set.insert(cust.second[i]);
		}
	}
	_customItems.clear();
	LogConsole(("Found " + std::to_string(set.size()) + " custom items. Beginning deletion").c_str());
	for (auto entry : set) {
		for (int i = 0; i < entry->items.size(); i++)
			delete entry->items[i];
		for (int i = 0; i < entry->death.size(); i++)
			delete entry->death[i];
		for (int i = 0; i < entry->potions.size(); i++)
			delete entry->potions[i];
		for (int i = 0; i < entry->poisons.size(); i++)
			delete entry->poisons[i];
		for (int i = 0; i < entry->fortify.size(); i++)
			delete entry->fortify[i];
		for (int i = 0; i < entry->food.size(); i++)
			delete entry->food[i];
		delete entry;
	}
	set.clear();
	LogConsole("Reset custom items");
}

void Distribution::ResetRules()
{
	_magicEffectAlchMap.clear();
	_alcohol.clear();
	_whitelistNPCsPlugin.clear();
	_whitelistNPCs.clear();
	_excludedPlugins_NPCs.clear();
	_excludedEffects.clear();
	_dosageEffectMap.clear();
	_dosageItemMap.clear();
	_followerFactions.clear();
	_actorStrengthMap.clear();
	_itemStrengthMap.clear();
	_excludedPlugins.clear();
	_customItems.clear();
	_whitelistItems.clear();
	_baselineExclusions.clear();
	_excludedItemsPlayer.clear();
	_excludedItems.clear();
	_excludedAssoc.clear();
	_excludedNPCs.clear();
	_bosses.clear();
	_assocMap.clear();
	_npcMap.clear();
	_hardExclusions.clear();
	_hardExclusions_Plugins_NPCs.clear();
	for (auto& rule : _rules) {
		delete rule;
	}
	for (auto [str, cat] : _internEffectCategories)
	{
		delete cat;
	}
	for (auto [str, catpre] : _internEffectCategoryPresets)
	{
		delete catpre;
	}
	for (auto [str, effpre] : _internEffectPresets)
	{
		delete effpre;
	}
	_rules.clear();
}

#pragma endregion
