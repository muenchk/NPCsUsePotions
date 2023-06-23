#include <exception>

#include "ActorInfo.h"
#include "Settings.h"
#include "Utility.h"
#include "Distribution.h"
#include "BufferOperations.h"
#include "ActorManipulation.h"
#include "Data.h"


void ActorInfo::Init()
{
	playerRef = RE::PlayerCharacter::GetSingleton();
}

ActorInfo::ActorInfo(RE::Actor* _actor)
{
	LOG_3("{}[ActorInfo] [ActorInfo]");
	actor = _actor->GetHandle();
	if (_actor) {
		formid.SetID(_actor->GetFormID());
		// get original id
		if (const auto extraLvlCreature = _actor->extraList.GetByType<RE::ExtraLeveledCreature>()) {
			if (const auto originalBase = extraLvlCreature->originalBase) {
				formid.SetOriginalID(originalBase->GetFormID());
			}
			if (const auto templateBase = extraLvlCreature->templateBase) {
				formid.AddTemplateID(templateBase->GetFormID());
			}
		} else {
			formid.SetOriginalID(_actor->GetActorBase()->GetFormID());
		}
		name = std::string(_actor->GetName());
		pluginname = Utility::Mods::GetPluginName(_actor);
		pluginID = Utility::Mods::GetPluginIndex(pluginname);
		// if there is no plugin ID, it means that npc is temporary, so base it off of the base npc
		if (pluginID == 0x1) {
			pluginID = Utility::ExtractTemplateInfo(_actor->GetActorBase()).pluginID;
		}
		if (_actor->HasKeyword(Settings::ActorTypeDwarven) || _actor->GetRace()->HasKeyword(Settings::ActorTypeDwarven))
			_automaton = true;
		if (_actor->HasKeyword(Settings::Vampire) || _actor->GetRace()->HasKeyword(Settings::Vampire))
			_vampire = true;
		for (auto slot : _actor->GetRace()->equipSlots) {
			if (slot->GetFormID() == 0x13F43) // LeftHand
				_haslefthand = true;
		}
		_formstring = Utility::PrintForm(this);
		// Run since [actor] is valid
		UpdateMetrics(_actor);
		// set to valid
		valid = true;
		timestamp_invalid = 0;
		dead = false;
	}
}

void ActorInfo::Reset(RE::Actor* _actor)
{
	LOG_1("{}[ActorInfo] [Reset]");
	if (blockReset) {
		LOG_1("{}[ActorInfo] [Reset] Reset has been blocked.");
		return;
	}
	aclock;
	actor = _actor->GetHandle();
	durHealth = 0;
	durMagicka = 0;
	durStamina = 0;
	durFortify = 0;
	durRegeneration = 0;
	globalCooldownTimer = 0;
	citems.Reset();
	formid = ID();
	pluginname = "";
	pluginID = 1;
	name = "";
	nextFoodTime = 0;
	lastDistrTime = 0;
	durCombat = 0;
	_distributedCustomItems = 0;
	_boss = false;
	_automaton = false;
	Animation_busy = false;
	whitelisted = false;
	whitelistedcalculated = false;
	combatstate = CombatState::OutOfCombat;
	combatdata = 0;
	tcombatdata = 0;
	target = std::weak_ptr<ActorInfo>{};
	handleactor = false;
	if (_actor) {
		formid.SetID(_actor->GetFormID());
		// get original id
		if (const auto extraLvlCreature = _actor->extraList.GetByType<RE::ExtraLeveledCreature>()) {
			if (const auto originalBase = extraLvlCreature->originalBase) {
				formid.SetOriginalID(originalBase->GetFormID());
			}
			if (const auto templateBase = extraLvlCreature->templateBase) {
				formid.AddTemplateID(templateBase->GetFormID());
			}
		} else {
			formid.SetOriginalID(_actor->GetActorBase()->GetFormID());
		}
		name = std::string(_actor->GetName());
		pluginname = Utility::Mods::GetPluginName(_actor);
		pluginID = Utility::Mods::GetPluginIndex(pluginname);
		// if there is no plugin ID, it means that npc is temporary, so base it off of the base npc
		if (pluginID == 0x1) {
			pluginID = Utility::ExtractTemplateInfo(_actor->GetActorBase()).pluginID;
		}
		if (_actor->HasKeyword(Settings::ActorTypeDwarven) || _actor->GetRace()->HasKeyword(Settings::ActorTypeDwarven))
			_automaton = true;
		for (auto slot : _actor->GetRace()->equipSlots) {
			if (slot->GetFormID() == 0x13F43)  // LeftHand
				_haslefthand = true;
		}
		_formstring = Utility::PrintForm(this);
		// Run since [actor] is valid
		UpdateMetrics(_actor);
		// set to valid
		valid = true;
		timestamp_invalid = 0;
		dead = false;
	}
}

ActorInfo::ActorInfo(bool blockedReset)
{
	blockReset = blockedReset;
}

ActorInfo::ActorInfo()
{

}

bool ActorInfo::IsValid()
{
	aclock;
	return valid && actor.get() && actor.get().get();
}

void ActorInfo::SetValid()
{
	aclock;
	valid = true;
	timestamp_invalid = 0;
}

#define CurrentMilliseconds std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()

void ActorInfo::SetInvalid()
{
	aclock;
	valid = false;
	timestamp_invalid = CurrentMilliseconds;
}

bool ActorInfo::IsExpired()
{
	aclock;
	// if object was invalidated more than 10 seconds ago, it is most likely not needed anymore
	if (valid == false && (CurrentMilliseconds - timestamp_invalid) > 10000)
	{
		return true;
	}
	return false;
}

void ActorInfo::SetDead()
{
	aclock;
	dead = true;
}

bool ActorInfo::GetDead()
{
	aclock;
	return dead;
}

RE::Actor* ActorInfo::GetActor()
{
	aclock;
	if (!valid || dead)
		return nullptr;

	if (actor.get() && actor.get().get())
		return actor.get().get();
	return nullptr;
}

