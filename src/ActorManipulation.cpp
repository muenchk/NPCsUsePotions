#include <random>
#include <tuple>
#include <vector>

#include "ActorManipulation.h"
#include "AlchemyEffect.h"
#include "Data.h"
#include "Statistics.h"


static std::mt19937 randan((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));

void ACM::Init()
{
	ACM::data = Data::GetSingleton();
	ACM::comp = Compatibility::GetSingleton();
}
// calc all alchemyeffects of the item and return the duration and magnitude of the effect with the highest product mag * dur
bool ACM::HasPoisonResistValue(RE::MagicItem* item)
{
	LOG_3("");
	auto [mapf, poison] = data->GetMagicItemPoisonResist(item->GetFormID());

	if (mapf) {
		if (poison) {
			LOG_4("fast success");
			return true;
		} else {
			LOG_4("fast fail: not a poison");
			return false;
		}
	} else {
		RE::EffectSetting* sett = nullptr;
		if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
			LOG_4("fail: no item effects");
			return false;
		}
		bool poison = false;
		if (item->effects.size() > 0) {
			for (uint32_t i = 0; i < item->effects.size(); i++) {
				sett = item->effects[i]->baseEffect;
				if (sett && sett->data.resistVariable == RE::ActorValue::kPoisonResist) {
					poison = true;
				}
			}
		} else {
			if (item->avEffectSetting && item->avEffectSetting->data.resistVariable == RE::ActorValue::kPoisonResist) {
				poison = true;
			}
		}
		
		LOG_4("slow doing");
		// save calculated values to data
		data->SetMagicItemPoisonResist(item->GetFormID(), poison);
		return poison;
	}
}

