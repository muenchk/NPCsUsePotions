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
#include "Distribution.h"
#include "Settings.h"

using ItemType = Settings::ItemType;

static std::mt19937 randi((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
/// <summary>
/// trims random numbers to 1 to RR
/// </summary>
static std::uniform_int_distribution<signed> randRR(1, RandomRange);
static std::uniform_int_distribution<signed> rand100(1, 100);

#pragma region Rule

// private

Distribution::EffectDistr Distribution::Rule::GetScaledDistribution(Settings::ItemType type, EffectPreset* preset, std::shared_ptr<ActorInfo> const& acinfo)
{
	RE::TESCombatStyle* style = acinfo->GetCombatStyle();
	std::unordered_map<AlchemicEffect, Effect> effectMap = preset->effects;
	int customnum = 0;
	switch (type) {
	case Settings::ItemType::kFood:
		customnum = (int)acinfo->citems.foodset.size();
		for (auto& [effect, adjuster] : _probabilityAdjustersFood) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	case Settings::ItemType::kFortifyPotion:
		customnum = (int)acinfo->citems.fortifyset.size();
		for (auto& [effect, adjuster] : _probabilityAdjustersFortify) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	case Settings::ItemType::kPoison:
		customnum = (int)acinfo->citems.poisonsset.size();
		for (auto& [effect, adjuster] : _probabilityAdjustersPoison) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	case Settings::ItemType::kPotion:
		customnum = (int)acinfo->citems.potionsset.size();
		for (auto& [effect, adjuster] : _probabilityAdjustersPotion) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				float weight = eff.weight;
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	}

	if (effectMap.size() == 0 && customnum > 0)
	{
		Effect eff;
		eff.current = 0;
		eff.effect = AlchemicEffect::kCustom;
		eff.max = 0;
		eff.weight = 1000;
		effectMap.insert_or_assign(AlchemicEffect::kCustom, eff);
	}

	float scale = 0.0f;

	if (styleScaling && style) {
		// get primary combat score
		// and then scale the appropiate alchemic effects up by the scaling value
		int prefscore = 0;
		int prefscore2 = 0;
		double max = 0;
		double max2 = 0;
		if ((style->generalData.magicScoreMult == style->generalData.meleeScoreMult) == (style->generalData.meleeScoreMult == style->generalData.rangedScoreMult) == (style->generalData.rangedScoreMult == style->generalData.shoutScoreMult) == (style->generalData.staffScoreMult == style->generalData.magicScoreMult)) {
		} else {
			if (style->generalData.magicScoreMult > max) {
				max = style->generalData.magicScoreMult;
				prefscore = 0x01;  // magic
			} else if (style->generalData.magicScoreMult > max2) {
				max2 = style->generalData.magicScoreMult;
				prefscore2 = 0x01;
			}

			if (style->generalData.rangedScoreMult > max) {
				max = style->generalData.rangedScoreMult;
				prefscore = 0x02;
			} else if (style->generalData.rangedScoreMult > max2) {
				max2 = style->generalData.rangedScoreMult;
				prefscore2 = 0x02;
			}

			if (style->generalData.meleeScoreMult > max) {
				max = style->generalData.meleeScoreMult;
				prefscore = 0x04;
			} else if (style->generalData.meleeScoreMult > max2) {
				max2 = style->generalData.meleeScoreMult;
				prefscore2 = 0x04;
			}

			if (style->generalData.staffScoreMult > max) {
				max = style->generalData.staffScoreMult;
				prefscore = 0x08;
			} else if (style->generalData.staffScoreMult > max2) {
				max2 = style->generalData.staffScoreMult;
				prefscore2 = 0x08;
			}

			if (style->generalData.unarmedScoreMult > max) {
				max = style->generalData.unarmedScoreMult;
				prefscore = 0x10;
			} else if (style->generalData.unarmedScoreMult > max2) {
				max2 = style->generalData.unarmedScoreMult;
				prefscore2 = 0x10;
			}
		}
		scale = Settings::Distr::_StyleScalingPrimary;
		// scale general effects first
		auto itr = effectMap.find(AlchemicEffect::kDamageResist);
		if (itr != effectMap.end()) {
			auto eff = itr->second;
			eff.weight *= scale;
			effectMap.insert_or_assign(AlchemicEffect::kDamageResist, eff);
		}
		itr = effectMap.find(AlchemicEffect::kResistMagic);
		if (itr != effectMap.end()) {
			auto eff = itr->second;
			eff.weight *= scale;
			effectMap.insert_or_assign(AlchemicEffect::kResistMagic, eff);
		}
		itr = effectMap.find(AlchemicEffect::kPoisonResist);
		if (itr != effectMap.end()) {
			auto eff = itr->second;
			eff.weight *= scale;
			effectMap.insert_or_assign(AlchemicEffect::kPoisonResist, eff);
		}
		itr = effectMap.find(AlchemicEffect::kResistDisease);
		if (itr != effectMap.end()) {
			auto eff = itr->second;
			eff.weight *= scale;
			effectMap.insert_or_assign(AlchemicEffect::kResistDisease, eff);
		}
		itr = effectMap.find(AlchemicEffect::kReflectDamage);
		if (itr != effectMap.end()) {
			auto eff = itr->second;
			eff.weight *= scale;
			effectMap.insert_or_assign(AlchemicEffect::kReflectDamage, eff);
		}

		if (prefscore & 0x08) {  // staff
			if (prefscore2 & 0x08 && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
			goto GetScaledDistributionMagic;
		}
		if (prefscore & 0x01) {  // magic
			if (prefscore2 & 0x01 && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
GetScaledDistributionMagic:
			itr = effectMap.find(AlchemicEffect::kMagicka);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kMagicka, eff);
			}
			itr = effectMap.find(AlchemicEffect::kMagickaRate);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kMagickaRate, eff);
			}
			itr = effectMap.find(AlchemicEffect::kMagickaRateMult);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kMagickaRateMult, eff);
			}
			itr = effectMap.find(AlchemicEffect::kFortifyMagicka);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kFortifyMagicka, eff);
			}
			itr = effectMap.find(AlchemicEffect::kAlteration);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kAlteration, eff);
			}
			itr = effectMap.find(AlchemicEffect::kConjuration);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kConjuration, eff);
			}
			itr = effectMap.find(AlchemicEffect::kDestruction);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kDestruction, eff);
			}
			itr = effectMap.find(AlchemicEffect::kIllusion);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kIllusion, eff);
			}
			itr = effectMap.find(AlchemicEffect::kRestoration);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kRestoration, eff);
			}
		}
		scale = Settings::Distr::_StyleScalingPrimary;
		if (prefscore & 0x02) {  // ranged
			if (prefscore2 & 0x02 && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
			itr = effectMap.find(AlchemicEffect::kArchery);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kArchery, eff);
			}
			itr = effectMap.find(AlchemicEffect::kBowSpeed);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kBowSpeed, eff);
			}
		}
		scale = Settings::Distr::_StyleScalingPrimary;
		if (prefscore & 0x04) {  // melee
			if (prefscore2 & 0x04 && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
			itr = effectMap.find(AlchemicEffect::kOneHanded);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kOneHanded, eff);
			}
			itr = effectMap.find(AlchemicEffect::kTwoHanded);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kTwoHanded, eff);
			}
			itr = effectMap.find(AlchemicEffect::kBlock);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kBlock, eff);
			}
		}
		scale = Settings::Distr::_StyleScalingPrimary;
		if (prefscore & 0x10) {  // unarmed
			if (prefscore2 & 0x10 && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
			itr = effectMap.find(AlchemicEffect::kUnarmedDamage);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kUnarmedDamage, eff);
			}
		}
		scale = Settings::Distr::_StyleScalingPrimary;
		if (prefscore & 0x16) {
			// stamina pots
			if ((prefscore & 0x16) == (prefscore & 0x16) && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
			itr = effectMap.find(AlchemicEffect::kStamina);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kStamina, eff);
			}
			itr = effectMap.find(AlchemicEffect::kStaminaRate);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kStaminaRate, eff);
			}
			itr = effectMap.find(AlchemicEffect::kStaminaRateMult);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kStaminaRateMult, eff);
			}
			itr = effectMap.find(AlchemicEffect::kFortifyStamina);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kFortifyStamina, eff);
			}
			// weap speed
			itr = effectMap.find(AlchemicEffect::kWeaponSpeedMult);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kWeaponSpeedMult, eff);
			}
			itr = effectMap.find(AlchemicEffect::kAttackDamageMult);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kAttackDamageMult, eff);
			}
			itr = effectMap.find(AlchemicEffect::kCriticalChance);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kCriticalChance, eff);
			}
			itr = effectMap.find(AlchemicEffect::kHeavyArmor);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kHeavyArmor, eff);
			}
			itr = effectMap.find(AlchemicEffect::kLightArmor);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kLightArmor, eff);
			}
		}
		scale = Settings::Distr::_StyleScalingPrimary;
		if (prefscore & 0x14) {
			if ((prefscore & 0x14) == (prefscore2 & 0x14) && max != max2)
				scale = Settings::Distr::_StyleScalingSecondary;
			itr = effectMap.find(AlchemicEffect::kHealth);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kHealth, eff);
			}
			itr = effectMap.find(AlchemicEffect::kHealRate);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kHealRate, eff);
			}
			itr = effectMap.find(AlchemicEffect::kHealRateMult);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kHealRateMult, eff);
			}
			itr = effectMap.find(AlchemicEffect::kFortifyHealth);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kFortifyHealth, eff);
			}
			itr = effectMap.find(AlchemicEffect::kSpeedMult);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kSpeedMult, eff);
			}
			itr = effectMap.find(AlchemicEffect::kMeleeDamage);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kMeleeDamage, eff);
			}
		}
	}
	return GetEffectDistribution(effectMap);
}