RE::ActorHandle ActorInfo::GetHandle()
{
	aclock;
	if (!valid || dead)
		return RE::ActorHandle();

	if (actor.get().get())
		return actor;
	return RE::ActorHandle();
}

RE::FormID ActorInfo::GetFormID()
{
	aclock;
	if (!valid)
		return 0;
	return formid;
}

RE::FormID ActorInfo::GetFormIDBlank()
{
	return formid;
}

RE::FormID ActorInfo::GetFormIDOriginal()
{
	aclock;
	if (!valid)
		return 0;
	return formid.GetOriginalID();
}

std::vector<RE::FormID> ActorInfo::GetTemplateIDs()
{
	aclock;
	if (!valid)
		return {};
	return formid.GetTemplateIDs();
}

std::string ActorInfo::GetPluginname()
{
	aclock;
	if (!valid)
		return "";
	return pluginname;
}

uint32_t ActorInfo::GetPluginID()
{
	aclock;
	if (!valid)
		return 0x1;
	return pluginID;
}

std::string ActorInfo::GetName()
{
	aclock;
	if (!valid)
		return "";
	return name;
}

std::string ActorInfo::ToString()
{
	aclock;
	if (!valid || !actor.get() || !actor.get().get())
		return "Invalid Actor Info";
	return "actor addr: " + Utility::GetHex(reinterpret_cast<std::uintptr_t>(actor.get().get())) + "\tactor:" + Utility::PrintForm(actor.get().get());
}

void ActorInfo::UpdateMetrics(RE::Actor* reac)
{
	playerDistance = reac->GetPosition().GetSquaredDistance(playerPosition);
	playerHostile = reac->IsHostileToActor(playerRef);
}

std::vector<CustomItemAlch*> ActorInfo::FilterCustomConditionsDistr(std::vector<CustomItemAlch*> itms)
{
	LOG_3("{}[ActorInfo] [FilterCustomConditionsDistr]");
	aclock;
	std::vector<CustomItemAlch*> dist;
	if (!valid)
		return dist;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0)
			continue;
		bool val = CalcDistrConditionsIntern(itms[i]);
		if (val == true)
			dist.push_back(itms[i]);
	}
	return dist;
}

bool ActorInfo::CheckCustomConditionsDistr(std::vector<CustomItemAlch*> itms)
{
	LOG_3("{}[ActorInfo] [CheckCustomConditionsDistr]");
	aclock;
	if (!valid)
		return false;
	bool distributable = false;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0)
			continue;
		bool val = CalcDistrConditionsIntern(itms[i]);
		if (val == true)
			distributable |= val;
	}
	return distributable;
}

std::vector<CustomItemAlch*> ActorInfo::FilterCustomConditionsUsage(std::vector<CustomItemAlch*> itms)
{
	LOG_3("{}[ActorInfo] [FilterCustomConditionsDistr]");
	aclock;
	std::vector<CustomItemAlch*> dist;
	if (!valid)
		return dist;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0) {
			LOG_3("{}[ActorInfo] [FilterCustomConditionsDistr] error");
			continue;
		}
		bool val = CalcUsageConditionsIntern(itms[i]);
		if (val == true)
			dist.push_back(itms[i]);
	}
	return dist;
}

std::vector<CustomItem*> ActorInfo::FilterCustomConditionsDistrItems(std::vector<CustomItem*> itms)
{
	LOG_3("{}[ActorInfo] [FilterCustomConditionsDistrItems]");
	aclock;
	std::vector<CustomItem*> dist;
	if (!valid)
		return dist;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0) {
			LOG_3("{}[ActorInfo] [FilterCustomConditionsDistrItems] error");
			continue;
		}
		bool val = CalcDistrConditionsIntern(itms[i]);
		if (val == true)
			dist.push_back(itms[i]);
	}
	return dist;
}

bool ActorInfo::CheckCustomConditionsDistrItems(std::vector<CustomItem*> itms)
{
	LOG_3("{}[ActorInfo] [CheckCustomConditionsDistrItems]");
	aclock;
	if (!valid)
		return false;
	bool distributable = false;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0) {
			continue;
		}
		bool val = CalcDistrConditionsIntern(itms[i]);
		if (val == true)
			distributable |= val;
	}
	return distributable;
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
	aclock;
	if (!valid)
		return false;
	auto itr = citems.potionsset.find(item);
	if (itr == citems.potionsset.end() || itr->second < 0 || itr->second > citems.potions.size())
		return false;
	return CalcUsageConditionsIntern(citems.potions[itr->second]);
}
bool ActorInfo::CanUsePoison(RE::FormID item) 
{
	LOG_3("{}[ActorInfo] [CanUsePoison]");
	aclock;
	if (!valid)
		return false;
	auto itr = citems.poisonsset.find(item);
	if (itr == citems.poisonsset.end() || itr->second < 0 || itr->second > citems.poisons.size())
		return false;
	return CalcUsageConditionsIntern(citems.poisons[itr->second]);
}
bool ActorInfo::CanUseFortify(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseFortify]");
	aclock;
	if (!valid)
		return false;
	auto itr = citems.fortifyset.find(item);
	if (itr == citems.fortifyset.end() || itr->second < 0 || itr->second > citems.fortify.size())
		return false;
	return CalcUsageConditionsIntern(citems.fortify[itr->second]);
}
bool ActorInfo::CanUseFood(RE::FormID item)
{
	LOG_3("{}[ActorInfo] [CanUseFood]");
	aclock;
	if (!valid)
		return false;
	auto itr = citems.foodset.find(item);
	if (itr == citems.foodset.end() || itr->second < 0 || itr->second > citems.food.size())
		return false;
	return CalcUsageConditionsIntern(citems.food[itr->second]);
}