// calc all alchemyeffects of the item and return the duration and magnitude of the effect with the highest product mag * dur
std::tuple<bool, float, int, AlchemicEffect, bool> ACM::HasAlchemyEffect(RE::AlchemyItem* item, AlchemicEffect alchemyEffect, bool excluderestore)
{
	LOG_3("");
	// check if the ite is excluded
	if (Distribution::excludedItems()->contains(item->GetFormID()))
		return { false, -1.0f, -1, AlchemicEffect::kNone, false };
	auto [mapf, eff, dur, mag, detr, dosage] = data->GetAlchItemEffects(item->GetFormID());
	LOG_4("Item: {}, Effect: {}, Dur: {}, Mag: {}, Detr:{}, Dosage: {}", Utility::PrintForm(item), eff.string(), dur, mag, detr, dosage);
	static AlchemicEffect restoreeff = AlchemicEffect::kHealth | AlchemicEffect::kMagicka | AlchemicEffect::kStamina;
	if (mapf) {
		if ((eff & alchemyEffect) != 0 && (!excluderestore || (eff & restoreeff) == 0)) { // don't use potion if it has restore on it and we want to exclude them
			LOG_4("fast success");
			return { true, mag, dur, eff, detr };
		}
		else {
			LOG_4("fast fail: does not match effect");
			return { false, mag, dur, AlchemicEffect::kNone, false };
		}
	} else {
		RE::EffectSetting* sett = nullptr;
		RE::EffectSetting* prim = nullptr;
		bool detrimental = false;
		bool positive = false;
		if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
			LOG_4("fail: no item effects");
			return { false, 0.0f, 0, 0, false };
		}
		AlchemicEffect out = 0;
		AlchemicEffect tmp = 0;
		bool found = false;
		if (item->effects.size() > 0) {
			int durtmp = 0;
			float magtmp = 0;
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
					magtmp = item->effects[i]->effectItem.magnitude;
					if (magtmp == 0)
						magtmp = 1;
					durtmp = item->effects[i]->effectItem.duration;
					if (durtmp == 0)
						durtmp = 1;
					if ((tmp = (ConvertToAlchemyEffectPrimary(sett) & alchemyEffect)) > 0) {
						found = true;

						//LOG_1("alch effect id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.primaryAV));
						out |= tmp;
						if (mag * dur < magtmp * durtmp) {
							mag = magtmp;
							dur = durtmp;
						}
					} else if (tmp != 0)
						out |= tmp;
					if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = ((ConvertToAlchemyEffectSecondary(sett) & alchemyEffect))) > 0) {
						found = true;

						//LOG_1("alch effect2 id{} searched {}, target {}", std::to_string(tmp), alchemyEffect, ConvAlchULong(sett->data.secondaryAV));
						out |= tmp;
						if (mag * dur < magtmp * durtmp) {
							mag = magtmp;
							dur = durtmp;
						}
					} else if (tmp != 0)
						out |= tmp;
				}
			}
		} else {
			RE::MagicItem::SkillUsageData err;
			item->GetSkillUsageData(err);
			if (item->avEffectSetting && (out = ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV) & alchemyEffect) > 0) {
				detrimental |= item->avEffectSetting->IsDetrimental();
				positive |= !item->avEffectSetting->IsDetrimental();
				found = true;
				mag = err.magnitude;
			}
		}
		if (found && (!excluderestore || (out & restoreeff) == 0)) {
			//LOG_1("dur {} mag {}, effect {}, converted {}", dur, mag, out);
			LOG_4("slow success");
			// save calculated values to data
			dosage = Distribution::GetPoisonDosage(item, out);
			if (item->IsPoison()) {
				data->SetAlchItemEffects(item->GetFormID(), out, dur, mag, positive, dosage);
				return { true, mag, dur, out, positive };
			}
			data->SetAlchItemEffects(item->GetFormID(), out, dur, mag, detrimental, dosage);
			return { true, mag, dur, out, detrimental };
		}
		LOG_4("slow fail: does not match effect");
		return { false, mag, dur, AlchemicEffect::kNone, false };
	}
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ACM::GetMatchingPotions(std::shared_ptr<ActorInfo> const& acinfo, AlchemicEffect alchemyEffect, bool fortify)
{
	LOG_3("");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ret{};
	std::tuple<bool, float, int, AlchemicEffect, bool> res;
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (Utility::ValidateForm(iter->first) &&
			std::get<1>(iter->second).get() &&
			std::get<1>(iter->second).get()->IsQuestObject() == false && 
			(acinfo->IsPlayer() == false ||
				acinfo->IsPlayer() &&
					(Settings::player._UseFavoritedItemsOnly == false ||
						std::get<1>(iter->second).get()->IsFavorited()) &&
					(Settings::player._DontUseFavoritedItems == false ||
						std::get<1>(iter->second).get()->IsFavorited() == false) &&
					Distribution::excludedItemsPlayer()->contains(iter->first->GetFormID()) == false)) {
			item = iter->first->As<RE::AlchemyItem>();
			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && iter->second.first > 0) {
				LOG_5("checking item {}", Utility::PrintForm(item));
				if (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion)) {
					LOG_4("found medicine");
					if (fortify && acinfo->CanUseFortify(item->GetFormID()) || !fortify && acinfo->CanUsePotion(item->GetFormID())) {
						auto [mapf, eff, dur, mag, detr, dosage] = data->GetAlchItemEffects(item->GetFormID());
						ret.insert(ret.begin(), { mag, dur, item, AlchemicEffect::kCustom });
					} else if (res = HasAlchemyEffect(item, alchemyEffect, fortify);
							   std::get<0>(res) &&
							   (Settings::potions._AllowDetrimentalEffects || std::get<4>(res) == false /*either we allow detrimental effects or there are none*/)) {
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
						//LOG_1("dur {} mag {} effect {}", std::get<2>(res), std::get<1>(res), std::get<3>(res));
					}
				}
			}
		}
		iter++;
	}
	LOG_3("finished. found: {} potions", ret.size());
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	LOG_3("");
	std::list<RE::AlchemyItem*> ret{};
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	while (iter != itemmap.end()) {
		if (iter->first && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_5("checking item");
			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && iter->second.first > 0 && (item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion)) && Distribution::excludedItems()->contains(item->GetFormID()) == false) {
				LOG_4("found potion {}", Utility::PrintForm(item));
				ret.insert(ret.begin(), item);
			}
		}
		iter++;
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ACM::GetMatchingPoisons(std::shared_ptr<ActorInfo> const& acinfo, AlchemicEffect alchemyEffect)
{
	LOG_3("");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ret{};
	std::tuple<bool, float, int, AlchemicEffect, bool> res;
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (Utility::ValidateForm(iter->first) &&
			std::get<1>(iter->second).get() &&
			std::get<1>(iter->second).get()->IsQuestObject() == false &&
			(acinfo->IsPlayer() == false ||
				acinfo->IsPlayer() &&
					(Settings::player._UseFavoritedItemsOnly == false ||
						std::get<1>(iter->second).get()->IsFavorited()) &&
					(Settings::player._DontUseFavoritedItems == false ||
						std::get<1>(iter->second).get()->IsFavorited() == false) &&
					Distribution::excludedItemsPlayer()->contains(iter->first->GetFormID()) == false)) {
			item = iter->first->As<RE::AlchemyItem>();
			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && iter->second.first > 0) {
				LOG_5("checking item {}", Utility::PrintForm(item));
				if (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison)) {
					LOG_4("found poison");
					if (acinfo->CanUsePoison(item->GetFormID())) {
						auto [mapf, eff, dur, mag, detr, dosage] = data->GetAlchItemEffects(item->GetFormID());
						ret.insert(ret.begin(), { mag, dur, item, AlchemicEffect::kCustom });
					} else if (res = HasAlchemyEffect(item, alchemyEffect);
							   std::get<0>(res) &&
							   (Settings::poisons._AllowPositiveEffects || std::get<4>(res) == false /*either we allow poisons with positive effects, or there are no positive effects*/)) {
						LOG_3("Adding Poison to list: {}", Utility::PrintForm(item));
						ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
					}
				}
			}
		}
		iter++;
	}
	LOG_3("finished. found: {} poisons", ret.size());
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllPoisons(std::shared_ptr<ActorInfo> const& acinfo)
{
	LOG_3("");
	std::list<RE::AlchemyItem*> ret{};
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	while (iter != itemmap.end()) {
		if (iter->first && iter->second.first > 0 && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_5("checking item");
			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison)) && Distribution::excludedItems()->contains(item->GetFormID()) == false) {
				LOG_4("found poison {}", Utility::PrintForm(item));
				ret.insert(ret.begin(), item);
			}
		}
		iter++;
	}
	return ret;
}

