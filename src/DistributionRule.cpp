#include "DistributionRule.h"
#include "ActorInfo.h"
#include "Distribution.h"
#include "Utility.h"

#include <random>

static std::mt19937 randi((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
/// <summary>
/// trims random numbers to 1 to RR
/// </summary>
static std::uniform_int_distribution<signed> randRR(1, RandomRange);
static std::uniform_int_distribution<signed> rand100(1, 100);

#pragma region Rule

// private

DistributionRule::DistributionRule(int _ruleVersion, int _ruleType, std::string _ruleName, int _rulePriority, bool _allowMixed, bool _styleScaling, int maxPotions, std::vector<int> potion1Chance, std::vector<int> potion2Chance, std::vector<int> potion3Chance, std::vector<int> potion4Chance, std::vector<int> potionAdditionalChance, int potionTierAdjust, int maxFortify, std::vector<int> fortify1Chance, std::vector<int> fortify2Chance, std::vector<int> fortify3Chance, std::vector<int> fortify4Chance, std::vector<int> fortifyAddtionalChance, int fortifyTierAdjust, int maxPoisons, std::vector<int> poison1Chance, std::vector<int> poison2Chance, std::vector<int> poison3Chance, std::vector<int> poison4Chance, std::vector<int> poisonAdditionalChance, int poisonTierAdjust, std::vector<int> foodChance, std::vector<std::tuple<int, AlchemicEffect>> potionDistr, std::vector<std::tuple<int, AlchemicEffect>> poisonDistr, std::vector<std::tuple<int, AlchemicEffect>> fortifyDistr, std::vector<std::tuple<int, AlchemicEffect>> foodDistr, AlchemicEffect validPotions, AlchemicEffect validPoisons, AlchemicEffect validFortifyPotions, AlchemicEffect validFood) :
	ruleVersion{ _ruleVersion },
	ruleType{ _ruleType },
	ruleName{ _ruleName },
	rulePriority{ _rulePriority },
	allowMixed{ _allowMixed },
	styleScaling{ _styleScaling }
{
	potions = new EffCategoryPreset();
	potions->name = ruleName + "_potions_catpreset";
	Distribution::_internEffectCategoryPresets.insert_or_assign(potions->name, potions);
	poisons = new EffCategoryPreset();
	poisons->name = ruleName + "_poisons_catpreset";
	Distribution::_internEffectCategoryPresets.insert_or_assign(poisons->name, poisons);
	fortify = new EffCategoryPreset();
	fortify->name = ruleName + "_fortify_catpreset";
	Distribution::_internEffectCategoryPresets.insert_or_assign(fortify->name, fortify);
	food = new EffCategoryPreset();
	food->name = ruleName + "_food_catpreset";
	Distribution::_internEffectCategoryPresets.insert_or_assign(food->name, food);

	potionEffects = new EffectPreset();
	potionEffects->name = ruleName + "_potion_effectspreset";
	Distribution::_internEffectPresets.insert_or_assign(potionEffects->name, potionEffects);
	poisonEffects = new EffectPreset();
	poisonEffects->name = ruleName + "_poison_effectspreset";
	Distribution::_internEffectPresets.insert_or_assign(poisonEffects->name, poisonEffects);
	fortifyEffects = new EffectPreset();
	fortifyEffects->name = ruleName + "_fortify_effectspreset";
	Distribution::_internEffectPresets.insert_or_assign(fortifyEffects->name, fortifyEffects);
	foodEffects = new EffectPreset();
	foodEffects->name = ruleName + "_food_effectspreset";
	Distribution::_internEffectPresets.insert_or_assign(foodEffects->name, foodEffects);

	auto meanfalloff = [](std::vector<int> vec1, std::vector<int> vec2, std::vector<int> vec3, std::vector<int> vec4, int num) {
		float mean = 0;
		int mac = 0;
		if (vec1[num] != 0) {
			mean += (float)vec2[num] / (float)vec1[num];
			mac++;
		}
		if (vec2[num] != 0) {
			mean += (float)vec3[num] / (float)vec2[num];
			mac++;
		}
		if (vec3[num] != 0) {
			mean += (float)vec4[num] / (float)vec3[num];
			mac++;
		}
		if (mac != 0)
			mean = mean / (float)mac;
		return mean;
	};
	auto addcats = [](EffCategoryPreset* preset) {
		for (int i = 0; i < 5; i++) {
			preset->cats[i] = new EffCategory();
			preset->cats[i]->name = preset->name + "_" + std::to_string(i);
			Distribution::_internEffectCategories.insert_or_assign(preset->cats[i]->name, preset->cats[i]);
		}
	};
	auto setcat = [](EffCategoryPreset* preset, int num, int base, float falloff, int max) {
		preset->cats[num]->baseChance = base;
		preset->cats[num]->falloff = falloff;
		preset->cats[num]->max = max;
	};
	addcats(potions);
	addcats(poisons);
	addcats(fortify);
	addcats(food);
	potions->tieradjust = potionTierAdjust;
	poisons->tieradjust = poisonTierAdjust;
	fortify->tieradjust = fortifyTierAdjust;
	food->tieradjust = 0;

	potionEffects->validEffects = validPotions;
	poisonEffects->validEffects = validPoisons;
	fortifyEffects->validEffects = validFortifyPotions;
	foodEffects->validEffects = validFood;

	setcat(potions, 0, potion1Chance[0], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 0), maxPotions);
	setcat(potions, 1, potion1Chance[1], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 1), maxPotions);
	setcat(potions, 2, potion1Chance[2], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 2), maxPotions);
	setcat(potions, 3, potion1Chance[3], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 3), maxPotions);
	setcat(potions, 4, potion1Chance[4], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 4), maxPotions);

	setcat(poisons, 0, poison1Chance[0], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 0), maxPoisons);
	setcat(poisons, 1, poison1Chance[1], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 1), maxPoisons);
	setcat(poisons, 2, poison1Chance[2], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 2), maxPoisons);
	setcat(poisons, 3, poison1Chance[3], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 3), maxPoisons);
	setcat(poisons, 4, poison1Chance[4], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 4), maxPoisons);

	setcat(fortify, 0, fortify1Chance[0], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 0), maxFortify);
	setcat(fortify, 1, fortify1Chance[1], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 1), maxFortify);
	setcat(fortify, 2, fortify1Chance[2], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 2), maxFortify);
	setcat(fortify, 3, fortify1Chance[3], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 3), maxFortify);
	setcat(fortify, 4, fortify1Chance[4], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 4), maxFortify);

	setcat(food, 0, foodChance[0], 0, 1);
	setcat(food, 1, foodChance[1], 0, 1);
	setcat(food, 2, foodChance[2], 0, 1);
	setcat(food, 3, foodChance[3], 0, 1);
	setcat(food, 4, foodChance[4], 0, 1);

	for (auto [weight, alch] : potionDistr) {
		Effect eff;
		eff.effect = alch;
		eff.max = 0;
		eff.current = 0;
		eff.weight = 1000 * weight;
		potionEffects->effects.insert_or_assign(alch, eff);
	}
	potionEffects->standardDistr = Utility::GetEffectDistribution(potionEffects->effects);
	potionEffects->validEffects = Utility::SumAlchemyEffects(potionEffects->standardDistr);

	for (auto [weight, alch] : poisonDistr) {
		Effect eff;
		eff.effect = alch;
		eff.max = 0;
		eff.current = 0;
		eff.weight = 1000 * weight;
		poisonEffects->effects.insert_or_assign(alch, eff);
	}
	poisonEffects->standardDistr = Utility::GetEffectDistribution(poisonEffects->effects);
	poisonEffects->validEffects = Utility::SumAlchemyEffects(poisonEffects->standardDistr);

	for (auto [weight, alch] : fortifyDistr) {
		Effect eff;
		eff.effect = alch;
		eff.max = 0;
		eff.current = 0;
		eff.weight = 1000 * weight;
		fortifyEffects->effects.insert_or_assign(alch, eff);
	}
	fortifyEffects->standardDistr = Utility::GetEffectDistribution(fortifyEffects->effects);
	fortifyEffects->validEffects = Utility::SumAlchemyEffects(fortifyEffects->standardDistr);

	for (auto [weight, alch] : foodDistr) {
		Effect eff;
		eff.effect = alch;
		eff.max = 0;
		eff.current = 0;
		eff.weight = 1000 * weight;
		foodEffects->effects.insert_or_assign(alch, eff);
	}
	foodEffects->standardDistr = Utility::GetEffectDistribution(foodEffects->effects);
	foodEffects->validEffects = Utility::SumAlchemyEffects(foodEffects->standardDistr);
}