bool ActorInfo::IsCustomAlchItem(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomAlchItem]");
	auto itr = citems.potionsset.find(item->GetFormID());
	if (itr != citems.potionsset.end())
		return true;
	itr = citems.poisonsset.find(item->GetFormID());
	if (itr != citems.poisonsset.end())
		return true;
	itr = citems.fortifyset.find(item->GetFormID());
	if (itr != citems.fortifyset.end())
		return true;
	itr = citems.foodset.find(item->GetFormID());
	if (itr != citems.foodset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomPotion(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomPotion]");
	auto itr = citems.potionsset.find(item->GetFormID());
	if (itr != citems.potionsset.end())
		return true;
	itr = citems.fortifyset.find(item->GetFormID());
	if (itr != citems.fortifyset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomPoison(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomPoison]");
	auto itr = citems.poisonsset.find(item->GetFormID());
	if (itr != citems.poisonsset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomFood(RE::AlchemyItem* item)
{
	LOG_3("{}[ActorInfo] [IsCustomFood]");
	auto itr = citems.foodset.find(item->GetFormID());
	if (itr != citems.foodset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomItem(RE::TESBoundObject* item)
{
	LOG_3("{}[ActorInfo] [IsCustomItem]");
	auto itr = citems.itemsset.find(item->GetFormID());
	if (itr != citems.itemsset.end())
		return true;
	return false;
}

bool ActorInfo::CalcUsageConditions(CustomItem* item)
{
	aclock;
	if (!valid)
		return false;
	return CalcUsageConditionsIntern(item);
}

bool ActorInfo::CalcUsageConditionsIntern(CustomItem* item)
{
	LOG_3("{}[ActorInfo] [CalcUsageConditions]");
	if (actor.get() && actor.get().get()) {
		RE::Actor* reac = actor.get().get();
		// obligatory conditions (all must be fulfilled)
		// only if there are conditions
		for (int i = 0; i < item->conditionsall.size(); i++) {
			switch (std::get<0>(item->conditionsall[i])) {
			case CustomItemConditionsAll::kIsBoss:
				if (!_boss)
					return false;
				break;
			case CustomItemConditionsAll::kActorTypeDwarven:
				if (!_automaton)
					return false;
				break;
			case CustomItemConditionsAll::kHealthThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kHealth) > Settings::Potions::_healthThreshold)  // over plugin health threshold
					return false;
				break;
			case CustomItemConditionsAll::kMagickaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kMagicka) > Settings::Potions::_magickaThreshold)  // over plugin magicka threshold
					return false;
				break;
			case CustomItemConditionsAll::kStaminaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kStamina) > Settings::Potions::_staminaThreshold)  // over plugin stamina threshold
					return false;
				break;
			case CustomItemConditionsAll::kHasMagicEffect:
				{
					auto tmp = Data::GetSingleton()->FindMagicEffect(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr || reac->AsMagicTarget()->HasMagicEffect(tmp) == false)
						return false;
				}
				break;
			case CustomItemConditionsAll::kHasPerk:
				{
					auto tmp = Data::GetSingleton()->FindPerk(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr || reac->HasPerk(tmp) == false)
						return false;
				}
				break;
			case CustomItemConditionsAll::kHasKeyword:
				{
					auto tmp = Data::GetSingleton()->FindForm(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr)
						return false;
					RE::BGSKeyword* kwd = tmp->As<RE::BGSKeyword>();
					if (kwd == nullptr || (reac->HasKeyword(kwd) == false && reac->GetRace()->HasKeyword(kwd) == false))
						return false;
				}
				break;
			case CustomItemConditionsAll::kNoCustomObjectUsage:
				return false;
			}
		}

		if (item->conditionsany.size() == 0)
			return true;

		for (int i = 0; i < item->conditionsany.size(); i++) {
			switch (std::get<0>(item->conditionsany[i])) {
			case CustomItemConditionsAny::kIsBoss:
				if (_boss)
					return true;
				break;
			case CustomItemConditionsAny::kActorTypeDwarven:
				if (_automaton)
					return true;
				break;
			case CustomItemConditionsAny::kHealthThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kHealth) <= Settings::Potions::_healthThreshold)  // under health threshold
					return true;
				break;
			case CustomItemConditionsAny::kMagickaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kMagicka) <= Settings::Potions::_magickaThreshold)  // under magicka threshold
					return true;
				break;
			case CustomItemConditionsAny::kStaminaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kStamina) <= Settings::Potions::_staminaThreshold)  // under stamina threshold
					return true;
				break;
			case CustomItemConditionsAny::kHasMagicEffect:
				{
					auto tmp = Data::GetSingleton()->FindMagicEffect(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp != nullptr && reac->AsMagicTarget()->HasMagicEffect(tmp) == true)
						return true;
				}
				break;
			case CustomItemConditionsAny::kHasPerk:
				{
					auto tmp = Data::GetSingleton()->FindPerk(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp != nullptr && reac->HasPerk(tmp) == true)
						return true;
				}
				break;
			case CustomItemConditionsAny::kHasKeyword:
				{
					auto tmp = Data::GetSingleton()->FindForm(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp != nullptr) {
						RE::BGSKeyword* kwd = tmp->As<RE::BGSKeyword>();
						if (kwd != nullptr && (reac->HasKeyword(kwd) || reac->GetRace()->HasKeyword(kwd)))
							return true;
					}
				}
				break;
			}
		}
	}
	return false;
}

bool ActorInfo::CalcDistrConditions(CustomItem* item)
{
	aclock;
	if (!valid)
		return false;
	return CalcDistrConditionsIntern(item);
}

