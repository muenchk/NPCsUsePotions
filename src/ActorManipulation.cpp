#include <random>
#include <tuple>
#include <vector>

#include "ActorManipulation.h"
#include "AlchemyEffect.h"
#include "Data.h"
#include "Statistics.h"

#define ConvAlchULong(x) static_cast<uint64_t>(ConvertToAlchemyEffect(x))
#define ULong(x) static_cast<uint64_t>(x)


static std::mt19937 randan((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));

void ACM::Init()
{
	ACM::data = Data::GetSingleton();
	ACM::comp = Compatibility::GetSingleton();
}

// calc all alchemyeffects of the item and return the duration and magnitude of the effect with the highest product mag * dur
std::tuple<bool, float, int, AlchemyEffectBase, bool> ACM::HasAlchemyEffect(RE::AlchemyItem* item, AlchemyEffectBase alchemyEffect)
{
	LOG_3("{}[ActorManipulation] [HasAlchemyEffect]");
	// check if the ite is excluded
	if (Distribution::excludedItems()->contains(item->GetFormID()))
		return { false, -1.0f, -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), false };
	auto [mapf, eff, dur, mag, detr, dosage] = data->GetAlchItemEffects(item->GetFormID());
	if (mapf) {
		if ((eff & alchemyEffect) != 0) {
			LOG_4("{}[ActorManipulation] [HasAlchemyEffect] fast success");
			return { true, mag, dur, eff, detr };
		}
		else {
			LOG_4("{}[ActorManipulation] [HasAlchemyEffect] fast fail: does not match effect");
			return { false, mag, dur, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), false };
		}
	} else {
		RE::EffectSetting* sett = nullptr;
		RE::EffectSetting* prim = nullptr;
		bool detrimental = false;
		bool positive = false;
		if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
			LOG_4("{}[ActorManipulation] [HasAlchemyEffect] fail: no item effects");
			return { false, 0.0f, 0, 0, false };
		}
		uint64_t out = 0;
		uint64_t tmp = 0;
		bool found = false;
		if (item->effects.size() > 0) {
			for (uint32_t i = 0; i < item->effects.size(); i++) {
				sett = item->effects[i]->baseEffect;
				if (sett) {
					if (prim == nullptr) {
						prim = sett;
						mag = item->effects[i]->effectItem.magnitude;
						dur = item->effects[i]->effectItem.duration;
					}
					detrimental |= sett->IsDetrimental();
					positive |= !sett->IsDetrimental();
					uint32_t formid = sett->GetFormID();
					if ((tmp = (static_cast<uint64_t>(ConvertToAlchemyEffectPrimary(sett)) & alchemyEffect)) > 0) {
						found = true;

						//logger::info("[ActorManipulation] [HasAlchemyEffect] alch effect id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.primaryAV));
						out |= tmp;
						if (mag * dur < item->effects[i]->effectItem.magnitude * item->effects[i]->effectItem.duration) {
							mag = item->effects[i]->effectItem.magnitude;
							dur = item->effects[i]->effectItem.duration;
						}
					} else if (tmp != 0)
						out |= tmp;
					if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = ((static_cast<uint64_t>(ConvertToAlchemyEffectSecondary(sett)) & alchemyEffect))) > 0) {
						found = true;

						//logger::info("[ActorManipulation] [HasAlchemyEffect] alch effect2 id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.secondaryAV));
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
			if (item->avEffectSetting && (out = ConvAlchULong(item->avEffectSetting->data.primaryAV) & alchemyEffect) > 0) {
				detrimental |= item->avEffectSetting->IsDetrimental();
				positive |= !item->avEffectSetting->IsDetrimental();
				found = true;
				mag = err.magnitude;
			}
		}
		if (found) {
			//logger::info("[ActorManipulation] [HasAlchemyEffect] dur {} mag {}, effect {}, converted {}", dur, mag, out, static_cast<uint64_t>(static_cast<AlchemyEffect>(out)));
			LOG_4("{}[ActorManipulation] [HasAlchemyEffect] slow success");
			// save calculated values to data
			dosage = Distribution::GetPoisonDosage(item, out);
			if (item->IsPoison()) {
				data->SetAlchItemEffects(item->GetFormID(), out, dur, mag, positive, dosage);
				return { true, mag, dur, out, positive };
			}
			data->SetAlchItemEffects(item->GetFormID(), out, dur, mag, detrimental, dosage);
			return { true, mag, dur, out, detrimental };
		}
		LOG_4("{}[ActorManipulation] [HasAlchemyEffect] slow fail: does not match effect");
		return { false, mag, dur, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), false };
	}
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ACM::GetMatchingPotions(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect)
{
	LOG_3("{}[ActorManipulation] [GetMatchingPotions]");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		std::tuple<bool, float, int, AlchemyEffectBase, bool> res;
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != itemmap.end() && alchemyEffect != 0) {
			if (Utility::ValidateForm(iter->first) && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[ActorManipulation] [GetMatchingPotions] checking item");
				if (item && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion))) {
					LOG1_4("{}[ActorManipulation] [GetMatchingPotions] found medicine {}", Utility::PrintForm(item));
					if (acinfo->CanUsePot(item->GetFormID()))
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
					else if (res = HasAlchemyEffect(item, alchemyEffect);
							 std::get<0>(res) &&
							 (Settings::Potions::_AllowDetrimentalEffects || std::get<4>(res) == false /*either we allow detrimental effects or there are none*/)) {
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
						//logger::info("[ActorManipulation] [GetMatchingPotions] dur {} mag {} effect {}", std::get<2>(res), std::get<1>(res), static_cast<uint64_t>(std::get<3>(res)));
					}
					
				}
			}
			iter++;
		}
	}
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllPotions(ActorInfo* acinfo)
{
	LOG_3("{}[ActorManipulation] [GetAllPotions]");
	std::list<RE::AlchemyItem*> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != itemmap.end()) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[ActorManipulation] [GetAllPotions] checking item");
				if (item && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion))) {
					LOG1_4("{}[ActorManipulation] [GetAllPotions] found potion {}", Utility::PrintForm(item));
					ret.insert(ret.begin(), item);
				}
			}
			iter++;
		}
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ACM::GetMatchingPoisons(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect)
{
	LOG_3("{}[ActorManipulation] [GetMatchingPoisons]");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		std::tuple<bool, float, int, AlchemyEffectBase, bool> res;
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != itemmap.end() && alchemyEffect != 0) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[ActorManipulation] [GetMatchingPoisons] checking item");
				if (item && (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison))) {
					LOG1_4("{}[ActorManipulation] [GetMatchingPoisons] found poison {}", Utility::PrintForm(item));
					if (acinfo->CanUsePoison(item->GetFormID()))
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
					else if (res = HasAlchemyEffect(item, alchemyEffect);
							 std::get<0>(res) &&
							 (Settings::Poisons::_AllowPositiveEffects || std::get<4>(res) == false /*either we allow poisons with positive effects, or there are no positive effects*/))	{
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
					}
				}
			}
			iter++;
		}
		LOG1_3("{}[ActorManipulation] [GetMatchingPoisons] finished. found: {} poisons", ret.size());
	}
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllPoisons(ActorInfo* acinfo)
{
	LOG_3("{}[ActorManipulation] [GetAllPoisons]");
	std::list<RE::AlchemyItem*> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != itemmap.end()) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[ActorManipulation] [GetAllPoisons] checking item");
				if (item && (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison))) {
					LOG1_4("{}[ActorManipulation] [GetAllPoisons] found poison {}", Utility::PrintForm(item));
					ret.insert(ret.begin(), item);
				}
			}
			iter++;
		}
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ACM::GetMatchingFood(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool raw)
{
	LOG_3("{}[ActorManipulation] [GetMatchingFood]");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		std::tuple<bool, float, int, AlchemyEffectBase, bool> res;
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != itemmap.end() && alchemyEffect != 0) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[ActorManipulation] [GetMatchingFood] checking item");
				if (item && raw == false && (item->IsFood() || item->HasKeyword(Settings::VendorItemFood))) {
					LOG1_4("{}[ActorManipulation] [GetMatchingFood] found food {}", Utility::PrintForm(item));
					if (acinfo->CanUseFood(item->GetFormID()))
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
					else if (res = HasAlchemyEffect(item, alchemyEffect);
							 std::get<0>(res) &&
							 (Settings::Food::_AllowDetrimentalEffects || std::get<4>(res) == false /*either we allow detrimental effects or there are none*/)) {
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
					}
				} else if (item && raw == true && item->HasKeyword(Settings::VendorItemFoodRaw)) {
					LOG1_4("{}[ActorManipulation] [GetMatchingFood] found food raw {}", Utility::PrintForm(item));
					if (acinfo->CanUseFood(item->GetFormID()))
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, static_cast<AlchemyEffectBase>(AlchemyEffect::kCustom) });
					else if (res = HasAlchemyEffect(item, alchemyEffect);
							 std::get<0>(res) &&
							 (Settings::Potions::_AllowDetrimentalEffects || std::get<4>(res) == false /*either we allow detrimental effects or there are none*/)) {
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
					}
				}
			}
			iter++;
		}
		LOG1_4("{}[ActorManipulation] [GetMatchingFood] return: {}", ret.size());
	}
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllFood(ActorInfo* acinfo)
{
	LOG_3("{}[ActorManipulation] [GetAllFood]");
	std::list<RE::AlchemyItem*> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		while (iter != itemmap.end()) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[GetAllFood] checking item");
				if (item && (item->IsFood() || item->HasKeyword(Settings::VendorItemFoodRaw) || item->HasKeyword(Settings::VendorItemFood))) {
					LOG1_4("{}[GetAllFood] found food {}", Utility::PrintForm(item));
					ret.insert(ret.begin(), item);
				}
			}
			iter++;
		}
	}
	return ret;
}