std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ACM::GetMatchingFood(std::shared_ptr<ActorInfo> const& acinfo, AlchemicEffect alchemyEffect, bool raw)
{
	LOG_3("");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ret{};
	std::tuple<bool, float, int, AlchemicEffect, bool> res;
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	while (iter != itemmap.end() && alchemyEffect != 0) {
		if (Utility::ValidateForm(iter->first) &&
			std::get<1>(iter->second).get() &&
			std::get<1>(iter->second).get()->IsQuestObject() == false &&
			(acinfo->IsPlayer() == false ||
				acinfo->IsPlayer() &&
					(Settings::player._UseFavoritedItemsOnly == false ||
						std::get<1>(iter->second).get()->IsFavorited()) &&
					(Settings::player._DontUseFavoritedItems == false ||
						std::get<1>(iter->second).get()->IsFavorited() == false) &&
					Distribution::excludedItemsPlayer()->contains(iter->first->GetFormID()) == false)) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_5("checking item");

			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && iter->second.first > 0 &&
				(item->IsFood() ||
					item->HasKeyword(Settings::VendorItemFood) ||
					(item->HasKeyword(Settings::VendorItemFoodRaw))) &&
				(raw == true || !item->HasKeyword(Settings::VendorItemFoodRaw))) {
				LOG_4("found food {}", Utility::PrintForm(item));
				if (acinfo->CanUseFood(item->GetFormID())) {
					auto [mapf, eff, dur, mag, detr, dosage] = data->GetAlchItemEffects(item->GetFormID());
					ret.insert(ret.begin(), { mag, dur, item, AlchemicEffect::kCustom });
				} else if (res = HasAlchemyEffect(item, alchemyEffect);
						   std::get<0>(res) &&
						   (Settings::food._AllowDetrimentalEffects || std::get<4>(res) == false /*either we allow detrimental effects or there are none*/)) {
					ret.insert(ret.begin(), { std::get<1>(res), std::get<2>(res), item, std::get<3>(res) });
				}
			}
		}
		iter++;
	}
	LOG_4("return: {}", ret.size());
	return ret;
}