bool ActorInfo::CalcDistrConditionsIntern(CustomItem* item)
{
	LOG_3("{}[ActorInfo] [CalcDistrConditions]");
	if (actor.get() && actor.get().get()) {
		RE::Actor* reac = actor.get().get();
		// only check these if there are conditions
		for (int i = 0; i < item->conditionsall.size(); i++)
		{
			switch (std::get<0>(item->conditionsall[i])) {
			case CustomItemConditionsAll::kActorTypeDwarven:
				if (!_automaton)
					return false;
				break;
			case CustomItemConditionsAll::kIsBoss:
				if (!_boss)
					return false;
				break;
			case CustomItemConditionsAll::kHasMagicEffect:
				{
					auto tmp = Data::GetSingleton()->FindMagicEffect(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr || reac->AsMagicTarget()->HasMagicEffect(tmp) == false)
						return false;
				}
				break;
			case CustomItemConditionsAll::kHasPerk:
				{
					auto tmp = Data::GetSingleton()->FindPerk(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr || reac->HasPerk(tmp) == false)
						return false;
				}
				break;
			case CustomItemConditionsAll::kHasKeyword:
				{
					auto tmp = Data::GetSingleton()->FindForm(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr)
						return false;
					RE::BGSKeyword* kwd = tmp->As<RE::BGSKeyword>();
					if (kwd == nullptr || (reac->HasKeyword(kwd) == false && reac->GetRace()->HasKeyword(kwd) == false))
						return false;
				}
				break;
			case CustomItemConditionsAll::kIsGhost:
				{
					if (reac->IsGhost() == false)
						return false;
				}
				break;
			case CustomItemConditionsAll::kActorStrengthEq:
				{
					if (static_cast<uint32_t>(this->actorStrength) != std::get<1>(item->conditionsall[i]))
						return false;
				}
				break;
			case CustomItemConditionsAll::kActorStrengthLesserEq:
				{
					if (static_cast<uint32_t>(this->actorStrength) > std::get<1>(item->conditionsall[i]))
						return false;
				}
				break;
			case CustomItemConditionsAll::kActorStrengthGreaterEq:
				{
					if (static_cast<uint32_t>(this->actorStrength) < std::get<1>(item->conditionsall[i]))
						return false;
				}
				break;
			case CustomItemConditionsAll::kIsInFaction:
				{
					auto tmp = Data::GetSingleton()->FindForm(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp == nullptr)
						return false;
					RE::TESFaction* fac = tmp->As<RE::TESFaction>();
					if (fac == nullptr || reac->IsInFaction(fac) == false)
						return false;
				}
				break;
			}
		}

		// no conditions at all
		if (item->conditionsany.size() == 0)
			return true;

		for (int i = 0; i < item->conditionsany.size(); i++) {
			switch (std::get<0>(item->conditionsany[i])) {
			case CustomItemConditionsAny::kActorTypeDwarven:
				if (_automaton)
					return true;
				break;
			case CustomItemConditionsAny::kIsBoss:
				if (_boss)
					return true;
				break;
			case CustomItemConditionsAny::kHasMagicEffect:
				{
					auto tmp = Data::GetSingleton()->FindMagicEffect(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp != nullptr && reac->AsMagicTarget()->HasMagicEffect(tmp) == true)
						return true;
				}
				break;
			case CustomItemConditionsAny::kHasPerk:
				{
					auto tmp = Data::GetSingleton()->FindPerk(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp != nullptr && reac->HasPerk(tmp) == true)
						return true;
				}
				break;
			case CustomItemConditionsAny::kHasKeyword:
				{
					auto tmp = Data::GetSingleton()->FindForm(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					RE::BGSKeyword* kwd = tmp->As<RE::BGSKeyword>();
					if (kwd != nullptr && (reac->HasKeyword(kwd) || reac->GetRace()->HasKeyword(kwd)))
						return true;
				}
				break;
			case CustomItemConditionsAny::kIsGhost:
				{
					if (reac->IsGhost())
						return true;
				}
			case CustomItemConditionsAny::kActorStrengthEq:
				{
					if (static_cast<uint32_t>(this->actorStrength) == std::get<1>(item->conditionsall[i]))
						return true;
				}
			case CustomItemConditionsAny::kActorStrengthLesserEq:
				{
					if (static_cast<uint32_t>(this->actorStrength) <= std::get<1>(item->conditionsall[i]))
						return true;
				}
			case CustomItemConditionsAny::kActorStrengthGreaterEq:
				{
					if (static_cast<uint32_t>(this->actorStrength) >= std::get<1>(item->conditionsall[i]))
						return true;
				}
				break;
			case CustomItemConditionsAll::kIsInFaction:
				{
					auto tmp = Data::GetSingleton()->FindForm(std::get<1>(item->conditionsall[i]), std::get<2>(item->conditionsall[i]));
					if (tmp != nullptr) {
						RE::TESFaction* fac = tmp->As<RE::TESFaction>();
						if (fac == nullptr || reac->IsInFaction(fac))
							return true;
					}
				}
				break;
			}
		}
	}
	return false;
}

bool ActorInfo::IsInCombat()
{
	aclock;
	if (!valid || dead)
		return false;
	else if (combatstate == CombatState::InCombat || combatstate == CombatState::Searching)
		return true;
	else // combatstate == CombatState::OutOfCombat
		return false;
}

CombatState ActorInfo::GetCombatState()
{
	aclock;
	if (!valid || dead)
		return CombatState::OutOfCombat;
	return combatstate;
}

void ActorInfo::SetCombatState(CombatState state)
{
	aclock;
	if (!valid || dead)
		combatstate = CombatState::OutOfCombat;
	combatstate = state;
}

bool ActorInfo::IsWeaponDrawn()
{
	aclock;
	if (!valid || dead)
		return false;
	return weaponsDrawn;
}

void ActorInfo::CustomItems::CreateMaps()
{
	LOG_3("{}[ActorInfo] [CreatMaps]");
	// items map
	itemsset.clear();
	for (int i = 0; i < items.size(); i++) {
		itemsset.insert_or_assign(items[i]->object->GetFormID(), i);
	}
	// death map
	deathset.clear();
	for (int i = 0; i < death.size(); i++) {
		deathset.insert_or_assign(death[i]->object->GetFormID(), i);
	}
	// potions map
	potionsset.clear();
	for (int i = 0; i < potions.size(); i++) {
		potionsset.insert_or_assign(potions[i]->object->GetFormID(), i);
	}
	// poisons map
	poisonsset.clear();
	for (int i = 0; i < poisons.size(); i++) {
		poisonsset.insert_or_assign(poisons[i]->object->GetFormID(), i);
	}
	// food map
	foodset.clear();
	for (int i = 0; i < food.size(); i++) {
		foodset.insert_or_assign(food[i]->object->GetFormID(), i);
	}
	// fortify map
	fortifyset.clear();
	for (int i = 0; i < fortify.size(); i++) {
		fortifyset.insert_or_assign(fortify[i]->object->GetFormID(), i);
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
	// durCombat
	//size += 4;
	// distributedCustomItems
	//size += 1;
	// actorStrength, itemStrength -> int
	//size += 8;
	// _boss
	//size += 1;
	// Animation_busy
	//size += 1;
	// globalCooldownTimer;
	//size += 4;
	// valid
	//size += 1;
	// combatstate
	//size += 4;
	// haslefthand
	//size = 1;

	// all except string are constant:
	size += 61;
	return size;
}

int32_t ActorInfo::GetMinDataSize(int32_t vers)
{
	switch (vers) {
	case 1:
		return 46;
	case 2:
		return 55;
	case 3:
		return 60;
	case 4:
		return 61;
	default:
		return 0;
	}
}

bool ActorInfo::WriteData(unsigned char* buffer, int offset)
{
	aclock;
	int addoff = 0;
	// version
	Buffer::Write(version, buffer, offset);
	// valid
	Buffer::Write(valid, buffer, offset);
	// actor id
	if ((formid & 0xFF000000) == 0xFF000000) {
		// temporary id, save whole id
		Buffer::Write(formid, buffer, offset);
	} else if ((formid & 0xFF000000) == 0xFE000000) {
		// only save index in light plugin
		Buffer::Write(formid & 0x00000FFF, buffer, offset);
	} else {
		// save index in normal plugin
		Buffer::Write(formid & 0x00FFFFFF, buffer, offset);
	}
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
	// durCombat
	Buffer::Write(durCombat, buffer, offset);
	// distributedCustomItems
	Buffer::Write(_distributedCustomItems, buffer, offset);
	// actorStrength
	Buffer::Write(static_cast<uint32_t>(actorStrength), buffer, offset);
	// itemStrength
	Buffer::Write(static_cast<uint32_t>(itemStrength), buffer, offset);
	// _boss
	Buffer::Write(_boss, buffer, offset);
	// Animation_busy
	Buffer::Write(Animation_busy, buffer, offset);
	// globalCooldownTimer
	Buffer::Write(globalCooldownTimer, buffer, offset);
	// combatstate
	Buffer::Write(static_cast<uint32_t>(combatstate), buffer, offset);
	// haslefthand
	Buffer::Write(_haslefthand, buffer, offset);
	return true;
}

bool ActorInfo::ReadData(unsigned char* buffer, int offset, int length)
{
	aclock;
	int ver = Buffer::ReadUInt32(buffer, offset);
	try {
		switch (ver) {
		case 0x00000001:
			{
				// first try to make sure that the buffer contains all necessary data and we do not go out of bounds
				int size = GetMinDataSize(ver);
				int strsize = (int)Buffer::CalcStringLength(buffer, offset + 4);  // offset + actorid is begin of pluginname
				if (length < size + strsize)
					return false;

				formid.SetID(Buffer::ReadUInt32(buffer, offset));
				pluginname = Buffer::ReadString(buffer, offset);
				RE::TESForm* form = Utility::GetTESForm(RE::TESDataHandler::GetSingleton(), formid, pluginname);
				if (!form) {
					return false;
				}
				RE::Actor* reac = form->As<RE::Actor>();
				if (reac == nullptr) {
					return false;
				}
				actor = reac->GetHandle();
				// set formid to the full formid including plugin index
				formid.SetID(reac->GetFormID());

				name = reac->GetName();
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

				// set new variables
				Animation_busy = true;
				globalCooldownTimer = 0;
				valid = true;
				timestamp_invalid = 0;
				combatstate = CombatState::OutOfCombat;

				// init dependend stuff
				pluginID = Utility::Mods::GetPluginIndex(pluginname);
				if (pluginID == 0x1) {
					pluginID = Utility::ExtractTemplateInfo(reac->GetActorBase()).pluginID;
				}
				_formstring = Utility::PrintForm(this);
				// get original id
				if (const auto extraLvlCreature = reac->extraList.GetByType<RE::ExtraLeveledCreature>()) {
					if (const auto originalBase = extraLvlCreature->originalBase) {
						formid.SetOriginalID(originalBase->GetFormID());
					}
					if (const auto templateBase = extraLvlCreature->templateBase) {
						formid.AddTemplateID(templateBase->GetFormID());
					}
				} else {
					formid.SetOriginalID(reac->GetActorBase()->GetFormID());
				}
			}
			return true;
		case 0x00000002:
			{
				// first try to make sure that the buffer contains all necessary data and we do not go out of bounds
				int size = GetMinDataSize(ver);
				int strsize = (int)Buffer::CalcStringLength(buffer, offset + 4);  // offset + actorid is begin of pluginname
				if (length < size + strsize)
					return false;

				formid.SetID(Buffer::ReadUInt32(buffer, offset));
				pluginname = Buffer::ReadString(buffer, offset);
				RE::TESForm* form = Utility::GetTESForm(RE::TESDataHandler::GetSingleton(), formid, pluginname);
				if (!form) {
					form = RE::TESForm::LookupByID(formid);
					if (!form) {
						return false;
					}
				}
				RE::Actor* reac = form->As<RE::Actor>();
				if (reac == nullptr) {
					return false;
				}
				actor = reac->GetHandle();
				// set formid to the full formid including plugin index
				formid.SetID(reac->GetFormID());

				name = reac->GetName();
				durHealth = Buffer::ReadInt32(buffer, offset);
				durMagicka = Buffer::ReadInt32(buffer, offset);
				durStamina = Buffer::ReadInt32(buffer, offset);
				durFortify = Buffer::ReadInt32(buffer, offset);
				durRegeneration = Buffer::ReadInt32(buffer, offset);
				nextFoodTime = Buffer::ReadFloat(buffer, offset);
				lastDistrTime = Buffer::ReadFloat(buffer, offset);
				durCombat = Buffer::ReadInt32(buffer, offset);
				_distributedCustomItems = Buffer::ReadBool(buffer, offset);
				actorStrength = static_cast<ActorStrength>(Buffer::ReadUInt32(buffer, offset));
				itemStrength = static_cast<ItemStrength>(Buffer::ReadUInt32(buffer, offset));
				_boss = Buffer::ReadBool(buffer, offset);
				Animation_busy = Buffer::ReadBool(buffer, offset);
				globalCooldownTimer = Buffer::ReadInt32(buffer, offset);

				// set new variables
				valid = true;
				timestamp_invalid = 0;
				combatstate = CombatState::OutOfCombat;

				// init dependend stuff
				pluginID = Utility::Mods::GetPluginIndex(pluginname);
				if (pluginID == 0x1) {
					pluginID = Utility::ExtractTemplateInfo(reac->GetActorBase()).pluginID;
				}
				_formstring = Utility::PrintForm(this);
				// get original id
				if (const auto extraLvlCreature = reac->extraList.GetByType<RE::ExtraLeveledCreature>()) {
					if (const auto originalBase = extraLvlCreature->originalBase) {
						formid.SetOriginalID(originalBase->GetFormID());
					}
					if (const auto templateBase = extraLvlCreature->templateBase) {
						formid.AddTemplateID(templateBase->GetFormID());
					}
				} else {
					formid.SetOriginalID(reac->GetActorBase()->GetFormID());
				}
			}
			return true;
		case 0x00000003:
			{
				valid = Buffer::ReadBool(buffer, offset);

				// first try to make sure that the buffer contains all necessary data and we do not go out of bounds
				int size = GetMinDataSize(ver);
				int strsize = (int)Buffer::CalcStringLength(buffer, offset + 4);  // offset + actorid is begin of pluginname
				if (length < size + strsize)
					return false;

				formid.SetID(Buffer::ReadUInt32(buffer, offset));
				pluginname = Buffer::ReadString(buffer, offset);
				// if the actorinfo is not valid, then do not evaluate the actor
				RE::TESForm* form = Utility::GetTESForm(RE::TESDataHandler::GetSingleton(), formid, pluginname);
				if (form == nullptr) {
					form = RE::TESForm::LookupByID(formid);
					if (!form) {
						return false;
					}
				}
				RE::Actor* reac = form->As<RE::Actor>();
				if (reac == nullptr) {
					return false;
				}
				actor = reac->GetHandle();
				// set formid to the full formid including plugin index
				formid.SetID(reac->GetFormID());

				name = reac->GetName();
				durHealth = Buffer::ReadInt32(buffer, offset);
				durMagicka = Buffer::ReadInt32(buffer, offset);
				durStamina = Buffer::ReadInt32(buffer, offset);
				durFortify = Buffer::ReadInt32(buffer, offset);
				durRegeneration = Buffer::ReadInt32(buffer, offset);
				nextFoodTime = Buffer::ReadFloat(buffer, offset);
				lastDistrTime = Buffer::ReadFloat(buffer, offset);
				durCombat = Buffer::ReadInt32(buffer, offset);
				_distributedCustomItems = Buffer::ReadBool(buffer, offset);
				actorStrength = static_cast<ActorStrength>(Buffer::ReadUInt32(buffer, offset));
				itemStrength = static_cast<ItemStrength>(Buffer::ReadUInt32(buffer, offset));
				_boss = Buffer::ReadBool(buffer, offset);
				Animation_busy = Buffer::ReadBool(buffer, offset);
				globalCooldownTimer = Buffer::ReadInt32(buffer, offset);
				combatstate = static_cast<CombatState>(Buffer::ReadUInt32(buffer, offset));

				// init dependend stuff
				pluginID = Utility::Mods::GetPluginIndex(pluginname);
				if (pluginID == 0x1) {
					pluginID = Utility::ExtractTemplateInfo(reac->GetActorBase()).pluginID;
				}
				_formstring = Utility::PrintForm(this);
				// get original id
				if (const auto extraLvlCreature = reac->extraList.GetByType<RE::ExtraLeveledCreature>()) {
					if (const auto originalBase = extraLvlCreature->originalBase) {
						formid.SetOriginalID(originalBase->GetFormID());
					}
					if (const auto templateBase = extraLvlCreature->templateBase) {
						formid.AddTemplateID(templateBase->GetFormID());
					}
				} else {
					formid.SetOriginalID(reac->GetActorBase()->GetFormID());
				}
			}
			return true;
		case 0x00000004:
			{
				valid = Buffer::ReadBool(buffer, offset);

				// first try to make sure that the buffer contains all necessary data and we do not go out of bounds
				int size = GetMinDataSize(ver);
				int strsize = (int)Buffer::CalcStringLength(buffer, offset + 4);  // offset + actorid is begin of pluginname
				if (length < size + strsize)
					return false;

				formid.SetID(Buffer::ReadUInt32(buffer, offset));
				pluginname = Buffer::ReadString(buffer, offset);
				// if the actorinfo is not valid, then do not evaluate the actor
				RE::TESForm* form = Utility::GetTESForm(RE::TESDataHandler::GetSingleton(), formid, pluginname);
				if (form == nullptr) {
					form = RE::TESForm::LookupByID(formid);
					if (!form) {
						return false;
					}
				}
				RE::Actor* reac = form->As<RE::Actor>();
				if (reac == nullptr) {
					return false;
				}
				actor = reac->GetHandle();
				// set formid to the full formid including plugin index
				formid.SetID(reac->GetFormID());

				name = reac->GetName();
				durHealth = Buffer::ReadInt32(buffer, offset);
				durMagicka = Buffer::ReadInt32(buffer, offset);
				durStamina = Buffer::ReadInt32(buffer, offset);
				durFortify = Buffer::ReadInt32(buffer, offset);
				durRegeneration = Buffer::ReadInt32(buffer, offset);
				nextFoodTime = Buffer::ReadFloat(buffer, offset);
				lastDistrTime = Buffer::ReadFloat(buffer, offset);
				durCombat = Buffer::ReadInt32(buffer, offset);
				_distributedCustomItems = Buffer::ReadBool(buffer, offset);
				actorStrength = static_cast<ActorStrength>(Buffer::ReadUInt32(buffer, offset));
				itemStrength = static_cast<ItemStrength>(Buffer::ReadUInt32(buffer, offset));
				_boss = Buffer::ReadBool(buffer, offset);
				Animation_busy = Buffer::ReadBool(buffer, offset);
				globalCooldownTimer = Buffer::ReadInt32(buffer, offset);
				combatstate = static_cast<CombatState>(Buffer::ReadUInt32(buffer, offset));
				_haslefthand = Buffer::ReadBool(buffer, offset);

				// init dependend stuff
				pluginID = Utility::Mods::GetPluginIndex(pluginname);
				if (pluginID == 0x1) {
					pluginID = Utility::ExtractTemplateInfo(reac->GetActorBase()).pluginID;
				}
				_formstring = Utility::PrintForm(this);
				// get original id
				if (const auto extraLvlCreature = reac->extraList.GetByType<RE::ExtraLeveledCreature>()) {
					if (const auto originalBase = extraLvlCreature->originalBase) {
						formid.SetOriginalID(originalBase->GetFormID());
					}
					if (const auto templateBase = extraLvlCreature->templateBase) {
						formid.AddTemplateID(templateBase->GetFormID());
					}
				} else {
					formid.SetOriginalID(reac->GetActorBase()->GetFormID());
				}
			}
			return true;
		default:
			return false;
		}
	} catch (std::exception&) {
		return false;
	}
}

void ActorInfo::Update()
{
	aclock;
	if (!valid)
		return;
	if (RE::Actor* reac = actor.get().get(); reac != nullptr) {
		// update vampire status
		_vampire = false;
		if (reac->HasKeyword(Settings::Vampire) || reac->GetRace()->HasKeyword(Settings::Vampire))
			_vampire = true;
		// update the metrics, since we are sure our object is valid
		UpdateMetrics(reac);
	}
	else
	{
		SetInvalid();
	}
}

std::weak_ptr<ActorInfo> ActorInfo::GetTarget()
{
	aclock;
	if (!valid)
		return std::weak_ptr<ActorInfo>{};
	return target;
}

void ActorInfo::ResetTarget()
{
	aclock;
	target = std::weak_ptr<ActorInfo>{};
}

void ActorInfo::SetTarget(std::weak_ptr<ActorInfo> tar)
{
	aclock;
	if (!valid || dead)
		target = std::weak_ptr<ActorInfo>{};
	else
		target = tar;
}

short ActorInfo::GetTargetLevel()
{
	aclock;
	if (!valid || dead)
		return 1;
	if (std::shared_ptr<ActorInfo> tar = target.lock()) {
		return tar->GetLevel();
	}
	return 1;
}

uint32_t ActorInfo::GetCombatData()
{
	aclock;
	if (!valid || dead)
		return 0;
	return combatdata;
}

void ActorInfo::SetCombatData(uint32_t data)
{
	aclock;
	if (!valid || dead)
		combatdata = 0;
	else
		combatdata = data;
}

uint32_t ActorInfo::GetCombatDataTarget()
{
	aclock;
	if (!valid || dead)
		return 0;
	return tcombatdata;
}

void ActorInfo::SetCombatDataTarget(uint32_t data)
{
	aclock;
	if (!valid || dead)
		tcombatdata = 0;
	else
		tcombatdata = data;
}

bool ActorInfo::GetHandleActor()
{
	aclock;
	if (!valid || dead)
		return false;
	return handleactor;
}

void ActorInfo::SetHandleActor(bool handle)
{
	aclock;
	if (!valid || dead)
		handleactor = false;
	else
		handleactor = handle;
}

float ActorInfo::GetPlayerDistance()
{
	aclock;
	if (!valid)
		return FLT_MAX;
	return playerDistance;
}

void ActorInfo::SetPlayerDistance(float distance)
{
	aclock;
	if (!valid || dead)
		playerDistance = FLT_MAX;
	else
		playerDistance = distance;
}

bool ActorInfo::GetPlayerHostile()
{
	aclock;
	if (!valid || dead)
		return false;
	return playerHostile;
}

void ActorInfo::SetPlayerHostile(bool hostile)
{
	aclock;
	if (!valid || dead)
		playerHostile = false;
	playerHostile = hostile;
}

bool ActorInfo::GetWeaponsDrawn()
{
	aclock;
	if (!valid || dead)
		return false;
	return weaponsDrawn;
}

void ActorInfo::SetWeaponsDrawn(bool drawn)
{
	aclock;
	if (!valid || dead)
		weaponsDrawn = false;
	else
		weaponsDrawn = drawn;
}

void ActorInfo::UpdateWeaponsDrawn()
{
	aclock;
	if (!valid || dead)
		weaponsDrawn = false;

	if (actor.get() && actor.get().get())
		weaponsDrawn = actor.get().get()->AsActorState()->IsWeaponDrawn();
}

#pragma region ActorSpecificFunctions

bool ActorInfo::IsFollower()
{
	aclock;
	if (!valid)
		return false;

	if (actor.get() && actor.get().get()) {
		RE::Actor* reac = actor.get().get();
		bool follower = reac->IsInFaction(Settings::CurrentFollowerFaction) || reac->IsInFaction(Settings::CurrentHirelingFaction);
		if (follower)
			return true;
		if (reac->GetActorBase()) {
			auto itr = reac->GetActorBase()->factions.begin();
			while (itr != reac->GetActorBase()->factions.end()) {
				if (Distribution::followerFactions()->contains(itr->faction->GetFormID()) && itr->rank >= 0)
					return true;
				itr++;
			}
		}
	}
	return false;
}

bool ActorInfo::IsPlayer()
{
	return formid == 0x14;
}

RE::TESObjectREFR::InventoryItemMap ActorInfo::GetInventory()
{
	aclock;
	if (!valid)
		return RE::TESObjectREFR::InventoryItemMap{};

	if (actor.get() && actor.get().get())
		return actor.get().get()->GetInventory();
	return RE::TESObjectREFR::InventoryItemMap{};
}

RE::TESObjectREFR::InventoryCountMap ActorInfo::GetInventoryCounts()
{
	aclock;
	if (!valid)
		return RE::TESObjectREFR::InventoryCountMap{};

	if (actor.get() && actor.get().get())
		return actor.get().get()->GetInventoryCounts();
	return RE::TESObjectREFR::InventoryCountMap{};
}

bool ActorInfo::HasMagicEffect(RE::EffectSetting* effect)
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->AsMagicTarget()->HasMagicEffect(effect);
	return false;
}

bool ActorInfo::DrinkPotion(RE::AlchemyItem* potion, RE::ExtraDataList* extralist)
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->DrinkPotion(potion, extralist);
	return false;
}

