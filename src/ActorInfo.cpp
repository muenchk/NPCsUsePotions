#include <exception>

#include "ActorInfo.h"
#include "Settings.h"
#include "Utility.h"
#include "Distribution.h"
#include "BufferOperations.h"

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

ActorInfo::ActorInfo()
{
	actor = nullptr;
	citems = new CustomItems();
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

// data functions

uint32_t ActorInfo::GetVersion()
{
	return version;
}

int32_t ActorInfo::GetDataSize()
{
	int32_t size = 0;
	// versionid
	//size += 4;
	// actor id
	//size += 4;
	// pluginname
	size += Buffer::CalcStringLength(std::string(Utility::GetPluginName(actor)));
	// durHealth, durMagicka, durStamina, durFortify, durRegeneration
	//size += 20;
	// nextFoodTime
	//size ´+= 4;
	// lastDistrTime
	//size += 4;
	// distributedCustomItems
	//size += 1;
	// actorStrength, itemStrength -> int
	//size += 8;
	// _boss
	//size += 1;

	// all except string are constant:
	size += 46;
	return size;
}

int32_t ActorInfo::GetMinDataSize(int32_t vers)
{
	switch (vers) {
	case 1:
		return 46;
	default:
		return 0;
	}
}

void ActorInfo::WriteData(unsigned char* buffer, int offset)
{
	int addoff = 0;
	// version
	Buffer::Write(version, buffer, offset);
	// actor id
	Buffer::Write(actor->GetFormID(), buffer, offset);
	// pluginname
	Buffer::Write(std::string(Utility::GetPluginName(actor)), buffer, offset);
	std::string tmp = std::string(Utility::GetPluginName(actor));
	// durHealth
	Buffer::Write(durHealth, buffer, offset);
	// durMagicka
	Buffer::Write(durMagicka, buffer, offset);
	// durStamina
	Buffer::Write(durStamina, buffer, offset);
	// durFortify
	Buffer::Write(durFortify, buffer, offset);
	// durRegeneration
	Buffer::Write(durRegeneration, buffer, offset);
	// nextFoodTime
	Buffer::Write(nextFoodTime, buffer, offset);
	// lastDistrTime
	Buffer::Write(lastDistrTime, buffer, offset);
	// distributedCustomItems
	Buffer::Write(_distributedCustomItems, buffer, offset);
	// actorStrength
	Buffer::Write(static_cast<uint32_t>(actorStrength), buffer, offset);
	// itemStrength
	Buffer::Write(static_cast<uint32_t>(itemStrength), buffer, offset);
	// _boss
	Buffer::Write(_boss, buffer, offset);
}

bool ActorInfo::ReadData(unsigned char* buffer, int offset, int length)
{
	int ver = Buffer::ReadUInt32(buffer, offset);
	try {
		switch (ver) {
		case 1:
			{
				// first try to make sure that the buffer contains all necessary data and we do not go out of bounds
				int size = GetMinDataSize(ver);
				int strsize = (int)Buffer::CalcStringLength(buffer, offset + 4);  // offset + actorid is begin of pluginname
				if (length < size + strsize)
					return false;

				uint32_t actorid = Buffer::ReadUInt32(buffer, offset);
				std::string pluginname = Buffer::ReadString(buffer, offset);
				RE::TESForm* form = Utility::GetTESForm(RE::TESDataHandler::GetSingleton(), actorid, pluginname);
				if (!form)
					return false;
				actor = form->As<RE::Actor>();
				if (!actor)
					return false;
				durHealth = Buffer::ReadInt32(buffer, offset);
				durMagicka = Buffer::ReadInt32(buffer, offset);
				durStamina = Buffer::ReadInt32(buffer, offset);
				durFortify = Buffer::ReadInt32(buffer, offset);
				durRegeneration = Buffer::ReadInt32(buffer, offset);
				nextFoodTime = Buffer::ReadFloat(buffer, offset);
				lastDistrTime = Buffer::ReadFloat(buffer, offset);
				_distributedCustomItems = Buffer::ReadBool(buffer, offset);
				actorStrength = static_cast<ActorStrength>(Buffer::ReadUInt32(buffer, offset));
				itemStrength = static_cast<ItemStrength>(Buffer::ReadUInt32(buffer, offset));
				_boss = Buffer::ReadBool(buffer, offset);
			}
			return true;
		default:
			return false;
		}
	} catch (std::exception&) {
		return false;
	}
}
