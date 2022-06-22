#include "ActorManipulation.h"

using AlchemyEffect = Settings::AlchemyEffect;

#define ConvAlchULong(x) static_cast<uint64_t>(Settings::ConvertToAlchemyEffect(x))
#define ULong(x) static_cast<uint64_t>(x)

//
std::tuple<bool, float, int, Settings::AlchemyEffect> ACM::HasAlchemyEffect(RE::AlchemyItem* item, uint64_t alchemyEffect)
{
	LOG_4("{}[HasAlchemyEffect] begin");
	//if (item && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion))) {
		RE::EffectSetting* sett = nullptr;
		LOG_4("{}[HasAlchemyEffect] found medicine");
		if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
			LOG_4("{}[HasAlchemyEffect] end fail1");
			return { false, 0.0f, 0, AlchemyEffect::kNone};
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
					uint32_t formid = sett->GetFormID();
					// Paralysis
					// Just straight up detect this using the formid, since literally everyone seems to like
					// to mess with this.
					if (formid == 0x73F30) {
						out = static_cast<uint64_t>(Settings::AlchemyEffect::kParalysis);
						mag = item->effects[i]->effectItem.magnitude;
						dur = item->effects[i]->effectItem.duration;
						found = true;
						break;
					}
					if ((tmp = (ConvAlchULong(sett->data.primaryAV) & alchemyEffect)) > 0) {
						found = true;

						//logger::info("alch effect id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.primaryAV));
						out = tmp;
						mag = item->effects[i]->effectItem.magnitude;
						dur = item->effects[i]->effectItem.duration;
						break;
					}
					if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = (ConvAlchULong(sett->data.secondaryAV) & alchemyEffect)) > 0) {
						found = true;

						//logger::info("alch effect2 id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.secondaryAV));
						out = tmp;
						mag = item->effects[i]->effectItem.magnitude;
						dur = item->effects[i]->effectItem.duration;
						break;
					}
					// COMPATIBILITY FOR CACO
					if (Settings::_CompatibilityCACO) {
						//logger::info("caco comp");
						// DamageStaminaRavage
						if (formid == 0x73F23) {
							out = static_cast<uint64_t>(Settings::AlchemyEffect::kStamina);
							mag = item->effects[i]->effectItem.magnitude;
							dur = item->effects[i]->effectItem.duration;
							found = true;
							break;
						}
						// DamageMagickaRavage
						else if (formid == 0x73F27) {
							out = static_cast<uint64_t>(Settings::AlchemyEffect::kMagicka);
							mag = item->effects[i]->effectItem.magnitude;
							dur = item->effects[i]->effectItem.duration;
							found = true;
							break;
						}
					}
					// COMPATIBILITY FOR APOTHECARY
					if (Settings::_CompatibilityApothecary) {
						// DamageWeapon
						if (formid == 0x73F26) {
							out = static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult);
							mag = item->effects[i]->effectItem.magnitude;
							dur = item->effects[i]->effectItem.duration;
							found = true;
							break;
						}
						// Silence
						else if (formid == 0x73F2B) {
							out = static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRate);
							mag = item->effects[i]->effectItem.magnitude;
							dur = item->effects[i]->effectItem.duration;
							found = true;
							break;
						}
					}
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
			return { true, mag, dur, static_cast<AlchemyEffect>(out) };
		}
		LOG_4("{}[HasAlchemyEffect] end fail2");
		return { false, 0.0f, 0, AlchemyEffect::kNone };
	//}
	LOG_4("{}[HasAlchemyEffect] end fail3");
	return { false, 0.0f, 0, AlchemyEffect::kNone };
}