RE::InventoryEntryData* ActorInfo::GetEquippedEntryData(bool leftHand)
{
	aclock;
	if (!valid || dead)
		return nullptr;

	if (!leftHand || _haslefthand)
		if (actor.get() && actor.get().get())
			return actor.get().get()->GetEquippedEntryData(leftHand);
	return nullptr;
}

void ActorInfo::RemoveItem(RE::TESBoundObject* item, int32_t count)
{
	aclock;
	if (!valid)
		return;

	if (actor.get() && actor.get().get())
		actor.get().get()->RemoveItem(item, count, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
}

void ActorInfo::AddItem(RE::TESBoundObject* item, int32_t count)
{
	aclock;
	if (!valid)
		return;

	if (actor.get() && actor.get().get())
		actor.get().get()->AddObjectToContainer(item, nullptr, count, nullptr);
}

uint32_t ActorInfo::GetFormFlags()
{
	aclock;
	if (!valid || dead)
		return 0;

	if (actor.get() && actor.get().get())
		return actor.get().get()->formFlags;
	return 0;
}

bool ActorInfo::IsDead()
{
	aclock;
	if (!valid || dead)
		return true;

	if (actor.get() && actor.get().get())
		return actor.get().get()->IsDead();
	return true;
}

RE::TESNPC* ActorInfo::GetActorBase()
{
	aclock;
	if (!valid)
		return nullptr;

	if (actor.get() && actor.get().get())
		return actor.get().get()->GetActorBase();
	return nullptr;
}

RE::FormID ActorInfo::GetActorBaseFormID()
{
	aclock;
	if (!valid)
		return 0;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase())
			return actor.get().get()->GetActorBase()->GetFormID();
	return 0;
}

