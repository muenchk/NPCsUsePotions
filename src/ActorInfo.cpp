#include <exception>

#include "ActorInfo.h"
#include "Settings.h"
#include "Utility.h"
#include "Distribution.h"
#include "DistributionRule.h"
#include "ActorManipulation.h"
#include "Data.h"
#include "Compatibility.h"


void ActorInfo::Init()
{
	playerRef = RE::PlayerCharacter::GetSingleton();
}

ActorInfo::ActorInfo(RE::Actor* _actor)
{
	LOG_3("");
	cooldowns = util::make_shared<CooldownData>();
	if (_actor) {
		actor = _actor->GetHandle();
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
		pluginname = Mods::GetPluginName(_actor);
		pluginID = Mods::GetPluginIndex(pluginname);
		// if there is no plugin ID, it means that npc is temporary, so base it off of the base npc
		if (pluginID == MAXUINT32) {
			pluginID = Utility::ExtractTemplateInfo(_actor->GetActorBase()).pluginID;
		}
		if (_actor->HasKeyword(Settings::ActorTypeDwarven) || _actor->GetRace()->HasKeyword(Settings::ActorTypeDwarven))
			_automaton = true;
		if (_actor->HasKeyword(Settings::Vampire) || _actor->GetRace()->HasKeyword(Settings::Vampire))
			_vampire = true;
		if (_actor->HasKeyword(Settings::ActorTypeNPC) || _actor->GetActorBase()->HasKeyword(Settings::ActorTypeNPC) || _actor->GetRace() && _actor->GetRace()->HasKeyword(Settings::ActorTypeNPC))
			_actorTypeNPC = true;
		for (auto slot : _actor->GetRace()->equipSlots) {
			if (slot->GetFormID() == 0x13F43) // LeftHand
				_haslefthand = true;
		}
		// Run since [actor] is valid
		UpdateMetrics(actor);
		// update poison resistance
		UpdatePermanentPoisonResist();
		// set to valid
		valid = true;
		_formstring = Utility::PrintFormNonDebug(this);
		timestamp_invalid = 0;
		dead = false;
	}
}

void ActorInfo::Reset(RE::Actor* _actor)
{
	LOG_1("");
	if (blockReset) {
		LOG_1("Reset has been blocked.");
		return;
	}
	aclock;
	cooldowns->durHealth = 0;
	cooldowns->durMagicka = 0;
	cooldowns->durStamina = 0;
	cooldowns->durFortify = 0;
	cooldowns->durRegeneration = 0;
	globalCooldownTimer = 0;
	citems.Reset();
	formid = ActorID();
	pluginname = "";
	pluginID = MAXUINT32;
	name = "";
	cooldowns->nextFoodTime = 0;
	cooldowns->lastDistrTime = 0;
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
	lastRuleCalcTime = std::chrono::steady_clock::time_point::min();
	_distributionRule = nullptr;
	_showWidgets = false;
	UpdateWidgets();
	if (_actor) {
		actor = _actor->GetHandle();
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
		pluginname = Mods::GetPluginName(_actor);
		pluginID = Mods::GetPluginIndex(pluginname);
		// if there is no plugin ID, it means that npc is temporary, so base it off of the base npc
		if (pluginID == MAXUINT32) {
			pluginID = Utility::ExtractTemplateInfo(_actor->GetActorBase()).pluginID;
		}
		if (_actor->HasKeyword(Settings::ActorTypeDwarven) || _actor->GetRace()->HasKeyword(Settings::ActorTypeDwarven))
			_automaton = true;
		if (_actor->HasKeyword(Settings::Vampire) || _actor->GetRace()->HasKeyword(Settings::Vampire))
			_vampire = true;
		if (_actor->HasKeyword(Settings::ActorTypeNPC) || _actor->GetActorBase()->HasKeyword(Settings::ActorTypeNPC) || _actor->GetRace() && _actor->GetRace()->HasKeyword(Settings::ActorTypeNPC))
			_actorTypeNPC = true;
		for (auto slot : _actor->GetRace()->equipSlots) {
			if (slot->GetFormID() == 0x13F43)  // LeftHand
				_haslefthand = true;
		}
		// Run since [actor] is valid
		UpdateMetrics(actor);
		// update poison resistance
		UpdatePermanentPoisonResist();
		// set to valid
		valid = true;
		_formstring = Utility::PrintForm(this);
		timestamp_invalid = 0;
		dead = false;
	}
}

ActorInfo::ActorInfo(bool blockedReset)
{
	blockReset = blockedReset;
	cooldowns = util::make_shared<CooldownData>();
}