std::list<RE::AlchemyItem*> ACM::GetAllFood(std::shared_ptr<ActorInfo> const& acinfo)
{
	LOG_3("");
	std::list<RE::AlchemyItem*> ret{};
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	while (iter != itemmap.end()) {
		if (iter->first && iter->second.first > 0 && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_5("checking item");
			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && (item->IsFood() || item->HasKeyword(Settings::VendorItemFoodRaw) || item->HasKeyword(Settings::VendorItemFood)) && Distribution::excludedItems()->contains(item->GetFormID()) == false) {
				LOG_4("found food {}", Utility::PrintForm(item));
				ret.insert(ret.begin(), item);
			}
		}
		iter++;
	}
	return ret;
}

std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> ACM::GetRandomFood(std::shared_ptr<ActorInfo> const& acinfo, bool raw)
{
	LOG_3("");
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> ret{};
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* item = nullptr;
	RE::EffectSetting* sett = nullptr;
	float mag = 0;
	int dur = 0;
	AlchemicEffect tmp = AlchemicEffect::kNone;
	AlchemicEffect out = 0;
	int count = 0;
	std::tuple<bool, float, int, AlchemicEffect, bool> res;
	while (iter != itemmap.end() && count < 1000) {
		if (Utility::ValidateForm(iter->first) &&
			std::get<1>(iter->second).get() &&
			std::get<1>(iter->second).get()->IsQuestObject() == false &&
			(acinfo->IsPlayer() == false ||
				acinfo->IsPlayer() &&
					(Settings::player._UseFavoritedItemsOnly == false ||
						std::get<1>(iter->second).get()->IsFavorited()) &&
					(Settings::player._DontUseFavoritedItems == false ||
						std::get<1>(iter->second).get()->IsFavorited() == false))) {
			item = iter->first->As<RE::AlchemyItem>();
			LOG_5("checking item");
			// check whether the item has excluding keywords
			if (item && item->HasKeyword(comp->NUP_IgnoreItem) == false && iter->second.first > 0 &&
				(item->IsFood() ||
					item->HasKeyword(Settings::VendorItemFood) ||
					(item->HasKeyword(Settings::VendorItemFoodRaw))) &&
				(raw == true || !item->HasKeyword(Settings::VendorItemFoodRaw))) {
				if (acinfo->CanUseFood(item->GetFormID())) {
					auto [mapf, eff, dur2, mag2, detr, dosage] = data->GetAlchItemEffects(item->GetFormID());
					ret.insert(ret.begin(), { mag2, dur2, item, AlchemicEffect::kCustom });
				} else if (Distribution::excludedItems()->contains(item->GetFormID()) == false) {
					mag = 0;
					dur = 0;
					out = 0;
					int durtmp = 0;
					float magtmp = 0;
					if (item->effects.size() > 0) {
						for (uint32_t i = 0; i < item->effects.size(); i++) {
							sett = item->effects[i]->baseEffect;
							if (sett) {
								// skip food with detrimental effects
								if (sett->IsDetrimental()) {
									// break out of inner loop and set out to zero, which will result in skipping this item
									out = 0;
									break;
								}
								magtmp = item->effects[i]->effectItem.magnitude;
								if (magtmp == 0)
									magtmp = 1;
								durtmp = item->effects[i]->effectItem.duration;
								if (durtmp == 0)
									durtmp = 1;
								if ((tmp = ConvertToAlchemyEffectPrimary(sett)) != AlchemicEffect::kNone) {
									out |= tmp;
									if (mag * dur < magtmp * durtmp) {
										mag = magtmp;
										dur = durtmp;
									}
								}
								if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = ConvertToAlchemyEffectSecondary(sett)) != AlchemicEffect::kNone) {
									out |= tmp;
									if (mag * dur < magtmp * durtmp) {
										mag = magtmp;
										dur = durtmp;
									}
								}
							}
						}
					}
					if (out == 0) {
						iter++;
						count++;
						continue;
					}
					LOG_4("found food {}", Utility::PrintForm(item));
					ret.insert(ret.begin(), { mag, dur, item, out });
				}
			}
		}
		iter++;
		count++;
	}
	if (count == 1000) {
		logcritical("Maximum number of Items Exceeded! Forcibly excluding Actor {}", Utility::PrintForm(acinfo));
		Distribution::ForceExcludeNPC(acinfo->GetFormID());
		return { 0.0f, 0, nullptr, AlchemicEffect::kNone };
	}
	LOG_4("number of found items: {}", ret.size());
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
		return { 0.0f, 0, nullptr, AlchemicEffect::kNone };
	}
	return { 0.0f, 0, nullptr, AlchemicEffect::kNone };
}