std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> ACM::GetMatchingPotions(RE::Actor* actor, uint64_t alchemyEffect)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> ret{};
	auto itemmap = actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetMatchingItemsPotions] trying to find potion");
	while (iter != itemmap.end() && alchemyEffect != 0) {
		item = iter->first->As<RE::AlchemyItem>();
		LOG_4("{}[GetMatchingItemsPotions] checking item");
		if (item && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion))) {
			LOG_4("{}[GetMatchingItemsPotions] found medicine");
			if (auto res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
				ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
				//logger::info("[getMatch] dur {} mag {} effect {}", std::get<2>(res), std::get<1>(res), static_cast<uint64_t>(std::get<3>(res)));
			}
		}
		iter++;
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> ACM::GetMatchingPoisons(RE::Actor* actor, uint64_t alchemyEffect)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> ret{};
	auto itemmap = actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetMatchingItemsPoisons] trying to find poison");
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (iter->first) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_4("{}[GetMatchingItemsPoisons] checking item");
			if (item && (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison))) {
				LOG_4("{}[GetMatchingItemsPoisons] found poison");
				if (auto res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
				}
			}
		}
		iter++;
	}
	LOG1_3("{}[GetMatchingItemsPoisons] finished. found: {} poisons", ret.size());
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> ACM::GetMatchingFood(RE::Actor* actor, uint64_t alchemyEffect, bool raw)
{
	std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> ret{};
	auto itemmap = actor->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	LOG_3("{}[GetMatchingItemsFood] trying to find food");
	while (iter != itemmap.end() && alchemyEffect != 0) {
		item = iter->first->As<RE::AlchemyItem>();
		LOG_4("{}[GetMatchingItemsFood] checking item");
		if (item && raw == false && (item->IsFood() || item->HasKeyword(Settings::VendorItemFood))) {
			LOG_4("{}[GetMatchingItemsFood] found food");
			if (auto res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
				ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
			}
		} else if (item && item->HasKeyword(Settings::VendorItemFoodRaw)) {
			LOG_4("{}[GetMatchingItemsFood] found food raw");
			if (auto res = HasAlchemyEffect(item, alchemyEffect); std::get<0>(res)) {
				ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
			}
		}
		iter++;
	}
	LOG1_4("{}[GetMatchingItemsFood] return: {}", ret.size());
	return ret;
}

std::tuple<int, Settings::AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>>> ACM::ActorUsePotion(RE::Actor* _actor, uint64_t alchemyEffect, bool compatibility)
{
	auto begin = std::chrono::steady_clock::now();
	// if no effect is specified, return
	if (alchemyEffect == 0) {
		return { -1, AlchemyEffect::kNone, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>>{} };
	}
	auto itemmap = _actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	//RE::EffectSetting* sett = nullptr;
	LOG_2("{}[ActorUsePotion] trying to find potion");
	auto ls = GetMatchingPotions(_actor, alchemyEffect);
	ls.sort(Utility::SortMagnitude);
	// got all potions the actor has sorted by magnitude.
	// now use the one with the highest magnitude;
	return ActorUsePotion(_actor, ls, compatibility);
}

std::tuple<int, Settings::AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>>> ACM::ActorUsePotion(RE::Actor* _actor, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> &ls, bool compatibility)
{
	if (ls.size() > 0 && std::get<2>(ls.front())) {
		LOG_2("{}[ActorUsePotion] Drink Potion");
		LOG1_3("{}[ActorUsePotion] use potion on: {}", Utility::GetHex(_actor->GetFormID()));
		if (Settings::CompatibilityPotionPapyrus() || compatibility) {

			LOG_3("{}[ActorUsePotion] Compatibility Mode");
			if (!Settings::CompatibilityPotionPlugin(_actor)) {
				return { -1, AlchemyEffect::kNone, ls };
				LOG_3("{}[ActorUsePotion] Cannot use potion due to compatibility");
			}
			// preliminary, has check built in wether it applies
			Settings::ApplyCompatibilityPotionAnimatedPapyrus(_actor);
			SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = _actor;
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
			ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = std::get<2>(ls.front());
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
		} else {
			// apply compatibility stuff before using potion
			if (!Settings::CompatibilityPotionPlugin(_actor)) {
				return { -1, AlchemyEffect::kNone, ls };
				LOG_3("{}[ActorUsePotion] Cannot use potion due to compatibility");
			}
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->SetOwner(_actor);

			if (Settings::_DisableEquipSounds)
				RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, std::get<2>(ls.front()), extra, 1, nullptr, true, false, false);
			else
				RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, std::get<2>(ls.front()), extra);
		}
		auto val = ls.front();
		ls.pop_front();
		return { std::get<1>(val), std::get<3>(val), ls };
	}
	return { -1, AlchemyEffect::kNone, ls };
}

