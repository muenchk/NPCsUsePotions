#include "ActorManipulation.h"

using AlchemyEffect = Settings::AlchemyEffect;

#define ConvAlchULong(x) static_cast<uint64_t>(Settings::ConvertToAlchemyEffect(x))
#define ULong(x) static_cast<uint64_t>(x)


static std::mt19937 randan((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));

// calc all alchemyeffects of the item and return the duration and magnitude of the effect with the highest product mag * dur
std::tuple<bool, float, int, AlchemyEffectBase> ACM::HasAlchemyEffect(RE::AlchemyItem* item, AlchemyEffectBase alchemyEffect)
{
	LOG_4("{}[HasAlchemyEffect] begin");
	RE::EffectSetting* sett = nullptr;
	RE::EffectSetting* prim = nullptr;
	LOG_4("{}[HasAlchemyEffect] found medicine");
	if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
		LOG_4("{}[HasAlchemyEffect] end fail1");
		return { false, 0.0f, 0, 0 };
	}
	uint64_t out = 0;
	uint64_t tmp = 0;
	bool found = false;
	float mag = 0;
	int dur = 0;
	if (item->effects.size() > 0) {
		for (uint32_t i = 0; i < item->effects.size(); i++) {
			sett = item->effects[i]->baseEffect;
			//logger::info("[HasAlchemyEffect] base effect {} effect {} dur {} mag {} name {}", sett->data.primaryAV, ConvAlchULong(sett->data.primaryAV), item->effects[i]->effectItem.duration, item->effects[i]->effectItem.magnitude, std::to_string(item->GetFormID()));
			if (sett) {
				if (prim == nullptr) {
					prim = sett;
					mag = item->effects[i]->effectItem.magnitude;
					dur = item->effects[i]->effectItem.duration;
				}
				uint32_t formid = sett->GetFormID();
				if ((tmp = (static_cast<uint64_t>(Settings::ConvertToAlchemyEffectPrimary(sett)) & alchemyEffect)) > 0) {
					found = true;

					//logger::info("alch effect id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.primaryAV));
					out |= tmp;
					if (mag * dur < item->effects[i]->effectItem.magnitude * item->effects[i]->effectItem.duration) {
						mag = item->effects[i]->effectItem.magnitude;
						dur = item->effects[i]->effectItem.duration;
					}
				} else if (tmp != 0)
					out |= tmp;
				if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = ((static_cast<uint64_t>(Settings::ConvertToAlchemyEffectSecondary(sett)) & alchemyEffect))) > 0) {
					found = true;

					//logger::info("alch effect2 id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.secondaryAV));
					out |= tmp;
					if (mag * dur < item->effects[i]->effectItem.magnitude * item->effects[i]->effectItem.duration) {
						mag = item->effects[i]->effectItem.magnitude;
						dur = item->effects[i]->effectItem.duration;
					}
				} else if (tmp != 0)
					out |= tmp;
			}
		}
	} else {
		RE::MagicItem::SkillUsageData err;
		item->GetSkillUsageData(err);
		if ((out = ConvAlchULong(item->avEffectSetting->data.primaryAV) & alchemyEffect) > 0) {
			found = true;
			mag = err.magnitude;
		}
	}
	if (found) {
		//logger::info("[HasAlchemyEffect] dur {} mag {}, effect {}, converted {}", dur, mag, out, static_cast<uint64_t>(static_cast<AlchemyEffect>(out)));
		LOG_4("{}[HasAlchemyEffect] end success");
		return { true, mag, dur, out };
	}
	LOG_4("{}[HasAlchemyEffect] end fail2");
	return { false, mag, dur, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ACM::GetMatchingPotions(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	std::tuple<bool, float, int, AlchemyEffectBase> res;
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetMatchingItemsPotions] trying to find potion");
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetMatchingItemsPotions] checking item");
			if (item && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion))) {
				LOG_4("{}[GetMatchingItemsPotions] found medicine");
				if (res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
					//logger::info("[getMatch] dur {} mag {} effect {}", std::get<2>(res), std::get<1>(res), static_cast<uint64_t>(std::get<3>(res)));
				}
				if (acinfo->IsCustomAlchItem(item))
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
			}
		}
		iter++;
	}
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllPotions(ActorInfo* acinfo)
{
	std::list<RE::AlchemyItem*> ret{};
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetAllPotions] trying to find potion");
	while (iter != itemmap.end()) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetAllPotions] checking item");
			if (item && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion))) {
				LOG_4("{}[GetAllPotions] found potion.");
				ret.insert(ret.begin(), item);
			}
		}
		iter++;
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ACM::GetMatchingPoisons(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	std::tuple<bool, float, int, AlchemyEffectBase> res;
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetMatchingItemsPoisons] trying to find poison");
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetMatchingItemsPoisons] checking item");
			if (item && (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison))) {
				LOG_4("{}[GetMatchingItemsPoisons] found poison");
				if (res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
				}
				if (acinfo->IsCustomAlchItem(item))
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
			}
		}
		iter++;
	}
	LOG1_3("{}[GetMatchingItemsPoisons] finished. found: {} poisons", ret.size());
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllPoisons(ActorInfo* acinfo)
{
	std::list<RE::AlchemyItem*> ret{};
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetAllPoisons] trying to find poison");
	while (iter != itemmap.end()) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetAllPoisons] checking item");
			if (item && (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison))) {
				LOG_4("{}[GetAllPoisons] found poison.");
				ret.insert(ret.begin(), item);
			}
		}
		iter++;
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ACM::GetMatchingFood(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool raw)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	std::tuple<bool, float, int, AlchemyEffectBase> res;
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetMatchingItemsFood] trying to find food");
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetMatchingItemsFood] checking item");
			if (item && raw == false && (item->IsFood() || item->HasKeyword(Settings::VendorItemFood))) {
				LOG_4("{}[GetMatchingItemsFood] found food");
				if (res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
				}
				if (acinfo->IsCustomAlchItem(item))
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
			} else if (item && raw == true && item->HasKeyword(Settings::VendorItemFoodRaw)) {
				LOG_4("{}[GetMatchingItemsFood] found food raw");
				if (res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
				}
				if (acinfo->IsCustomAlchItem(item))
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
			}
		}
		iter++;
	}
	LOG1_4("{}[GetMatchingItemsFood] return: {}", ret.size());
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllFood(ActorInfo* acinfo)
{
	std::list<RE::AlchemyItem*> ret{};
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetAllFood] trying to find food");
	while (iter != itemmap.end()) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetAllFood] checking item");
			if (item && (item->IsFood() || item->HasKeyword(Settings::VendorItemFoodRaw) || item->HasKeyword(Settings::VendorItemFood))) {
				LOG_4("{}[GetAllFood] found food.");
				ret.insert(ret.begin(), item);
			}
		}
		iter++;
	}
	return ret;
}