std::unordered_map<uint32_t, int> ACM::GetCustomItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	LOG_3("");
	std::unordered_map<uint32_t, int> ret{};
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	while (iter != itemmap.end()) {
		if (iter->first && iter->second.first > 0 && std::get<1>(iter->second).get() && std::get<1>(iter->second).get()->IsQuestObject() == false) {
			if (acinfo->IsCustomItem(iter->first)) {
				ret.insert_or_assign(iter->first->GetFormID(), std::get<0>(iter->second));
				LOG_4("found custom item {}", Utility::PrintForm(iter->first));
			}
		}
		iter++;
	}
	LOG_4("return: {}", ret.size());
	return ret;
}


std::vector<std::unordered_map<uint32_t, int>> ACM::GetCustomAlchItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	LOG_3("");
	std::vector<std::unordered_map<uint32_t, int>> ret;
	std::unordered_map<uint32_t, int> all{};
	std::unordered_map<uint32_t, int> potions{};
	std::unordered_map<uint32_t, int> poisons{};
	std::unordered_map<uint32_t, int> fortify{};
	std::unordered_map<uint32_t, int> food{};
	auto itemmap = acinfo->GetInventory();
	auto iter = itemmap.begin();
	RE::AlchemyItem* alch = nullptr;
	while (iter != itemmap.end()) {
		if (iter->first && iter->second.first > 0 && std::get<1>(iter->second).get() &&
			std::get<1>(iter->second).get()->IsQuestObject() == false &&
			(alch = iter->first->As<RE::AlchemyItem>()) != nullptr &&
			alch->HasKeyword(comp->NUP_IgnoreItem) == false && 
			acinfo->IsCustomAlchItem(alch)) {
			// check whether it a medicine and in the custom potion list
			if (alch->IsMedicine() && acinfo->citems.potionsset.contains(alch->GetFormID())) {
				fortify.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
				LOG_4("found custom potion {}", Utility::PrintForm(alch));
			} else if (alch->IsMedicine()) {
				potions.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
				LOG_4("found custom potion", Utility::PrintForm(alch));
			}
			if (alch->IsPoison()) {
				poisons.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
				LOG_4("found custom potion", Utility::PrintForm(alch));
			}
			if (alch->IsFood()) {
				food.insert_or_assign(alch->GetFormID(), std::get<0>(iter->second));
				LOG_4("found custom potion", Utility::PrintForm(alch));
			}
		}
		iter++;
	}
	ret.push_back(all);
	ret.push_back(potions);
	ret.push_back(poisons);
	ret.push_back(fortify);
	ret.push_back(food);
	return ret;
}

std::tuple<int, AlchemicEffect, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>>> ACM::ActorUsePotion(std::shared_ptr<ActorInfo> const& acinfo, AlchemicEffect alchemyEffect, bool fortify)
{
	LOG_2("");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		// if no effect is specified, return
		if (alchemyEffect == 0) {
			return { -1, AlchemicEffect::kNone, 0.0f, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>>{} };
		}
		auto itemmap = acinfo->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		//RE::EffectSetting* sett = nullptr;
		LOG_2("trying to find potion");
		auto ls = GetMatchingPotions(acinfo, alchemyEffect, fortify);
		if (fortify)
			ls.sort(Utility::SortFortify);
		else
			ls.sort(Utility::SortPotion);
		ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> tup) { return (std::get<3>(tup) & AlchemicEffect::kCureDisease).IsValid() && acinfo->CanUsePot(std::get<2>(tup)->GetFormID()) == false; });
		// got all potions the actor has sorted by magnitude.
		// now use the one with the highest magnitude;
		return ActorUsePotion(acinfo, ls);
	}
	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> lstemp;
	return { 0, 0, 0.0f, lstemp };
}