ActorInfo::ActorInfo()
{
	cooldowns = util::make_shared<CooldownData>();
}
ActorInfo::~ActorInfo()
{
	for (int i = 0; i < handles.size(); i++) {
		try {
			if (handles[i] != nullptr)
				handles[i]->Invalidate();
		} catch (std::exception&) {}
	}

	_showWidgets = false;
	UpdateWidgets();
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

	LOG_4("Unsafe Operation {}: GetActor", name);

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
		return MAXUINT32;
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

void ActorInfo::UpdateMetrics(RE::ActorHandle handle)
{
	if (RE::Actor* reac = actor.get().get(); reac != nullptr) {
		playerDistance = reac->GetPosition().GetSquaredDistance(playerPosition);
		playerHostile = reac->IsHostileToActor(playerRef);
	}
}

std::vector<CustomItemAlch*> ActorInfo::FilterCustomConditionsDistr(std::vector<CustomItemAlch*> itms)
{
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
	aclock;
	std::vector<CustomItemAlch*> dist;
	if (!valid)
		return dist;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0) {
			LOG_3("error");
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
	LOG_3("");
	aclock;
	std::vector<CustomItem*> dist;
	if (!valid)
		return dist;
	for (int i = 0; i < itms.size(); i++) {
		if (itms[i]->object == nullptr || itms[i]->object->GetFormID() == 0) {
			LOG_3("error");
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
	LOG_3("");
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
	LOG_3("");
	return CanUsePotion(item) | CanUsePoison(item) | CanUseFortify(item) | CanUseFood(item);
}
bool ActorInfo::CanUsePot(RE::FormID item)
{
	LOG_3("");
	return CanUsePotion(item) | CanUseFortify(item);
}
bool ActorInfo::CanUsePotion(RE::FormID item)
{
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
	auto itr = citems.poisonsset.find(item->GetFormID());
	if (itr != citems.poisonsset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomFood(RE::AlchemyItem* item)
{
	LOG_3("");
	auto itr = citems.foodset.find(item->GetFormID());
	if (itr != citems.foodset.end())
		return true;
	return false;
}
bool ActorInfo::IsCustomItem(RE::TESBoundObject* item)
{
	LOG_3("");
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
	LOG_3("");
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
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kHealth) > Settings::potions._healthThreshold)  // over plugin health threshold
					return false;
				break;
			case CustomItemConditionsAll::kMagickaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kMagicka) > Settings::potions._magickaThreshold)  // over plugin magicka threshold
					return false;
				break;
			case CustomItemConditionsAll::kStaminaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kStamina) > Settings::potions._staminaThreshold)  // over plugin stamina threshold
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
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kHealth) <= Settings::potions._healthThreshold)  // under health threshold
					return true;
				break;
			case CustomItemConditionsAny::kMagickaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kMagicka) <= Settings::potions._magickaThreshold)  // under magicka threshold
					return true;
				break;
			case CustomItemConditionsAny::kStaminaThreshold:
				if (ACM::GetAVPercentage(reac, RE::ActorValue::kStamina) <= Settings::potions._staminaThreshold)  // under stamina threshold
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
	LOG_3("");
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
	LOG_3("");
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
	LOG_3("");
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

bool ActorInfo::WriteData(unsigned char* buffer, size_t offset)
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
	Buffer::Write(cooldowns->durHealth, buffer, offset);
	// durMagicka
	Buffer::Write(cooldowns->durMagicka, buffer, offset);
	// durStamina
	Buffer::Write(cooldowns->durStamina, buffer, offset);
	// durFortify
	Buffer::Write(cooldowns->durFortify, buffer, offset);
	// durRegeneration
	Buffer::Write(cooldowns->durRegeneration, buffer, offset);
	// nextFoodTime
	Buffer::Write(cooldowns->nextFoodTime, buffer, offset);
	// lastDistrTime
	Buffer::Write(cooldowns->lastDistrTime, buffer, offset);
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