std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> ACM::GetRandomFood(ActorInfo* acinfo)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	RE::EffectSetting* sett = nullptr;
	float mag = 0;
	int dur = 0;
	Settings::AlchemyEffect tmp = Settings::AlchemyEffect::kNone;
	AlchemyEffectBase out = 0;
	bool found = false;
	LOG_3("{}[GetRandomFood] trying to find food");
	while (iter != itemmap.end()) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetRandomFood] checking item");
			if (item && (item->IsFood() || item->HasKeyword(Settings::VendorItemFoodRaw) || item->HasKeyword(Settings::VendorItemFood))) {
				mag = 0;
				dur = 0;
				out = 0;
				if (item->effects.size() > 0) {
					for (uint32_t i = 0; i < item->effects.size(); i++) {
						sett = item->effects[i]->baseEffect;
						if (sett) {
							if ((tmp = Settings::ConvertToAlchemyEffectPrimary(sett)) != Settings::AlchemyEffect::kNone) {
								found = true;
								out = static_cast<AlchemyEffectBase>(tmp);
								mag = item->effects[i]->effectItem.magnitude;
								dur = item->effects[i]->effectItem.duration;
								break;
							}
						}
					}
				}
				LOG_4("{}[GetRandomFood] found food.");
				ret.insert(ret.begin(), { mag, dur, item, out });
			}
		}
		iter++;
	}
	LOG1_4("{}[GetRandomFood] number of found items: {}", ret.size());
	if (ret.size() > 0) {
		std::uniform_int_distribution<signed> dist(1, (int)ret.size());
		int x = dist(randan);
		auto itr = ret.begin();
		int i = 1;
		while (itr != ret.end() && i < x) {
			i++;
			itr++;
		}
		if (itr == ret.end())
			return ret.front();
		else
			return *itr;
	} else {
		return { 0.0f, 0, nullptr, static_cast<AlchemyEffectBase>(Settings::AlchemyEffect::kNone) };
	}
}