std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> ACM::GetRandomFood(ActorInfo* acinfo)
{
	LOG_3("{}[ActorManipulation] [GetRandomFood]");
	if (Utility::VerifyActorInfo(acinfo)) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> ret{};
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* item = nullptr;
		RE::EffectSetting* sett = nullptr;
		float mag = 0;
		int dur = 0;
		AlchemyEffect tmp = AlchemyEffect::kNone;
		AlchemyEffectBase out = 0;
		int count = 0;
		while (iter != itemmap.end() && count < 1000) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				item = iter->first->As<RE::AlchemyItem>();
				LOG_5("{}[ActorManipulation] [GetRandomFood] checking item");
				if (item && (item->IsFood() || item->HasKeyword(Settings::VendorItemFoodRaw) || item->HasKeyword(Settings::VendorItemFood))) {
					mag = 0;
					dur = 0;
					out = 0;
					if (item->effects.size() > 0) {
						for (uint32_t i = 0; i < item->effects.size(); i++) {
							sett = item->effects[i]->baseEffect;
							if (sett) {
								// skip food with detrimental effects
								if (sett->IsDetrimental()) {
									iter++;
									i++;
									continue;
								}
								if ((tmp = ConvertToAlchemyEffectPrimary(sett)) != AlchemyEffect::kNone) {
									out = static_cast<AlchemyEffectBase>(tmp);
									mag = item->effects[i]->effectItem.magnitude;
									dur = item->effects[i]->effectItem.duration;
									break;
								}
							}
						}
					}
					LOG1_4("{}[ActorManipulation] [GetRandomFood] found food {}", Utility::PrintForm(item));
					ret.insert(ret.begin(), { mag, dur, item, out });
				}
			}
			iter++;
			count++;
		}
		if (count == 1000) {
			logcritical("[ActorManipulation] [GetRandomFood] Maximum number of Items Exceeded! Forcibly excluding Actor {}", Utility::PrintForm(acinfo->actor));
			Distribution::ForceExcludeNPC(acinfo->actor->GetFormID());
			return { 0.0f, 0, nullptr, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
		}
		LOG1_4("{}[ActorManipulation] [GetRandomFood] number of found items: {}", ret.size());
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
			return { 0.0f, 0, nullptr, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
		}
	}
	return { 0.0f, 0, nullptr, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}