std::tuple<int, AlchemicEffect, float, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>>> ACM::ActorUsePotion(std::shared_ptr<ActorInfo> const& acinfo, std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>>& ls)
{
	LOG_2("list bound");
	if (Utility::VerifyActorInfo(acinfo)) {
		if (ls.size() > 0) {
			RE::AlchemyItem* potion;
			if (potion = std::get<2>(ls.front()); potion) {
				std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> val = ls.front();
				LOG_2("Drink Potion {} with duration {} and magnitude {}", Utility::PrintForm(potion), std::get<1>(val), std::get<0>(val));
				logusage("Actor:\t{}\tItem:\t{}\tDuration:\t{}\tMagnitude:\t{}", acinfo->GetFormString(), Utility::PrintFormNonDebug(potion), std::get<1>(val), std::get<0>(val));
				if (comp->LoadedAnimatedPotions() && acinfo->IsPlayer() == false) {
					LOG_2("AnimatedPotions loaded, apply potion later");
					comp->AnPoti_AddActorPotion(acinfo->GetFormID(), potion);

					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPotionsEvent");
					ev->strArg = RE::BSFixedString(std::to_string(potion->GetFormID()));
					ev->numArg = 0.0f;
					ev->sender = acinfo->GetActor();
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
				} else {
					// save statistics
					Statistics::Misc_PotionsAdministered++;
					LOG_2("equip potion");

					SKSE::GetTaskInterface()->AddTask([acinfo, potion]() {
						RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->GetActor(), potion, nullptr, 1, nullptr, true, false, false);
					});
					//RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, std::get<2>(ls.front()), extra, 1, nullptr, true, false, false);
				}
				ls.pop_front();
				return { std::get<1>(val), std::get<3>(val), std::get<0>(val), ls };
			}
		}
	}
	return { -1, AlchemicEffect::kNone, 0.0f, ls };
}

std::pair<int, AlchemicEffect> ACM::ActorUseFood(std::shared_ptr<ActorInfo> const& acinfo, AlchemicEffect alchemyEffect, bool raw)
{
	LOG_2("");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		// if no effect is specified, return
		if (alchemyEffect == 0) {
			return { -1, AlchemicEffect::kNone };
		}
		auto itemmap = acinfo->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		//RE::EffectSetting* sett = nullptr;
		LOG_2("trying to find food");
		auto ls = GetMatchingFood(acinfo, alchemyEffect, raw);
		//LOG_2("step1");
		ls.sort(Utility::SortMagnitude);
		ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> tup) { return (std::get<3>(tup) & AlchemicEffect::kCureDisease).IsValid() && acinfo->CanUseFood(std::get<2>(tup)->GetFormID()) == false; });
		//LOG_2("step2");
		// got all potions the actor has sorted by magnitude.
		// now use the one with the highest magnitude;
		if (ls.size() > 0) {
			RE::AlchemyItem* food;
			if (food = std::get<2>(ls.front()); food) {
				// add statistics
				Statistics::Misc_FoodEaten++;
				LOG_2("Use Food {} with duration {} and magnitude {}", Utility::PrintForm(food), std::get<1>(ls.front()), std::get<0>(ls.front()));
				logusage("Actor:\t{}\tItem:\t{}\tDuration:\t{}\tMagnitude:\t{}", acinfo->GetFormString(), Utility::PrintFormNonDebug(food), std::get<1>(ls.front()), std::get<0>(ls.front()));
				SKSE::GetTaskInterface()->AddTask([acinfo, food]() {
					RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->GetActor(), food, nullptr, 1, nullptr, true, false, false);
				});
				return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
			}
		}
		//LOG_2("step3");
	}
	return { -1, AlchemicEffect::kNone };
}

