#include "Settings.h"
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

using ActorStrength = Settings::ActorStrength;
using AlchemyEffect = Settings::AlchemyEffect;
using ItemStrength = Settings::ItemStrength;
using ItemType = Settings::ItemType;

static std::mt19937 randi((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
/// <summary>
/// trims random numbers to 1 to RR
/// </summary>
static std::uniform_int_distribution<signed> randRR(1, RandomRange);
static std::uniform_int_distribution<signed> rand100(1, 100);

#pragma region Rule

// private

Settings::AlchemyEffect Settings::Distribution::Rule::GetRandomEffect(Settings::ItemType type)
{
	int random = randRR(randi);
	switch (type) {
	case Settings::ItemType::kPotion:
		for (int i = 0; i < this->potionDistr.size(); i++) {
			if (random <= std::get<0>(this->potionDistr[i])) {
				return std::get<1>(this->potionDistr[i]);
			}
		}
		break;
	case Settings::ItemType::kPoison:
		for (int i = 0; i < this->poisonDistr.size(); i++) {
			if (random <= std::get<0>(this->poisonDistr[i])) {
				return std::get<1>(this->poisonDistr[i]);
			}
		}
		break;
	case Settings::ItemType::kFortifyPotion:
		for (int i = 0; i < this->fortifyDistr.size(); i++) {
			if (random <= std::get<0>(this->fortifyDistr[i])) {
				return std::get<1>(this->fortifyDistr[i]);
			}
		}
		break;
	case Settings::ItemType::kFood:
		for (int i = 0; i < this->foodDistr.size(); i++) {
			if (random <= std::get<0>(this->foodDistr[i])) {
				return std::get<1>(this->foodDistr[i]);
			}
		}
		break;
	}
	return Settings::AlchemyEffect::kNone;
}

RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion1(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potion1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion2(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potion2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion3(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potion3Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotionAdditional(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potionAdditionalChance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion(int str)
{
	str += potionTierAdjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffect(ItemType::kPotion);
	if (eff == AlchemyEffect::kBlood) {
		items = Settings::GetMatchingItems(*Settings::potionsBlood(), static_cast<uint64_t>(eff));
	} else {
RetryPotion:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::potionsWeak_main(), static_cast<uint64_t>(eff));
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::potionsStandard_main(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::potionsPotent_main(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::potionsInsane_main(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		}
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison1(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poison1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison2(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poison2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison3(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poison3Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoisonAdditional(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poisonAdditionalChance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison(int str)
{
	str += poisonTierAdjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
RetryPoison:
	switch (str) {
	case 1:  // weak
		items = Settings::GetMatchingItems(*Settings::poisonsWeak(), static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		break;
	case 2:  // standard
		items = Settings::GetMatchingItems(*Settings::poisonsStandard(), static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryPoison;
		}
		break;
	case 3:  // potent
		items = Settings::GetMatchingItems(*Settings::poisonsPotent(), static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryPoison;
		}
		break;
	case 4:  // insane
		items = Settings::GetMatchingItems(*Settings::poisonsInsane(), static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryPoison;
		}
		break;
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion1(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= fortify1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomFortifyPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion2(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= fortify2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomFortifyPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion(int str)
{
	// get matching items
	std::vector<RE::AlchemyItem*> items;
RetryFortify:
	switch (str) {
	case 1:  // weak
		items = Settings::GetMatchingItems(*Settings::potionsWeak_rest(), static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		break;
	case 2:  // standard
		items = Settings::GetMatchingItems(*Settings::potionsStandard_rest(), static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryFortify;
		}
		break;
	case 3:  // potent
		items = Settings::GetMatchingItems(*Settings::potionsPotent_rest(), static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryFortify;
		}
		break;
	case 4:  // insane
		items = Settings::GetMatchingItems(*Settings::potionsInsane_rest(), static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryFortify;
		}
		break;
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFood_intern(Settings::ItemStrength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	if (rand100(randi) <= foodChance[astr]) {
		// we rolled successfully
	} else
		return nullptr;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto reff = GetRandomEffect(ItemType::kFood);
	switch (reff) {
	case AlchemyEffect::kMagicka:
	case AlchemyEffect::kMagickaRate:
	case AlchemyEffect::kMagickaRateMult:
		items = Settings::GetMatchingItems(*Settings::foodmagicka(), static_cast<uint64_t>(reff));
		break;
	case AlchemyEffect::kHealRate:
	case AlchemyEffect::kHealRateMult:
	case AlchemyEffect::kHealth:
		items = Settings::GetMatchingItems(*Settings::foodstamina(), static_cast<uint64_t>(reff));
		break;
	case AlchemyEffect::kStamina:
	case AlchemyEffect::kStaminaRate:
	case AlchemyEffect::kStaminaRateMult:
		items = Settings::GetMatchingItems(*Settings::foodhealth(), static_cast<uint64_t>(reff));
		break;
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}

// public

std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomPotions(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomPotion1(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPotion2(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPotion3(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	while (ret.size() <= this->maxPotions && (tmp = GetRandomPotionAdditional(strength, acstrength)) != nullptr) {
		ret.push_back(tmp);
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomPoisons(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomPoison1(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPoison2(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPoison3(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	while (ret.size() <= this->maxPoisons && (tmp = GetRandomPoisonAdditional(strength, acstrength)) != nullptr) {
		ret.push_back(tmp);
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomFortifyPotions(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomFortifyPotion1(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomFortifyPotion2(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomFood(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	return std::vector<RE::AlchemyItem*>{ GetRandomFood_intern(strength, acstrength) };
}

#pragma endregion

#pragma region Distribution

void Settings::Distribution::CalcStrength(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	CalcRule(actor, acs, is);
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrItems(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	Rule* rule = CalcRule(actor, acs, is);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		auto ritems = rule->GetRandomPotions(is, acs);
		LOG_4("{}[GetDistrItems] matching potions");
		auto items = ACM::GetMatchingPotions(actor, rule->validPotions);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] potions size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
		//logger::info("potions to give:\t{}", ritems.size());
	}
	if (Settings::_featDistributePoisons) {
		auto ritems = rule->GetRandomPoisons(is, acs);
		LOG_4("{}[GetDistrItems] matching poisons");
		auto items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] poisons size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
		//logger::info("poisons to give:\t{}", ritems.size());
	}
	if (Settings::_featDistributeFortifyPotions) {
		auto ritems = rule->GetRandomFortifyPotions(is, acs);
		LOG_4("{}[GetDistrItems] matching fortify");
		auto items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] fortify size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
		//logger::info("fortify potions to give:\t{}", ritems.size());
	}
	if (Settings::_featDistributeFood) {
		auto ritems = rule->GetRandomFood(is, acs);
		LOG_4("{}[GetDistrItems] matching food");
		auto items = ACM::GetMatchingFood(actor, rule->validFood, false);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		LOG1_4("{}[GetDistrItems] diff: {}", diff);
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			LOG_4("{}[GetDistrItems] diff greater 0");
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] food size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
		//logger::info("food to give:\t{}", ritems.size());
	}
	if (ret.size() > 0 && ret.back() == nullptr) {
		LOG_4("{}[GetDistrItems] remove last item");
		ret.pop_back();
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrPotions(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	Rule* rule = CalcRule(actor, acs, is);
	auto ritems = rule->GetRandomPotions(is, acs);
	auto items = ACM::GetMatchingPotions(actor, rule->validPotions);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrPoisons(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	Rule* rule = CalcRule(actor, acs, is);
	auto ritems = rule->GetRandomPoisons(is, acs);
	auto items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrFortifyPotions(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	Rule* rule = CalcRule(actor, acs, is);
	auto ritems = rule->GetRandomFortifyPotions(is, acs);
	auto items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrFood(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	Rule* rule = CalcRule(actor, acs, is);
	auto ritems = rule->GetRandomFood(is, acs);
	auto items = ACM::GetMatchingFood(actor, rule->validFood, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetMatchingInventoryItemsUnique(RE::Actor* actor)
{
	Rule* rule = CalcRule(actor);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validPotions);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingFood(actor, rule->validFood, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0 && ret.back() == nullptr)
		ret.pop_back();
	return ret;
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetMatchingInventoryItems(RE::Actor* actor)
{
	//logger::info("GetMatchingInventoryItems enter");
	Rule* rule = CalcRule(actor);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validPotions);
		//logger::info("GetMatchingInventoryItems| potions {} | found: {}", Utility::GetHex(rule->validPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
		//logger::info("GetMatchingInventoryItems| poisons {} | found: {}", Utility::GetHex(rule->validPoisons), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
		//logger::info("GetMatchingInventoryItems| fortify {} | found: {}", Utility::GetHex(rule->validFortifyPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingFood(actor, rule->validFood, false);
		//logger::info("GetMatchingInventoryItems| food {} | found: {}", Utility::GetHex(rule->validFood), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = actor->GetInventoryCounts();
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

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor)
{
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	if ((actor->GetActorBase()->GetFormID() & 0xFF000000) == 0xFF000000) {
		//logger::info("calc c 1");
		auto info = Settings::Distribution::ExtractTemplateInfo(actor->GetActorBase());
		return CalcRule(actor, acs, is, &info);
	} else {
		//logger::info("calc c 2");
		return CalcRule(actor, acs, is, nullptr);
	}
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::TESNPC* npc)
{
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	return CalcRule(npc, acs, is);
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	//logger::info("fire 1");
	if ((actor->GetActorBase()->GetFormID() & 0xFF000000) == 0xFF000000) {
		//logger::info("fire 2");
		auto info = Settings::Distribution::ExtractTemplateInfo(actor->GetActorBase());
		return CalcRule(actor, acs, is, &info);
	} else
		return CalcRule(actor, acs, is, nullptr);
}

/// <summary>
/// returns wether an npc is excluded from item distribution
/// </summary>
/// <param name="actor">ActorRef to check</param>
/// <returns></returns>
bool Settings::Distribution::ExcludedNPC(RE::Actor* actor)
{
	// skip fucking deleted references
	if (actor->formFlags & RE::TESForm::RecordFlags::kDeleted)
		return true;
	bool ret = Settings::Distribution::excludedNPCs()->contains(actor->GetFormID()) ||
	           actor->IsInFaction(Settings::CurrentFollowerFaction) ||
	           actor->IsInFaction(Settings::CurrentHirelingFaction) ||
	           (Settings::Distribution::excludedNPCs()->contains(actor->GetActorBase()->GetFormID())) ||
	           actor->IsGhost();
	// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
	if (!Settings::Distribution::npcMap()->contains(actor->GetFormID()) && ret == false) {
		auto base = actor->GetActorBase();
		for (uint32_t i = 0; i < base->numKeywords; i++) {
			if (base->keywords[i])
				ret |= Settings::Distribution::excludedAssoc()->contains(base->keywords[i]->GetFormID());
		}
		for (uint32_t i = 0; i < base->factions.size(); i++) {
			if (base->factions[i].faction)
				ret |= Settings::Distribution::excludedAssoc()->contains(base->factions[i].faction->GetFormID());
		}
		auto race = actor->GetRace();
		if (race) {
			ret |= Settings::Distribution::excludedAssoc()->contains(race->GetFormID());
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				ret |= Settings::Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID());
			}
		}
	}
	return ret;
}
/// <summary>
/// returns wether an npc is excluded from item distribution
/// </summary>
/// <param name="npc">ActorBase to check</param>
/// <returns></returns>
bool Settings::Distribution::ExcludedNPC(RE::TESNPC* npc)
{
	// skip fucking deleted references
	if (npc->formFlags & RE::TESForm::RecordFlags::kDeleted)
		return true;
	bool ret = (Settings::Distribution::excludedNPCs()->contains(npc->GetFormID())) ||
	           npc->IsInFaction(Settings::CurrentFollowerFaction) ||
	           npc->IsInFaction(Settings::CurrentHirelingFaction) ||
	           npc->IsGhost();
	// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
	if (!Settings::Distribution::npcMap()->contains(npc->GetFormID()) && ret == false) {
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				ret |= Settings::Distribution::excludedAssoc()->contains(npc->keywords[i]->GetFormID());
		}
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				ret |= Settings::Distribution::excludedAssoc()->contains(npc->factions[i].faction->GetFormID());
		}
		auto race = npc->GetRace();
		if (race) {
			ret |= Settings::Distribution::excludedAssoc()->contains(race->GetFormID());
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				ret |= Settings::Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID());
			}
		}
	}
	return ret;
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is, NPCTPLTInfo* tpltinfo)
{
	// calc strength section
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
		if (diff == 0 || diff == 1) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acs = Settings::ActorStrength::Normal;
			is = Settings::ItemStrength::kStandard;
		} else if (diff == 4) {
			acs = Settings::ActorStrength::Powerful;
			is = Settings::ItemStrength::kPotent;
		} else {  // diff == 5
			acs = Settings::ActorStrength::Insane;
			is = Settings::ItemStrength::kInsane;
		}
	} else {
		// get level dependencies
		short lvl = actor->GetLevel();
		if (lvl <= _LevelEasy) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
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
		}
	}
	//logger::info("rule 1");
	// now calculate rule and on top get the boss override

	bool bossoverride = false;

	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	auto base = actor->GetActorBase();

	Rule* rule = nullptr;
	// define general stuff
	auto race = actor->GetRace();
	//logger::info("rule 2");

	//std::vector<Rule*> rls;
	// find rule in npc map
	// npc rules always have the highest priority
	auto itnpc = npcMap()->find(actor->GetFormID());
	if (itnpc != npcMap()->end()) {  // found the right rule!
		rule = itnpc->second;     // this can be null if the specific npc is excluded
		//logger::info("assign rule 1");
		ruleoverride = true;
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(actor->GetFormID());
	//logger::info("rule 3");

	if (ruleoverride && bossoverride) {
		goto SKIPActor;
	}

	// now also perform a check on the actor base
	if (!ruleoverride) {
		itnpc = npcMap()->find(actor->GetActorBase()->GetFormID());
		if (itnpc != npcMap()->end()) {  // found the right rule!
			rule = itnpc->second;     // this can be null if the specific npc is excluded
			//logger::info("assign rule 2");
			ruleoverride = true;
			prio = INT_MAX;
		}
	}
	//logger::info("rule 4");
	bossoverride |= bosses()->contains(actor->GetActorBase()->GetFormID());

	if (ruleoverride && bossoverride) {
		goto SKIPActor;
	}
	//logger::info("rule 5");

	if (tpltinfo && tpltinfo->tpltrace)
		race = tpltinfo->tpltrace;
	// now that we didnt't find something so far, check the rest
	// this time all the priorities are the same
	if (!ruleoverride) {
		//logger::info("rule 6");
		auto it = assocMap()->find(race->GetFormID());
		if (it != assocMap()->end())
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				//logger::info("assign rule 3");
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				//logger::info("assign rule 4");
				prio = std::get<1>(it->second)->rulePriority;
			}
		baseexcluded |= baselineExclusions()->contains(race->GetFormID());
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			auto itr = assocMap()->find(race->keywords[i]->GetFormID());
			if (itr != assocMap()->end())
			{
				if (prio < std::get<0>(itr->second)) {
					rule = std::get<1>(itr->second);
					//logger::info("assign rule 5 {} {} {}", Utility::GetHex((uintptr_t)std::get<1>(itr->second)), race->keywords[i]->GetFormEditorID(), Utility::GetHex(race->keywords[i]->GetFormID()));
					prio = std::get<0>(itr->second);
				} else if (prio < std::get<1>(itr->second)->rulePriority) {
					rule = std::get<1>(itr->second);
					//logger::info("assign rule 6");
					prio = std::get<1>(itr->second)->rulePriority;
				}
				baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
			}
		}
	}
	//logger::info("rule 7");
	bossoverride |= bosses()->contains(base->GetRace()->GetFormID());

	if (ruleoverride && bossoverride) {
		goto SKIPActor;
	}

	//logger::info("rule 8");
	// handle keywords
	for (unsigned int i = 0; i < base->numKeywords; i++) {
		auto key = base->keywords[i];
		if (key) {
			if (!ruleoverride) {
				auto it = assocMap()->find(key->GetFormID());
				if (it != assocMap()->end())
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						//logger::info("assign rule 7");
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						//logger::info("assign rule 8");
						prio = std::get<1>(it->second)->rulePriority;
					}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
		}
	}
	if (tpltinfo) {
		//logger::info("rule 10");
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
				auto it = assocMap()->find(tpltinfo->tpltkeywords[i]->GetFormID());
				if (it != assocMap()->end())
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						//logger::info("assign rule 9");
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						//logger::info("assign rule 10");
						prio = std::get<1>(it->second)->rulePriority;
					}
				baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
			}
		}
	}
	//logger::info("rule 11");
	if (ruleoverride && bossoverride) {
		goto SKIPActor;
	}

	//logger::info("rule 12");
	// handle factions
	for (uint32_t i = 0; i < base->factions.size(); i++) {
		if (!ruleoverride) {
			auto it = assocMap()->find(base->factions[i].faction->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//logger::info("assign rule 11");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//logger::info("assign rule 12");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
			baseexcluded |= baselineExclusions()->contains(base->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(base->factions[i].faction->GetFormID());
	}
	if (tpltinfo) {
		//logger::info("rule 13");
		for (int i = 0; i < tpltinfo->tpltfactions.size(); i++) {
			if (tpltinfo->tpltfactions[i]) {
				auto it = assocMap()->find(tpltinfo->tpltfactions[i]->GetFormID());
				if (it != assocMap()->end()) {
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						//logger::info("assign rule 13");
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						//logger::info("assign rule 14");
						prio = std::get<1>(it->second)->rulePriority;
					}
				}
				baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
			}
		}
	}
	//logger::info("rule 14");
	if (bossoverride && ruleoverride || ruleoverride)
		goto SKIPActor;

	// dont use tplt for class and combatstyle, since they may have been modified during runtime

	// handle classes
	if (base->npcClass) {
		//logger::info("rule 15");
		auto it = assocMap()->find(base->npcClass->GetFormID());
		if (it != assocMap()->end()) {
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				//logger::info("assign rule 15");
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				//logger::info("assign rule 16");
				prio = std::get<1>(it->second)->rulePriority;
			}
		}
	}

	// handle combat styles
	if (base->combatStyle) {
		//logger::info("rule 16");
		auto it = assocMap()->find(base->combatStyle->GetFormID());
		if (it != assocMap()->end()) {
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				//logger::info("assign rule 17");
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				//logger::info("assign rule 18");
				prio = std::get<1>(it->second)->rulePriority;
			}
		}
	}

SKIPActor:

	if (bossoverride)
		acs = ActorStrength::Boss;
	//logger::info("rule 17");

	if (rule) {
		//logger::info("rule 18 {}", Utility::GetHex((uintptr_t)rule));
		LOG1_1("{}[CalcRuleBase] rule found: {}", rule->ruleName);
		return rule;
	} else {
		// there are no rules!!!
		//logger::info("rule 19");
		if (baseexcluded) {
			//logger::info("rule 20");
			return Settings::Distribution::emptyRule;
		}
		LOG1_1("{}[CalcRuleBase] default rule found: {}", Settings::Distribution::defaultRule->ruleName);
		return Settings::Distribution::defaultRule;
	}
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::TESNPC* npc, ActorStrength& acs, ItemStrength& is, NPCTPLTInfo* tpltinfo)
{
	// calc strength section
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
		if (diff == 0 || diff == 1) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acs = Settings::ActorStrength::Normal;
			is = Settings::ItemStrength::kStandard;
		} else if (diff == 4) {
			acs = Settings::ActorStrength::Powerful;
			is = Settings::ItemStrength::kPotent;
		} else {  // diff == 5
			acs = Settings::ActorStrength::Insane;
			is = Settings::ItemStrength::kInsane;
		}
	} else {
		// level not available for BaseActors

		/*
		// get level dependencies
		short lvl = actor->GetLevel();
		if (lvl <= _LevelEasy) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
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

	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	Rule* rule = nullptr;
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

	if (ruleoverride && bossoverride) {
		goto SKIPNPC;
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
			}
		}
	}
	bossoverride |= bosses()->contains(npc->GetRace()->GetFormID());

	if (ruleoverride && bossoverride) {
		goto SKIPNPC;
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
		}
	}
	if (tpltinfo) {
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
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
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
			}
		}
	}
	if (ruleoverride && bossoverride) {
		goto SKIPNPC;
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
	}
	if (tpltinfo) {
		for (int i = 0; i < tpltinfo->tpltfactions.size(); i++) {
			if (tpltinfo->tpltfactions[i]) {
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
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
			}
		}
	}

	if (bossoverride && ruleoverride || ruleoverride)
		goto SKIPNPC;

	// handle classes
	if (tpltinfo && tpltinfo->tpltclass)
		cls = tpltinfo->tpltclass;
	if (cls) {
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
	// handle combat styles
	if (tpltinfo && tpltinfo->tpltstyle)
		style = tpltinfo->tpltstyle;
	if (style) {
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

SKIPNPC:

	if (bossoverride)
		acs = ActorStrength::Boss;

	if (rule) {
		LOG1_1("{}[CalcRuleBase] rule found: {}", rule->ruleName);
		return rule;
	} else {
		// there are no rules!!!
		if (baseexcluded)
			return Settings::Distribution::emptyRule;
		LOG1_1("{}[CalcRuleBase] default rule found: {}", Settings::Distribution::defaultRule->ruleName);
		return Settings::Distribution::defaultRule;
	}
}

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

std::vector<std::tuple<int, Settings::Distribution::Rule*, std::string>> Settings::Distribution::CalcAllRules(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	// calc strength section
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
		if (diff == 0 || diff == 1) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acs = Settings::ActorStrength::Normal;
			is = Settings::ItemStrength::kStandard;
		} else if (diff == 4) {
			acs = Settings::ActorStrength::Powerful;
			is = Settings::ItemStrength::kPotent;
		} else {  // diff == 5
			acs = Settings::ActorStrength::Insane;
			is = Settings::ItemStrength::kInsane;
		}
	} else {
		// get level dependencies
		short lvl = actor->GetLevel();
		if (lvl <= _LevelEasy) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
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
		}
	}

	// now calculate rule and on top get the boss override

	bool bossoverride = false;

	bool baseexcluded = false;
	int prio = INT_MIN;
	const bool ruleoverride = false;

	auto base = actor->GetActorBase();

	std::vector<std::tuple<int, Settings::Distribution::Rule*, std::string>> rls;
	Rule* rule = nullptr;

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
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Race\t" + Utility::GetHex(base->GetRace()->GetFormID()) + "\t" + std::string(base->GetRace()->GetName()) });
		}
		baseexcluded |= baselineExclusions()->contains(base->GetRace()->GetFormID());
		auto race = base->GetRace();
		for (uint32_t i = 0; i < race->numKeywords; i++) {
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
				rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Racekwd\t" + Utility::GetHex(race->keywords[i]->GetFormID()) + "\t" + std::string(race->keywords[i]->GetFormEditorID()) });
			}
			baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
		}
	}
	bossoverride |= bosses()->contains(base->GetRace()->GetFormID());

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
					rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Keyword\t" + Utility::GetHex(key->GetFormID()) + "\t" + std::string(key->GetFormEditorID()) });
				}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
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
				rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Faction\t" + Utility::GetHex(base->factions[i].faction->GetFormID()) + "\t" + GetFormEditorID(base->factions[i].faction) });
			}
			baseexcluded |= baselineExclusions()->contains(base->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(base->factions[i].faction->GetFormID());
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
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Class\t" + Utility::GetHex(base->npcClass->GetFormID()) + "\t" + std::string(base->npcClass->GetFormEditorID()) });
		}
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
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "CombatStyle\t" + Utility::GetHex(base->combatStyle->GetFormID()) + "\t" + std::string(base->combatStyle->GetFormEditorID()) });
		}
	}

	if (bossoverride)
		acs = ActorStrength::Boss;

	if (rule) {
		LOG1_1("{}[CalcRuleBase] rule found: {}", rule->ruleName);
		rls.insert(rls.begin(), { INT_MIN, rule, "Chosen" });
		return rls;
	} else {
		// there are no rules!!!
		if (baseexcluded) {
			return std::vector<std::tuple<int, Settings::Distribution::Rule*, std::string>>{ { INT_MIN, Settings::Distribution::emptyRule, "Empty" } };
		}
		LOG1_1("{}[CalcRuleBase] default rule found: {}", Settings::Distribution::defaultRule->ruleName);
		return std::vector<std::tuple<int, Settings::Distribution::Rule*, std::string>>{ { INT_MIN, Settings::Distribution::defaultRule, "Default" } };
	}
}
#pragma endregion