std::unordered_map<uint32_t, int> ACM::GetCustomItems(ActorInfo* acinfo)
{
	std::unordered_map<uint32_t, int> ret{};
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	LOG_3("{}[GetCustomItems] checking items");
	while (iter != itemmap.end()) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			if (acinfo->IsCustomItem(iter->first)) {
				ret.insert_or_assign(iter->first->GetFormID(), std::get<0>(iter->second));
				LOG_4("{}[GetCustomItems] found custom item");
			}
		}
		iter++;
	}
	LOG1_4("{}[GetCustomItems] return: {}", ret.size());
	return ret;
}

std::tuple<int, AlchemyEffectBase, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>> ACM::ActorUsePotion(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool compatibility)
{
	auto begin = std::chrono::steady_clock::now();
	// if no effect is specified, return
	if (alchemyEffect == 0) {
		return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>{} };
	}
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	//RE::EffectSetting* sett = nullptr;
	LOG_2("{}[ActorUsePotion] trying to find potion");
	auto ls = GetMatchingPotions(acinfo, alchemyEffect);
	ls.sort(Utility::SortMagnitude);
	ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> tup) { return (bool)(std::get<3>(tup) & ULong(Settings::AlchemyEffect::kCureDisease)) && acinfo->CanUsePot(std::get<2>(tup)->GetFormID()) == false; });
	// got all potions the actor has sorted by magnitude.
	// now use the one with the highest magnitude;
	return ActorUsePotion(acinfo, ls, compatibility);
}

std::tuple<int, AlchemyEffectBase, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>> ACM::ActorUsePotion(ActorInfo* acinfo, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>& ls, bool compatibility)
{
	if (ls.size() > 0 && std::get<2>(ls.front())) {
		LOG_2("{}[ActorUsePotion] Drink Potion");
		LOG1_3("{}[ActorUsePotion] use potion on: {}", Utility::GetHex(acinfo->actor->GetFormID()));
		if (Settings::CompatibilityPotionPapyrus() || compatibility) {

			LOG_3("{}[ActorUsePotion] Compatibility Mode");
			if (!Settings::CompatibilityPotionPlugin(acinfo->actor)) {
				return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, ls };
				LOG_3("{}[ActorUsePotion] Cannot use potion due to compatibility");
			}
			// preliminary, has check built in wether it applies
			Settings::ApplyCompatibilityPotionAnimatedPapyrus(acinfo->actor);
			SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = acinfo->actor;
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
			ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = std::get<2>(ls.front());
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
		} else {
			// apply compatibility stuff before using potion
			if (!Settings::CompatibilityPotionPlugin(acinfo->actor)) {
				return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, ls };
				LOG_3("{}[ActorUsePotion] Cannot use potion due to compatibility");
			}
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->SetOwner(acinfo->actor);

			if (Settings::_DisableEquipSounds)
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), extra, 1, nullptr, true, false, false);
			else
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), extra);
		}
		auto val = ls.front();
		ls.pop_front();
		return { std::get<1>(val), std::get<3>(val), std::get<0>(val), ls };
	}
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, ls };
}

std::pair<int, AlchemyEffectBase> ACM::ActorUseFood(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool raw)
{
	auto begin = std::chrono::steady_clock::now();
	// if no effect is specified, return
	if (alchemyEffect== 0) {
		return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
	}
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	//RE::EffectSetting* sett = nullptr;
	LOG_2("{}[ActorUseFood] trying to find food");
	auto ls = GetMatchingFood(acinfo, alchemyEffect, raw);
	//LOG_2("{}[ActorUseFood] step1");
	ls.sort(Utility::SortMagnitude);
	ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> tup) { return (bool)(std::get<3>(tup) & ULong(Settings::AlchemyEffect::kCureDisease)) && acinfo->CanUseFood(std::get<2>(tup)->GetFormID()) == false; });
	//LOG_2("{}[ActorUseFood] step2");
	// got all potions the actor has sorted by magnitude.
	// now use the one with the highest magnitude;
	if (ls.size() > 0 && std::get<2>(ls.front())) {
		LOG_2("{}[ActorUseFood] Use Food");
		if (Settings::CompatibilityFoodPapyrus()) {
			LOG_3("{}[ActorUseFood] Compatibility Mode");
			// use same event as for potions, since it takes a TESForm* and works for anything
			SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = acinfo->actor;
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
			ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = std::get<2>(ls.front());
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
		} else {
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->SetOwner(acinfo->actor);
			if (Settings::_DisableEquipSounds)
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), extra, 1, nullptr, true, false, false);
			else
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), extra);
		}
		return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
	}
	//LOG_2("{}[ActorUseFood] step3");
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}