std::string ActorInfo::GetActorBaseFormEditorID()
{
	aclock;
	if (!valid)
		return "";

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase())
			return actor.get().get()->GetActorBase()->GetFormEditorID();
	return "";
}

RE::TESCombatStyle* ActorInfo::GetCombatStyle()
{
	aclock;
	if (!valid)
		return nullptr;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase())
			return actor.get().get()->GetActorBase()->GetCombatStyle();
	return nullptr;
}

RE::TESRace* ActorInfo::GetRace()
{
	aclock;
	if (!valid)
		return nullptr;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase())
			return actor.get().get()->GetActorBase()->GetRace();
	return nullptr;
}

RE::FormID ActorInfo::GetRaceFormID()
{
	aclock;
	if (!valid)
		return 0;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase() && actor.get().get()->GetActorBase()->GetRace())
			return actor.get().get()->GetActorBase()->GetRace()->GetFormID();
	return 0;
}

bool ActorInfo::IsGhost()
{
	aclock;
	if (!valid)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->IsGhost();
	return false;
}

bool ActorInfo::IsSummonable()
{
	aclock;
	if (!valid)
		return false;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase())
			return actor.get().get()->GetActorBase()->IsSummonable();
	return false;
}

bool ActorInfo::Bleeds()
{
	aclock;
	if (!valid)
		return false;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorBase())
			return actor.get().get()->GetActorBase()->Bleeds();
	return false;
}