std::pair<int, AlchemicEffect> ACM::ActorUseFood(std::shared_ptr<ActorInfo> const& acinfo, bool raw)
{
	LOG_2("");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		auto itemmap = acinfo->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		LOG_2("trying to find food");
		auto item = GetRandomFood(acinfo, raw);
		//LOG_2("step1");
		// use the random food
		if (RE::AlchemyItem* food = std::get<2>(item); food) {
			// save statistics
			Statistics::Misc_FoodEaten++;
			LOG_2("Use Food {} with duration {} and magnitude {}", Utility::PrintForm(std::get<2>(item)), std::get<1>(item), std::get<0>(item));
			logusage("Actor:\t{}\tItem:\t{}\tDuration:\t{}\tMagnitude:\t{}", acinfo->GetFormString(), Utility::PrintFormNonDebug(std::get<2>(item)), std::get<1>(item), std::get<0>(item));
			SKSE::GetTaskInterface()->AddTask([acinfo, food]() {
				RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->GetActor(), food, nullptr, 1, nullptr, true, false, false);
			});
			return { std::get<1>(item), std::get<3>(item) };
		}
		//LOG_2("step3");
	}
	return { -1, AlchemicEffect::kNone };
}

/// <summary>
/// Game audiomanager which plays sounds.
/// </summary>
static RE::BSAudioManager* audiomanager;