std::pair<int, AlchemyEffectBase> ACM::ActorUseFood(ActorInfo* acinfo)
{
	auto begin = std::chrono::steady_clock::now();
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	LOG_2("{}[ActorUseFood-Random] trying to find food");
	auto item = GetRandomFood(acinfo);
	//LOG_2("{}[ActorUseFood-Random] step1");
	// use the random food
	if (std::get<2>(item)) {
		LOG_2("{}[ActorUseFood-Random] Use Food");
		if (Settings::CompatibilityFoodPapyrus()) {
			LOG_3("{}[ActorUseFood-Random] Compatibility Mode");
			// use same event as for potions, since it takes a TESForm* and works for anything
			SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = acinfo->actor;
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
			ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = std::get<2>(item);
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
		} else {
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->SetOwner(acinfo->actor);
			if (Settings::_DisableEquipSounds)
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(item), extra, 1, nullptr, true, false, false);
			else
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(item), extra);
		}
		return { std::get<1>(item), std::get<3>(item) };
	}
	//LOG_2("{}[ActorUseFood-Random] step3");
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}

/// <summary>
/// Game audiomanager which plays sounds.
/// </summary>
static RE::BSAudioManager* audiomanager;

std::pair<int, AlchemyEffectBase> ACM::ActorUsePoison(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect)
{
	auto begin = std::chrono::steady_clock::now();
	// if no effect is specified, return
	if (alchemyEffect== 0) {
		return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
	}
	auto itemmap = acinfo->actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	//RE::EffectSetting* sett = nullptr;
	LOG_2("{}[ActorUsePoison] trying to find poison");
	auto ls = GetMatchingPoisons(acinfo, alchemyEffect);
	ls.sort(Utility::SortMagnitude);
	ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> tup) { return (bool)(std::get<3>(tup) & ULong(Settings::AlchemyEffect::kCureDisease)) && acinfo->CanUsePoison(std::get<2>(tup)->GetFormID()) == false; });
	// got all potions the actor has sorted by magnitude.
	// now use the one with the highest magnitude;
	if (ls.size() > 0 && std::get<2>(ls.front())) {
		LOG_2("{}[ActorUsePoison] Use Poison");
		if (!audiomanager)
			audiomanager = RE::BSAudioManager::GetSingleton();
		RE::ExtraDataList* extra = new RE::ExtraDataList();
		extra->Add(new RE::ExtraPoison(std::get<2>(ls.front()), 1));
		auto ied = acinfo->actor->GetEquippedEntryData(false);
		if (ied) {
			ied->AddExtraList(extra);
			acinfo->actor->RemoveItem(std::get<2>(ls.front()), 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
			{
				// play poison sound
				RE::BSSoundHandle handle;
				if (std::get<2>(ls.front())->data.consumptionSound)
					audiomanager->BuildSoundDataFromDescriptor(handle, std::get<2>(ls.front())->data.consumptionSound->soundDescriptor);
				else
					audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
				handle.SetObjectToFollow(acinfo->actor->Get3D());
				handle.SetVolume(1.0);
				handle.Play();
			}
			return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
		} else {
			ied = acinfo->actor->GetEquippedEntryData(true);
			if (ied) {
				ied->AddExtraList(extra);
				acinfo->actor->RemoveItem(std::get<2>(ls.front()), 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
				{
					// play poison sound
					RE::BSSoundHandle handle;
					if (std::get<2>(ls.front())->data.consumptionSound)
						audiomanager->BuildSoundDataFromDescriptor(handle, std::get<2>(ls.front())->data.consumptionSound->soundDescriptor);
					else
						audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
					handle.SetObjectToFollow(acinfo->actor->Get3D());
					handle.SetVolume(1.0);
					handle.Play();
				}
				return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
			}
		}
	}
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}