AlchemicEffect Distribution::Rule::GetRandomEffect(EffectDistr& distr)
{
	std::uniform_int_distribution<signed> randDistr(1, distr.total);
	int random = randDistr(randi);
	int current = 0;
	auto itr = distr.effects.begin();
	while (itr != distr.effects.end()) {
		//logusage("Checking Effect: {}", std::get<1>(distribution[i]).string());
		current += itr->first;
		if (random <= current) {
			//logusage("Choosing Effect: {}, val {}", std::get<1>(distribution[i]).string(), random);
			itr->second.current++;
			auto ret = itr->second.effect;
			if (itr->second.max == itr->second.current) { // if it is larger, max was 0 and the max limit is disabled
				distr.total -= itr->first;
				distr.effects.erase(itr);
			}
			return ret;
		}
		itr++;
	}
	return AlchemicEffect::kNone;
}

RE::AlchemyItem* Distribution::Rule::GetRandomPotion(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
{
	str += potions->tieradjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffect(distr);
	loginfo("Effect: {}", eff.string());
		GetRandomPotioneff:;
	if (eff == AlchemicEffect::kCustom) {
		auto potions = acinfo->FilterCustomConditionsDistr(acinfo->citems.potions);
		if (potions.size() == 0) {
			distr.RemoveEffect(AlchemicEffect::kCustom);
			eff = GetRandomEffect(distr);
			goto GetRandomPotioneff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(potions.size() - 1));
		auto itm = potions[ra(randi)];
		return itm->object->As<RE::AlchemyItem>();
	} else if (eff == AlchemicEffect::kBlood) {
		items = Settings::GetMatchingItems(*Settings::potionsBlood(), eff);
	} else {
RetryPotion:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::potionsWeak_main(), eff);
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::potionsStandard_main(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::potionsPotent_main(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::potionsInsane_main(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		}
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0) {
		return items[r(randi)];
		//logusage("Looking for effect {}, gave {}", eff.string(), Utility::PrintForm(item));

	} else {
		//logusage("Looking for effect {}, item size 0", eff.string());
	}
	return nullptr;
}
RE::AlchemyItem* Distribution::Rule::GetRandomPoison(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
{
	str += poisons->tieradjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffect(distr);
GetRandomPoisoneff:;
	if (eff == AlchemicEffect::kCustom) {
		auto poisons = acinfo->FilterCustomConditionsDistr(acinfo->citems.poisons);
		if (poisons.size() == 0) {
			distr.RemoveEffect(AlchemicEffect::kCustom);
			eff = GetRandomEffect(distr);
			goto GetRandomPoisoneff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(poisons.size() - 1));
		auto itm = poisons[ra(randi)];
		//if (std::get<1>(itm) <= rand100(randi)) // calc chances
		return itm->object->As<RE::AlchemyItem>();
		//else
		//	return nullptr;
	} else {
RetryPoison:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::poisonsWeak(), eff);
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::poisonsStandard(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::poisonsPotent(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::poisonsInsane(), eff);
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
RE::AlchemyItem* Distribution::Rule::GetRandomFortifyPotion(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
{
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffect(distr);
GetRandomFortifyeff:;
	if (eff == AlchemicEffect::kCustom) {
		auto fortify = acinfo->FilterCustomConditionsDistr(acinfo->citems.fortify);
		if (fortify.size() == 0) {
			distr.RemoveEffect(AlchemicEffect::kCustom);
			eff = GetRandomEffect(distr);
			goto GetRandomFortifyeff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(fortify.size() - 1));
		auto itm = fortify[ra(randi)];
		return itm->object->As<RE::AlchemyItem>();
	} else {
RetryFortify:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::potionsWeak_rest(), eff);
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::potionsStandard_rest(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::potionsPotent_rest(), eff);
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::potionsInsane_rest(), eff);
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
RE::AlchemyItem* Distribution::Rule::GetRandomFood_intern(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
{
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffect(distr);
GetRandomFortifyeff:;
	if (eff == AlchemicEffect::kCustom) {
		auto fortify = acinfo->FilterCustomConditionsDistr(acinfo->citems.fortify);
		if (fortify.size() == 0) {
			distr.RemoveEffect(AlchemicEffect::kCustom);
			eff = GetRandomEffect(distr);
			goto GetRandomFortifyeff;
		}
		std::uniform_int_distribution<signed> ra(0, (int)(fortify.size() - 1));
		auto itm = fortify[ra(randi)];
		return itm->object->As<RE::AlchemyItem>();
	} else {
RetryFortify:
		items = Settings::GetMatchingItems(*Settings::foodall(), eff);
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}

// public

std::vector<RE::AlchemyItem*> Distribution::Rule::GetRandomPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	if (ruleVersion == 2 || ruleVersion == 3) {
		EffectDistr distr = potionEffects->standardDistr;
		if (styleScaling || Distribution::_probabilityAdjustersPotion.size() > 0) {
			distr = GetScaledDistribution(Settings::ItemType::kPotion, potionEffects, acinfo);
		}

		int astr = static_cast<int>(acinfo->GetActorStrength());
		int str = static_cast<int>(acinfo->GetItemStrength());
		int koeff = 0;
		switch (potions->cats[astr]->falloffFunc) {
		case FalloffFunction::Exponential:
			for (int i = 0; i < potions->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				loginfo("Potion Prob: {}", potions->cats[astr]->baseChance * std::pow(potions->cats[astr]->falloff, koeff) * Settings::Distr::_ProbabilityScaling);
				if (rand100(randi) <= potions->cats[astr]->baseChance * std::pow(potions->cats[astr]->falloff, i) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomPotion(str, acinfo, distr);
					ret.push_back(pot);
					loginfo("Potion: {}", Utility::PrintForm(pot));
				}
			}
			break;
		case FalloffFunction::Linear:
			for (int i = 0; i < potions->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				loginfo("Potion Prob: {}", (potions->cats[astr]->baseChance - potions->cats[astr]->falloff * koeff) * Settings::Distr::_ProbabilityScaling);
				if (rand100(randi) <= (potions->cats[astr]->baseChance - potions->cats[astr]->falloff * i) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomPotion(str, acinfo, distr);
					ret.push_back(pot);
					loginfo("Potion: {}", Utility::PrintForm(pot));
				}
			}
			break;
		}
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Distribution::Rule::GetRandomPoisons(std::shared_ptr<ActorInfo> const& acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	if (ruleVersion == 2 || ruleVersion == 3) {
		EffectDistr distr = poisonEffects->standardDistr;
		if (styleScaling || Distribution::_probabilityAdjustersPoison.size() > 0)
			distr = GetScaledDistribution(Settings::ItemType::kPoison, poisonEffects, acinfo);

		int astr = static_cast<int>(acinfo->GetActorStrength());
		int str = static_cast<int>(acinfo->GetItemStrength());
		int koeff = 0;
		switch (poisons->cats[astr]->falloffFunc) {
		case FalloffFunction::Exponential:
			for (int i = 0; i < poisons->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				if (rand100(randi) <= poisons->cats[astr]->baseChance * std::pow(poisons->cats[astr]->falloff, koeff) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomPoison(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		case FalloffFunction::Linear:
			for (int i = 0; i < poisons->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				if (rand100(randi) <= (poisons->cats[astr]->baseChance - poisons->cats[astr]->falloff * koeff) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomPoison(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		}
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Distribution::Rule::GetRandomFortifyPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	if (ruleVersion == 2 || ruleVersion == 3) {
		EffectDistr distr = fortifyEffects->standardDistr;
		if (styleScaling || Distribution::_probabilityAdjustersFortify.size() > 0)
			distr = GetScaledDistribution(Settings::ItemType::kFortifyPotion, fortifyEffects, acinfo);

		int astr = static_cast<int>(acinfo->GetActorStrength());
		int str = static_cast<int>(acinfo->GetItemStrength());
		int koeff = 0;
		switch (fortify->cats[astr]->falloffFunc) {
		case FalloffFunction::Exponential:
			for (int i = 0; i < fortify->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				if (rand100(randi) <= fortify->cats[astr]->baseChance * std::pow(fortify->cats[astr]->falloff, koeff) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomFortifyPotion(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		case FalloffFunction::Linear:
			for (int i = 0; i < fortify->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				if (rand100(randi) <= (fortify->cats[astr]->baseChance - fortify->cats[astr]->falloff * koeff) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomFortifyPotion(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		}
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Distribution::Rule::GetRandomFood(std::shared_ptr<ActorInfo> const& acinfo)
{
	std::vector<RE::AlchemyItem*> ret;
	if (ruleVersion == 2 || ruleVersion == 3) {
		EffectDistr distr = foodEffects->standardDistr;
		if (styleScaling || Distribution::_probabilityAdjustersFood.size() > 0)
			distr = GetScaledDistribution(Settings::ItemType::kFood, foodEffects, acinfo);

		int astr = static_cast<int>(acinfo->GetActorStrength());
		int str = static_cast<int>(acinfo->GetItemStrength());
		int koeff = 0;
		switch (food->cats[astr]->falloffFunc) {
		case FalloffFunction::Exponential:
			for (int i = 0; i < food->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				if (rand100(randi) <= food->cats[astr]->baseChance * std::pow(food->cats[astr]->falloff, koeff) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomFood_intern(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		case FalloffFunction::Linear:
			for (int i = 0; i < food->cats[astr]->max; i++) {
				if (potions->cats[astr]->falloffdelay > i)
					koeff = 0;
				else
					koeff = i - potions->cats[astr]->falloffdelay;
				if (rand100(randi) <= (food->cats[astr]->baseChance - food->cats[astr]->falloff * koeff) * Settings::Distr::_ProbabilityScaling) {
					auto pot = GetRandomFood_intern(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		}
	}
	return ret;
}

void Distribution::Rule::RemoveAlchemyEffectPotion(AlchemicEffect effect)
{
	potionEffects->effects.erase(effect);
	potionEffects->standardDistr = GetEffectDistribution(potionEffects->effects);
	potionEffects->validEffects = SumAlchemyEffects(potionEffects->standardDistr);
}

void Distribution::Rule::RemoveAlchemyEffectPoison(AlchemicEffect effect)
{
	poisonEffects->effects.erase(effect);
	poisonEffects->standardDistr = GetEffectDistribution(poisonEffects->effects);
	poisonEffects->validEffects = SumAlchemyEffects(poisonEffects->standardDistr);
}

void Distribution::Rule::RemoveAlchemyEffectFortifyPotion(AlchemicEffect effect)
{
	fortifyEffects->effects.erase(effect);
	fortifyEffects->standardDistr = GetEffectDistribution(fortifyEffects->effects);
	fortifyEffects->validEffects = SumAlchemyEffects(fortifyEffects->standardDistr);
}

void Distribution::Rule::RemoveAlchemyEffectFood(AlchemicEffect effect)
{
	foodEffects->effects.erase(effect);
	foodEffects->standardDistr = GetEffectDistribution(foodEffects->effects);
	foodEffects->validEffects = SumAlchemyEffects(foodEffects->standardDistr);
}

#pragma endregion

#pragma region Distribution

std::vector<RE::TESBoundObject*> Distribution::GetDistrItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	std::vector<RE::TESBoundObject*> ret;
	if (rule == Distribution::emptyRule) {
		// if there is no rule, we have to check for cutsom items anyway, so assign a default rule that 
		auto items = ACM::GetCustomAlchItems(acinfo);
		int diff = 0;
		if (Settings::Distr::_DistributePotions) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.potions);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::Distr::_ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[1].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
		if (Settings::Distr::_DistributePoisons) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.poisons);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::Distr::_ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[2].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
		if (Settings::Distr::_DistributeFortifyPotions) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.fortify);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::Distr::_ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[3].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
		if (Settings::Distr::_DistributeFortifyPotions) {
			auto ritems = acinfo->FilterCustomConditionsDistr(acinfo->citems.food);
			std::vector<RE::TESBoundObject*> vec;
			for (int i = 0; i < ritems.size(); i++) {
				auto item = ritems[i];
				if (acinfo->CalcDistrConditions(item) == false)
					continue;
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::Distr::_ProbabilityScaling) {
						vec.push_back(item->object);
					}
				}
			}
			diff = (int)(items[4].size()) - (int)vec.size();
			// if there are more items to be distributed than present give some
			if (diff < 0) {
				vec.resize(-diff);
				ret.insert(ret.end(), vec.begin(), vec.end());
			}
		}
	} else {
		if (Settings::Distr::_DistributePotions) {
			auto ritems = rule->GetRandomPotions(acinfo);
			acinfo->potionDistr.clear();
			acinfo->potionDistr.shrink_to_fit();
			LOG_4("matching potions");
			auto items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, true);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				ritems.resize(diff);
				LOG_4("potions size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("potions to give:\t{}", ritems.size());
		}
		if (Settings::Distr::_DistributePoisons) {
			auto ritems = rule->GetRandomPoisons(acinfo);
			acinfo->poisonDistr.clear();
			acinfo->poisonDistr.shrink_to_fit();
			LOG_4("matching poisons");
			auto items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				ritems.resize(diff);
				LOG_4("poisons size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("poisons to give:\t{}", ritems.size());
		}
		if (Settings::Distr::_DistributeFortifyPotions) {
			auto ritems = rule->GetRandomFortifyPotions(acinfo);
			acinfo->fortifyDistf.clear();
			acinfo->fortifyDistf.shrink_to_fit();
			LOG_4("matching fortify");
			auto items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				ritems.resize(diff);
				LOG_4("fortify size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("fortify potions to give:\t{}", ritems.size());
		}
		if (Settings::Distr::_DistributeFood) {
			auto ritems = rule->GetRandomFood(acinfo);
			acinfo->foodDistr.clear();
			acinfo->foodDistr.shrink_to_fit();
			LOG_4("matching food");
			auto items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
			int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
			LOG_4("diff: {}", diff);
			// if the number of found items is less then the number of items to add
			// then add the difference in numbers
			if (diff > 0) {
				LOG_4("diff greater 0");
				ritems.resize(diff);
				LOG_4("food size: {}", std::to_string(ritems.size()));
				ret.insert(ret.end(), ritems.begin(), ritems.end());
			}
			//LOG_4("food to give:\t{}", ritems.size());
		}
	}
	// custom generic items are distributed whenever one of the options above is used
	if (Settings::Distr::_DistributeCustomItems) {
		std::unordered_map<uint32_t, int> items = ACM::GetCustomItems(acinfo);
		auto ritems = acinfo->FilterCustomConditionsDistrItems(acinfo->citems.items);
		for (int i = 0; i < ritems.size(); i++) {
			auto item = ritems[i];
			if (item->giveonce == true && acinfo->DistributedItems())  // if item is only given once and we already gave items: skip
				continue;
			if (acinfo->CalcDistrConditions(item) == false)
				continue;
			auto itr = items.find(item->object->GetFormID());
			if (itr == items.end()) {
				// run for number of items to give
				for (int x = 0; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::Distr::_ProbabilityScaling) {
						ret.push_back(item->object);
					}
				}
			} else {
				for (int x = itr->second; x < item->num; x++) {
					if (rand100(randi) < item->chance * Settings::Distr::_ProbabilityScaling) {
						ret.push_back(item->object);
					}
				}
			}
		}
		acinfo->DistributedItems(true);
	}
	if (ret.size() > 0 && ret.back() == nullptr) {
		LOG_4("remove last item");
		ret.pop_back();
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomPotions(acinfo);
	acinfo->potionDistr.clear();
	acinfo->potionDistr.shrink_to_fit();
	auto items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrPoisons(std::shared_ptr<ActorInfo> const& acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomPoisons(acinfo);
	acinfo->poisonDistr.clear();
	acinfo->poisonDistr.shrink_to_fit();
	auto items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrFortifyPotions(std::shared_ptr<ActorInfo> const& acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomFortifyPotions(acinfo);
	acinfo->fortifyDistf.clear();
	acinfo->fortifyDistf.shrink_to_fit();
	auto items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Distribution::GetDistrFood(std::shared_ptr<ActorInfo> const& acinfo)
{
	Rule* rule = CalcRule(acinfo, nullptr);
	auto ritems = rule->GetRandomFood(acinfo);
	acinfo->foodDistr.clear();
	acinfo->foodDistr.shrink_to_fit();
	auto items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}

std::vector<RE::AlchemyItem*> Distribution::GetMatchingInventoryItemsUnique(std::shared_ptr<ActorInfo> const& acinfo)
{
	Rule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::Distr::_DistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::Distr::_DistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::Distr::_DistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::Distr::_DistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0 && ret.back() == nullptr)
		ret.pop_back();
	return ret;
}

std::vector<RE::AlchemyItem*> Distribution::GetMatchingInventoryItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	//LOG_4("[SettingsDistribution] GetMatchingInventoryItems enter");
	Rule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::Distr::_DistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->potionEffects->validEffects, false);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| potions {} | found: {}", Utility::GetHex(rule->validPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::Distr::_DistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPoisons(acinfo, rule->poisonEffects->validEffects);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| poisons {} | found: {}", Utility::GetHex(rule->validPoisons), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::Distr::_DistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, rule->fortifyEffects->validEffects, true);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| fortify {} | found: {}", Utility::GetHex(rule->validFortifyPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::Distr::_DistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingFood(acinfo, rule->foodEffects->validEffects, false);
		//LOG_4("[SettingsDistribution] GetMatchingInventoryItems| food {} | found: {}", Utility::GetHex(rule->validFood), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = acinfo->GetInventoryCounts();
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

std::vector<RE::AlchemyItem*> Distribution::GetAllInventoryItems(std::shared_ptr<ActorInfo> const& acinfo)
{
	//LOG_4("[SettingsDistribution] GetMatchingInventoryItems enter");
	Rule* rule = CalcRule(acinfo);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::Distr::_DistributePotions || Settings::Distr::_DistributeFortifyPotions) {
		std::list<RE::AlchemyItem*> items = ACM::GetAllPotions(acinfo);
		LOG_4("Potions: {}", items.size());
		for (auto i : items) {
			ret.push_back(i);
		}
	}
	//if (Settings::Distr::_DistributePotions) {
	//	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, AlchemicEffect::kAnyPotion, false);
	//	for (auto i : items) {
	//		ret.insert(ret.end(), std::get<2>(i));
	//	}
	//}
	if (Settings::Distr::_DistributePoisons) {
		std::list<RE::AlchemyItem*> items = ACM::GetAllPoisons(acinfo);
		LOG_4("Poisons: {}", items.size());
		for (auto i : items) {
			ret.push_back(i);
		}
	}
	//if (Settings::Distr::_DistributeFortifyPotions) {
	//	std::list<std::tuple<float, int, RE::AlchemyItem*, AlchemicEffect>> items = ACM::GetMatchingPotions(acinfo, AlchemicEffect::kAnyFortify, true);
	//	for (auto i : items) {
	//		ret.insert(ret.end(), std::get<2>(i));
	//	}
	//}
	if (Settings::Distr::_DistributeFood) {
		std::list<RE::AlchemyItem*> items = ACM::GetAllFood(acinfo);
		LOG_4("Food: {}", items.size());
		for (auto i : items) {
			ret.push_back(i);
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = acinfo->GetInventoryCounts();
		size_t currsize = ret.size();
		for (int i = 0; i < currsize; i++) {
			if (auto it = map.find(ret[i]); it != map.end()) {
				if (it->second > 1)
					for (int c = 1; c < it->second; c++)
						ret.push_back(ret[i]);
			}
		}
	}
	LOG_4("Total: {}", ret.size());
	return ret;
}

void Distribution::FilterDistributionExcludedItems(std::vector<RE::AlchemyItem*>& items)
{
	auto ritr = items.begin();
	while (ritr != items.end()) {
		if (_excludedDistrItems.contains((*ritr)->GetFormID()))
			ritr == items.erase(ritr);
		else
			ritr++;
	}
}

int Distribution::GetPoisonDosage(RE::AlchemyItem* poison, AlchemicEffect effects, bool forcenonzero)
{
	int dosage = 0;
	auto itr = dosageItemMap()->find(poison->GetFormID());
	if (itr != dosageItemMap()->end()) {
		auto [force, setting, dos] = itr->second;
		if (force && setting && forcenonzero == false)
			dosage = Settings::Poisons::_Dosage;
		else if (force)
			dosage = dos;
		else if (setting && forcenonzero == false)
			dosage = Settings::Poisons::_Dosage;
		else if (Settings::Poisons::_BaseDosage == Settings::Poisons::_Dosage)
			dosage = dos;
	}
	// we evaluated the item specific dosage
	// if we did not find anything check the effect specific dosages
	if (dosage != 0) {
		// find the minimum explicit value we can apply
		std::vector<AlchemicEffect> effvec = AlchEff::GetAlchemyEffects(effects);
		int min = INT_MAX;
		for (int i = 0; i < effvec.size(); i++) {
			auto itra = dosageEffectMap()->find(effvec[i]);
			if (itra != dosageEffectMap()->end()) {
				auto [force, setting, dos] = itr->second;
				if (force && setting && forcenonzero == false)
					dos = Settings::Poisons::_Dosage;
				else if (force)
					dos = dos;
				else if (setting && forcenonzero == false)
					dos = Settings::Poisons::_Dosage;
				else if (Settings::Poisons::_BaseDosage == Settings::Poisons::_Dosage)
					dos = dos;
				if (dos < min)
					min = dos;
			}
		}
		if (min < INT_MAX)
			dosage = min;
	}
	// if we did not find anything at all, assign the setting
	if (dosage <= 0 && forcenonzero == false)
		dosage = Settings::Poisons::_Dosage;
	return dosage;
}

bool Distribution::ExcludedNPC(std::shared_ptr<ActorInfo> const& acinfo)
{
	if (!acinfo->IsValid() || acinfo->GetActor() == nullptr)
		return true;
	//LOG_1("1");
	if (Settings::Whitelist::EnabledNPCs) {
		if (acinfo->IsWhitelistCalculated()) {
			if (!acinfo->IsWhitelisted())
				return true;
		} else {
			CalcRule(acinfo);
			if (!acinfo->IsWhitelisted())
				return true;
		}
	}
	bool ret = Distribution::excludedNPCs()->contains(acinfo->GetFormID());
	ret |= Distribution::excludedPlugins_NPCs()->contains(acinfo->GetPluginID());
	ret |= Distribution::excludedPlugins_NPCs()->contains(acinfo->GetRaceFormID());
	ret |= acinfo->IsFollower();
	ret |= (Distribution::excludedNPCs()->contains(acinfo->GetFormIDOriginal()));
	for (auto& id : acinfo->GetTemplateIDs())
		ret |= Distribution::excludedNPCs()->contains(id);
	ret |= acinfo->IsGhost();
	ret |= acinfo->IsSummonable();
	if (acinfo->Bleeds() == false && Utility::ToLower(acinfo->GetActorBaseFormEditorID()).find("ghost") != std::string::npos) {
		Distribution::ForceExcludeNPC(acinfo->GetFormID());
		return true;
	}
	//LOG_1("4 {}", ret);
	// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
	if (!Distribution::npcMap()->contains(acinfo->GetFormID()) && !Distribution::npcMap()->contains(acinfo->GetFormIDOriginal()) && ret == false) {
		auto base = acinfo->GetActorBase();
		for (uint32_t i = 0; i < base->numKeywords; i++) {
			if (base->keywords[i])
				ret |= Distribution::excludedAssoc()->contains(base->keywords[i]->GetFormID());
		}
		//LOG1_1("5 {}", ret);
		for (uint32_t i = 0; i < base->factions.size(); i++) {
			if (base->factions[i].faction)
				ret |= Distribution::excludedAssoc()->contains(base->factions[i].faction->GetFormID());
		}
		auto race = acinfo->GetRace();
		if (race) {
			ret |= Distribution::excludedAssoc()->contains(race->GetFormID());
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				ret |= Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID());
			}
		}
	}
	//LOG1_1("7 {}", ret);
	return ret;
}

/// <summary>
/// returns whether the npc should no be handled at all
/// </summary>
/// <param name="actor"></param>
/// <returns></returns>
bool Distribution::ExcludedNPCFromHandling(RE::Actor* actor)
{
	if (actor->formFlags & RE::TESForm::RecordFlags::kDeleted)
		return true;
	if (Settings::Usage::_DisableItemUsageForExcludedNPCs) {
		ID id = ID(actor);
		// only view them as excluded from handling if they are either excluded themselves, or their race is excluded
		bool ret = Distribution::excludedNPCs()->contains(id);
		ret |= Distribution::excludedPlugins_NPCs()->contains(Utility::Mods::GetPluginIndex(actor));
		ret |= (Utility::Mods::GetPluginIndex(actor) == 0x1 && Distribution::excludedPlugins_NPCs()->contains(Utility::ExtractTemplateInfo(actor->GetActorBase()).pluginID));
		ret |= actor->GetActorBase() && Distribution::excludedNPCs()->contains(id.GetOriginalID());
		ret |= actor->IsGhost();
		ret |= actor->GetActorBase() && actor->GetActorBase()->IsSummonable();
		if (ret == false && !Distribution::npcMap()->contains(id) && !Distribution::npcMap()->contains(id.GetOriginalID())) {
			auto race = actor->GetRace();
			if (race) {
				ret |= Distribution::excludedPlugins_NPCs()->contains(race->GetFormID());
				ret |= Distribution::excludedAssoc()->contains(race->GetFormID());
				for (uint32_t i = 0; i < race->numKeywords; i++) {
					ret |= Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID());
				}
			}
		}
		return ret;
	}
	return false;
}

/// <summary>
/// returns wether an npc is excluded from item distribution
/// </summary>
/// <param name="npc">ActorBase to check</param>
/// <returns></returns>
bool Distribution::ExcludedNPC(RE::TESNPC* npc)
{
	// skip fucking deleted references
	if (npc->formFlags & RE::TESForm::RecordFlags::kDeleted)
		return true;
	bool ret = (Distribution::excludedNPCs()->contains(npc->GetFormID()));
	ret |= npc->IsInFaction(Settings::CurrentFollowerFaction);
	ret |= npc->IsInFaction(Settings::CurrentHirelingFaction);
	ret |= npc->IsGhost();
	ret |= npc->IsSummonable();
	if (npc->Bleeds() == false && Utility::ToLower(std::string(npc->GetFormEditorID())).find("ghost") != std::string::npos) {
		Distribution::ForceExcludeNPC(npc->GetFormID());
		return true;
	}
	// if the actor has an exclusive rule then this goes above Race, Faction and Keyword exclusions
	if (!Distribution::npcMap()->contains(npc->GetFormID()) && ret == false) {
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				ret |= Distribution::excludedAssoc()->contains(npc->keywords[i]->GetFormID());
		}
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				ret |= Distribution::excludedAssoc()->contains(npc->factions[i].faction->GetFormID());
		}
		auto race = npc->GetRace();
		if (race) {
			ret |= Distribution::excludedAssoc()->contains(race->GetFormID());
			for (uint32_t i = 0; i < race->numKeywords; i++) {
				ret |= Distribution::excludedAssoc()->contains(race->keywords[i]->GetFormID());
			}
		}
	}
	return ret;
}

bool Distribution::ForceExcludeNPC(uint32_t actorid)
{
	_excludedNPCs.insert(actorid);
	return true;
}

Distribution::Rule* Distribution::CalcRule(RE::TESNPC* npc, ActorStrength& acs, ItemStrength& is, Misc::NPCTPLTInfo* tpltinfo, CustomItemStorage* custItems)
{
	// calc strength section
	if (Settings::Distr::_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty;
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
	int acsadj = 0;
	auto adjustacs = [&acsadj](RE::FormID id) {
		try {
			acsadj += actorStrengthMap()->at(id);
		} catch (std::out_of_range&) {
		}
	};

	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	std::vector<CustomItemStorage*>* citems = nullptr;
	std::unordered_set<std::string>* citemsset = nullptr;
	if (custItems) {
		citems = new std::vector<CustomItemStorage*>{};
		citemsset = new std::unordered_set<std::string>{};
	}

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
	adjustacs(npc->GetFormID());
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(npc->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			/* for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}*/
			/* for (int b = 0; b < vec.size(); b++) {
				for (int c = 0; c < vec[b]->items.size(); c++) {
					custItems->items.push_back(vec[b]->items[c]);
				}
				for (int c = 0; c < vec[b]->death.size(); c++) {
					custItems->death.push_back(vec[b]->death[c]);
				}
				for (int c = 0; c < vec[b]->poisons.size(); c++) {
					custItems->poisons.push_back(vec[b]->poisons[c]);
				}
				for (int c = 0; c < vec[b]->potions.size(); c++) {
					custItems->potions.push_back(vec[b]->potions[c]);
				}
				for (int c = 0; c < vec[b]->fortify.size(); c++) {
					custItems->fortify.push_back(vec[b]->fortify[c]);
				}
			}*/
			for (int b = 0; b < vec.size(); b++) {
				if (citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
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
				bossoverride |= bosses()->contains(race->keywords[i]->GetFormID());
				adjustacs(race->keywords[i]->GetFormID());
			}
		}
	}
	bossoverride |= bosses()->contains(npc->GetRace()->GetFormID());
	adjustacs(npc->GetRace()->GetFormID());
	// get custom items
	if (custItems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			/* for (int d = 0; d < vec.size(); d++) {
				custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
				custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
				custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
				custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
				custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
			}*/
			/* for (int b = 0; b < vec.size(); b++) {
				for (int c = 0; c < vec[b]->items.size(); c++) {
					custItems->items.push_back(vec[b]->items[c]);
				}
				for (int c = 0; c < vec[b]->death.size(); c++) {
					custItems->death.push_back(vec[b]->death[c]);
				}
				for (int c = 0; c < vec[b]->poisons.size(); c++) {
					custItems->poisons.push_back(vec[b]->poisons[c]);
				}
				for (int c = 0; c < vec[b]->potions.size(); c++) {
					custItems->potions.push_back(vec[b]->potions[c]);
				}
				for (int c = 0; c < vec[b]->fortify.size(); c++) {
					custItems->fortify.push_back(vec[b]->fortify[c]);
				}
			}*/
			for (int b = 0; b < vec.size(); b++) {
				if (citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				/*for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}*/
				/*for (int b = 0; b < vec.size(); b++) {
					for (int c = 0; c < vec[b]->items.size(); c++) {
						custItems->items.push_back(vec[b]->items[c]);
					}
					for (int c = 0; c < vec[b]->death.size(); c++) {
						custItems->death.push_back(vec[b]->death[c]);
					}
					for (int c = 0; c < vec[b]->poisons.size(); c++) {
						custItems->poisons.push_back(vec[b]->poisons[c]);
					}
					for (int c = 0; c < vec[b]->potions.size(); c++) {
						custItems->potions.push_back(vec[b]->potions[c]);
					}
					for (int c = 0; c < vec[b]->fortify.size(); c++) {
						custItems->fortify.push_back(vec[b]->fortify[c]);
					}
				}*/
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
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
			adjustacs(key->GetFormID());
			// get custom items
			if (custItems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					/* for (int d = 0; d < vec.size(); d++) {
						custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
						custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
						custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
						custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
						custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
					}*/
					/* for (int b = 0; b < vec.size(); b++) {
						for (int c = 0; c < vec[b]->items.size(); c++) {
							custItems->items.push_back(vec[b]->items[c]);
						}
						for (int c = 0; c < vec[b]->death.size(); c++) {
							custItems->death.push_back(vec[b]->death[c]);
						}
						for (int c = 0; c < vec[b]->poisons.size(); c++) {
							custItems->poisons.push_back(vec[b]->poisons[c]);
						}
						for (int c = 0; c < vec[b]->potions.size(); c++) {
							custItems->potions.push_back(vec[b]->potions[c]);
						}
						for (int c = 0; c < vec[b]->fortify.size(); c++) {
							custItems->fortify.push_back(vec[b]->fortify[c]);
						}
					}*/
					for (int b = 0; b < vec.size(); b++) {
						if (citemsset->contains(vec[b]->id) == false) {
							citems->push_back(vec[b]);
							citemsset->insert(vec[b]->id);
					}
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
				adjustacs(tpltinfo->tpltkeywords[i]->GetFormID());
				// get custom items
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						/* for (int d = 0; d < vec.size(); d++) {
							custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}*/
						/* for (int b = 0; b < vec.size(); b++) {
							for (int c = 0; c < vec[b]->items.size(); c++) {
								custItems->items.push_back(vec[b]->items[c]);
							}
							for (int c = 0; c < vec[b]->death.size(); c++) {
								custItems->death.push_back(vec[b]->death[c]);
							}
							for (int c = 0; c < vec[b]->poisons.size(); c++) {
								custItems->poisons.push_back(vec[b]->poisons[c]);
							}
							for (int c = 0; c < vec[b]->potions.size(); c++) {
								custItems->potions.push_back(vec[b]->potions[c]);
							}
							for (int c = 0; c < vec[b]->fortify.size(); c++) {
								custItems->fortify.push_back(vec[b]->fortify[c]);
							}
						}*/
						for (int b = 0; b < vec.size(); b++) {
							if (citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
			}
		}
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
		adjustacs(npc->factions[i].faction->GetFormID());
		if (custItems) {
			auto itc = customItems()->find(npc->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				/* for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}*/
				/* for (int b = 0; b < vec.size(); b++) {
					for (int c = 0; c < vec[b]->items.size(); c++) {
						custItems->items.push_back(vec[b]->items[c]);
					}
					for (int c = 0; c < vec[b]->death.size(); c++) {
						custItems->death.push_back(vec[b]->death[c]);
					}
					for (int c = 0; c < vec[b]->poisons.size(); c++) {
						custItems->poisons.push_back(vec[b]->poisons[c]);
					}
					for (int c = 0; c < vec[b]->potions.size(); c++) {
						custItems->potions.push_back(vec[b]->potions[c]);
					}
					for (int c = 0; c < vec[b]->fortify.size(); c++) {
						custItems->fortify.push_back(vec[b]->fortify[c]);
					}
				}*/
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
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
				adjustacs(tpltinfo->tpltfactions[i]->GetFormID());
				if (custItems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						/* for (int d = 0; d < vec.size(); d++) {
							custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
							custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
							custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
							custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
							custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
						}*/
						/* for (int b = 0; b < vec.size(); b++) {
							for (int c = 0; c < vec[b]->items.size(); c++) {
								custItems->items.push_back(vec[b]->items[c]);
							}
							for (int c = 0; c < vec[b]->death.size(); c++) {
								custItems->death.push_back(vec[b]->death[c]);
							}
							for (int c = 0; c < vec[b]->poisons.size(); c++) {
								custItems->poisons.push_back(vec[b]->poisons[c]);
							}
							for (int c = 0; c < vec[b]->potions.size(); c++) {
								custItems->potions.push_back(vec[b]->potions[c]);
							}
							for (int c = 0; c < vec[b]->fortify.size(); c++) {
								custItems->fortify.push_back(vec[b]->fortify[c]);
							}
						}*/
						for (int b = 0; b < vec.size(); b++) {
							if (citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
			}
		}
	}

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
		adjustacs(cls->GetFormID());
		if (custItems) {
			auto itc = customItems()->find(cls->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				/* for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}*/
				/* for (int b = 0; b < vec.size(); b++) {
					for (int c = 0; c < vec[b]->items.size(); c++) {
						custItems->items.push_back(vec[b]->items[c]);
					}
					for (int c = 0; c < vec[b]->death.size(); c++) {
						custItems->death.push_back(vec[b]->death[c]);
					}
					for (int c = 0; c < vec[b]->poisons.size(); c++) {
						custItems->poisons.push_back(vec[b]->poisons[c]);
					}
					for (int c = 0; c < vec[b]->potions.size(); c++) {
						custItems->potions.push_back(vec[b]->potions[c]);
					}
					for (int c = 0; c < vec[b]->fortify.size(); c++) {
						custItems->fortify.push_back(vec[b]->fortify[c]);
					}
				}*/
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
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
		adjustacs(style->GetFormID());
		if (custItems) {
			auto itc = customItems()->find(style->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				/* for (int d = 0; d < vec.size(); d++) {
					custItems->items.insert(custItems->items.end(), vec[d]->items.begin(), vec[d]->items.end());
					custItems->potions.insert(custItems->potions.end(), vec[d]->potions.begin(), vec[d]->potions.end());
					custItems->poisons.insert(custItems->poisons.end(), vec[d]->poisons.begin(), vec[d]->poisons.end());
					custItems->fortify.insert(custItems->fortify.end(), vec[d]->fortify.begin(), vec[d]->fortify.end());
					custItems->food.insert(custItems->food.end(), vec[d]->food.begin(), vec[d]->food.end());
				}*/
				/* for (int b = 0; b < vec.size(); b++) {
					for (int c = 0; c < vec[b]->items.size(); c++) {
						custItems->items.push_back(vec[b]->items[c]);
					}
					for (int c = 0; c < vec[b]->death.size(); c++) {
						custItems->death.push_back(vec[b]->death[c]);
					}
					for (int c = 0; c < vec[b]->poisons.size(); c++) {
						custItems->poisons.push_back(vec[b]->poisons[c]);
					}
					for (int c = 0; c < vec[b]->potions.size(); c++) {
						custItems->potions.push_back(vec[b]->potions[c]);
					}
					for (int c = 0; c < vec[b]->fortify.size(); c++) {
						custItems->fortify.push_back(vec[b]->fortify[c]);
					}
				}*/
				for (int b = 0; b < vec.size(); b++) {
					if (citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}

	if (acsadj != 0) {
		int str = static_cast<int>(acs);
		str += acsadj;
		if (str < 0)
			str = 0;
		if (str > static_cast<int>(ActorStrength::Boss))
			str = static_cast<int>(ActorStrength::Boss);
		acs = static_cast<ActorStrength>(str);
	}
	if (bossoverride)
		acs = ActorStrength::Boss;

	if (custItems) {
		auto itc = customItems()->find(0x0);
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
		// work the accumulated items
		for (int b = 0; b < citems->size(); b++) {
			for (int c = 0; c < citems->at(b)->items.size(); c++) {
				custItems->items.push_back(citems->at(b)->items[c]);
			}
			for (int c = 0; c < citems->at(b)->death.size(); c++) {
				custItems->death.push_back(citems->at(b)->death[c]);
			}
			for (int c = 0; c < citems->at(b)->poisons.size(); c++) {
				custItems->poisons.push_back(citems->at(b)->poisons[c]);
			}
			for (int c = 0; c < citems->at(b)->potions.size(); c++) {
				custItems->potions.push_back(citems->at(b)->potions[c]);
			}
			for (int c = 0; c < citems->at(b)->fortify.size(); c++) {
				custItems->fortify.push_back(citems->at(b)->fortify[c]);
			}
		}
		delete citems;
		delete citemsset;
	}

	if (rule) {
		LOG_1("rule found: {}", rule->ruleName);
		return rule;
	} else {
		// there are no rules!!!
		if (baseexcluded)
			return Distribution::emptyRule;
		LOG_1("default rule found: {}", Distribution::defaultRule->ruleName);
		return Distribution::defaultRule;
	}
}

bool CheckDistributability(std::shared_ptr<ActorInfo> const& acinfo, Distribution::CustomItemStorage* cust)
{
	if (cust == nullptr)
		return false;
	bool distr = false;
	distr |= acinfo->CheckCustomConditionsDistr(cust->food);
	distr |= acinfo->CheckCustomConditionsDistr(cust->fortify);
	distr |= acinfo->CheckCustomConditionsDistr(cust->poisons);
	distr |= acinfo->CheckCustomConditionsDistr(cust->potions);
	distr |= acinfo->CheckCustomConditionsDistrItems(cust->death);
	distr |= acinfo->CheckCustomConditionsDistrItems(cust->items);
	return distr;
}

Distribution::Rule* Distribution::CalcRule(std::shared_ptr<ActorInfo> const& acinfo, Misc::NPCTPLTInfo* tpltinfo)
{
	auto begin = std::chrono::steady_clock::now();
	if (acinfo == nullptr || acinfo->IsValid() == false)
		return emptyRule;
	// get npc template info
	Misc::NPCTPLTInfo tplt;
	if (tpltinfo == nullptr) {
		tplt = Utility::ExtractTemplateInfo(acinfo->GetActor());
		tpltinfo = &tplt;
	}
	// calc strength section
	if (Settings::Distr::_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty;
		if (diff == 0 || diff == 1) {
			acinfo->SetActorStrength(ActorStrength::Weak);
			acinfo->SetItemStrength(ItemStrength::kWeak);
		} else if (diff == 2 || diff == 3) {
			acinfo->SetActorStrength(ActorStrength::Normal);
			acinfo->SetItemStrength(ItemStrength::kStandard);
		} else if (diff == 4) {
			acinfo->SetActorStrength(ActorStrength::Powerful);
			acinfo->SetItemStrength(ItemStrength::kPotent);
		} else {  // diff == 5
			acinfo->SetActorStrength(ActorStrength::Insane);
			acinfo->SetItemStrength(ItemStrength::kInsane);
		}
	} else {
		// get level dependencies
		short lvl = acinfo->GetLevel();
		if (lvl <= Settings::Distr::_LevelEasy) {
			acinfo->SetActorStrength(ActorStrength::Weak);
			acinfo->SetItemStrength(ItemStrength::kWeak);
			// weak actor
		} else if (lvl <= Settings::Distr::_LevelNormal) {
			acinfo->SetActorStrength(ActorStrength::Normal);
			acinfo->SetItemStrength(ItemStrength::kStandard);
			// normal actor
		} else if (lvl <= Settings::Distr::_LevelDifficult) {
			acinfo->SetActorStrength(ActorStrength::Powerful);
			acinfo->SetItemStrength(ItemStrength::kPotent);
			// difficult actor
		} else if (lvl <= Settings::Distr::_LevelInsane) {
			acinfo->SetActorStrength(ActorStrength::Insane);
			acinfo->SetItemStrength(ItemStrength::kInsane);
			// insane actor
		} else {
			acinfo->SetActorStrength(ActorStrength::Boss);
			acinfo->SetItemStrength(ItemStrength::kInsane);
			// boss actor
		}
	}
	// now calculate rule and on top get the boss override

	bool bossoverride = false;
	int acsadj = 0;
	auto adjustacs = [&acsadj](RE::FormID id) {
		try {
			acsadj += actorStrengthMap()->at(id);
		} catch (std::out_of_range&) {
		}
	};


	bool ruleoverride = false;
	bool baseexcluded = false;
	int prio = INT_MIN;

	bool calccustitems = !acinfo->citems.calculated;
	bool calcwhite = !acinfo->IsWhitelistCalculated();

	std::vector<CustomItemStorage*>* citems = nullptr;
	std::unordered_set<std::string>* citemsset = nullptr;
	if (calccustitems) {
		citems = new std::vector<CustomItemStorage*>{};
		citemsset = new std::unordered_set<std::string>{};
	}

	auto base = acinfo->GetActorBase();

	Rule* rule = nullptr;
	// define general stuff
	auto race = acinfo->GetRace();

	// find rule in npc map
	// npc rules always have the highest priority
	auto itnpc = npcMap()->find(acinfo->GetFormID());
	if (itnpc != npcMap()->end()) {  // found the right rule!
		rule = itnpc->second;        // this can be null if the specific npc is excluded
		ruleoverride = true;
		prio = INT_MAX;
	}
	bossoverride |= bosses()->contains(acinfo->GetFormID());
	adjustacs(acinfo->GetFormID());
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(acinfo->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
	}
	if (calcwhite && (whitelistNPCs()->contains(acinfo->GetFormID()) || whitelistNPCsPlugin()->contains(acinfo->GetPluginID())))
		acinfo->SetWhitelisted();

	// now also perform a check on the actor base
	if (!ruleoverride) {
		itnpc = npcMap()->find(acinfo->GetActorBaseFormID());
		if (itnpc != npcMap()->end()) {  // found the right rule!
			rule = itnpc->second;        // this can be null if the specific npc is excluded
			//loginfo("assign rule 2");
			ruleoverride = true;
			prio = INT_MAX;
		}
	}
	bossoverride |= bosses()->contains(acinfo->GetActorBaseFormID());
	adjustacs(acinfo->GetActorBaseFormID());
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(acinfo->GetActorBaseFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
	}
	if (calcwhite && whitelistNPCs()->contains(acinfo->GetActorBaseFormID()))
		acinfo->SetWhitelisted();
	// perform check on tpltactorbaseinformation
	if (tpltinfo->base != nullptr && tpltinfo->base != acinfo->GetActorBase()) {
		if (!ruleoverride) {
			itnpc = npcMap()->find(tpltinfo->base->GetFormID());
			if (itnpc != npcMap()->end()) {  // found the right rule!
				rule = itnpc->second;        // this can be null if the specific npc is excluded
				//loginfo("assign rule 2");
				ruleoverride = true;
				prio = INT_MAX;
			}
		}
		bossoverride |= bosses()->contains(tpltinfo->base->GetFormID());
		adjustacs(tpltinfo->base->GetFormID());
		// get custom items
		if (calccustitems) {
			auto itc = customItems()->find(tpltinfo->base->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite && whitelistNPCs()->contains(tpltinfo->base->GetFormID()))
			acinfo->SetWhitelisted();
	}

	if (tpltinfo && tpltinfo->tpltrace)
		race = tpltinfo->tpltrace;
	// now that we didnt't find something so far, check the rest
	// this time all the priorities are the same
	if (!ruleoverride) {
		//loginfo("rule 6");
		auto it = assocMap()->find(race->GetFormID());
		if (it != assocMap()->end())
			if (prio < std::get<0>(it->second)) {
				rule = std::get<1>(it->second);
				//loginfo("assign rule 3");
				prio = std::get<0>(it->second);
			} else if (prio < std::get<1>(it->second)->rulePriority) {
				rule = std::get<1>(it->second);
				//loginfo("assign rule 4");
				prio = std::get<1>(it->second)->rulePriority;
			}
		baseexcluded |= baselineExclusions()->contains(race->GetFormID());
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			auto itr = assocMap()->find(race->keywords[i]->GetFormID());
			if (itr != assocMap()->end()) {
				if (prio < std::get<0>(itr->second)) {
					rule = std::get<1>(itr->second);
					//loginfo("assign rule 5 {} {} {}", Utility::GetHex((uintptr_t)std::get<1>(itr->second)), race->keywords[i]->GetFormEditorID(), Utility::GetHex(race->keywords[i]->GetFormID()));
					prio = std::get<0>(itr->second);
				} else if (prio < std::get<1>(itr->second)->rulePriority) {
					rule = std::get<1>(itr->second);
					//loginfo("assign rule 6");
					prio = std::get<1>(itr->second)->rulePriority;
				}
				baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
				bossoverride |= bosses()->contains(race->keywords[i]->GetFormID());
				adjustacs(race->keywords[i]->GetFormID());
			}
		}
	}
	bossoverride |= bosses()->contains(base->GetRace()->GetFormID());
	adjustacs(base->GetRace()->GetFormID());
	// get custom items
	if (calccustitems) {
		auto itc = customItems()->find(race->GetFormID());
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
			}
		}
		}
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			itc = customItems()->find(race->keywords[i]->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
	}
	if (calcwhite) { 
		if (whitelistNPCs()->contains(acinfo->GetRaceFormID()))
			acinfo->SetWhitelisted();
		for (uint32_t i = 0; i < race->numKeywords; i++) {
			if (whitelistNPCs()->contains(race->keywords[i]->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}

	// handle keywords
	for (unsigned int i = 0; i < base->numKeywords; i++) {
		auto key = base->keywords[i];
		if (key) {
			if (!ruleoverride) {
				auto it = assocMap()->find(key->GetFormID());
				if (it != assocMap()->end())
					if (prio < std::get<0>(it->second)) {
						rule = std::get<1>(it->second);
						//loginfo("assign rule 7");
						prio = std::get<0>(it->second);
					} else if (prio < std::get<1>(it->second)->rulePriority) {
						rule = std::get<1>(it->second);
						//loginfo("assign rule 8");
						prio = std::get<1>(it->second)->rulePriority;
					}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
			adjustacs(key->GetFormID());
			// get custom items
			if (calccustitems) {
				auto itc = customItems()->find(key->GetFormID());
				if (itc != customItems()->end()) {
					auto vec = itc->second;
					for (int b = 0; b < vec.size(); b++) {
						if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
							citems->push_back(vec[b]);
							citemsset->insert(vec[b]->id);
						}
					}
				}
			}
			if (calcwhite) {
				if (whitelistNPCs()->contains(key->GetFormID()))
					acinfo->SetWhitelisted();
			}
		}
	}
	if (tpltinfo) {
		//loginfo("rule 10");
		for (int i = 0; i < tpltinfo->tpltkeywords.size(); i++) {
			if (tpltinfo->tpltkeywords[i]) {
				if (!ruleoverride) {
					auto it = assocMap()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (it != assocMap()->end())
						if (prio < std::get<0>(it->second)) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 9");
							prio = std::get<0>(it->second);
						} else if (prio < std::get<1>(it->second)->rulePriority) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 10");
							prio = std::get<1>(it->second)->rulePriority;
						}
					baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltkeywords[i]->GetFormID());
				adjustacs(tpltinfo->tpltkeywords[i]->GetFormID());
				// get custom items
				if (calccustitems) {
					auto itc = customItems()->find(tpltinfo->tpltkeywords[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int b = 0; b < vec.size(); b++) {
							if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
				if (calcwhite) {
					if (whitelistNPCs()->contains(tpltinfo->tpltkeywords[i]->GetFormID()))
						acinfo->SetWhitelisted();
				}
			}
		}
	}

	// handle factions
	for (uint32_t i = 0; i < base->factions.size(); i++) {
		if (!ruleoverride) {
			auto it = assocMap()->find(base->factions[i].faction->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 11");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 12");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
			baseexcluded |= baselineExclusions()->contains(base->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(base->factions[i].faction->GetFormID());
		adjustacs(base->factions[i].faction->GetFormID());
		if (calccustitems) {
			auto itc = customItems()->find(base->factions[i].faction->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite) {
			if (whitelistNPCs()->contains(base->factions[i].faction->GetFormID()))
				acinfo->SetWhitelisted();
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
							//loginfo("assign rule 13");
							prio = std::get<0>(it->second);
						} else if (prio < std::get<1>(it->second)->rulePriority) {
							rule = std::get<1>(it->second);
							//loginfo("assign rule 14");
							prio = std::get<1>(it->second)->rulePriority;
						}
					}
					baseexcluded |= baselineExclusions()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				}
				bossoverride |= bosses()->contains(tpltinfo->tpltfactions[i]->GetFormID());
				adjustacs(tpltinfo->tpltfactions[i]->GetFormID());
				if (calccustitems) {
					auto itc = customItems()->find(tpltinfo->tpltfactions[i]->GetFormID());
					if (itc != customItems()->end()) {
						auto vec = itc->second;
						for (int b = 0; b < vec.size(); b++) {
							if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
								citems->push_back(vec[b]);
								citemsset->insert(vec[b]->id);
							}
						}
					}
				}
				if (calcwhite) {
					if (whitelistNPCs()->contains(tpltinfo->tpltfactions[i]->GetFormID()))
						acinfo->SetWhitelisted();
				}
			}
		}
	}
	
	// dont use tplt for class and combatstyle, since they may have been modified during runtime

	// handle classes
	if (base->npcClass) {
		if (!ruleoverride) {
			auto it = assocMap()->find(base->npcClass->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 15");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 16");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
		}
		adjustacs(base->npcClass->GetFormID());
		if (calccustitems) {
			auto itc = customItems()->find(base->npcClass->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite) {
			if (whitelistNPCs()->contains(base->npcClass->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}
	// handle combat styles
	if (base->combatStyle) {
		//loginfo("rule 16");
		if (!ruleoverride) {
			auto it = assocMap()->find(base->combatStyle->GetFormID());
			if (it != assocMap()->end()) {
				if (prio < std::get<0>(it->second)) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 17");
					prio = std::get<0>(it->second);
				} else if (prio < std::get<1>(it->second)->rulePriority) {
					rule = std::get<1>(it->second);
					//loginfo("assign rule 18");
					prio = std::get<1>(it->second)->rulePriority;
				}
			}
		}
		adjustacs(base->combatStyle->GetFormID());
		if (calccustitems) {
			auto itc = customItems()->find(base->combatStyle->GetFormID());
			if (itc != customItems()->end()) {
				auto vec = itc->second;
				for (int b = 0; b < vec.size(); b++) {
					if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
						citems->push_back(vec[b]);
						citemsset->insert(vec[b]->id);
					}
				}
			}
		}
		if (calcwhite) {
			if (whitelistNPCs()->contains(base->combatStyle->GetFormID()))
				acinfo->SetWhitelisted();
		}
	}

	if (acsadj != 0) {
		int str = static_cast<int>(acinfo->GetActorStrength());
		str += acsadj;
		if (str < 0)
			str = 0;
		if (str > static_cast<int>(ActorStrength::Boss))
			str = static_cast<int>(ActorStrength::Boss);
		acinfo->SetActorStrength(static_cast<ActorStrength>(str));
	}
	if (bossoverride)
		acinfo->SetActorStrength(ActorStrength::Boss);

	if (calccustitems) {
		auto itc = customItems()->find(0x0);
		LOG_1("custom item 0x0");
		if (itc != customItems()->end()) {
			auto vec = itc->second;
			for (int b = 0; b < vec.size(); b++) {
				if (CheckDistributability(acinfo, vec[b]) && citemsset->contains(vec[b]->id) == false) {
					citems->push_back(vec[b]);
					citemsset->insert(vec[b]->id);
				}
			}
		}
		// work the accumulated items
		for (int b = 0; b < citems->size(); b++) {
			for (int c = 0; c < citems->at(b)->items.size(); c++) {
				acinfo->citems.items.push_back(citems->at(b)->items[c]);
			}
			for (int c = 0; c < citems->at(b)->death.size(); c++) {
				acinfo->citems.death.push_back(citems->at(b)->death[c]);
			}
			for (int c = 0; c < citems->at(b)->poisons.size(); c++) {
				acinfo->citems.poisons.push_back(citems->at(b)->poisons[c]);
			}
			for (int c = 0; c < citems->at(b)->potions.size(); c++) {
				acinfo->citems.potions.push_back(citems->at(b)->potions[c]);
			}
			for (int c = 0; c < citems->at(b)->fortify.size(); c++) {
				acinfo->citems.fortify.push_back(citems->at(b)->fortify[c]);
			}
		}
		delete citems;
		delete citemsset;

		acinfo->citems.calculated = true;
		acinfo->citems.CreateMaps();
	}
	acinfo->SetBoss(bossoverride);

	if (calcwhite) {
		acinfo->SetWhitelistCalculated();
	}

	PROF_1(TimeProfiling, "execution time");

	if (rule) {
		LOG_1("rule found: {}", rule->ruleName);
		return rule;
	} else {
		// there are no rules!!!
		if (baseexcluded) {
			return Distribution::emptyRule;
		}
		LOG_1("default rule found: {}", Distribution::defaultRule->ruleName);
		return Distribution::defaultRule;
	}
}
/*
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
*/
std::vector<std::tuple<int, Distribution::Rule*, std::string>> Distribution::CalcAllRules(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	// calc strength section
	if (Settings::Distr::_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->GetGameStatsData().difficulty;
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
		if (lvl <= Settings::Distr::_LevelEasy) {
			acs = ActorStrength::Weak;
			is = ItemStrength::kWeak;
			// weak actor
		} else if (lvl <= Settings::Distr::_LevelNormal) {
			acs = ActorStrength::Normal;
			is = ItemStrength::kStandard;
			// normal actor
		} else if (lvl <= Settings::Distr::_LevelDifficult) {
			acs = ActorStrength::Powerful;
			is = ItemStrength::kPotent;
			// difficult actor
		} else if (lvl <= Settings::Distr::_LevelInsane) {
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
	int acsadj = 0;
	auto adjustacs = [&acsadj](RE::FormID id) {
		try {
			acsadj += actorStrengthMap()->at(id);
		} catch (std::out_of_range&) {
		}
	};

	bool baseexcluded = false;
	int prio = INT_MIN;
	const bool ruleoverride = false;

	auto base = actor->GetActorBase();

	std::vector<std::tuple<int, Distribution::Rule*, std::string>> rls;
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
	adjustacs(actor->GetFormID());

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
	adjustacs(actor->GetActorBase()->GetFormID());

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
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Race\t" + Utility::PrintForm(base->GetRace()) });
		}
		baseexcluded |= baselineExclusions()->contains(base->GetRace()->GetFormID());
		adjustacs(base->GetRace()->GetFormID());
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
				rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Racekwd\t" + Utility::PrintForm(race->keywords[i]) });
			}
			baseexcluded |= baselineExclusions()->contains(race->keywords[i]->GetFormID());
			bossoverride |= bosses()->contains(race->keywords[i]->GetFormID());
			adjustacs(race->keywords[i]->GetFormID());
		}
	}
	bossoverride |= bosses()->contains(base->GetRace()->GetFormID());
	adjustacs(base->GetRace()->GetFormID());

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
					rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Keyword\t" + Utility::PrintForm(key)  });
				}
				baseexcluded |= baselineExclusions()->contains(key->GetFormID());
			}
			bossoverride |= bosses()->contains(key->GetFormID());
			adjustacs(key->GetFormID());
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
				rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Faction\t" + Utility::PrintForm(base->factions[i].faction) });
			}
			baseexcluded |= baselineExclusions()->contains(base->factions[i].faction->GetFormID());
		}
		bossoverride |= bosses()->contains(base->factions[i].faction->GetFormID());
		adjustacs(base->factions[i].faction->GetFormID());
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
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "Class\t" + Utility::PrintForm(base->npcClass) });
		}
		adjustacs(base->npcClass->GetFormID());
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
			rls.push_back({ std::get<0>(it->second), std::get<1>(it->second), "CombatStyle\t" + Utility::PrintForm(base->combatStyle) });
		}
		adjustacs(base->combatStyle->GetFormID());
	}

	if (acsadj != 0) {
		int str = static_cast<int>(acs);
		str += acsadj;
		if (str < 0)
			str = 0;
		if (str > static_cast<int>(ActorStrength::Boss))
			str = static_cast<int>(ActorStrength::Boss);
		acs = static_cast<ActorStrength>(str);
	}
	if (bossoverride)
		acs = ActorStrength::Boss;

	if (rule) {
		LOG_1("rule found: {}", rule->ruleName);
		rls.insert(rls.begin(), { INT_MIN, rule, "Chosen" });
		return rls;
	} else {
		// there are no rules!!!
		if (baseexcluded) {
			return std::vector<std::tuple<int, Distribution::Rule*, std::string>>{ { INT_MIN, Distribution::emptyRule, "Empty" } };
		}
		LOG_1("default rule found: {}", Distribution::defaultRule->ruleName);
		return std::vector<std::tuple<int, Distribution::Rule*, std::string>>{ { INT_MIN, Distribution::defaultRule, "Default" } };
	}
}

void Distribution::ResetCustomItems()
{
	std::unordered_set<Distribution::CustomItemStorage*> set;
	LogConsole("Gathering custom items...");
	for (auto cust : _customItems) {
		for (int i = 0; i < cust.second.size(); i++) {
			if (cust.second[i] != nullptr)
				set.insert(cust.second[i]);
		}
	}
	_customItems.clear();
	LogConsole(("Found " + std::to_string(set.size()) + " custom items. Beginning deletion").c_str());
	for (auto entry : set) {
		for (int i = 0; i < entry->items.size(); i++)
			delete entry->items[i];
		for (int i = 0; i < entry->death.size(); i++)
			delete entry->death[i];
		for (int i = 0; i < entry->potions.size(); i++)
			delete entry->potions[i];
		for (int i = 0; i < entry->poisons.size(); i++)
			delete entry->poisons[i];
		for (int i = 0; i < entry->fortify.size(); i++)
			delete entry->fortify[i];
		for (int i = 0; i < entry->food.size(); i++)
			delete entry->food[i];
		delete entry;
	}
	set.clear();
	LogConsole("Reset custom items");
}

void Distribution::ResetRules()
{
	_magicEffectAlchMap.clear();
	_alcohol.clear();
	_whitelistNPCsPlugin.clear();
	_whitelistNPCs.clear();
	_excludedPlugins_NPCs.clear();
	_excludedEffects.clear();
	_dosageEffectMap.clear();
	_dosageItemMap.clear();
	_followerFactions.clear();
	_actorStrengthMap.clear();
	_itemStrengthMap.clear();
	_excludedPlugins.clear();
	_customItems.clear();
	_whitelistItems.clear();
	_baselineExclusions.clear();
	_excludedItemsPlayer.clear();
	_excludedItems.clear();
	_excludedAssoc.clear();
	_excludedNPCs.clear();
	_bosses.clear();
	_assocMap.clear();
	_npcMap.clear();
	for (Rule* rule : _rules) {
		delete rule;
	}
	for (auto [str, cat] : _internEffectCategories)
	{
		delete cat;
	}
	for (auto [str, catpre] : _internEffectCategoryPresets)
	{
		delete catpre;
	}
	for (auto [str, effpre] : _internEffectPresets)
	{
		delete effpre;
	}
	_rules.clear();
}

#pragma endregion