EffectDistr DistributionRule::GetScaledDistribution(Settings::ItemType type, EffectPreset* preset, std::shared_ptr<ActorInfo> const& acinfo)
{
	RE::TESCombatStyle* style = acinfo->GetCombatStyle();
	std::unordered_map<AlchemicEffect, Effect> effectMap = preset->effects;
	int customnum = 0;
	switch (type) {
	case Settings::ItemType::kFood:
		customnum = (int)acinfo->citems.foodset.size();
		for (auto& [effect, adjuster] : Distribution::_probabilityAdjustersFood) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	case Settings::ItemType::kFortifyPotion:
		customnum = (int)acinfo->citems.fortifyset.size();
		for (auto& [effect, adjuster] : Distribution::_probabilityAdjustersFortify) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	case Settings::ItemType::kPoison:
		customnum = (int)acinfo->citems.poisonsset.size();
		for (auto& [effect, adjuster] : Distribution::_probabilityAdjustersPoison) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	case Settings::ItemType::kPotion:
		customnum = (int)acinfo->citems.potionsset.size();
		for (auto& [effect, adjuster] : Distribution::_probabilityAdjustersPotion) {
			if (effectMap.contains(effect)) {
				Effect eff = effectMap.at(effect);
				float weight = eff.weight;
				eff.weight *= adjuster;
				effectMap.insert_or_assign(effect, eff);
			}
		}
		break;
	}

	if (effectMap.size() == 0 && customnum > 0) {
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
		scale = Settings::distr._StyleScalingPrimary;
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
				scale = Settings::distr._StyleScalingSecondary;
			goto GetScaledDistributionMagic;
		}
		if (prefscore & 0x01) {  // magic
			if (prefscore2 & 0x01 && max != max2)
				scale = Settings::distr._StyleScalingSecondary;
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
		scale = Settings::distr._StyleScalingPrimary;
		if (prefscore & 0x02) {  // ranged
			if (prefscore2 & 0x02 && max != max2)
				scale = Settings::distr._StyleScalingSecondary;
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
		scale = Settings::distr._StyleScalingPrimary;
		if (prefscore & 0x04) {  // melee
			if (prefscore2 & 0x04 && max != max2)
				scale = Settings::distr._StyleScalingSecondary;
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
		scale = Settings::distr._StyleScalingPrimary;
		if (prefscore & 0x10) {  // unarmed
			if (prefscore2 & 0x10 && max != max2)
				scale = Settings::distr._StyleScalingSecondary;
			itr = effectMap.find(AlchemicEffect::kUnarmedDamage);
			if (itr != effectMap.end()) {
				auto eff = itr->second;
				eff.weight *= scale;
				effectMap.insert_or_assign(AlchemicEffect::kUnarmedDamage, eff);
			}
		}
		scale = Settings::distr._StyleScalingPrimary;
		if (prefscore & 0x16) {
			// stamina pots
			if ((prefscore & 0x16) == (prefscore & 0x16) && max != max2)
				scale = Settings::distr._StyleScalingSecondary;
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
		scale = Settings::distr._StyleScalingPrimary;
		if (prefscore & 0x14) {
			if ((prefscore & 0x14) == (prefscore2 & 0x14) && max != max2)
				scale = Settings::distr._StyleScalingSecondary;
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
	return Utility::GetEffectDistribution(effectMap);
}

AlchemicEffect DistributionRule::GetRandomEffect(EffectDistr& distr)
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
			if (itr->second.max == itr->second.current) {  // if it is larger, max was 0 and the max limit is disabled
				distr.total -= itr->first;
				distr.effects.erase(itr);
			}
			return ret;
		}
		itr++;
	}
	return AlchemicEffect::kNone;
}

RE::AlchemyItem* DistributionRule::GetRandomPotion(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
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
		items = Settings::GetMatchingItems(*Settings::potionsBlood(), eff, numPotionEffects);
	} else {
RetryPotion:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(*Settings::potionsWeak_main(), eff, numPotionEffects);
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::potionsStandard_main(), eff, numPotionEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::potionsPotent_main(), eff, numPotionEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::potionsInsane_main(), eff, numPotionEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
			}
			break;
		}
	}
	// return random item
	if (items.size() > 0) {
		std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
		return items[r(randi)];
		//logusage("Looking for effect {}, gave {}", eff.string(), Utility::PrintForm(item));

	} else {
		//logusage("Looking for effect {}, item size 0", eff.string());
	}
	return nullptr;
}
RE::AlchemyItem* DistributionRule::GetRandomPoison(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
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
			items = Settings::GetMatchingItems(*Settings::poisonsWeak(), eff, numPoisonEffects);
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::poisonsStandard(), eff, numPoisonEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::poisonsPotent(), eff, numPoisonEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::poisonsInsane(), eff, numPoisonEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryPoison;
			}
			break;
		}
	}
	// return random item
	if (items.size() > 0) {
		std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
		return items[r(randi)];
	}
	return nullptr;
}
RE::AlchemyItem* DistributionRule::GetRandomFortifyPotion(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
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
			items = Settings::GetMatchingItems(*Settings::potionsWeak_rest(), eff, numFortifyEffects);
			break;
		case 2:  // standard
			items = Settings::GetMatchingItems(*Settings::potionsStandard_rest(), eff, numFortifyEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		case 3:  // potent
			items = Settings::GetMatchingItems(*Settings::potionsPotent_rest(), eff, numFortifyEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		case 4:  // insane
			items = Settings::GetMatchingItems(*Settings::potionsInsane_rest(), eff, numFortifyEffects);
			if (items.size() == 0) {
				str -= 1;
				goto RetryFortify;
			}
			break;
		}
	}
	// return random item
	if (items.size() > 0) {
		std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
		return items[r(randi)];
	}
	return nullptr;
}
RE::AlchemyItem* DistributionRule::GetRandomFood_intern(int str, std::shared_ptr<ActorInfo> const& acinfo, EffectDistr& distr)
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
		items = Settings::GetMatchingItems(*Settings::foodall(), eff, numFoodEffects);
	}
	// return random item
	if (items.size() > 0) {
		std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
		return items[r(randi)];
	}
	return nullptr;
}