std::pair<int, AlchemicEffect> ACM::ActorUsePoison(std::shared_ptr<ActorInfo> const& acinfo, AlchemicEffect alchemyEffect)
{
	LOG_2("");
	if (Utility::VerifyActorInfo(acinfo)) {
		auto begin = std::chrono::steady_clock::now();
		// if no effect is specified, return
		if (alchemyEffect == 0) {
			return { -1, AlchemicEffect::kNone };
		}
		auto itemmap = acinfo->GetInventory();
		auto iter = itemmap.begin();
		auto end = itemmap.end();
		//RE::EffectSetting* sett = nullptr;
		LOG_2("trying to find poison");
		auto ls = GetMatchingPoisons(acinfo, alchemyEffect);
		ls.sort(Utility::SortMagnitude);
		ls.remove_if([acinfo](std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect> tup) { return (std::get<3>(tup) & AlchemicEffect::kCureDisease).IsValid() && acinfo->CanUsePoison(std::get<2>(tup)->GetFormID()) == false; });
		// got all potions the actor has sorted by magnitude.
		// now use the one with the highest magnitude;
		RE::AlchemyItem* poison;
		if (ls.size() > 0) {
			if (poison = std::get<2>(ls.front()); poison) {
				// save statistics
				Statistics::Misc_PoisonsUsed++;
				if (comp->LoadedAnimatedPoisons()) {
					LOG_2("AnimatedPoisons loaded, apply poison later");
					comp->AnPois_AddActorPoison(acinfo->GetFormID(), poison);

					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPoisonsEvent");
					ev->strArg = RE::BSFixedString(std::to_string(poison->GetFormID()));
					ev->numArg = 0.0f;
					ev->sender = acinfo->GetActor();
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
					return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
				} else {
					LOG_2("Use Poison {} with duration {} and magnitude {}", Utility::PrintForm(poison), std::get<1>(ls.front()), std::get<0>(ls.front()));
					logusage("Actor:\t{}\tItem:\t{}\tDuration:\t{}\tMagnitude:\t{}", acinfo->GetFormString(), Utility::PrintFormNonDebug(poison), std::get<1>(ls.front()), std::get<0>(ls.front()));
					if (!audiomanager)
						audiomanager = RE::BSAudioManager::GetSingleton();
					//RE::ExtraDataList* extra = new RE::ExtraDataList();
					int dosage = Distribution::GetPoisonDosage(poison, 0xFFFFFFFFFFFFFFFF, true);
					if (dosage == 0)
						dosage = acinfo->GetBasePoisonDosage(comp);
					if (dosage == 0)  // if the actor doesn't have the perk
						dosage = data->GetPoisonDosage(poison);
					LOG_2("Set poison dosage to {} for item {}.", dosage, Utility::PrintForm(poison));
					auto ied = acinfo->GetEquippedEntryData(false);
					if (ied) {
						ied->PoisonObject(poison, dosage);
						acinfo->RemoveItem(poison, 1);
						{
							// play poison sound
							RE::BSSoundHandle handle;
							if (poison->data.consumptionSound)
								audiomanager->BuildSoundDataFromDescriptor(handle, poison->data.consumptionSound->soundDescriptor);
							else if (Settings::PoisonUse)
								audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
							handle.SetObjectToFollow(acinfo->GetActor()->Get3D());
							handle.SetVolume(1.0);
							handle.Play();
						}
						return { std::get<1>(ls.front()), std::get<3>(ls.front()) };
					} else {
						ied = acinfo->GetEquippedEntryData(true);
						if (ied) {
							if (ied->object && ied->object->IsWeapon()) {
								ied->PoisonObject(poison, dosage);
								acinfo->RemoveItem(poison, 1);
								{
									// play poison sound
									RE::BSSoundHandle handle;
									if (poison->data.consumptionSound)
										audiomanager->BuildSoundDataFromDescriptor(handle, poison->data.consumptionSound->soundDescriptor);
									else
										audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
									handle.SetObjectToFollow(acinfo->GetActor()->Get3D());
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
	return { -1, AlchemicEffect::kNone };
}
/* CTDs consistently when playing animations from here
bool ACM::AnimatedPoison_ApplyPoison(std::shared_ptr<ActorInfo> const& acinfo, RE::AlchemyItem* poison)
{
	LOG2_4("[AnimatedPoison_ApplyPoison] actor {} poison {}", Utility::PrintForm(acinfo), Utility::PrintForm(poison))

	// if parameters are invalid or compatibility disabled return
	if (!comp->LoadedAnimatedPoisons() || acinfo == nullptr || poison == nullptr || acinfo->Animation_busy || acinfo->actor == nullptr || acinfo->actor->IsPlayerRef()) {
		LOG2_4("[AnimatedPoison_ApplyPoison] {} {}", comp->LoadedAnimatedPoisons(), acinfo->Animation_busy);
		return false;
	}
	acinfo->Animation_busy = true;

	LOG_4("[AnimatedPoison_ApplyPoison] 1");

	// save form id, since this function will take longer to complete. If the actor is dead in the meantime we may run into problems
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

	LOG_4("[AnimatedPoison_ApplyPoison] 2");


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

	LOG_4("[AnimatedPoison_ApplyPoison] 3");
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

	LOG_4("[AnimatedPoison_ApplyPoison] 4");

	// slow effect cast
	if (comp->AnPois_TogglePlayerSlowEffect->value == 1) {
		acinfo->actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)->SpellCast(true, 0, comp->AnPois_SlowEffectSP);
		acinfo->actor->GetContainer()->AddObjectToContainer(comp->AnPois_SlowEffectItem, 1, nullptr);
		sloweffect = true;
	}

	LOG_4("[AnimatedPoison_ApplyPoison] 5");
	
	// send animation
	//std::this_thread::sleep_for(100ms);
	
	//acinfo->actor->NotifyAnimationGraph("TKDodgeForward");
	acinfo->actor->NotifyAnimationGraph(AnimationEventString);
	
	/* std::this_thread::sleep_for(2.6s);
	acinfo = data->FindActor(actorid);
	if (acinfo == nullptr)
		return false;

	LOG_4("[AnimatedPoison_ApplyPoison] 6");
	
	// requip lefthand weapon
	if (lefthand && leftbound)
	{
		if (leftspell)
			RE::ActorEquipManager::GetSingleton()->EquipSpell(acinfo->actor, leftspell, Settings::Equip_LeftHand);
		else
			RE::ActorEquipManager::GetSingleton()->EquipObject(acinfo->actor, leftbound, nullptr, 1, Settings::Equip_LeftHand);
		// skip rest of equipping stuff for now
	}

	LOG_4("[AnimatedPoison_ApplyPoison] 7");


	if (sloweffect)
	{
		RE::ActorHandle handle = acinfo->actor->GetHandle();
		acinfo->actor->DispelEffect(comp->AnPois_SlowEffectSP, handle);
		acinfo->actor->RemoveItem(comp->AnPois_SlowEffectItem, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
	}

	LOG_4("[AnimatedPoison_ApplyPoison] 8");


	acinfo->actor->SetGraphVariableBool("bSprintOK", true);

	LOG_4("[AnimatedPoison_ApplyPoison] 9");
	*/
/*
	acinfo->Animation_busy = false;
	return true;
}


*/