bool ActorInfo::ReadData(unsigned char* buffer, size_t offset, size_t length)
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
				cooldowns->durHealth = Buffer::ReadInt32(buffer, offset);
				cooldowns->durMagicka = Buffer::ReadInt32(buffer, offset);
				cooldowns->durStamina = Buffer::ReadInt32(buffer, offset);
				cooldowns->durFortify = Buffer::ReadInt32(buffer, offset);
				cooldowns->durRegeneration = Buffer::ReadInt32(buffer, offset);
				cooldowns->nextFoodTime = Buffer::ReadFloat(buffer, offset);
				cooldowns->lastDistrTime = Buffer::ReadFloat(buffer, offset);
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
				pluginID = Mods::GetPluginIndex(pluginname);
				if (pluginID == MAXUINT32) {
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
				// update poison resitance
				UpdatePermanentPoisonResist();
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
				cooldowns->durHealth = Buffer::ReadInt32(buffer, offset);
				cooldowns->durMagicka = Buffer::ReadInt32(buffer, offset);
				cooldowns->durStamina = Buffer::ReadInt32(buffer, offset);
				cooldowns->durFortify = Buffer::ReadInt32(buffer, offset);
				cooldowns->durRegeneration = Buffer::ReadInt32(buffer, offset);
				cooldowns->nextFoodTime = Buffer::ReadFloat(buffer, offset);
				cooldowns->lastDistrTime = Buffer::ReadFloat(buffer, offset);
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
				pluginID = Mods::GetPluginIndex(pluginname);
				if (pluginID == MAXUINT32) {
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
				// update poison resitance
				UpdatePermanentPoisonResist();
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
				cooldowns->durHealth = Buffer::ReadInt32(buffer, offset);
				cooldowns->durMagicka = Buffer::ReadInt32(buffer, offset);
				cooldowns->durStamina = Buffer::ReadInt32(buffer, offset);
				cooldowns->durFortify = Buffer::ReadInt32(buffer, offset);
				cooldowns->durRegeneration = Buffer::ReadInt32(buffer, offset);
				cooldowns->nextFoodTime = Buffer::ReadFloat(buffer, offset);
				cooldowns->lastDistrTime = Buffer::ReadFloat(buffer, offset);
				durCombat = Buffer::ReadInt32(buffer, offset);
				_distributedCustomItems = Buffer::ReadBool(buffer, offset);
				actorStrength = static_cast<ActorStrength>(Buffer::ReadUInt32(buffer, offset));
				itemStrength = static_cast<ItemStrength>(Buffer::ReadUInt32(buffer, offset));
				_boss = Buffer::ReadBool(buffer, offset);
				Animation_busy = Buffer::ReadBool(buffer, offset);
				globalCooldownTimer = Buffer::ReadInt32(buffer, offset);
				combatstate = static_cast<CombatState>(Buffer::ReadUInt32(buffer, offset));

				// init dependend stuff
				pluginID = Mods::GetPluginIndex(pluginname);
				if (pluginID == MAXUINT32) {
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
				// update poison resitance
				UpdatePermanentPoisonResist();
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
				cooldowns->durHealth = Buffer::ReadInt32(buffer, offset);
				if (cooldowns->durHealth < 0)
					cooldowns->durHealth = 0;
				cooldowns->durMagicka = Buffer::ReadInt32(buffer, offset);
				if (cooldowns->durMagicka < 0)
					cooldowns->durMagicka = 0;
				cooldowns->durStamina = Buffer::ReadInt32(buffer, offset);
				if (cooldowns->durStamina < 0)
					cooldowns->durStamina = 0;
				cooldowns->durFortify = Buffer::ReadInt32(buffer, offset);
				if (cooldowns->durFortify < 0)
					cooldowns->durFortify = 0;
				cooldowns->durRegeneration = Buffer::ReadInt32(buffer, offset);
				if (cooldowns->durRegeneration < 0)
					cooldowns->durRegeneration = 0;
				cooldowns->nextFoodTime = Buffer::ReadFloat(buffer, offset);
				cooldowns->lastDistrTime = Buffer::ReadFloat(buffer, offset);
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
				pluginID = Mods::GetPluginIndex(pluginname);
				if (pluginID == MAXUINT32) {
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
				UpdateMetrics(actor);
				timestamp_invalid = 0;
				// update poison resitance
				UpdatePermanentPoisonResist();
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
		UpdateMetrics(actor);
	}
	else
	{
		SetInvalid();
	}
}

void ActorInfo::SetLastRuleCalcTime() 
{ 
	lastRuleCalcTime = std::chrono::steady_clock::now(); 
}

std::chrono::steady_clock::time_point ActorInfo::GetLastRuleCalcTime()
{
	return lastRuleCalcTime;
}

void ActorInfo::SetDistributionRule(DistributionRule* rule)
{
	_distributionRule = rule;
}

DistributionRule* ActorInfo::GetDistributionRule()
{
	return _distributionRule;
}

int ActorInfo::GetDurHealth()
{
	return cooldowns->durHealth;
}

void ActorInfo::SetDurHealth(int value)
{
	cooldowns->durHealth = value;
	cooldowns->durHealthMax = value;
}
void ActorInfo::DecDurHealth(int value)
{
	cooldowns->durHealth -= value;
}
int ActorInfo::GetDurMagicka()
{
	return cooldowns->durMagicka;
}
void ActorInfo::SetDurMagicka(int value)
{
	cooldowns->durMagicka = value;
	cooldowns->durMagickaMax = value;
}
void ActorInfo::DecDurMagicka(int value)
{
	cooldowns->durMagicka -= value;
}
int ActorInfo::GetDurStamina()
{
	return cooldowns->durStamina;
}
void ActorInfo::SetDurStamina(int value)
{
	cooldowns->durStamina = value;
	cooldowns->durStaminaMax = value;
}
void ActorInfo::DecDurStamina(int value)
{
	cooldowns->durStamina -= value;
}
int ActorInfo::GetDurFortify()
{
	return cooldowns->durFortify;
}
void ActorInfo::SetDurFortify(int value)
{
	cooldowns->durFortify = value;
	cooldowns->durFortifyMax = value;
}
void ActorInfo::DecDurFortify(int value)
{
	cooldowns->durFortify -= value;
}
int ActorInfo::GetDurRegeneration()
{
	return cooldowns->durRegeneration;
}
void ActorInfo::SetDurRegeneration(int value)
{
	cooldowns->durRegeneration = value;
	cooldowns->durRegenerationMax = value;
}
void ActorInfo::DecDurRegeneration(int value)
{
	cooldowns->durRegeneration -= value;
}
float ActorInfo::GetNextFoodTime()
{
	return cooldowns->nextFoodTime;
}
void ActorInfo::SetNextFoodTime(float value)
{
	cooldowns->nextFoodTime = value;
}
float ActorInfo::GetLastDistrTime()
{
	return cooldowns->lastDistrTime;
}
void ActorInfo::SetLastDistrTime(float value)
{
	cooldowns->lastDistrTime = value;
}

void ActorInfo::UpdateWidgets()
{
	if (!(_follower && Settings::widgets._showFollowerWidgets ||
		IsPlayer() && Settings::widgets._showPlayerWidgets ||
		!_follower && !IsPlayer() && Settings::widgets._showOtherNPCWidgets)) {
		return;
	}

	static const ImU32 _colourOverride = 0xFFFFFFFF;
	if (_showWidgets) {
		if (_widgetData == nullptr)
			_widgetData = new WidgetData;
		if (_widgetData->_widgetPanel == nullptr)
		{
			_widgetData->_widgetPanel = dynamic_pointer_cast<IWidgetPanel>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetPanel, ""));
			_widgetData->_widgetPanel->SetRows(1);
			_widgetData->_widgetPanel->SetColumns(6);
			_widgetData->_widgetPanel->SetLocationObjectAnchor(Offset{ 0, false, OffsetPosition ::Low }, Offset{ 25.f, false, OffsetPosition ::Low }, Dimension{ true, 0.1f }, Dimension{ true, 0.1f }, false, AlignmentFlags::CenterX | AlignmentFlags::Bottom, 0.5f, RE::ObjectRefHandle{ actor }, LocationObjectAnchor::ObjectAnchor::ActorHead);
			_widgetData->_widgetPanel->SetFillDirection(AlignmentFlags::Left | AlignmentFlags::Bottom);
			_widgetData->_widgetPanel->Show();
			LibImGuiUI::LibImGuiUI_APIv1::instance->RegisterWidget(_widgetData->_widgetPanel);

			_widgetData->_potionHealthWidget = dynamic_pointer_cast<ITextureWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetTexture, ""));
			_widgetData->_potionHealthWidget->SetTexturePath(Settings::icon_path + "Fallback.svg");
			_widgetData->_potionHealthWidget->SetMultiplyTextureColour(_colourOverride);
			_widgetData->_potionHealthWidget->SetDimensions({ 1.f, 1.f });
			_widgetData->_potionHealthWidget->Show();
			_widgetData->_potionHealthCooldownWidget = dynamic_pointer_cast<ICooldownWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetCooldown, ""));
			_widgetData->_potionHealthCooldownWidget->Show();
			_widgetData->_potionHealthCooldownWidget->SetCooldownStyle(Settings::widgets._widgetCooldownStyle);

			_widgetData->_potionHealthWidget->AddSubordinateWidget(_widgetData->_potionHealthCooldownWidget);

			_widgetData->_potionMagickaWidget = dynamic_pointer_cast<ITextureWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetTexture, ""));
			_widgetData->_potionMagickaWidget->SetTexturePath(Settings::icon_path + "Fallback.svg");
			_widgetData->_potionMagickaWidget->SetMultiplyTextureColour(_colourOverride);
			_widgetData->_potionMagickaWidget->SetDimensions({ 1.f, 1.f });
			_widgetData->_potionMagickaWidget->Show();
			_widgetData->_potionStaminaCooldownWidget = dynamic_pointer_cast<ICooldownWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetCooldown, ""));
			_widgetData->_potionStaminaCooldownWidget->Show();
			_widgetData->_potionStaminaCooldownWidget->SetCooldownStyle(Settings::widgets._widgetCooldownStyle);

			_widgetData->_potionMagickaWidget->AddSubordinateWidget(_widgetData->_potionStaminaCooldownWidget);

			_widgetData->_potionStaminaWidget = dynamic_pointer_cast<ITextureWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetTexture, ""));
			_widgetData->_potionStaminaWidget->SetTexturePath(Settings::icon_path + "Fallback.svg");
			_widgetData->_potionStaminaWidget->SetMultiplyTextureColour(_colourOverride);
			_widgetData->_potionStaminaWidget->SetDimensions({ 1.f, 1.f });
			_widgetData->_potionStaminaWidget->Show();
			_widgetData->_potionMagickaCooldownWidget = dynamic_pointer_cast<ICooldownWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetCooldown, ""));
			_widgetData->_potionMagickaCooldownWidget->Show();
			_widgetData->_potionMagickaCooldownWidget->SetCooldownStyle(Settings::widgets._widgetCooldownStyle);

			_widgetData->_potionStaminaWidget->AddSubordinateWidget(_widgetData->_potionMagickaCooldownWidget);

			_widgetData->_fortifyWidget = dynamic_pointer_cast<ITextureWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetTexture, ""));
			_widgetData->_fortifyWidget->SetTexturePath(Settings::icon_path + "Fallback.svg");
			_widgetData->_fortifyWidget->SetMultiplyTextureColour(_colourOverride);
			_widgetData->_fortifyWidget->SetDimensions({ 1.f, 1.f });
			_widgetData->_fortifyWidget->Show();
			_widgetData->_fortifyCooldownWidget = dynamic_pointer_cast<ICooldownWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetCooldown, ""));
			_widgetData->_fortifyCooldownWidget->Show();
			_widgetData->_fortifyCooldownWidget->SetCooldownStyle(Settings::widgets._widgetCooldownStyle);

			_widgetData->_fortifyWidget->AddSubordinateWidget(_widgetData->_fortifyCooldownWidget);

			_widgetData->_regenWidget = dynamic_pointer_cast<ITextureWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetTexture, ""));
			_widgetData->_regenWidget->SetTexturePath(Settings::icon_path + "Fallback.svg");
			_widgetData->_regenWidget->SetMultiplyTextureColour(_colourOverride);
			_widgetData->_regenWidget->SetDimensions({ 1.f, 1.f });
			_widgetData->_regenWidget->Show();
			_widgetData->_regenCooldownWidget = dynamic_pointer_cast<ICooldownWidget>(LibImGuiUI::LibImGuiUI_APIv1::instance->CreateWidget(WidgetType::IWidgetCooldown, ""));
			_widgetData->_regenCooldownWidget->Show();
			_widgetData->_regenCooldownWidget->SetCooldownStyle(Settings::widgets._widgetCooldownStyle);

			_widgetData->_regenWidget->AddSubordinateWidget(_widgetData->_regenCooldownWidget);
		} else {
			if (_widgetData->_potionHealthWidgetShown && cooldowns->durHealth <= 0) {
				_widgetData->_widgetPanel->RemoveWidget(_widgetData->_potionHealthWidget);
				_widgetData->_potionHealthWidgetShown = false;
			} else if (_widgetData->_potionHealthWidgetShown == false && cooldowns->durHealth > 0) {
				_widgetData->_widgetPanel->AddWidget(_widgetData->_potionHealthWidget);
				_widgetData->_potionHealthWidgetShown = true;
			}
			_widgetData->_potionHealthCooldownWidget->SetProgress((float)cooldowns->durHealth / (float)cooldowns->durHealthMax);
			if (_widgetData->_potionMagickaWidgetShown && cooldowns->durMagicka <= 0) {
				_widgetData->_widgetPanel->RemoveWidget(_widgetData->_potionMagickaWidget);
				_widgetData->_potionMagickaWidgetShown = false;
			} else if (_widgetData->_potionMagickaWidgetShown == false && cooldowns->durMagicka > 0) {
				_widgetData->_widgetPanel->AddWidget(_widgetData->_potionMagickaWidget);
				_widgetData->_potionMagickaWidgetShown = true;
			}
			_widgetData->_potionMagickaCooldownWidget->SetProgress((float)cooldowns->durMagicka / (float)cooldowns->durMagickaMax);
			if (_widgetData->_potionStaminaWidgetShown && cooldowns->durStamina <= 0) {
				_widgetData->_widgetPanel->RemoveWidget(_widgetData->_potionStaminaWidget);
				_widgetData->_potionStaminaWidgetShown = false;
			} else if (_widgetData->_potionStaminaWidgetShown == false && cooldowns->durStamina > 0) {
				_widgetData->_widgetPanel->AddWidget(_widgetData->_potionStaminaWidget);
				_widgetData->_potionStaminaWidgetShown = true;
			}
			_widgetData->_potionStaminaCooldownWidget->SetProgress((float)cooldowns->durStamina / (float)cooldowns->durStaminaMax);
			if (_widgetData->_fortifyWidgetShown && cooldowns->durFortify <= 0) {
				_widgetData->_widgetPanel->RemoveWidget(_widgetData->_fortifyWidget);
				_widgetData->_fortifyWidgetShown = false;
			} else if (_widgetData->_fortifyWidgetShown == false && cooldowns->durFortify > 0) {
				_widgetData->_widgetPanel->AddWidget(_widgetData->_fortifyWidget);
				_widgetData->_fortifyWidgetShown = true;
			}
			_widgetData->_fortifyCooldownWidget->SetProgress((float)cooldowns->durFortify / (float)cooldowns->durFortifyMax);
			if (_widgetData->_regenWidgetShown && cooldowns->durRegeneration <= 0) {
				_widgetData->_widgetPanel->RemoveWidget(_widgetData->_regenWidget);
				_widgetData->_regenWidgetShown = true;
			} else if (_widgetData->_regenWidgetShown == false && cooldowns->durRegeneration > 0) {
				_widgetData->_widgetPanel->AddWidget(_widgetData->_regenWidget);
				_widgetData->_regenWidgetShown = true;
			}
			_widgetData->_regenCooldownWidget->SetProgress((float)cooldowns->durRegeneration / (float)cooldowns->durRegenerationMax);
		}

	} else {
		if (_widgetData) {
			// reset all widget data and pointers
			_widgetData->_potionHealthWidget.reset();
			_widgetData->_potionHealthCooldownWidget.reset();
			_widgetData->_potionStaminaWidget.reset();
			_widgetData->_potionStaminaCooldownWidget.reset();
			_widgetData->_potionMagickaWidget.reset();
			_widgetData->_potionMagickaCooldownWidget.reset();
			_widgetData->_fortifyWidget.reset();
			_widgetData->_fortifyCooldownWidget.reset();
			_widgetData->_regenWidget.reset();
			_widgetData->_regenCooldownWidget.reset();
			if (_widgetData->_widgetPanel) {
				LibImGuiUI::LibImGuiUI_APIv1::instance->UnregisterWidget(_widgetData->_widgetPanel);
				_widgetData->_widgetPanel.reset();
			}
			delete _widgetData;
			_widgetData = nullptr;
		}
	}
}