short ActorInfo::GetLevel()
{
	aclock;
	if (!valid)
		return 1;
	if (actor.get() && actor.get().get())
		return actor.get().get()->GetLevel();
	return 1;
}

SKSE::stl::enumeration<RE::Actor::BOOL_BITS, uint32_t> ActorInfo::GetBoolBits()
{
	aclock;
	if (!valid)
		return SKSE::stl::enumeration<RE::Actor::BOOL_BITS, uint32_t>{};

	if (actor.get() && actor.get().get())
		return actor.get().get()->GetActorRuntimeData().boolBits;
	return SKSE::stl::enumeration<RE::Actor::BOOL_BITS, uint32_t>{};
}

bool ActorInfo::IsFlying()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->AsActorState()->IsFlying();
	return false;
}

bool ActorInfo::IsInKillMove()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->IsInKillMove();
	return false;
}

bool ActorInfo::IsInMidair()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->IsInMidair();
	return false;
}

bool ActorInfo::IsInRagdollState()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->IsInRagdollState();
	return false;
}

bool ActorInfo::IsUnconscious()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->AsActorState()->IsUnconscious();
	return false;
}

bool ActorInfo::IsParalyzed()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		if (actor.get().get()->GetActorRuntimeData().boolBits & RE::Actor::BOOL_BITS::kParalyzed)
			return true;
	return false;
}

bool ActorInfo::IsStaggered()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->AsActorState()->actorState2.staggered;
	return false;
}

bool ActorInfo::IsBleedingOut()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->AsActorState()->IsBleedingOut();
	return false;
}

#pragma endregion