std::unordered_map<uint32_t, int> ACM::GetCustomItems(ActorInfo* acinfo)
{
	LOG_3("{}[ActorManipulation] [GetCustomItems]");
	std::unordered_map<uint32_t, int> ret{};
	if (Utility::VerifyActorInfo(acinfo)) {
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		while (iter != itemmap.end()) {
			if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
				if (acinfo->IsCustomItem(iter->first)) {
					ret.insert_or_assign(iter->first->GetFormID(), std::get<0>(iter->second));
					LOG1_4("{}[ActorManipulation] [GetCustomItems] found custom item {}", Utility::PrintForm(iter->first));
				}
			}
			iter++;
		}
		LOG1_4("{}[ActorManipulation] [GetCustomItems] return: {}", ret.size());
	}
	return ret;

}


std::vector<std::unordered_map<uint32_t, int>> ACM::GetCustomAlchItems(ActorInfo* acinfo)
{
	LOG_3("{}[ActorManipulation] [GetCustomAlchItems]");
	std::vector<std::unordered_map<uint32_t, int>> ret;
	std::unordered_map<uint32_t, int> all{};
	std::unordered_map<uint32_t, int> potions{};
	std::unordered_map<uint32_t, int> poisons{};
	std::unordered_map<uint32_t, int> fortify{};
	std::unordered_map<uint32_t, int> food{};
	if (Utility::VerifyActorInfo(acinfo)) {
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		RE::AlchemyItem* alch = nullptr;
		while (iter != itemmap.end()) {
			if (iter->first && std::get<1>(iter->second).get() &&
				std::get<1>(iter->second).get()->IsQuestObject() == false &&
				(alch = iter->first->As<RE::AlchemyItem>()) != nullptr &&
				acinfo->IsCustomAlchItem(alch)) {
				// check whether it a medicine and in the custom potion list
				if (alch->IsMedicine() && acinfo->citems->potionsset.contains(alch->GetFormID())) {
					fortify.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
					LOG1_4("{}[ActorManipulation] [GetCustomAlchItems] found custom potion {}", Utility::PrintForm(alch));
				} else if (alch->IsMedicine()) {
					potions.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
					LOG1_4("{}[ActorManipulation] [GetCustomAlchItems] found custom potion", Utility::PrintForm(alch));
				}
				if (alch->IsPoison()) {
					poisons.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
					LOG1_4("{}[ActorManipulation] [GetCustomAlchItems] found custom potion", Utility::PrintForm(alch));
				}
				if (alch->IsFood()) {
					food.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
					LOG1_4("{}[ActorManipulation] [GetCustomAlchItems] found custom potion", Utility::PrintForm(alch));
				}
			}
			iter++;
		}
	}
	ret.push_back(all);
	ret.push_back(potions);
	ret.push_back(poisons);
	ret.push_back(fortify);
	ret.push_back(food);
	return ret;
}