void ActorInfo::ShowWidgets(bool shown)
{
	_showWidgets = shown;
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
	if (!valid || dead) {
		handleactor = false;
		LOG_1("invalid");
	} else {
		handleactor = handle;
		LOG_1("set {}", handle);
	}
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

void ActorInfo::UpdatePermanentPoisonResist()
{
	aclock;
	if (!valid || dead)
		return;
	if (RE::Actor* ac = actor.get().get(); ac != nullptr) {
		auto race = ac->GetRace();
		if (race && race->actorEffects && race->actorEffects->numSpells > 0) {
			// find all abilities that add poison resistance
			for (int i = 0; i < race->actorEffects->numSpells; i++) {
				if (race->actorEffects->spells[i]) {
					RE::EffectSetting* sett = nullptr;
					for (int c = 0; c < race->actorEffects->spells[i]->effects.size(); c++) {
						sett = race->actorEffects->spells[i]->effects[c]->baseEffect;
						if (sett) {
							if ((ConvertToAlchemyEffectPrimary(sett) & AlchemicEffect::kPoisonResist).IsValid() || (ConvertToAlchemyEffectSecondary(sett) & AlchemicEffect::kPoisonResist).IsValid()) {
								// found effect wth poison resist
								if (sett->IsDetrimental())
									_permanentPoisonResist -= race->actorEffects->spells[i]->effects[c]->effectItem.magnitude;
								else
									_permanentPoisonResist += race->actorEffects->spells[i]->effects[c]->effectItem.magnitude;
							}
						}
					}
				}
			}
		}
	}
}

#pragma region ActorSpecificFunctions

bool ActorInfo::IsFollower()
{
	aclock;
	if (!valid)
		return false;

	if (actor.get() && actor.get().get()) {
		RE::Actor* reac = actor.get().get();
		_follower = reac->IsInFaction(Settings::CurrentFollowerFaction) || reac->IsInFaction(Settings::CurrentHirelingFaction);
		if (_follower)
			return true;
		if (reac->GetActorBase()) {
			auto itr = reac->GetActorBase()->factions.begin();
			while (itr != reac->GetActorBase()->factions.end()) {
				if (Distribution::followerFactions()->contains(itr->faction->GetFormID()) && itr->rank >= 0) {
					_follower = true;
					return _follower;
				}
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

bool ActorInfo::Is3DLoaded()
{
	aclock;
	if (!valid)
		return false;
	if (RE::Actor* ac = actor.get().get(); ac != nullptr)
	{
		return ac->Is3DLoaded();
	}
	return false;
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

	loginfo("Modifying {}: DrinkPotion", name);

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

	loginfo("Modifying {}: RemoveItem", name);

	if (actor.get() && actor.get().get())
		actor.get().get()->RemoveItem(item, count, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
}

void ActorInfo::AddItem(RE::TESBoundObject* item, int32_t count)
{
	aclock;
	if (!valid)
		return;

	loginfo("Modifying {}: AddItem", name);

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

REX::EnumSet<RE::Actor::BOOL_BITS, std::uint32_t> ActorInfo::GetBoolBits()
{
	aclock;
	if (!valid)
		return REX::EnumSet<RE::Actor::BOOL_BITS, std::uint32_t>{};

	if (actor.get() && actor.get().get())
		return actor.get().get()->GetActorRuntimeData().boolBits;
	return REX::EnumSet<RE::Actor::BOOL_BITS, std::uint32_t>{};
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

bool ActorInfo::IsSleeping()
{
	aclock;
	if (!valid || dead)
		return false;

	if (actor.get() && actor.get().get())
		return actor.get().get()->AsActorState()->actorState1.sitSleepState == RE::SIT_SLEEP_STATE::kIsSleeping || actor.get().get()->AsActorState()->actorState1.sitSleepState == RE::SIT_SLEEP_STATE::kWaitingForSleepAnim;
	return false;
}

/*
class PerkEntryVisitorActor : public RE::PerkEntryVisitor
{
public:
	std::list<RE::BGSPerkEntry*> entries;

	int32_t GetDosage()
	{
		entries.sort([](RE::BGSPerkEntry* first, RE::BGSPerkEntry* second) {
			return first->GetPriority() > second->GetPriority();
		});
		auto itr = entries.begin();
		while (itr != entries.end())
		{
			switch ((*itr)->GetFunction() == RE::BGSPerkEntry::EntryPoint::kModPoisonDoseCount)
				(*itr)->GetFunctionData()
			itr++;
		}
	}

	RE::BSContainer::ForEachResult Visit(RE::BGSPerkEntry* a_perkEntry) override
	{
		if (a_perkEntry)
			entries.push_back(a_perkEntry);
		return RE::BSContainer::ForEachResult::kContinue;
	}
};
*/
int32_t ActorInfo::GetBasePoisonDosage(Compatibility* comp)
{
	aclock;
	if (!valid || dead)
		return 0;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		int32_t dosage = 0;
		//PerkEntryVisitorActor visit;
		//if (act->HasPerkEntries(RE::Actor::EntryPoint::kModPoisonDoseCount)) {
		//	act->ForEachPerkEntry(RE::Actor::EntryPoint::kModPoisonDoseCount, visit);

		//} else
		//	return 1;
		if (comp->LoadedOrdinator()) {
			if (act->HasPerk(comp->ConcPoison)) {
				// apply ordinator values
				// Function: Add Actor Value Mult, Data: 0.1 * Alchemy
				float alchemy = ACM::GetAV(act, RE::ActorValue::kAlchemy);
				return 1 /*base value*/ + (int32_t)(0.1 * alchemy);
			}
		} else if (comp->LoadedVokrii()) {
			if (act->HasPerk(comp->ConcPoison3)) {
				// apply vokrii values
				// Function: Add Value, Data: 6
				return 7;
			}
			else if (act->HasPerk(comp->ConcPoison2))
			{
				// apply vokrii values
				// Function: Add Value, Data: 4
				return 5;
			}
			else if (act->HasPerk(comp->ConcPoison))
			{
				// apply vokrii values
				// Function: Add Value, Data: 2
				return 3;
			}
		} else if (comp->LoadedAdamant()) {
			if (act->HasPerk(comp->ConcPoison)) {
				// admant removes this functionality
				return 1;
			}
		} else {
			if (act->HasPerk(comp->ConcPoison)) {
				// apply base skyrim values
				// Function: Set Value, Data: 2
				return 2;
			}
		}
	}
	return 0;
}

class VisitorPoisoned: public RE::MagicTarget::ForEachActiveEffectVisitor
{
public:
	std::vector<RE::EffectSetting*> effects;
	int32_t total = 0;

	virtual RE::BSContainer::ForEachResult Accept(RE::ActiveEffect* a_effect) override
	{
		total++;
		//loginfo("Visiting Active Effect: {}", total);
		if (a_effect) {
			effects.push_back(a_effect->GetBaseObject());
			//loginfo("list element: {}", Utility::PrintForm(a_effect->GetBaseObject()));
			/* if (a_effect->GetBaseObject()->IsDetrimental()) {
				//if (RE::AlchemyItem* alch = (*itr)->spell->As<RE::AlchemyItem>(); alch != nullptr)
				//	if (alch->IsPoison())
				//		return true;
				loginfo("list resist value: {}", (int)(a_effect->GetBaseObject()->data.resistVariable));
				found = a_effect->GetBaseObject()->data.resistVariable == RE::ActorValue::kPoisonResist;
				if (found)
					return RE::BSContainer::ForEachResult::kStop;
				//return ACM::HasPoisonResistValue((*itr)->spell);
			}
			if (a_effect->effect) {
			}*/
		}
		return RE::BSContainer::ForEachResult::kContinue;
	}
};

bool ActorInfo::IsPoisoned()
{
	aclock;
	if (!valid || dead)
		return false;

	int32_t total = 0;
	std::vector<RE::EffectSetting*> effects;
	bool found = false;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		if (!REL::Module::IsVR()) {
			auto list = act->AsMagicTarget()->GetActiveEffectList();
			if (list) {
				//loginfo("list empty: {}", list->empty());
				auto itr = list->begin();
				while (itr != list->end()) {
					if (*itr) {
						effects.push_back((*itr)->GetBaseObject());
					}
					static_cast<void>(itr++);
				}
				for (auto eff : effects) {
					if (eff) {
						//loginfo("list element: {}", Utility::PrintForm(eff));
						if (eff->IsDetrimental()) {
							//loginfo("list resist value: {}", (int)(eff->data.resistVariable));
							found = eff->data.resistVariable == RE::ActorValue::kPoisonResist;
							if (found)
								return true;
							//return ACM::HasPoisonResistValue((*itr)->spell);
						}
					}
				}
			} else {
				//loginfo("not list");
			}
		} else {

			VisitorPoisoned vi;
			act->AsMagicTarget()->VisitEffects(vi);
			for (auto eff : vi.effects) {
				if (eff) {
					//loginfo("list element: {}", Utility::PrintForm(eff));
					if (eff->IsDetrimental()) {
						//loginfo("list resist value: {}", (int)(eff->data.resistVariable));
						found = eff->data.resistVariable == RE::ActorValue::kPoisonResist;
						if (found)
							return true;
						//return ACM::HasPoisonResistValue((*itr)->spell);
					}
				}
			}
		}
	}
	return false;
}

std::vector<RE::TESFaction*> ActorInfo::GetFactions()
{
	aclock;
	if (!valid || dead)
		return {};

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		std::vector<RE::TESFaction*> factions;
		auto factionVisitor = [&factions](RE::TESFaction* a_faction, int8_t a_rank) {
			if (a_rank >= 0)
				factions.push_back(a_faction);
			return true;
		};
		act->VisitFactions(factionVisitor);
	}
	return std::vector<RE::TESFaction*>();
}

void ActorInfo::EvaluatePackage()
{
	aclock;
	if (!valid || dead)
		return;

	if (actor.get() && actor.get().get()) {
		actor.get()->EvaluatePackage();
	}
}

#pragma endregion


#pragma region ACM

float ActorInfo::GetAVMax(RE::ActorValue av)
{
	aclock;
	if (!valid || dead)
		return 0;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		// add base value, permanent modifiers and temporary modifiers (magic effects for instance)
		return act->AsActorValueOwner()->GetPermanentActorValue(av) + act->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av);
	}
	return 0;
}

float ActorInfo::GetAV(RE::ActorValue av)
{
	aclock;
	if (!valid || dead)
		return 0;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		return act->AsActorValueOwner()->GetActorValue(av);
	}
	return 0;
}

float ActorInfo::GetAVPercentage(RE::ActorValue av)
{
	aclock;
	if (!valid || dead)
		return 0;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		return act->AsActorValueOwner()->GetActorValue(av) / (act->AsActorValueOwner()->GetPermanentActorValue(av) + act->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
	}
	return 0;
}

float ActorInfo::GetAVPercentageFromValue(RE::ActorValue av, float curr)
{
	aclock;
	if (!valid || dead)
		return 0;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		return curr / (act->AsActorValueOwner()->GetPermanentActorValue(av) + act->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIER::kTemporary, av));
	}
	return 0;
}

void ActorInfo::RestoreAV(RE::ActorValue av, float value)
{
	aclock;
	if (!valid || dead)
		return;

	if (actor.get() && actor.get().get()) {
		RE::Actor* act = actor.get().get();
		act->AsActorValueOwner()->RestoreActorValue(av, value);
	}
	return;
}

#pragma endregion