std::pair<int, Settings::AlchemyEffect> ACM::ActorUseFood(RE::Actor* _actor, uint64_t alchemyEffect, bool raw)
{
	auto begin = std::chrono::steady_clock::now();
	// if no effect is specified, return
	if (alchemyEffect== 0) {
		return { -1, AlchemyEffect::kNone };
	}
	auto itemmap = _actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	//RE::EffectSetting* sett = nullptr;
	LOG_2("{}[ActorUseFood] trying to find food");
	auto ls = GetMatchingFood(_actor, alchemyEffect, raw);
	//LOG_2("{}[ActorUseFood] step1");
	ls.sort(Utility::SortMagnitude);
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
			ev->sender = _actor;
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
			ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 0.0f;
			ev->sender = std::get<2>(ls.front());
			SKSE::GetModCallbackEventSource()->SendEvent(ev);
		} else {
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->SetOwner(_actor);
			if (Settings::_DisableEquipSounds)
				RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, std::get<2>(ls.front()), extra, 1, nullptr, true, false, false);
			else
				RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, std::get<2>(ls.front()), extra);
		}
		return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
	}
	//LOG_2("{}[ActorUseFood] step3");
	return { -1, AlchemyEffect::kNone };
}

static RE::BSAudioManager* audiomanager;

std::pair<int, AlchemyEffect> ACM::ActorUsePoison(RE::Actor* _actor, uint64_t alchemyEffect)
{
	auto begin = std::chrono::steady_clock::now();
	// if no effect is specified, return
	if (alchemyEffect== 0) {
		return { -1, AlchemyEffect::kNone };
	}
	auto itemmap = _actor->GetInventory();
	auto iter = itemmap.begin();
	auto end = itemmap.end();
	//RE::EffectSetting* sett = nullptr;
	LOG_2("{}[ActorUsePoison] trying to find poison");
	auto ls = GetMatchingPoisons(_actor, alchemyEffect);
	ls.sort(Utility::SortMagnitude);
	// got all potions the actor has sorted by magnitude.
	// now use the one with the highest magnitude;
	if (ls.size() > 0 && std::get<2>(ls.front())) {
		LOG_2("{}[ActorUsePoison] Use Poison");
		if (!audiomanager)
			audiomanager = RE::BSAudioManager::GetSingleton();
		RE::ExtraDataList* extra = new RE::ExtraDataList();
		extra->Add(new RE::ExtraPoison(std::get<2>(ls.front()), 1));
		auto ied = _actor->GetEquippedEntryData(false);
		if (ied) {
			ied->AddExtraList(extra);
			_actor->RemoveItem(std::get<2>(ls.front()), 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
			{
				// play poison sound
				RE::BSSoundHandle handle;
				if (std::get<2>(ls.front())->data.consumptionSound)
					audiomanager->BuildSoundDataFromDescriptor(handle, std::get<2>(ls.front())->data.consumptionSound->soundDescriptor);
				else
					audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
				handle.SetObjectToFollow(_actor->Get3D());
				handle.SetVolume(1.0);
				handle.Play();
			}
			return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
		} else {
			ied = _actor->GetEquippedEntryData(true);
			if (ied) {
				ied->AddExtraList(extra);
				_actor->RemoveItem(std::get<2>(ls.front()), 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
				{
					// play poison sound
					RE::BSSoundHandle handle;
					if (std::get<2>(ls.front())->data.consumptionSound)
						audiomanager->BuildSoundDataFromDescriptor(handle, std::get<2>(ls.front())->data.consumptionSound->soundDescriptor);
					else
						audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
					handle.SetObjectToFollow(_actor->Get3D());
					handle.SetVolume(1.0);
					handle.Play();
				}
				return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
			}
		}
	}
	return { -1, AlchemyEffect::kNone };
}