// public

std::vector<RE::AlchemyItem*> DistributionRule::GetRandomPotions(std::shared_ptr<ActorInfo> const& acinfo)
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
				loginfo("Potion Prob: {}", potions->cats[astr]->baseChance * std::pow(potions->cats[astr]->falloff, koeff) * Settings::distr._ProbabilityScaling);
				if (rand100(randi) <= potions->cats[astr]->baseChance * std::pow(potions->cats[astr]->falloff, i) * Settings::distr._ProbabilityScaling) {
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
				loginfo("Potion Prob: {}", (potions->cats[astr]->baseChance - potions->cats[astr]->falloff * koeff) * Settings::distr._ProbabilityScaling);
				if (rand100(randi) <= (potions->cats[astr]->baseChance - potions->cats[astr]->falloff * i) * Settings::distr._ProbabilityScaling) {
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
std::vector<RE::AlchemyItem*> DistributionRule::GetRandomPoisons(std::shared_ptr<ActorInfo> const& acinfo)
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
				if (rand100(randi) <= poisons->cats[astr]->baseChance * std::pow(poisons->cats[astr]->falloff, koeff) * Settings::distr._ProbabilityScaling) {
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
				if (rand100(randi) <= (poisons->cats[astr]->baseChance - poisons->cats[astr]->falloff * koeff) * Settings::distr._ProbabilityScaling) {
					auto pot = GetRandomPoison(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		}
	}
	return ret;
}
std::vector<RE::AlchemyItem*> DistributionRule::GetRandomFortifyPotions(std::shared_ptr<ActorInfo> const& acinfo)
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
				if (rand100(randi) <= fortify->cats[astr]->baseChance * std::pow(fortify->cats[astr]->falloff, koeff) * Settings::distr._ProbabilityScaling) {
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
				if (rand100(randi) <= (fortify->cats[astr]->baseChance - fortify->cats[astr]->falloff * koeff) * Settings::distr._ProbabilityScaling) {
					auto pot = GetRandomFortifyPotion(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		}
	}
	return ret;
}
std::vector<RE::AlchemyItem*> DistributionRule::GetRandomFood(std::shared_ptr<ActorInfo> const& acinfo)
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
				if (rand100(randi) <= food->cats[astr]->baseChance * std::pow(food->cats[astr]->falloff, koeff) * Settings::distr._ProbabilityScaling) {
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
				if (rand100(randi) <= (food->cats[astr]->baseChance - food->cats[astr]->falloff * koeff) * Settings::distr._ProbabilityScaling) {
					auto pot = GetRandomFood_intern(str, acinfo, distr);
					ret.push_back(pot);
				}
			}
			break;
		}
	}
	return ret;
}