std::tuple<int, AlchemyEffectBase, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>> ACM::ActorUsePotion(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool compatibility)
{
	LOG_2("{}[ActorManipulation] [ActorUsePotion]");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		// if no effect is specified, return
		if (alchemyEffect == 0) {
			return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>{} };
		}
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		//RE::EffectSetting* sett = nullptr;
		LOG_2("{}[ActorManipulation] [ActorUsePotion] trying to find potion");
		auto ls = GetMatchingPotions(acinfo, alchemyEffect);
		ls.sort(Utility::SortMagnitude);
		ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> tup) { return (bool)(std::get<3>(tup) & ULong(AlchemyEffect::kCureDisease)) && acinfo->CanUsePot(std::get<2>(tup)->GetFormID()) == false; });
		// got all potions the actor has sorted by magnitude.
		// now use the one with the highest magnitude;
		return ActorUsePotion(acinfo, ls, compatibility);
	}
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>> lstemp;
	return { 0, 0, 0.0f, lstemp };
}

std::tuple<int, AlchemyEffectBase, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>> ACM::ActorUsePotion(ActorInfo* acinfo, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase>>& ls, bool compatibility)
{
	static auto CompPotAnimFx = [&acinfo]() {
		if (comp->LoadedAnimatedPotionFx()) {
			// compatibility mode for PotionAnimatedfx.esp activated, we may only use a potion if it is not on cooldown
			// if the actor does not have the cooldown effect we may use a potion
			if (comp->PAF_NPCDrinkingCoolDownEffect == nullptr || !(acinfo->actor->HasMagicEffect(comp->PAF_NPCDrinkingCoolDownEffect))) {
				return true;
			} else
				return false;
		}
		return true;
	};

	LOG_2("{}[ActorManipulation] [ActorUsePotion] list bound");
	if (Utility::VerifyActorInfo(acinfo)) {
		if (ls.size() > 0) {
			RE::AlchemyItem* potion;
			if (potion = std::get<2>(ls.front()); potion) {
				LOG1_2("{}[ActorManipulation] [ActorUsePotion] Drink Potion {}", Utility::PrintForm(potion));
				if (comp->LoadedAnimatedPotions() && acinfo->formid != 0x14) {
					LOG_2("{}[ActorManipulation] [ActorUsePotion] AnimatedPotions loaded, apply potion later");
					comp->AnPoti_AddActorPotion(acinfo->actor->GetFormID(), potion);

					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPotionsEvent");
					ev->strArg = RE::BSFixedString(std::to_string(potion->GetFormID()));
					ev->numArg = 0.0f;
					ev->sender = acinfo->actor;
					SKSE::GetModCallbackEventSource()->SendEvent(ev);

				} else if (Settings::CompatibilityPotionPapyrus() || compatibility) {
					LOG_3("{}[ActorManipulation] [ActorUsePotion] Compatibility Mode");
					if (!CompPotAnimFx()) {
						LOG_3("{}[ActorManipulation] [ActorUsePotion] Cannot use potion due to compatibility");
						return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, ls };
					}
					// save statistics
					Statistics::Misc_PotionsAdministered++;
					// preliminary, has check built in wether it applies
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
					if (!CompPotAnimFx()) {
						LOG_3("{}[ActorManipulation] [ActorUsePotion] Cannot use potion due to compatibility");
						return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, ls };
					}
					// save statistics
					Statistics::Misc_PotionsAdministered++;
					LOG_2("{}[ActorManipulation] [ActorUsePotion] equip potion");
					acinfo->actor->DrinkPotion(std::get<2>(ls.front()), nullptr);
					//RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), extra, 1, nullptr, true, false, false);
				}
				std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> val = ls.front();
				ls.pop_front();
				return { std::get<1>(val), std::get<3>(val), std::get<0>(val), ls };
			}
		}
	}
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone), 0.0f, ls };
}

