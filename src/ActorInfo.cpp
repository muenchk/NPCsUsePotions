#include<ActorInfo.h>
#include<Settings.h>
#include<Utility.h>
#include <Distribution.h>

ActorInfo::ActorInfo(RE::Actor* _actor, int _durHealth, int _durMagicka, int _durStamina, int _durFortify, int _durRegeneration)
{
	LOG_3("{}[ActorInfo] [ActorInfo]");
	actor = _actor;
	durHealth = _durHealth;
	durMagicka = _durMagicka;
	durStamina = _durStamina;
	durFortify = _durFortify;
	durRegeneration = _durRegeneration;
	citems = new CustomItems();
	CalcCustomItems();
}

std::string ActorInfo::ToString()
{
	return "actor addr: " + Utility::GetHex(reinterpret_cast<std::uintptr_t>(actor)) + "\tactor id:" + Utility::GetHex((actor ? actor->GetFormID() : 0));
}

void ActorInfo::CalcCustomItems()
{
	LOG_3("{}[ActorInfo] [CalcCustomItems]");
	Distribution::CalcRule(this);
}

#define CV(x) static_cast<uint64_t>(x)

std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> ActorInfo::FilterCustomConditionsDistr(std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> itms)
{
	LOG_3("{}[ActorInfo] [FilterCustomConditionsDistr]");
	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> dist;
	for (int i = 0; i < itms.size(); i++) {
		if (std::get<0>(itms[i]) == nullptr || std::get<0>(itms[i])->GetFormID() == 0)
			continue;
		uint64_t cond1 = std::get<3>(itms[i]);
		uint64_t cond2 = std::get<4>(itms[i]);
		bool valid = true;
		if ((cond1 & static_cast<uint64_t>(CustomItemConditions::IsBoss))) 
			if (IsBoss() == false)
				valid = false;


		if (valid == true)
			dist.push_back(itms[i]);
	}
	return dist;
}

std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> ActorInfo::FilterCustomConditionsDistrItems(std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> itms)
{
	LOG_3("{}[ActorInfo] [FilterCustomConditionsDistrItems]");
	std::vector<std::tuple<RE::TESBoundObject*, int, int8_t, uint64_t, uint64_t, bool>> dist;
	for (int i = 0; i < itms.size(); i++) {
		if (std::get<0>(itms[i]) == nullptr || std::get<0>(itms[i])->GetFormID() == 0)
			continue;
		uint64_t cond1 = std::get<3>(itms[i]);
		uint64_t cond2 = std::get<4>(itms[i]);
		bool valid = true;
		if ((cond1 & static_cast<uint64_t>(CustomItemConditions::IsBoss))) {
			if (IsBoss() == false)
				valid = false;
		}

		if (valid == true)
			dist.push_back(itms[i]);
	}
	return dist;
}

bool ActorInfo::CanUseItem(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseItem]");
	return CanUsePotion(item) | CanUsePoison(item) | CanUseFortify(item) | CanUseFood(item);
}
bool ActorInfo::CanUsePot(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUsePot]");
	return CanUsePotion(item) | CanUseFortify(item);
}
bool ActorInfo::CanUsePotion(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUsePotion]");
	auto itr = citems->potionsset.find(item);
	if (itr->second < 0 || itr->second > citems->potions.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->potions[itr->second]);
	uint64_t cond2 = std::get<4>(citems->potions[itr->second]);
	bool valid = true;

	//if (cond1 & CV(CustomItemConditions::None)) {
	//
	//}
	return valid;
}
bool ActorInfo::CanUsePoison(RE::FormID item) 
{
	LOG_3("{}[ActorInfo] [CanUsePoison]");
	auto itr = citems->poisonsset.find(item);
	if (itr->second < 0 || itr->second > citems->poisons.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->poisons[itr->second]);
	uint64_t cond2 = std::get<4>(citems->poisons[itr->second]);
	bool valid = true;

	//if (cond1 & CV(CustomItemConditions::None)) {
	//
	//}
	return valid;
}
bool ActorInfo::CanUseFortify(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseFortify]");
	auto itr = citems->fortifyset.find(item);
	if (itr->second < 0 || itr->second > citems->fortify.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->fortify[itr->second]);
	uint64_t cond2 = std::get<4>(citems->fortify[itr->second]);
	bool valid = true;

	//if (cond1 & CV(CustomItemConditions::None)) {
	//
	//}
	return valid;
}
bool ActorInfo::CanUseFood(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseFood]");
	auto itr = citems->foodset.find(item);
	if (itr->second < 0 || itr->second > citems->food.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->food[itr->second]);
	uint64_t cond2 = std::get<4>(citems->food[itr->second]);
	bool valid = true;

	//if (cond1 & CV(CustomItemConditions::None)) {
	//
	//}
	return valid;
}

bool ActorInfo::IsCustomAlchItem(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomAlchItem]");
	auto itr = citems->potionsset.find(item->GetFormID());
	if (itr != citems->potionsset.end())
		return true;
	itr = citems->poisonsset.find(item->GetFormID());
	if (itr != citems->poisonsset.end())
		return true;
	itr = citems->fortifyset.find(item->GetFormID());
	if (itr != citems->fortifyset.end())
		return true;
	itr = citems->foodset.find(item->GetFormID());
	if (itr != citems->foodset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomItem(RE::TESBoundObject* item)
{
	LOG_3("{}[ActorInfo] [IsCustomItem]");
	auto itr = citems->itemsset.find(item->GetFormID());
	if (itr != citems->itemsset.end())
		return true;
	return false;
}

void ActorInfo::CustomItems::CreateMaps()
{
	LOG_3("{}[ActorInfo] [CreatMaps]");
	// items map
	itemsset.clear();
	for (int i = 0; i < items.size(); i++) {
		itemsset.insert_or_assign(std::get<0>(items[i])->GetFormID(), i);
	}
	// death map
	deathset.clear();
	for (int i = 0; i < death.size(); i++) {
		deathset.insert_or_assign(std::get<0>(death[i])->GetFormID(), i);
	}
	// potions map
	potionsset.clear();
	for (int i = 0; i < potions.size(); i++) {
		potionsset.insert_or_assign(std::get<0>(potions[i])->GetFormID(), i);
	}
	// poisons map
	poisonsset.clear();
	for (int i = 0; i < poisons.size(); i++) {
		poisonsset.insert_or_assign(std::get<0>(poisons[i])->GetFormID(), i);
	}
	// food map
	foodset.clear();
	for (int i = 0; i < food.size(); i++) {
		foodset.insert_or_assign(std::get<0>(food[i])->GetFormID(), i);
	}
	// fortify map
	fortifyset.clear();
	for (int i = 0; i < fortify.size(); i++) {
		fortifyset.insert_or_assign(std::get<0>(fortify[i])->GetFormID(), i);
	}
}

void ActorInfo::CustomItems::Reset()
{
	LOG_3("{}[ActorInfo] [Reset]");
	items.clear();
	itemsset.clear();
	death.clear();
	deathset.clear();
	potions.clear();
	potionsset.clear();
	poisons.clear();
	poisonsset.clear();
	fortify.clear();
	fortifyset.clear();
	food.clear();
	foodset.clear();
	calculated = false;
}