void DistributionRule::RemoveAlchemyEffectPotion(AlchemicEffect effect)
{
	potionEffects->effects.erase(effect);
	potionEffects->standardDistr = Utility::GetEffectDistribution(potionEffects->effects);
	potionEffects->validEffects = Utility::SumAlchemyEffects(potionEffects->standardDistr);
}

void DistributionRule::RemoveAlchemyEffectPoison(AlchemicEffect effect)
{
	poisonEffects->effects.erase(effect);
	poisonEffects->standardDistr = Utility::GetEffectDistribution(poisonEffects->effects);
	poisonEffects->validEffects = Utility::SumAlchemyEffects(poisonEffects->standardDistr);
}

void DistributionRule::RemoveAlchemyEffectFortifyPotion(AlchemicEffect effect)
{
	fortifyEffects->effects.erase(effect);
	fortifyEffects->standardDistr = Utility::GetEffectDistribution(fortifyEffects->effects);
	fortifyEffects->validEffects = Utility::SumAlchemyEffects(fortifyEffects->standardDistr);
}

void DistributionRule::RemoveAlchemyEffectFood(AlchemicEffect effect)
{
	foodEffects->effects.erase(effect);
	foodEffects->standardDistr = Utility::GetEffectDistribution(foodEffects->effects);
	foodEffects->validEffects = Utility::SumAlchemyEffects(foodEffects->standardDistr);
}

#pragma endregion