std::pair<int, AlchemyEffectBase> ACM::ActorUseFood(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect, bool raw)
{
	LOG_2("{}[ActorManipulation] [ActorUseFood]");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		// if no effect is specified, return
		if (alchemyEffect == 0) {
			return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
		}
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		//RE::EffectSetting* sett = nullptr;
		LOG_2("{}[ActorManipulation] [ActorUseFood] trying to find food");
		auto ls = GetMatchingFood(acinfo, alchemyEffect, raw);
		//LOG_2("{}[ActorManipulation] [ActorUseFood] step1");
		ls.sort(Utility::SortMagnitude);
		ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> tup) { return (bool)(std::get<3>(tup) & ULong(AlchemyEffect::kCureDisease)) && acinfo->CanUseFood(std::get<2>(tup)->GetFormID()) == false; });
		//LOG_2("{}[ActorManipulation] [ActorUseFood] step2");
		// got all potions the actor has sorted by magnitude.
		// now use the one with the highest magnitude;
		if (ls.size() > 0 && std::get<2>(ls.front())) {
			// add statistics
			Statistics::Misc_FoodEaten++;
			LOG1_2("{}[ActorManipulation] [ActorUseFood] Use Food {}", Utility::PrintForm(std::get<2>(ls.front())));
			if (Settings::CompatibilityFoodPapyrus()) {
				LOG_3("{}[ActorManipulation] [ActorUseFood] Compatibility Mode");
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
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), nullptr, 1, nullptr, true, false, false);
			}
			return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
		}
		//LOG_2("{}[ActorManipulation] [ActorUseFood] step3");
	}
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}

