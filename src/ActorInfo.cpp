#include <exception>

#include "ActorInfo.h"
#include "Settings.h"
#include "Utility.h"
#include "Distribution.h"
#include "BufferOperations.h"
#include "ActorManipulation.h"

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
	if (_actor) {
		formid = _actor->GetFormID();
		name = std::string(_actor->GetName());
		pluginname = std::string(Utility::GetPluginName(actor));
		if (_actor->HasKeyword(Settings::ActorTypeDwarven) || _actor->GetRace()->HasKeyword(Settings::ActorTypeDwarven))
			_automaton = true;
	}
	CalcCustomItems();
}

ActorInfo::ActorInfo()
{
	actor = nullptr;
	formid = 0;
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
		bool valid = CalcDistrConditions(cond1, cond2);
		if (valid == true)
			dist.push_back(itms[i]);
	}
	return dist;
}

std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> ActorInfo::FilterCustomConditionsUsage(std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> itms)
{
	LOG_3("{}[ActorInfo] [FilterCustomConditionsDistr]");
	std::vector<std::tuple<RE::AlchemyItem*, int, int8_t, uint64_t, uint64_t>> dist;
	for (int i = 0; i < itms.size(); i++) {
		if (std::get<0>(itms[i]) == nullptr || std::get<0>(itms[i])->GetFormID() == 0) {
			LOG_3("{}[ActorInfo] [FilterCustomConditionsDistr] error");
			continue;
		}
		uint64_t cond1 = std::get<3>(itms[i]);
		uint64_t cond2 = std::get<4>(itms[i]);
		bool valid = CalcUsageConditions(cond1, cond2);
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
		if (std::get<0>(itms[i]) == nullptr || std::get<0>(itms[i])->GetFormID() == 0) {
			LOG_3("{}[ActorInfo] [FilterCustomConditionsDistrItems] error");
			continue;
		}
		uint64_t cond1 = std::get<3>(itms[i]);
		uint64_t cond2 = std::get<4>(itms[i]);
		bool valid = CalcDistrConditions(cond1, cond2);
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
	if (itr == citems->potionsset.end() || itr->second < 0 || itr->second > citems->potions.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->potions[itr->second]);
	uint64_t cond2 = std::get<4>(citems->potions[itr->second]);
	return CalcUsageConditions(cond1, cond2);
}
bool ActorInfo::CanUsePoison(RE::FormID item) 
{
	LOG_3("{}[ActorInfo] [CanUsePoison]");
	auto itr = citems->poisonsset.find(item);
	if (itr == citems->poisonsset.end() || itr->second < 0 || itr->second > citems->poisons.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->poisons[itr->second]);
	uint64_t cond2 = std::get<4>(citems->poisons[itr->second]);
	return CalcUsageConditions(cond1, cond2);
}
bool ActorInfo::CanUseFortify(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseFortify]");
	auto itr = citems->fortifyset.find(item);
	if (itr == citems->fortifyset.end() || itr->second < 0 || itr->second > citems->fortify.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->fortify[itr->second]);
	uint64_t cond2 = std::get<4>(citems->fortify[itr->second]);
	return CalcUsageConditions(cond1, cond2);
}
bool ActorInfo::CanUseFood(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseFood]");
	auto itr = citems->foodset.find(item);
	if (itr == citems->foodset.end() || itr->second < 0 || itr->second > citems->food.size())
		return false;
	uint64_t cond1 = std::get<3>(citems->food[itr->second]);
	uint64_t cond2 = std::get<4>(citems->food[itr->second]);
	return CalcUsageConditions(cond1, cond2);
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
bool ActorInfo::IsCustomPotion(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomPotion]");
	auto itr = citems->potionsset.find(item->GetFormID());
	if (itr != citems->potionsset.end())
		return true;
	itr = citems->fortifyset.find(item->GetFormID());
	if (itr != citems->fortifyset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomPoison(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomPoison]");
	auto itr = citems->poisonsset.find(item->GetFormID());
	if (itr != citems->poisonsset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomFood(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomFood]");
	auto itr = citems->foodset.find(item->GetFormID());
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

bool ActorInfo::CalcUsageConditions(uint64_t conditionsall, uint64_t conditionsany)
{
	LOG_3("{}[ActorInfo] [CalcUsageConditions]");
	// obligatory conditions (all must be fulfilled)
	// only if there are conditions
	if ((conditionsall & CV(CustomItemConditionsAll::kAllUsage)) != 0) {
		if (conditionsall & CV(CustomItemConditionsAll::kIsBoss)) {
			LOG_3("{}[ActorInfo] [CalcUsageConditions] [kIsBossAll]");
			if (!_boss)  // not a boss
				return false;
		}
		if (conditionsall & CV(CustomItemConditionsAll::kHealthThreshold)) {
			LOG_3("{}[ActorInfo] [CalcUsageConditions] [kHTAll]");
			if (ACM::GetAVPercentage(actor, RE::ActorValue::kHealth) > Settings::_healthThreshold)  // over health threshold
				return false;
		}
		if (conditionsall & CV(CustomItemConditionsAll::kMagickaThreshold)) {
			LOG_3("{}[ActorInfo] [CalcUsageConditions] [kMTAll]");
			if (ACM::GetAVPercentage(actor, RE::ActorValue::kMagicka) > Settings::_magickaThreshold)  // over magicka threshold
				return false;
		}
		if (conditionsall & CV(CustomItemConditionsAll::kStaminaThreshold)) {
			LOG_3("{}[ActorInfo] [CalcUsageConditions] [kSTAll]");
			if (ACM::GetAVPercentage(actor, RE::ActorValue::kStamina) > Settings::_staminaThreshold)  // over stamina threshold
				return false;
		}
		if (conditionsall & CV(CustomItemConditionsAll::kActorTypeDwarven)) {
			LOG_3("{}[ActorInfo] [CalcUsageConditions] [kActAutAll]");
			if (!_automaton)  // not an automaton
				return false;
		}
	}

	// any conditions (one must be fulfilled)
	if ((conditionsany & CV(CustomItemConditionsAny::kAllUsage)) == 0) {
		// no conditions at all
		return true;
	}
	if (conditionsany & CV(CustomItemConditionsAny::kIsBoss)) {
		LOG_3("{}[ActorInfo] [CalcUsageConditions] [kIsBossAny]");
		if (_boss)
			return true;
	}
	if (conditionsany & CV(CustomItemConditionsAll::kHealthThreshold)) {
		LOG_3("{}[ActorInfo] [CalcUsageConditions] [kHTAny]");
		if (ACM::GetAVPercentage(actor, RE::ActorValue::kHealth) <= Settings::_healthThreshold)  // under health threshold
			return true;
	}
	if (conditionsany & CV(CustomItemConditionsAll::kMagickaThreshold)) {
		LOG_3("{}[ActorInfo] [CalcUsageConditions] [kMTAny]");
		if (ACM::GetAVPercentage(actor, RE::ActorValue::kMagicka) <= Settings::_magickaThreshold)  // under magicka threshold
			return true;
	}
	if (conditionsany & CV(CustomItemConditionsAll::kStaminaThreshold)) {
		LOG_3("{}[ActorInfo] [CalcUsageConditions] [kSTAny]");
		if (ACM::GetAVPercentage(actor, RE::ActorValue::kStamina) <= Settings::_staminaThreshold)  // under stamina threshold
			return true;
	}
	if (conditionsall & CV(CustomItemConditionsAll::kActorTypeDwarven)) {
		LOG_3("{}[ActorInfo] [CalcUsageConditions] [kActAutAny]");
		if (_automaton)  // an automaton
			return true;
	}
	return false;
}

bool ActorInfo::CalcDistrConditions(uint64_t conditionsall, uint64_t conditionsany)
{
	LOG_3("{}[ActorInfo] [CalcDistrConditions]");
	// only check these if there are conditions
	if ((conditionsall & CV(CustomItemConditionsAny::kAllDistr)) != 0) {
		if (conditionsall & CV(CustomItemConditionsAll::kIsBoss)) {
			LOG_3("{}[ActorInfo] [CalcDistrConditions] [kIsBossAll]");
			if (!_boss)
				return false;
		}
	}

	if ((conditionsany & CV(CustomItemConditionsAny::kAllDistr)) == 0) {
		// no conditions at all
		return true;
	}
	if (conditionsany & CV(CustomItemConditionsAny::kIsBoss)) {
		LOG_3("{}[ActorInfo] [CalcDistrConditions] [kIsBossAny]");
		if (_boss)
			return true;
	}
	return false;
}

bool ActorInfo::IsFollower()
{
	return actor->IsInFaction(Settings::CurrentFollowerFaction);
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
	size += Buffer::CalcStringLength(pluginname);
	// durHealth, durMagicka, durStamina, durFortify, durRegeneration
	//size += 20;
	// nextFoodTime
	//size += 4;
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

bool ActorInfo::WriteData(unsigned char* buffer, int offset)
{
	int addoff = 0;
	// version
	Buffer::Write(version, buffer, offset);
	// actor id
	Buffer::Write(formid, buffer, offset);
	// pluginname
	Buffer::Write(pluginname, buffer, offset);
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
	return true;
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

				formid = Buffer::ReadUInt32(buffer, offset);
				pluginname = Buffer::ReadString(buffer, offset);
				RE::TESForm* form = Utility::GetTESForm(RE::TESDataHandler::GetSingleton(), formid, pluginname);
				if (!form)
					return false;
				actor = form->As<RE::Actor>();
				if (!actor)
					return false;
				name = actor->GetName();
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
