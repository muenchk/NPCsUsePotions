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

using ActorStrength = ActorStrength;
using AlchemyEffect = Settings::AlchemyEffect;
using ItemStrength = ItemStrength;
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

Settings::AlchemyEffect Settings::Distribution::Rule::GetRandomEffectChance(Settings::ItemType type)
{
	int random = randRR(randi);
	switch (type) {
	case Settings::ItemType::kPotion:
		for (int i = 0; i < this->potionDistrChance.size(); i++) {
			if (random <= std::get<0>(this->potionDistrChance[i])) {
				return std::get<1>(this->potionDistrChance[i]);
			}
		}
		break;
	case Settings::ItemType::kPoison:
		for (int i = 0; i < this->poisonDistrChance.size(); i++) {
			if (random <= std::get<0>(this->poisonDistrChance[i])) {
				return std::get<1>(this->poisonDistrChance[i]);
			}
		}
		break;
	case Settings::ItemType::kFortifyPotion:
		for (int i = 0; i < this->fortifyDistrChance.size(); i++) {
			if (random <= std::get<0>(this->fortifyDistrChance[i])) {
				return std::get<1>(this->fortifyDistrChance[i]);
			}
		}
		break;
	case Settings::ItemType::kFood:
		for (int i = 0; i < this->foodDistrChance.size(); i++) {
			if (random <= std::get<0>(this->foodDistrChance[i])) {
				return std::get<1>(this->foodDistrChance[i]);
			}
		}
		break;
	}
	return Settings::AlchemyEffect::kNone;
}

RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion1(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= potion1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion2(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= potion2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion3(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= potion3Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotionAdditional(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= potionAdditionalChance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion(int str, ActorInfo* acinfo)
{
	str += potionTierAdjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffectChance(ItemType::kPotion);
GetRandomPotioneff:;
	if (eff == AlchemyEffect::kCustom) {
		auto potions = acinfo->FilterCustomConditionsDistr(acinfo->citems->potions);
		if (potions.size() == 0) {
			eff = GetRandomEffect(ItemType::kPotion);
			goto GetRandomPotioneff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(potions.size() - 1));
		auto itm = potions[ra(randi)];
		return std::get<0>(itm);
	} else if (eff == AlchemyEffect::kBlood) {
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
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison1(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= poison1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison2(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= poison2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison3(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= poison3Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoisonAdditional(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= poisonAdditionalChance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison(int str, ActorInfo* acinfo)
{
	str += poisonTierAdjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffectChance(ItemType::kPoison);
GetRandomPoisoneff:;
	if (eff == AlchemyEffect::kCustom) {
		auto poisons = acinfo->FilterCustomConditionsDistr(acinfo->citems->poisons);
		if (poisons.size() == 0) {
			eff = GetRandomEffect(ItemType::kPoison);
			goto GetRandomPoisoneff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(poisons.size() - 1));
		auto itm = poisons[ra(randi)];
		//if (std::get<1>(itm) <= rand100(randi)) // calc chances
			return std::get<0>(itm);
		//else
		//	return nullptr;
	} else {
RetryPoison:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::poisonsWeak(), static_cast<uint64_t>(eff));
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::poisonsStandard(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::poisonsPotent(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::poisonsInsane(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
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
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion1(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= fortify1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomFortifyPotion(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion2(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	int str = static_cast<int>(acinfo->itemStrength);
	if (rand100(randi) <= fortify2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomFortifyPotion(str, acinfo);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion(int str, ActorInfo* acinfo)
{
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffectChance(ItemType::kFortifyPotion);
GetRandomFortifyeff:;
	if (eff == AlchemyEffect::kCustom) {
		auto fortify = acinfo->FilterCustomConditionsDistr(acinfo->citems->fortify);
		if (fortify.size() == 0) {
			eff = GetRandomEffect(ItemType::kFortifyPotion);
			goto GetRandomFortifyeff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(fortify.size() - 1));
		auto itm = fortify[ra(randi)];
		return std::get<0>(itm);
	} else {
RetryFortify:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::potionsWeak_rest(), static_cast<uint64_t>(eff));
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::potionsStandard_rest(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::potionsPotent_rest(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::potionsInsane_rest(), static_cast<uint64_t>(eff));
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
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
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFood_intern(ActorInfo* acinfo)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acinfo->actorStrength);
	if (rand100(randi) <= foodChance[astr]) {
		// we rolled successfully
	} else
		return nullptr;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto reff = GetRandomEffectChance(ItemType::kFood);
GetRandomFoodeff:;
	if (reff == AlchemyEffect::kCustom) {
		auto food = acinfo->FilterCustomConditionsDistr(acinfo->citems->food);
		if (food.size() == 0) {
			reff = GetRandomEffect(ItemType::kFood);
			goto GetRandomFoodeff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(food.size() - 1));
		auto itm = food[ra(randi)];
		return std::get<0>(itm);
	} else {
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
		default:
			items = Settings::GetMatchingItems(*Settings::foodall(), static_cast<uint64_t>(reff));
			if (items.size() == 0) {
				items = Settings::GetMatchingItems(*Settings::foodall(), validFood);
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

// public

std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomPotions(ActorInfo* acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomPotion1(acinfo);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPotion2(acinfo);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPotion3(acinfo);
	if (tmp)
		ret.push_back(tmp);
	while (ret.size() <= this->maxPotions && (tmp = GetRandomPotionAdditional(acinfo)) != nullptr) {
		ret.push_back(tmp);
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomPoisons(ActorInfo* acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomPoison1(acinfo);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPoison2(acinfo);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPoison3(acinfo);
	if (tmp)
		ret.push_back(tmp);
	while (ret.size() <= this->maxPoisons && (tmp = GetRandomPoisonAdditional(acinfo)) != nullptr) {
		ret.push_back(tmp);
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomFortifyPotions(ActorInfo* acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomFortifyPotion1(acinfo);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomFortifyPotion2(acinfo);
	if (tmp)
		ret.push_back(tmp);
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomFood(ActorInfo* acinfo)
{
	return std::vector<RE::AlchemyItem*>{ GetRandomFood_intern(acinfo) };
}

#pragma endregion

#pragma region Distribution

std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrItems(ActorInfo* acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		auto ritems = rule->GetRandomPotions(acinfo);
		LOG_4("{}[GetDistrItems] matching potions");
		auto items = ACM::GetMatchingPotions(acinfo, rule->validPotions);
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
		auto ritems = rule->GetRandomPoisons(acinfo);
		LOG_4("{}[GetDistrItems] matching poisons");
		auto items = ACM::GetMatchingPoisons(acinfo, rule->validPoisons);
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
		auto ritems = rule->GetRandomFortifyPotions(acinfo);
		LOG_4("{}[GetDistrItems] matching fortify");
		auto items = ACM::GetMatchingPotions(acinfo, rule->validFortifyPotions);
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
		auto ritems = rule->GetRandomFood(acinfo);
		LOG_4("{}[GetDistrItems] matching food");
		auto items = ACM::GetMatchingFood(acinfo, rule->validFood, false);
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
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrPotions(ActorInfo* acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomPotions(acinfo);
	auto items = ACM::GetMatchingPotions(acinfo, rule->validPotions);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrPoisons(ActorInfo* acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomPoisons(acinfo);
	auto items = ACM::GetMatchingPoisons(acinfo, rule->validPoisons);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrFortifyPotions(ActorInfo* acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomFortifyPotions(acinfo);
	auto items = ACM::GetMatchingPotions(acinfo, rule->validFortifyPotions);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrFood(ActorInfo* acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomFood(acinfo);
	auto items = ACM::GetMatchingFood(acinfo, rule->validFood, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetMatchingInventoryItemsUnique(ActorInfo* acinfo)
{
	Rule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingPotions(acinfo, rule->validPotions);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingPoisons(acinfo, rule->validPoisons);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingPotions(acinfo, rule->validFortifyPotions);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingFood(acinfo, rule->validFood, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0 && ret.back() == nullptr)
		ret.pop_back();
	return ret;
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetMatchingInventoryItems(ActorInfo* acinfo)
{
	//logger::info("GetMatchingInventoryItems enter");
	Rule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingPotions(acinfo, rule->validPotions);
		//logger::info("GetMatchingInventoryItems| potions {} | found: {}", Utility::GetHex(rule->validPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingPoisons(acinfo, rule->validPoisons);
		//logger::info("GetMatchingInventoryItems| poisons {} | found: {}", Utility::GetHex(rule->validPoisons), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingPotions(acinfo, rule->validFortifyPotions);
		//logger::info("GetMatchingInventoryItems| fortify {} | found: {}", Utility::GetHex(rule->validFortifyPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> items = ACM::GetMatchingFood(acinfo, rule->validFood, false);
		//logger::info("GetMatchingInventoryItems| food {} | found: {}", Utility::GetHex(rule->validFood), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = acinfo->actor->GetInventoryCounts();
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
/*
Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor)
{
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	if ((actor->GetActorBase()->GetFormID() & 0xFF000000) == 0xFF000000) {
		//logger::info("calc c 1");
		auto info = Settings::Distribution::ExtractTemplateInfo(actor->GetActorBase());
		return CalcRule(actor, acs, is, &info, nullptr);
	} else {
		//logger::info("calc c 2");
		return CalcRule(actor, acs, is, nullptr, nullptr);
	}
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor, CustomItemStorage* custItems)
{
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	if ((actor->GetActorBase()->GetFormID() & 0xFF000000) == 0xFF000000) {
		//logger::info("calc c 1");
		auto info = Settings::Distribution::ExtractTemplateInfo(actor->GetActorBase());
		return CalcRule(actor, acs, is, &info, custItems);
	} else {
		//logger::info("calc c 2");
		return CalcRule(actor, acs, is, nullptr, custItems);
	}
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::TESNPC* npc)
{
	ActorStrength acs = ActorStrength::Weak;
	ItemStrength is = ItemStrength::kWeak;
	return CalcRule(npc, acs, is, nullptr);
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	//logger::info("fire 1");
	if ((actor->GetActorBase()->GetFormID() & 0xFF000000) == 0xFF000000) {
		//logger::info("fire 2");
		auto info = Settings::Distribution::ExtractTemplateInfo(actor->GetActorBase());
		return CalcRule(actor, acs, is, &info, nullptr);
	} else
		return CalcRule(actor, acs, is, nullptr, nullptr);
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is, CustomItemStorage* custItems)
{
	//logger::info("fire 1");
	if ((actor->GetActorBase()->GetFormID() & 0xFF000000) == 0xFF000000) {
		//logger::info("fire 2");
		auto info = Settings::Distribution::ExtractTemplateInfo(actor->GetActorBase());
		return CalcRule(actor, acs, is, &info, custItems);
	} else
		return CalcRule(actor, acs, is, nullptr, custItems);
}
*/
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
/*
Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor, ActorStrength& acs, ItemStrength& is, NPCTPLTInfo* tpltinfo, CustomItemStorage* custItems)
{
	// calc strength section
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
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
		rule = itnpc->second;        // this can be null if the specific npc is excluded
		//logger::info("assign rule 1");
		ruleoverride = true;
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(actor->GetFormID());
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(actor->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
	}
	//logger::info("rule 3");

	if (custItems == nullptr && ruleoverride && bossoverride) {
		goto SKIPActor;
	}

	// now also perform a check on the actor base
	if (!ruleoverride) {
		itnpc = npcMap()->find(actor->GetActorBase()->GetFormID());
		if (itnpc != npcMap()->end()) {  // found the right rule!
			rule = itnpc->second;        // this can be null if the specific npc is excluded
			//logger::info("assign rule 2");
			ruleoverride = true;
			prio = INT_MAX;
		}
	}
	//logger::info("rule 4");
	bossoverride |= bosses()->contains(actor->GetActorBase()->GetFormID());
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(actor->GetActorBase()->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
	}

	if (custItems == nullptr && ruleoverride && bossoverride) {
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
			if (itr != assocMap()->end()) {
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
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}

	if (custItems == nullptr && ruleoverride && bossoverride) {
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
			// get custom items
			if (custItems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					for (int d = 0; d < vec.size(); d++) {
						custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
						custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
						custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
						custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
						custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
					}
				}
			}
		}
	}
	if (tpltinfo) {
		//logger::info("rule 10");
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
				if (!ruleoverride) {
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
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				// get custom items
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int d = 0; d < vec.size(); d++) {
							custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}
					}
				}
			}
		}
	}
	//logger::info("rule 11");
	if (custItems == nullptr && ruleoverride && bossoverride) {
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
		if (custItems) {
			auto itc = customItems()->find(base->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}
	if (tpltinfo) {
		//logger::info("rule 13");
		for (int i = 0; i < tpltinfo->tpltfactions.size(); i++) {
			if (tpltinfo->tpltfactions[i]) {
				if (!ruleoverride) {
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
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int d = 0; d < vec.size(); d++) {
							custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}
					}
				}
			}
		}
	}
	//logger::info("rule 14");
	if (custItems == nullptr && (ruleoverride && bossoverride || ruleoverride)) {
		goto SKIPActor;
	}

	// dont use tplt for class and combatstyle, since they may have been modified during runtime

	// handle classes
	if (base->npcClass) {
		//logger::info("rule 15");
		if (!ruleoverride) {
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
		if (custItems) {
			auto itc = customItems()->find(base->npcClass->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}

	// handle combat styles
	if (base->combatStyle) {
		//logger::info("rule 16");
		if (!ruleoverride) {
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
		if (custItems) {
			auto itc = customItems()->find(base->combatStyle->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
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
*/
Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::TESNPC* npc, ActorStrength& acs, ItemStrength& is, NPCTPLTInfo* tpltinfo, CustomItemStorage* custItems)
{
	// calc strength section
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
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
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(npc->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
	}

	if (custItems == nullptr && ruleoverride && bossoverride) {
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
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}

	if (custItems == nullptr && ruleoverride && bossoverride) {
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
			// get custom items
			if (custItems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					for (int d = 0; d < vec.size(); d++) {
						custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
						custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
						custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
						custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
						custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
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
				// get custom items
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int d = 0; d < vec.size(); d++) {
							custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}
					}
				}
			}
		}
	}
	if (custItems == nullptr && ruleoverride && bossoverride) {
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
		if (custItems) {
			auto itc = customItems()->find(npc->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
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
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int d = 0; d < vec.size(); d++) {
							custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}
					}
				}
			}
		}
	}

	if (custItems == nullptr && (bossoverride && ruleoverride || ruleoverride))
		goto SKIPNPC;

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
		if (custItems) {
			auto itc = customItems()->find(cls->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
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
		if (custItems) {
			auto itc = customItems()->find(style->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
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

Settings::Distribution::Rule* Settings::Distribution::CalcRule(ActorInfo* acinfo, NPCTPLTInfo* tpltinfo)
{
	if (acinfo == nullptr)
		return emptyRule;
	// calc strength section
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
		if (diff == 0 || diff == 1) {
			acinfo->actorStrength = ActorStrength::Weak;
			acinfo->itemStrength = ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acinfo->actorStrength = ActorStrength::Normal;
			acinfo->itemStrength = ItemStrength::kStandard;
		} else if (diff == 4) {
			acinfo->actorStrength = ActorStrength::Powerful;
			acinfo->itemStrength = ItemStrength::kPotent;
		} else {  // diff == 5
			acinfo->actorStrength = ActorStrength::Insane;
			acinfo->itemStrength = ItemStrength::kInsane;
		}
	} else {
		// get level dependencies
		short lvl = acinfo->actor->GetLevel();
		if (lvl <= _LevelEasy) {
			acinfo->actorStrength = ActorStrength::Weak;
			acinfo->itemStrength = ItemStrength::kWeak;
			// weak actor
		} else if (lvl <= _LevelNormal) {
			acinfo->actorStrength = ActorStrength::Normal;
			acinfo->itemStrength = ItemStrength::kStandard;
			// normal actor
		} else if (lvl <= _LevelDifficult) {
			acinfo->actorStrength = ActorStrength::Powerful;
			acinfo->itemStrength = ItemStrength::kPotent;
			// difficult actor
		} else if (lvl <= _LevelInsane) {
			acinfo->actorStrength = ActorStrength::Insane;
			acinfo->itemStrength = ItemStrength::kInsane;
			// insane actor
		} else {
			acinfo->actorStrength = ActorStrength::Boss;
			acinfo->itemStrength = ItemStrength::kInsane;
			// boss actor
		}
	}
	//logger::info("rule 1");
	// now calculate rule and on top get the boss override

	bool bossoverride = false;

	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	bool calccustitems = !acinfo->citems->calculated;

	auto base = acinfo->actor->GetActorBase();

	Rule* rule = nullptr;
	// define general stuff
	auto race = acinfo->actor->GetRace();
	//logger::info("rule 2");

	//std::vector<Rule*> rls;
	// find rule in npc map
	// npc rules always have the highest priority
	auto itnpc = npcMap()->find(acinfo->actor->GetFormID());
	if (itnpc != npcMap()->end()) {  // found the right rule!
		rule = itnpc->second;        // this can be null if the specific npc is excluded
		//logger::info("assign rule 1");
		ruleoverride = true;
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(acinfo->actor->GetFormID());
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(acinfo->actor->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
	}
	//logger::info("rule 3");

	if (!calccustitems && ruleoverride && bossoverride) {
		goto SKIPActor;
	}

	// now also perform a check on the actor base
	if (!ruleoverride) {
		itnpc = npcMap()->find(acinfo->actor->GetActorBase()->GetFormID());
		if (itnpc != npcMap()->end()) {  // found the right rule!
			rule = itnpc->second;        // this can be null if the specific npc is excluded
			//logger::info("assign rule 2");
			ruleoverride = true;
			prio = INT_MAX;
		}
	}
	//logger::info("rule 4");
	bossoverride |= bosses()->contains(acinfo->actor->GetActorBase()->GetFormID());
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(acinfo->actor->GetActorBase()->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
	}

	if (!calccustitems && ruleoverride && bossoverride) {
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
			if (itr != assocMap()->end()) {
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
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}

	if (!calccustitems && ruleoverride && bossoverride) {
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
			// get custom items
			if (calccustitems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					for (int d = 0; d < vec.size(); d++) {
						acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
						acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
						acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
						acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
						acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
					}
				}
			}
		}
	}
	if (tpltinfo) {
		//logger::info("rule 10");
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
				if (!ruleoverride) {
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
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				// get custom items
				if (calccustitems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int d = 0; d < vec.size(); d++) {
							acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}
					}
				}
			}
		}
	}
	//logger::info("rule 11");
	if (!calccustitems && ruleoverride && bossoverride) {
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
		if (calccustitems) {
			auto itc = customItems()->find(base->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}
	if (tpltinfo) {
		//logger::info("rule 13");
		for (int i = 0; i < tpltinfo->tpltfactions.size(); i++) {
			if (tpltinfo->tpltfactions[i]) {
				if (!ruleoverride) {
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
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				if (calccustitems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int d = 0; d < vec.size(); d++) {
							acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}
					}
				}
			}
		}
	}
	//logger::info("rule 14");
	if (!calccustitems && (ruleoverride && bossoverride || ruleoverride)) {
		goto SKIPActor;
	}

	// dont use tplt for class and combatstyle, since they may have been modified during runtime

	// handle classes
	if (base->npcClass) {
		//logger::info("rule 15");
		if (!ruleoverride) {
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
		if (calccustitems) {
			auto itc = customItems()->find(base->npcClass->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}

	// handle combat styles
	if (base->combatStyle) {
		//logger::info("rule 16");
		if (!ruleoverride) {
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
		if (calccustitems) {
			auto itc = customItems()->find(base->combatStyle->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int d = 0; d < vec.size(); d++) {
					acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}
			}
		}
	}

SKIPActor:

	if (bossoverride)
		acinfo->actorStrength = ActorStrength::Boss;
	//logger::info("rule 17");

	if (calccustitems) {
		auto itc = customItems()->find(0x0);
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int d = 0; d < vec.size(); d++) {
				acinfo->citems->items.insert(acinfo->citems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				acinfo->citems->potions.insert(acinfo->citems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				acinfo->citems->poisons.insert(acinfo->citems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				acinfo->citems->fortify.insert(acinfo->citems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				acinfo->citems->food.insert(acinfo->citems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}
		}
		acinfo->citems->calculated = true;
		acinfo->citems->CreateMaps();
	}
	acinfo->_boss = bossoverride;

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