std::pair<int, AlchemyEffectBase> ACM::ActorUseFood(ActorInfo* acinfo)
{
	LOG_2("{}[ActorManipulation] [ActorUseFood-Random]");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		LOG_2("{}[ActorManipulation] [ActorUseFood-Random] trying to find food");
		auto item = GetRandomFood(acinfo);
		//LOG_2("{}[ActorManipulation] [ActorUseFood-Random] step1");
		// use the random food
		if (std::get<2>(item)) {
			// save statistics
			Statistics::Misc_FoodEaten++;
			LOG1_2("{}[ActorManipulation] [ActorUseFood-Random] Use Food {}", Utility::PrintForm(std::get<2>(item)));
			if (Settings::CompatibilityFoodPapyrus()) {
				LOG_3("{}[ActorManipulation] [ActorUseFood-Random] Compatibility Mode");
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
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(item), nullptr, 1, nullptr, true, false, false);
			}
			return { std::get<1>(item), std::get<3>(item) };
		}
		//LOG_2("{}[ActorManipulation] [ActorUseFood-Random] step3");
	}
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}

/// <summary>
/// Game audiomanager which plays sounds.
/// </summary>
static RE::BSAudioManager* audiomanager;

std::pair<int, AlchemyEffectBase> ACM::ActorUsePoison(ActorInfo* acinfo, AlchemyEffectBase alchemyEffect)
{
	LOG_2("{}[ActorManipulation] [ActorUsePoison]");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		// if no effect is specified, return
		if (alchemyEffect == 0) {
			return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
		}
		auto itemmap = acinfo->actor->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		//RE::EffectSetting* sett = nullptr;
		LOG_2("{}[ActorManipulation] [ActorUsePoison] trying to find poison");
		auto ls = GetMatchingPoisons(acinfo, alchemyEffect);
		ls.sort(Utility::SortMagnitude);
		ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemyEffectBase> tup) { return (bool)(std::get<3>(tup) & ULong(AlchemyEffect::kCureDisease)) && acinfo->CanUsePoison(std::get<2>(tup)->GetFormID()) == false; });
		// got all potions the actor has sorted by magnitude.
		// now use the one with the highest magnitude;
		RE::AlchemyItem* poison;
		if (ls.size() > 0) {
			if (poison = std::get<2>(ls.front()); poison) {
				// save statistics
				Statistics::Misc_PoisonsUsed++;
				if (comp->LoadedAnimatedPoisons()) {
					LOG_2("{}[ActorManipulation] [ActorUsePoison] AnimatedPoisons loaded, apply poison later");
					comp->AnPois_AddActorPoison(acinfo->actor->GetFormID(), poison);

					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPoisonsEvent");
					ev->strArg = RE::BSFixedString(std::to_string(poison->GetFormID()));
					ev->numArg = 0.0f;
					ev->sender = acinfo->actor;
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
					return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
				} else {
					LOG1_2("{}[ActorManipulation] [ActorUsePoison] Use Poison {}", Utility::PrintForm(poison));
					if (!audiomanager)
						audiomanager = RE::BSAudioManager::GetSingleton();
					RE::ExtraDataList* extra = new RE::ExtraDataList();
					int dosage = data->GetPoisonDosage(poison);
					extra->Add(new RE::ExtraPoison(poison, dosage));
					auto ied = acinfo->actor->GetEquippedEntryData(false);
					if (ied) {
						ied->AddExtraList(extra);
						acinfo->actor->RemoveItem(poison, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
						{
							// play poison sound
							RE::BSSoundHandle handle;
							if (poison->data.consumptionSound)
								audiomanager->BuildSoundDataFromDescriptor(handle, poison->data.consumptionSound->soundDescriptor);
							else if (Settings::PoisonUse)
								audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
							handle.SetObjectToFollow(acinfo->actor->Get3D());
							handle.SetVolume(1.0);
							handle.Play();
						}
						return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
					} else {
						ied = acinfo->actor->GetEquippedEntryData(true);
						if (ied) {
							if (ied->object && ied->object->IsWeapon()) {
								ied->AddExtraList(extra);
								acinfo->actor->RemoveItem(poison, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
								{
									// play poison sound
									RE::BSSoundHandle handle;
									if (poison->data.consumptionSound)
										audiomanager->BuildSoundDataFromDescriptor(handle, poison->data.consumptionSound->soundDescriptor);
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
				}
			}
		}
	}
	return { -1, static_cast<AlchemyEffectBase>(AlchemyEffect::kNone) };
}
/* CTDs consistently when playing animations from here
bool ACM::AnimatedPoison_ApplyPoison(ActorInfo* acinfo, RE::AlchemyItem* poison)
{
	LOG2_4("{}[AnimatedPoison_ApplyPoison] actor {} poison {}", Utility::PrintForm(acinfo->actor), Utility::PrintForm(poison))

	// if parameters are invalid or compatibility disabled return
	if (!comp->LoadedAnimatedPoisons() || acinfo == nullptr || poison == nullptr || acinfo->Animation_busy || acinfo->actor == nullptr || acinfo->actor->IsPlayerRef()) {
		LOG2_4("{}[AnimatedPoison_ApplyPoison] {} {}", comp->LoadedAnimatedPoisons(), acinfo->Animation_busy);
		return false;
	}
	acinfo->Animation_busy = true;

	LOG_4("{}[AnimatedPoison_ApplyPoison] 1");

	// save form id, since this function will take longer to complete. If the actor is deleted in the meantime we may run into problems
	// if we do not reacquire all objects we are working with
	uint32_t actorid = acinfo->formid;

	// target condition check
	bool bisRiding = false;
	bool bAnimationDriven = false;
	bool isStaggering = false;
	bool isBleedingOut = false;
	if (acinfo->actor->GetFlyState() != RE::FLY_STATE::kNone || acinfo->actor->IsInKillMove() || acinfo->actor->IsSwimming() || acinfo->actor->GetKnockState() != RE::KNOCK_STATE_ENUM::kNormal) {
		acinfo->Animation_busy = false;
		return false;
	}
	if (acinfo->actor->GetGraphVariableBool("bisRiding", bisRiding) && bisRiding) {
		acinfo->Animation_busy = false;
		return false;
	}
	if (acinfo->actor->IsAnimationDriven()) {
		acinfo->Animation_busy = false;
		return false;
	}
	if (acinfo->actor->GetGraphVariableBool("isStaggering", isStaggering) && isStaggering) {
		acinfo->Animation_busy = false;
		return false;
	}
	if (acinfo->actor->GetGraphVariableBool("isBleedingOut", isBleedingOut) && isBleedingOut) {
		acinfo->Animation_busy = false;
		return false;
	}

	LOG_4("{}[AnimatedPoison_ApplyPoison] 2");


	// get poisonkeywtring // read json in future
	std::string AnimationEventString = "poisondamagehealthlinger01";
	// get animationeventstring
	// poison dosage ignored, own vales alreay used
	
	// do ui stuff
	RE::UI* ui = RE::UI::GetSingleton();
	//while (ui->GameIsPaused())
		//std::this_thread::sleep_for(100ms);
	// we don't what the player killed in the meantime using console etc. so use fallback
	acinfo = data->FindActor(actorid);
	if (acinfo == nullptr)
		return false;
	acinfo->actor->SetGraphVariableBool("bSprintOK", false);

	
	if (comp->AnPois_ToggleStopSprint->value == 1 && acinfo->actor->IsSprinting())
		true; // find out how to make actor stop sprinting
	if (Utility::GetItemType(acinfo->actor->GetEquippedObject(true)) != 7) {
		// draw weapon and wait for what?
		acinfo->actor->DrawWeaponMagicHands(true);
		bool isEquipping = false;
		//while (acinfo->actor->GetGraphVariableBool("isEquipping", isEquipping) && isEquipping)
		//	std::this_thread::sleep_for(100ms);
		acinfo = data->FindActor(actorid);
		if (acinfo == nullptr)
			return false;
	}

	LOG_4("{}[AnimatedPoison_ApplyPoison] 3");
	// ignore force third person
	bool lefthand = false;
	bool sloweffect = false;
	// left hand check
	RE::TESForm* leftitem = acinfo->actor->GetEquippedObject(true);
	RE::TESBoundObject* leftbound = nullptr;
	RE::SpellItem* leftspell = nullptr;
	if (leftitem)
		leftbound = leftitem->As<RE::TESBoundObject>();
	int itemtype = Utility::GetItemType(leftbound);
	if (itemtype == 9) // magic spell
	{
		RE::ActorEquipManager::GetSingleton()->UnequipObject(acinfo->actor, leftbound);
		leftspell = leftbound->As<RE::SpellItem>();
		lefthand = true;
	} else if (itemtype == 1 ||
			   itemtype == 2 ||
			   itemtype == 3 ||
			   itemtype == 4 ||
			   itemtype == 8 ||
			   itemtype == 10 ||
			   itemtype == 11) {
		RE::ActorEquipManager::GetSingleton()->UnequipObject(acinfo->actor, leftbound);
		lefthand = true;
	} else if (itemtype == 7) {
		// sheath weapon
		
		// currently unknown how to, unequip for now
		RE::ActorEquipManager::GetSingleton()->UnequipObject(acinfo->actor, leftbound);
		// choose arrow anim object
		RE::TESAmmo* ammo = acinfo->actor->GetCurrentAmmo();
		if (ammo) {
			if (ammo == comp->AnPois_DaedricArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_DaedricArrowAOSP);
			} else if (ammo == comp->AnPois_DraugrArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_DraugrArrowAOSP);
			} else if (ammo == comp->AnPois_DwarvenArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_DwarvenArrowAOSP);
			} else if (ammo == comp->AnPois_EbonyArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_EbonyArrowAOSP);
			} else if (ammo == comp->AnPois_ElvenArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_ElvenArrowAOSP);
			} else if (ammo == comp->AnPois_FalmerArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_FalmerArrowAOSP);
			} else if (ammo == comp->AnPois_ForswornArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_ForswornArrowAOSP);
			} else if (ammo == comp->AnPois_GlassArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_GlassArrowAOSP);
			} else if (ammo == comp->AnPois_IronArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_IronArrowAOSP);
			} else if (ammo == comp->AnPois_NordHeroArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_NordHeroArrowAOSP);
			} else if (ammo == comp->AnPois_OrcishArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_OrcishArrowAOSP);
			} else if (ammo == comp->AnPois_SteelArrow) {
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_SteelArrowAOSP);
			} else // iron
				acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_IronArrowAOSP);
		}
	}

	LOG_4("{}[AnimatedPoison_ApplyPoison] 4");

	// slow effect cast
	if (comp->AnPois_TogglePlayerSlowEffect->value == 1) {
		acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_SlowEffectSP);
		acinfo->actor->GetContainer()->AddObjectToContainer(comp->AnPois_SlowEffectItem, 1, nullptr);
		sloweffect = true;
	}

	LOG_4("{}[AnimatedPoison_ApplyPoison] 5");
	
	// send animation
	//std::this_thread::sleep_for(100ms);
	
	//acinfo->actor->NotifyAnimationGraph("TKDodgeForward");
	acinfo->actor->NotifyAnimationGraph(AnimationEventString);
	
	/* std::this_thread::sleep_for(2.6s);
	acinfo = data->FindActor(actorid);
	if (acinfo == nullptr)
		return false;

	LOG_4("{}[AnimatedPoison_ApplyPoison] 6");
	
	// requip lefthand weapon
	if (lefthand && leftbound)
	{
		if (leftspell)
			RE::ActorEquipManager::GetSingleton()->EquipSpell(acinfo->actor, leftspell, Settings::Equip_LeftHand);
		else
			RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, leftbound, nullptr, 1, Settings::Equip_LeftHand);
		// skip rest of equipping stuff for now
	}

	LOG_4("{}[AnimatedPoison_ApplyPoison] 7");


	if (sloweffect)
	{
		RE::ActorHandle handle = acinfo->actor->GetHandle();
		acinfo->actor->DispelEffect(comp->AnPois_SlowEffectSP, handle);
		acinfo->actor->RemoveItem(comp->AnPois_SlowEffectItem, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
	}

	LOG_4("{}[AnimatedPoison_ApplyPoison] 8");


	acinfo->actor->SetGraphVariableBool("bSprintOK", true);

	LOG_4("{}[AnimatedPoison_ApplyPoison] 9");
	*/
/*
	acinfo->Animation_busy = false;
	return true;
}


*/
