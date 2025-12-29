#include "Settings.h"
#include "Logging.h"
#include "Utility.h"
#include "Compatibility.h"

using Comp = Compatibility;


/// <summary>
/// changes the output model of all consumable sounds to third person
/// </summary>
void Settings::FixConsumables()
{
	// get ITMPoisonUse as sound for applying poisons
	// ITMPoisonUse
	PoisonUse = RE::TESForm::LookupByID<RE::BGSSoundDescriptorForm>(0x106614);
	// get ITMPotionUse for sound fixes
	// ITMPotionUse
	PotionUse = RE::TESForm::LookupByID<RE::BGSSoundDescriptorForm>(0xB6435);
	// ITMFoodEat
	FoodEat = RE::TESForm::LookupByID<RE::BGSSoundDescriptorForm>(0xCAF94);

	if (!Settings::fixes._ForceFixPotionSounds)
		return;

	RE::TESForm* SOM_player1st = RE::TESForm::LookupByID(0xb4058);
	RE::TESForm* SOM_verb = RE::TESForm::LookupByID(0xd78b4);

	static std::unordered_set<RE::FormID> exlsounds{ 
		0x3EDC0 /*ITMPotionDownSD*/ ,
		0x3EDBD /*ITMPotionUpSD*/,
		0x3C7B9 /*ITMGenericDownSD*/,
		0x3C7BA /*ITMGenericUpSD*/,
		0xC8C6f /*UIAlchemyCreatePoison*/,
		0xC8C74 /*UIAlchemyCreatePotion*/,
		0xC8C72 /*UIAlchemyFail*/,
		0xC8C77 /*UIAlchemyLearnEffect*/,
		0x3C7B4 /*UIItemGenericDownSD*/,
		0x3C7B0 /*UIItemGenericUpSD*/
	};

	auto datahandler = RE::TESDataHandler::GetSingleton();

	if (SOM_player1st && SOM_verb) {
		RE::BGSSoundOutput* SOMMono01400_verb = SOM_verb->As<RE::BGSSoundOutput>();
		bool soundscomp = false;
		RE::TESForm* NUP_SOM_verb = nullptr;
		RE::BGSSoundOutput* NUP_SOMMono01400_verb = nullptr;
		if (Utility::Mods::GetPluginIndex("NPCsUsePotions.esp") != 0x1) {
			NUP_SOM_verb = Data::GetSingleton()->FindForm(0xD63, "NPCsUsePotions.esp");
			if (NUP_SOM_verb) {
				NUP_SOMMono01400_verb = NUP_SOM_verb->As<RE::BGSSoundOutput>();
				if (NUP_SOMMono01400_verb)
				{
					soundscomp = true;
				}
			}
		}
		RE::BGSSoundOutput* SOMMono01400Player1st = SOM_player1st->As<RE::BGSSoundOutput>();

		RE::BGSSoundDescriptor* sounddesc = nullptr;
		RE::BGSStandardSoundDef* soundOM = nullptr;

		// get all alchemyitems and then fix their consumption sounds
		auto alchs = datahandler->GetFormArray<RE::AlchemyItem>();
		for (auto& alch : alchs) {
			if (alch && alch->GetFormID() != 0x7) {
				if (alch->data.consumptionSound) {
					if (alch->IsPoison() && exlsounds.contains(alch->data.consumptionSound->GetFormID())) {
						alch->data.consumptionSound = PoisonUse;
						LOGL_4("changed consumption sound for {} to ITMPotionUse", Utility::PrintForm(alch));
					} else if (alch->IsFood() && exlsounds.contains(alch->data.consumptionSound->GetFormID())) {
						alch->data.consumptionSound = FoodEat;
						LOGL_4("changed consumption sound for {} to ITMPotionUse", Utility::PrintForm(alch));
					} else if (exlsounds.contains(alch->data.consumptionSound->GetFormID())) {
						alch->data.consumptionSound = PotionUse;
						LOGL_4("changed consumption sound for {} to ITMPotionUse", Utility::PrintForm(alch));
					} else {
						// consumption sound is non-empty
						sounddesc = alch->data.consumptionSound->soundDescriptor;
						soundOM = (RE::BGSStandardSoundDef*)sounddesc;
						if (soundscomp && (soundOM->outputModel == SOMMono01400Player1st || soundOM->outputModel == SOMMono01400_verb))
						{
							soundOM->outputModel = NUP_SOMMono01400_verb;
							LOGL_4("changed output model for sound {} used by item {} to custom ouput model", Utility::PrintForm(alch->data.consumptionSound), Utility::PrintForm(alch));
						}
						else if (soundOM->outputModel == SOMMono01400Player1st) {
							soundOM->outputModel = SOMMono01400_verb;
							LOGL_4("changed output model for sound {} used by item {}", Utility::PrintForm(alch->data.consumptionSound), Utility::PrintForm(alch));
						}
					}
				}
			}
		}
	}


	// Fix potion flags of specific items
	{
		RE::AlchemyItem* item = nullptr;

		// -----------Sacrilege - Minimalistic Vampires of Skyrim.esp-----------
		// Potion of Embracing
		if (item = Data::GetSingleton()->FindAlchemyItem(0xA1934, "Sacrilege - Minimalistic Vampires of Skyrim.esp"); item != nullptr) {
			item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
		}
		// Potion of Viscera
		if (item = Data::GetSingleton()->FindAlchemyItem(0xA193A, "Sacrilege - Minimalistic Vampires of Skyrim.esp"); item != nullptr) {
			item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
		}
		// Potion of Hunger
		if (item = Data::GetSingleton()->FindAlchemyItem(0x158E03, "Sacrilege - Minimalistic Vampires of Skyrim.esp"); item != nullptr) {
			item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
		}
		// Potion of Purging
		if (item = Data::GetSingleton()->FindAlchemyItem(0x176977, "Sacrilege - Minimalistic Vampires of Skyrim.esp"); item != nullptr) {
			item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
		}
		// Potion of Thaumaturgy
		if (item = Data::GetSingleton()->FindAlchemyItem(0x17B8C9, "Sacrilege - Minimalistic Vampires of Skyrim.esp"); item != nullptr) {
			item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
		}
	}
}

void Settings::Load()
{
	constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

	bool Ultimateoptions = false;
	{
		CSimpleIniA ini;

		ini.SetUnicode();
		ini.LoadFile(path);

		bool error = false;

		// read settings
		{
			// potions
			Settings::potions._enableHealthRestoration = ini.GetBoolValue("Potions", "EnableHealthRestoration", Settings::potions._enableHealthRestoration);
			loginfo("Setting: {} {}", "Potions:           EnableHealthRestoration", std::to_string(Settings::potions._enableHealthRestoration));
			Settings::potions._enableMagickaRestoration = ini.GetBoolValue("Potions", "EnableMagickaRestoration", Settings::potions._enableMagickaRestoration);
			loginfo("Setting: {} {}", "Potions:           EnableMagickaRestoration", std::to_string(Settings::potions._enableMagickaRestoration));
			Settings::potions._enableStaminaRestoration = ini.GetBoolValue("Potions", "EnableStaminaRestoration", Settings::potions._enableStaminaRestoration);
			loginfo("Setting: {} {}", "Potions:           EnableStaminaRestoration", std::to_string(Settings::potions._enableStaminaRestoration));
			Settings::potions._AllowDetrimentalEffects = ini.GetBoolValue("Potions", "AllowDetrimentalEffects", Settings::potions._AllowDetrimentalEffects);
			loginfo("Setting: {} {}", "Potions:           AllowDetrimentalEffects", std::to_string(Settings::potions._AllowDetrimentalEffects));
			Settings::potions._HandleWeaponSheathedAsOutOfCombat = ini.GetBoolValue("Potions", "HandleWeaponSheathedAsOutOfCombat", Settings::potions._HandleWeaponSheathedAsOutOfCombat);
			loginfo("Setting: {} {}", "Potions:           HandleWeaponSheathedAsOutOfCombat", std::to_string(Settings::potions._HandleWeaponSheathedAsOutOfCombat));

			Settings::potions._healthThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "HealthThresholdPercent", Settings::potions._healthThreshold));
			Settings::potions._healthThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "HealthThresholdLowerPercent", Settings::potions._healthThreshold));
			if (Settings::potions._healthThreshold > 0.95f)
				Settings::potions._healthThreshold = 0.95f;
			loginfo("Setting: {} {}", "Potions:           HealthThresholdPercent", std::to_string(Settings::potions._healthThreshold));
			Settings::potions._magickaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "MagickaThresholdPercent", Settings::potions._magickaThreshold));
			Settings::potions._magickaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "MagickaThresholdLowerPercent", Settings::potions._magickaThreshold));
			if (Settings::potions._magickaThreshold > 0.95f)
				Settings::potions._magickaThreshold = 0.95f;
			loginfo("Setting: {} {}", "Potions:           MagickaThresholdPercent", std::to_string(Settings::potions._magickaThreshold));
			Settings::potions._staminaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "StaminaThresholdPercent", Settings::potions._staminaThreshold));
			Settings::potions._staminaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "StaminaThresholdLowerPercent", Settings::potions._staminaThreshold));
			if (Settings::potions._staminaThreshold > 0.95f)
				Settings::potions._staminaThreshold = 0.95f;
			loginfo("Setting: {} {}", "Potions:           StaminaThresholdPercent", std::to_string(Settings::potions._staminaThreshold));
			Settings::potions._UsePotionChance = static_cast<int>(ini.GetLongValue("Potions", "UsePotionChance", Settings::potions._UsePotionChance));
			loginfo("Setting: {} {}", "Potions:           UsePotionChance", std::to_string(Settings::potions._UsePotionChance));
			try {
				std::string prohib = ini.GetValue("Potions", "ProhibitedEffects", "");
				if (prohib.empty() == false)
					Settings::potions._prohibitedEffects = AlchemicEffect(std::string(prohib));
			} catch (std::exception) {
			}
			loginfo("Setting: {} {}", "Potions:           ProhibitedEffects", Settings::potions._prohibitedEffects.string());
			Distribution::_probabilityAdjustersPotion.clear();
			Utility::ReadAdjusters(ini.GetValue("Potions", "PotionProbabilityAdjusters", ""), &Distribution::_probabilityAdjustersPotion, error);
			if (error) {
				logcritical("Cannot read Potion Probability Adjusters");
				error = false;
			}
			loginfo("Setting: {} {}", "Potions:           PotionProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersPotion));

			// poisons
			Settings::poisons._enablePoisons = ini.GetBoolValue("Poisons", "EnablePoisonUsage", Settings::poisons._enablePoisons);
			loginfo("Setting: {} {}", "Poisons:           EnablePoisonUsage", std::to_string(Settings::poisons._enablePoisons));
			Settings::poisons._AllowPositiveEffects = ini.GetBoolValue("Poisons", "AllowPositiveEffects", Settings::poisons._AllowPositiveEffects);
			loginfo("Setting: {} {}", "Poisons:           AllowPositiveEffects", std::to_string(Settings::poisons._AllowPositiveEffects));
			Settings::poisons._DontUseWithWeaponsSheathed = ini.GetBoolValue("Poisons", "DontUseWithWeaponsSheathed", Settings::poisons._DontUseWithWeaponsSheathed);
			loginfo("Setting: {} {}", "Poisons:           DontUseWithWeaponsSheathed", std::to_string(Settings::poisons._DontUseWithWeaponsSheathed));
			Settings::poisons._DontUseAgainst100PoisonResist = ini.GetBoolValue("Poisons", "DontUseAgainst100PoisonResist", Settings::poisons._DontUseAgainst100PoisonResist);
			loginfo("Setting: {} {}", "Poisons:           DontUseAgainst100PoisonResist", std::to_string(Settings::poisons._DontUseAgainst100PoisonResist));
			Settings::poisons._EnemyLevelScalePlayerLevel = static_cast<float>(ini.GetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", Settings::poisons._EnemyLevelScalePlayerLevel));
			loginfo("Setting: {} {}", "Poisons:           EnemyLevelScalePlayerLevel", std::to_string(Settings::poisons._EnemyLevelScalePlayerLevel));
			Settings::poisons._EnemyNumberThreshold = ini.GetLongValue("Poisons", "FightingNPCsNumberThreshold", Settings::poisons._EnemyNumberThreshold);
			loginfo("Setting: {} {}", "Poisons:           FightingNPCsNumberThreshold", std::to_string(Settings::poisons._EnemyNumberThreshold));
			Settings::poisons._UsePoisonChance = static_cast<int>(ini.GetLongValue("Poisons", "UsePoisonChance", Settings::poisons._UsePoisonChance));
			loginfo("Setting: {} {}", "Poisons:           UsePoisonChance", std::to_string(Settings::poisons._UsePoisonChance));
			Settings::poisons._Dosage = static_cast<int>(ini.GetLongValue("Poisons", "Dosage", Settings::poisons._Dosage));
			loginfo("Setting: {} {}", "Poisons:           Dosage", std::to_string(Settings::poisons._Dosage));
			try {
				std::string prohib = ini.GetValue("Poisons", "ProhibitedEffects", "");
				if (prohib.empty() == false)
					Settings::poisons._prohibitedEffects = AlchemicEffect(prohib);
			} catch (std::exception) {}
			loginfo("Setting: {} {}", "Poisons:           ProhibitedEffects", Settings::poisons._prohibitedEffects.string());
			Distribution::_probabilityAdjustersPoison.clear();
			Utility::ReadAdjusters(ini.GetValue("Poisons", "PoisonProbabilityAdjusters", ""), &Distribution::_probabilityAdjustersPoison, error);
			if (error) {
				logcritical("Cannot read Poisons Probability Adjusters");
				error = false;
			}
			loginfo("Setting: {} {}", "Poisons:           PoisonProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersPoison));

			// fortify potions
			Settings::fortifyPotions._enableFortifyPotions = ini.GetBoolValue("FortifyPotions", "EnableFortifyPotionUsage", Settings::fortifyPotions._enableFortifyPotions);
			loginfo("Setting: {} {}", "Fortify Potions:   EnableFortifyPotionUsage", std::to_string(Settings::fortifyPotions._enableFortifyPotions));
			Settings::fortifyPotions._DontUseWithWeaponsSheathed = ini.GetBoolValue("FortifyPotions", "DontUseWithWeaponsSheathed", Settings::fortifyPotions._DontUseWithWeaponsSheathed);
			loginfo("Setting: {} {}", "Fortify Potions:   DontUseWithWeaponsSheathed", std::to_string(Settings::fortifyPotions._DontUseWithWeaponsSheathed));
			Settings::fortifyPotions._EnemyLevelScalePlayerLevelFortify = static_cast<float>(ini.GetDoubleValue("FortifyPotions", "EnemyLevelScalePlayerLevelFortify", Settings::fortifyPotions._EnemyLevelScalePlayerLevelFortify));
			loginfo("Setting: {} {}", "Fortify Potions:   EnemyLevelScalePlayerLevelFortify", std::to_string(Settings::fortifyPotions._EnemyLevelScalePlayerLevelFortify));
			Settings::fortifyPotions._EnemyNumberThresholdFortify = ini.GetLongValue("FortifyPotions", "FightingNPCsNumberThresholdFortify", Settings::fortifyPotions._EnemyNumberThresholdFortify);
			loginfo("Setting: {} {}", "Fortify Potions:   FightingNPCsNumberThresholdFortify", std::to_string(Settings::fortifyPotions._EnemyNumberThresholdFortify));
			Settings::fortifyPotions._UseFortifyPotionChance = static_cast<int>(ini.GetLongValue("FortifyPotions", "UseFortifyPotionChance", Settings::fortifyPotions._UseFortifyPotionChance));
			loginfo("Setting: {} {}", "Fortify Potions:   UseFortifyPotionChance", std::to_string(Settings::fortifyPotions._UseFortifyPotionChance));
			Distribution::_probabilityAdjustersFortify.clear();
			Utility::ReadAdjusters(ini.GetValue("FortifyPotions", "FortifyProbabilityAdjusters", ""), &Distribution::_probabilityAdjustersFortify, error);
			if (error) {
				logcritical("Cannot read Fortify Probability Adjusters");
				error = false;
			}
			loginfo("Setting: {} {}", "Fortify Potions:           FortifyProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersFortify));

			// food
			Settings::food._enableFood = ini.GetBoolValue("Food", "EnableFoodUsage", Settings::food._enableFood);
			loginfo("Setting: {} {}", "Food:              EnableFoodUsage", std::to_string(Settings::food._enableFood));
			Settings::food._AllowDetrimentalEffects = ini.GetBoolValue("Food", "AllowDetrimentalEffects", Settings::food._AllowDetrimentalEffects);
			loginfo("Setting: {} {}", "Food:              AllowDetrimentalEffects", std::to_string(Settings::food._AllowDetrimentalEffects));
			Settings::food._RestrictFoodToCombatStart = ini.GetBoolValue("Food", "OnlyAllowFoodAtCombatStart", Settings::food._RestrictFoodToCombatStart);
			loginfo("Setting: {} {}", "Food:              OnlyAllowFoodAtCombatStart", std::to_string(Settings::food._RestrictFoodToCombatStart));
			Settings::food._DisableFollowers = ini.GetBoolValue("Food", "DisableFollowers", Settings::food._DisableFollowers);
			loginfo("Setting: {} {}", "Food:              DisableFollowers", std::to_string(Settings::food._DisableFollowers));
			Settings::food._DontUseWithWeaponsSheathed = ini.GetBoolValue("Food", "DontUseWithWeaponsSheathed", Settings::food._DontUseWithWeaponsSheathed);
			loginfo("Setting: {} {}", "Food:              DontUseWithWeaponsSheathed", std::to_string(Settings::food._DontUseWithWeaponsSheathed));
			try {
				std::string prohib = ini.GetValue("Food", "ProhibitedEffects", "");
				if (prohib.empty() == false)
					Settings::food._prohibitedEffects = AlchemicEffect(prohib);
			} catch (std::exception) {}
			loginfo("Setting: {} {}", "Food:           ProhibitedEffects", Settings::food._prohibitedEffects.string());
			Distribution::_probabilityAdjustersFood.clear();
			Utility::ReadAdjusters(ini.GetValue("Food", "FoodProbabilityAdjusters", ""), &Distribution::_probabilityAdjustersFood, error);
			if (error) {
				logcritical("Cannot read Food Probability Adjusters");
				error = false;
			}
			loginfo("Setting: {} {}", "Food:           FoodProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersFood));

			// player
			Settings::player._playerPotions = ini.GetBoolValue("Player", "EnablePlayerPotions", Settings::player._playerPotions);
			loginfo("Setting: {} {}", "Player:            EnablePlayerPotions", std::to_string(Settings::player._playerPotions));
			Settings::player._playerPoisons = ini.GetBoolValue("Player", "EnablePlayerPoisonUsage", Settings::player._playerPoisons);
			loginfo("Setting: {} {}", "Player:            EnablePlayerPoisonUsage", std::to_string(Settings::player._playerPoisons));
			Settings::player._playerFortifyPotions = ini.GetBoolValue("Player", "EnablePlayerFortifyPotionUsage", Settings::player._playerFortifyPotions);
			loginfo("Setting: {} {}", "Player:            EnablePlayerFortifyPotionUsage", std::to_string(Settings::player._playerFortifyPotions));
			Settings::player._playerFood = ini.GetBoolValue("Player", "EnablePlayerFoodUsage", Settings::player._playerFood);
			loginfo("Setting: {} {}", "Player:            EnablePlayerFoodUsage", std::to_string(Settings::player._playerFood));

			Settings::player._UseFavoritedItemsOnly = ini.GetBoolValue("Player", "UseFavoritedItemsOnly", Settings::player._UseFavoritedItemsOnly);
			loginfo("Setting: {} {}", "Player:            UseFavoritedItemsOnly", std::to_string(Settings::player._UseFavoritedItemsOnly));
			Settings::player._DontUseFavoritedItems = ini.GetBoolValue("Player", "DontUseFavoritedItems", Settings::player._DontUseFavoritedItems);
			loginfo("Setting: {} {}", "Player:            DontUseFavoritedItems", std::to_string(Settings::player._DontUseFavoritedItems));
			if (Settings::player._UseFavoritedItemsOnly && Settings::player._DontUseFavoritedItems)
				Settings::player._UseFavoritedItemsOnly = false;
			Settings::player._DontEatRawFood = ini.GetBoolValue("Player", "DontEatRawFood", Settings::player._DontEatRawFood);
			loginfo("Setting: {} {}", "Player:            DontEatRawFood", std::to_string(Settings::player._DontEatRawFood));
			Settings::player._DontDrinkAlcohol = ini.GetBoolValue("Player", "DontDrinkAlcohol", Settings::player._DontDrinkAlcohol);
			loginfo("Setting: {} {}", "Player:            DontDrinkAlcohol", std::to_string(Settings::player._DontDrinkAlcohol));

			// usage
			Settings::usage._DisableItemUsageWhileStaggered = ini.GetBoolValue("Usage", "DisableItemUsageWhileStaggered", Settings::usage._DisableItemUsageWhileStaggered);
			loginfo("Setting: {} {}", "usage:             DisableItemUsageWhileStaggered", std::to_string(Settings::usage._DisableItemUsageWhileStaggered));
			Settings::usage._DisableItemUsageWhileFlying = ini.GetBoolValue("Usage", "DisableItemUsageWhileFlying", Settings::usage._DisableItemUsageWhileFlying);
			loginfo("Setting: {} {}", "usage:             DisableItemUsageWhileFlying", std::to_string(Settings::usage._DisableItemUsageWhileFlying));
			Settings::usage._DisableItemUsageWhileBleedingOut = ini.GetBoolValue("Usage", "DisableItemUsageWhileBleedingOut", Settings::usage._DisableItemUsageWhileBleedingOut);
			loginfo("Setting: {} {}", "usage:             DisableItemUsageWhileBleedingOut", std::to_string(Settings::usage._DisableItemUsageWhileBleedingOut));
			Settings::usage._DisableItemUsageWhileSleeping = ini.GetBoolValue("Usage", "DisableItemUsageWhileSleeping", Settings::usage._DisableItemUsageWhileSleeping);
			loginfo("Setting: {} {}", "usage:             DisableItemUsageWhileSleeping", std::to_string(Settings::usage._DisableItemUsageWhileSleeping));
			Settings::usage._DisableNonFollowerNPCs = ini.GetBoolValue("Usage", "DisableNonFollowerNPCs", Settings::usage._DisableNonFollowerNPCs);
			loginfo("Setting: {} {}", "usage:             DisableNonFollowerNPCs", std::to_string(Settings::usage._DisableNonFollowerNPCs));
			Settings::usage._DisableOutOfCombatProcessing = ini.GetBoolValue("Usage", "DisableOutOfCombatProcessing", Settings::usage._DisableOutOfCombatProcessing);
			loginfo("Setting: {} {}", "usage:             DisableOutOfCombatProcessing", std::to_string(Settings::usage._DisableOutOfCombatProcessing));
			Settings::usage._DisableItemUsageForExcludedNPCs = ini.GetBoolValue("Usage", "DisableItemUsageForExcludedNPCs", Settings::usage._DisableItemUsageForExcludedNPCs);
			loginfo("Setting: {} {}", "usage:             DisableItemUsageForExcludedNPCs", std::to_string(Settings::usage._DisableItemUsageForExcludedNPCs));
			Settings::usage._globalCooldown = ini.GetLongValue("Usage", "GlobalItemCooldown", Settings::usage._globalCooldown);
			loginfo("Setting: {} {}", "usage:             GlobalItemCooldown", std::to_string(Settings::usage._globalCooldown));

			// distribution

			Settings::distr._DistributePotions = ini.GetBoolValue("Distribution", "DistributePotions", Settings::distr._DistributePotions);
			loginfo("Setting: {} {}", "Distribution:      DistributePotions", std::to_string(Settings::distr._DistributePotions));
			Settings::distr._DistributePoisons = ini.GetBoolValue("Distribution", "DistributePoisons", Settings::distr._DistributePoisons);
			loginfo("Setting: {} {}", "Distribution:      DistributePoisons", std::to_string(Settings::distr._DistributePoisons));
			Settings::distr._DistributeFood = ini.GetBoolValue("Distribution", "DistributeFood", Settings::distr._DistributeFood);
			loginfo("Setting: {} {}", "Distribution:      DistributeFood", std::to_string(Settings::distr._DistributeFood));
			Settings::distr._DistributeFortifyPotions = ini.GetBoolValue("Distribution", "DistributeFortifyPotions", Settings::distr._DistributeFortifyPotions);
			loginfo("Setting: {} {}", "Distribution:      DistributeFortifyPotions", std::to_string(Settings::distr._DistributeFortifyPotions));
			Settings::distr._DistributeCustomItems = ini.GetBoolValue("Distribution", "DistributeCustomItems", Settings::distr._DistributeCustomItems);
			loginfo("Setting: {} {}", "Distribution:      DistributeCustomItems", std::to_string(Settings::distr._DistributeCustomItems));

			Settings::distr._LevelEasy = ini.GetLongValue("Distribution", "LevelEasy", Settings::distr._LevelEasy);
			loginfo("Setting: {} {}", "Distribution:      LevelEasy", std::to_string(Settings::distr._LevelEasy));
			Settings::distr._LevelNormal = ini.GetLongValue("Distribution", "LevelNormal", Settings::distr._LevelNormal);
			loginfo("Setting: {} {}", "Distribution:      LevelNormal", std::to_string(Settings::distr._LevelNormal));
			Settings::distr._LevelDifficult = ini.GetLongValue("Distribution", "LevelDifficult", Settings::distr._LevelDifficult);
			loginfo("Setting: {} {}", "Distribution:      LevelDifficult", std::to_string(Settings::distr._LevelDifficult));
			Settings::distr._LevelInsane = ini.GetLongValue("Distribution", "LevelInsane", Settings::distr._LevelInsane);
			loginfo("Setting: {} {}", "Distribution:      LevelInsane", std::to_string(Settings::distr._LevelInsane));

			Settings::distr._GameDifficultyScaling = ini.GetBoolValue("Distribution", "GameDifficultyScaling", Settings::distr._GameDifficultyScaling);
			loginfo("Setting: {} {}", "Distribution:      GameDifficultyScaling", std::to_string(Settings::distr._GameDifficultyScaling));

			Settings::distr._MaxMagnitudeWeak = ini.GetLongValue("Distribution", "MaxMagnitudeWeak", Settings::distr._MaxMagnitudeWeak);
			loginfo("Setting: {} {}", "Distribution:      MaxMagnitudeWeak", std::to_string(Settings::distr._MaxMagnitudeWeak));
			Settings::distr._MaxMagnitudeStandard = ini.GetLongValue("Distribution", "MaxMagnitudeStandard", Settings::distr._MaxMagnitudeStandard);
			loginfo("Setting: {} {}", "Distribution:      MaxMagnitudeStandard", std::to_string(Settings::distr._MaxMagnitudeStandard));
			Settings::distr._MaxMagnitudePotent = ini.GetLongValue("Distribution", "MaxMagnitudePotent", Settings::distr._MaxMagnitudePotent);
			loginfo("Setting: {} {}", "Distribution:      MaxMagnitudePotent", std::to_string(Settings::distr._MaxMagnitudePotent));

			Settings::distr._StyleScalingPrimary = (float)ini.GetDoubleValue("Distribution", "StyleScalingPrimary", Settings::distr._StyleScalingPrimary);
			loginfo("Setting: {} {}", "Distribution:      StyleScalingPrimary", std::to_string(Settings::distr._StyleScalingPrimary));
			Settings::distr._StyleScalingSecondary = (float)ini.GetDoubleValue("Distribution", "StyleScalingSecondary", Settings::distr._StyleScalingSecondary);
			loginfo("Setting: {} {}", "Distribution:      StyleScalingSecondary", std::to_string(Settings::distr._StyleScalingSecondary));

			Settings::distr._ProbabilityScaling = (float)ini.GetDoubleValue("Distribution", "ProbabilityScaling", Settings::distr._ProbabilityScaling);
			loginfo("Setting: {} {}", "Distribution:      ProbabilityScaling", std::to_string(Settings::distr._ProbabilityScaling));

			Settings::distr._DoNotDistributeMixedInvisPotions = ini.GetBoolValue("Distribution", "DoNotDistributeMixedInvisPotions", Settings::distr._DoNotDistributeMixedInvisPotions);
			loginfo("Setting: {} {}", "Distribution:      DoNotDistributeMixedInvisPotions", std::to_string(Settings::distr._DoNotDistributeMixedInvisPotions));

			// removal
			Settings::removal._RemoveItemsOnDeath = ini.GetBoolValue("Removal", "RemoveItemsOnDeath", Settings::removal._RemoveItemsOnDeath);
			loginfo("Setting: {} {}", "Removal:           RemoveItemsOnDeath", std::to_string(Settings::removal._RemoveItemsOnDeath));
			Settings::removal._ChanceToRemoveItem = ini.GetLongValue("Removal", "ChanceToRemoveItem", Settings::removal._ChanceToRemoveItem);
			loginfo("Setting: {} {}", "Removal:           ChanceToRemoveItem", std::to_string(Settings::removal._ChanceToRemoveItem));
			Settings::removal._MaxItemsLeft = ini.GetLongValue("Removal", "MaxItemsLeftAfterRemoval", Settings::removal._MaxItemsLeft);
			loginfo("Setting: {} {}", "Removal:           MaxItemsLeftAfterRemoval", std::to_string(Settings::removal._MaxItemsLeft));

			// whitelist mode
			Settings::whitelist.EnabledItems = ini.GetBoolValue("Whitelist Mode", "EnableWhitelistItems", Settings::whitelist.EnabledItems);
			loginfo("Setting: {} {}", "Whitelist Mode:    EnableWhitelistItems", std::to_string(Settings::whitelist.EnabledItems));
			Settings::whitelist.EnabledNPCs = ini.GetBoolValue("Whitelist Mode", "EnableWhitelistNPCs", Settings::whitelist.EnabledNPCs);
			loginfo("Setting: {} {}", "Whitelist Mode:    EnableWhitelistNPCs", std::to_string(Settings::whitelist.EnabledNPCs));

			// fixes
			Settings::fixes._ApplySkillBoostPerks = ini.GetBoolValue("Fixes", "ApplySkillBoostPerks", Settings::fixes._ApplySkillBoostPerks);
			loginfo("Setting: {} {}", "Fixes:             ApplySkillBoostPerks", std::to_string(Settings::fixes._ApplySkillBoostPerks));
			Settings::fixes._ForceFixPotionSounds = ini.GetBoolValue("Fixes", "ForceFixPotionSounds", Settings::fixes._ForceFixPotionSounds);
			loginfo("Setting: {} {}", "Fixes:             ForceFixPotionSounds", std::to_string(Settings::fixes._ForceFixPotionSounds));

			// system
			Settings::system._cycletime = ini.GetLongValue("System", "CycleWaitTime", Settings::system._cycletime);
			if (Settings::system._cycletime < 500)
				Settings::system._cycletime = 500;
			loginfo("Setting: {} {}", "System:            CycleWaitTime", std::to_string(Settings::system._cycletime));
			Settings::system._MaxDuration = ini.GetLongValue("System", "MaxDuration", Settings::system._MaxDuration);
			loginfo("Setting: {} {}", "System:            MaxDuration", std::to_string(Settings::system._MaxDuration));
			Settings::system._MaxFortifyDuration = ini.GetLongValue("System", "MaxFortifyDuration", Settings::system._MaxFortifyDuration);
			loginfo("Setting: {} {}", "System:            MaxFortifyDuration", std::to_string(Settings::system._MaxFortifyDuration));
			//Settings::system._alternateNPCRegistration = ini.GetBoolValue("System", "AlternateNPCRegistration", Settings::system._alternateNPCRegistration);
			//loginfo("Setting: {} {}", "System:            AlternateNPCRegistration", std::to_string(Settings::system._alternateNPCRegistration));
			Settings::system._killSwitch = ini.GetBoolValue("System", "KillSwitch", Settings::system._killSwitch);
			loginfo("Setting: {} {}", "System:            KillSwitch", std::to_string(Settings::system._killSwitch));

			// compatibility
			Settings::compatibility._DisableCreaturesWithoutRules = ini.GetBoolValue("Compatibility", "DisableCreaturesWithoutRules", Settings::compatibility._DisableCreaturesWithoutRules);
			loginfo("Setting: {} {}", "Compatibility:     DisableCreaturesWithoutRules", std::to_string(Settings::compatibility._DisableCreaturesWithoutRules));

			// compatibility animated poisons
			Settings::compatibility.animatedPoisons._Enable = ini.GetBoolValue("Compatibility: Animated Poisons", "EnableAnimatedPoisons", Settings::compatibility.animatedPoisons._Enable);
			loginfo("Setting: {} {}", "Compatibility:     EnableAnimatedPoisons", std::to_string(Settings::compatibility.animatedPoisons._Enable));
			Settings::compatibility.animatedPoisons._UsePoisonDosage = ini.GetBoolValue("Compatibility: Animated Poisons", "UseAnimatedPoisonsDosageSystem", Settings::compatibility.animatedPoisons._UsePoisonDosage);
			loginfo("Setting: {} {}", "Compatibility:     UseAnimatedPoisonsDosageSystem", std::to_string(Settings::compatibility.animatedPoisons._UsePoisonDosage));

			// compatibility animated potions
			Settings::compatibility.animatedPotions._Enable = ini.GetBoolValue("Compatibility: Animated Potions", "EnableAnimatedPotions", Settings::compatibility.animatedPotions._Enable);
			loginfo("Setting: {} {}", "Compatibility:     EnableAnimatedPotions", std::to_string(Settings::compatibility.animatedPotions._Enable));

			// debug
			Settings::debug.EnableLog = ini.GetBoolValue("Debug", "EnableLogging", Settings::debug.EnableLog);
			Logging::EnableLog = Settings::debug.EnableLog;
			loginfo("Setting: {} {}", "Debug:             EnableLogging", std::to_string(Settings::debug.EnableLog));
			Settings::debug.EnableLoadLog = ini.GetBoolValue("Debug", "EnableLoadLogging", Settings::debug.EnableLoadLog);
			Logging::EnableLoadLog = Settings::debug.EnableLoadLog;
			loginfo("Setting: {} {}", "Debug:             EnableLoadLogging", std::to_string(Settings::debug.EnableLoadLog));
			Settings::debug.LogLevel = ini.GetLongValue("Debug", "LogLevel", Settings::debug.LogLevel);
			Logging::LogLevel = Settings::debug.LogLevel;
			loginfo("Setting: {} {}", "Debug:             LogLevel", std::to_string(Settings::debug.LogLevel));
			Settings::debug.EnableProfiling = ini.GetBoolValue("Debug", "EnableProfiling", Settings::debug.EnableProfiling);
			Logging::EnableProfiling = Settings::debug.EnableProfiling;
			loginfo("Setting: {} {}", "Debug:             EnableProfiling", std::to_string(Settings::debug.EnableProfiling));
			Settings::debug.ProfileLevel = ini.GetLongValue("Debug", "ProfileLevel", Settings::debug.ProfileLevel);
			Logging::ProfileLevel = Settings::debug.ProfileLevel;
			loginfo("Setting: {} {}", "Debug:             ProfileLevel", std::to_string(Settings::debug.LogLevel));

			Settings::debug._CheckActorsWithoutRules = ini.GetBoolValue("Debug", "CheckActorWithoutRules", Settings::debug._CheckActorsWithoutRules);
			loginfo("Setting: {} {}", "Debug:             CheckActorWithoutRules", std::to_string(Settings::debug._CheckActorsWithoutRules));

			Settings::debug._CalculateCellRules = ini.GetBoolValue("Debug", "CalculateCellRules", Settings::debug._CalculateCellRules);
			loginfo("Setting: {} {}", "Debug:             CalculateCellRules", std::to_string(Settings::debug._CalculateCellRules));
			Settings::debug._Test = ini.GetBoolValue("Debug", "CalculateAllCellOnStartup", Settings::debug._Test);
			loginfo("Setting: {} {}", "Debug:             CalculateAllCellOnStartup", std::to_string(Settings::debug._Test));
			if (Settings::debug._CalculateCellRules && Settings::debug._Test == false) {
				std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv", std::ofstream::out);
				out << "CellName;RuleApplied;PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
			}

			Settings::debug._CompatibilityRemoveItemsBeforeDist = ini.GetBoolValue("Debug", "RemoveItemsBeforeDist", Settings::debug._CompatibilityRemoveItemsBeforeDist);
			loginfo("Setting: {} {}", "Debug:             RemoveItemsBeforeDist", std::to_string(Settings::debug._CompatibilityRemoveItemsBeforeDist));
			Settings::debug._CompatibilityRemoveItemsStartup = ini.GetBoolValue("Debug", "RemoveItemsStartup", Settings::debug._CompatibilityRemoveItemsStartup);
			loginfo("Setting: {} {}", "Debug:             RemoveItemsStartup", std::to_string(Settings::debug._CompatibilityRemoveItemsStartup));
			Settings::debug._CompatibilityRemoveItemsStartup_OnlyExcluded = ini.GetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", Settings::debug._CompatibilityRemoveItemsStartup_OnlyExcluded);
			loginfo("Setting: {} {}", "Debug:             RemoveItemsStartup_OnlyExcluded", std::to_string(Settings::debug._CompatibilityRemoveItemsStartup_OnlyExcluded));
		}
	}

	// save user settings, before applying adjustments
	//Save();

	FixConsumables();
}


void Settings::Save()
{
	// reset change flag
	Settings::_modifiedSettings = Settings::ChangeFlag::kNone;

	loginfo("Saving Settings.");

	constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

	CSimpleIniA ini;

	ini.SetUnicode();

	// potions
	ini.SetBoolValue("Potions", "EnableHealthRestoration", Settings::potions._enableHealthRestoration, "// NPCs use health potions to restore their missing hp in combat.");
	ini.SetBoolValue("Potions", "EnableMagickaRestoration", Settings::potions._enableMagickaRestoration, "// NPCs use magicka potions to restore their missing magicka in combat.");
	ini.SetBoolValue("Potions", "EnableStaminaRestoration", Settings::potions._enableStaminaRestoration, "// NPCs use stamina potions to restore their missing stamina in combat.");
	ini.SetBoolValue("Potions", "AllowDetrimentalEffects", Settings::potions._AllowDetrimentalEffects, "// If this is enabled NPCs will use potions that contain detrimental\n"
																								"// effects. For instance, impure potions, that restore health and damage magicka.\n"
																								"// !!!This setting also affects fortify potions.");
	ini.SetBoolValue("Potions", "HandleWeaponSheathedAsOutOfCombat", Settings::potions._HandleWeaponSheathedAsOutOfCombat, "// If weapons are not drawn in combat, it will be treated as Out-of-Combat.\n"
																													"// This currently means only health potions will be used.");

	ini.SetDoubleValue("Potions", "HealthThresholdPercent", Settings::potions._healthThreshold, "// Upon reaching this threshold, NPCs will start to use health potions.");
	ini.SetDoubleValue("Potions", "MagickaThresholdPercent", Settings::potions._magickaThreshold, "// Upon reaching this threshold, NPCs will start to use magicka potions.");
	ini.SetDoubleValue("Potions", "StaminaThresholdPercent", Settings::potions._staminaThreshold, "// Upon reaching this threshold, NPCs will start to use stamina potions.");
	ini.SetLongValue("Potions", "UsePotionChance", Settings::potions._UsePotionChance, "// Chance that an NPC will use a potion if they can. Set to 100 to always \n"
																				"// take a potion, when appropiate.\n"
																				"// An NPC can use a potion if they (1) have a potion in their inventory, \n"
																				"// and (2) when the respective value falls below the threshold.");
	ini.SetValue("Potions", "ProhibitedEffects", Settings::potions._prohibitedEffects.string().c_str(), "// Items with the listed effects will not be distributed to NPCs.");
	ini.SetValue("Potions", "PotionProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersPotion).c_str(),
		"// Global probability adjusters for potion effects");


	// poisons
	ini.SetBoolValue("Poisons", "EnablePoisonUsage", Settings::poisons._enablePoisons, "// NPCs use appropiate poisons in combat. Poisons are considered appropiate,\n"
																				"// if they can harm the enemy. For instance, damaging Magicka of an enemy \n"
																				"// that does not use spells, is not appropiate.");
	ini.SetBoolValue("Poisons", "AllowPositiveEffects", Settings::poisons._AllowPositiveEffects, "// This allows NPCs to use poisons that apply positive effects to their opponents.");
	ini.SetBoolValue("Poisons", "DontUseWithWeaponsSheathed", Settings::poisons._DontUseWithWeaponsSheathed, "// If the weapons are sheathed, poisons will not be used.");
	ini.SetBoolValue("Poisons", "DontUseAgainst100PoisonResist", Settings::poisons._DontUseAgainst100PoisonResist, "// NPCs and the player will not use Poisons against those that have 100% poison resistance anymore.");
	ini.SetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", Settings::poisons._EnemyLevelScalePlayerLevel, "// If the enemy they are facing has a level greater equal \n"
																										"// 'this value' * PlayerLevel followers use poisons.");
	ini.SetLongValue("Poisons", "FightingNPCsNumberThreshold", Settings::poisons._EnemyNumberThreshold, "// When the number of NPCs in a fight is at least at this value, followers\n"
																								"// start to use poisons regardless of the enemies level, to faster help out \n"
																								"// the player.\n"
																								"// This includes hostile and non-hostile NPCs.");
	ini.SetLongValue("Poisons", "UsePoisonChance", Settings::poisons._UsePoisonChance, "// Chance that an NPC will use a poison if they possess one with an \n// appropiate effect.");
	ini.SetLongValue("Poisons", "Dosage", Settings::poisons._Dosage,
		"// The dosage describes the number of hits a poison lasts on your weapons.\n"
		"// The dosage for specific poisons, or alchemic effects can also be defined \n"
		"// by distribution rules and may outrule this setting.");
	ini.SetValue("Poisons", "ProhibitedEffects", Settings::poisons._prohibitedEffects.string().c_str(), "// Items with the listed effects will not be distributed to NPCs.");
	ini.SetValue("Poisons", "PoisonProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersPoison).c_str(),
		"// Global probability adjusters for poison effects");


	// fortify potions
	ini.SetBoolValue("FortifyPotions", "EnableFortifyPotionUsage", Settings::fortifyPotions._enableFortifyPotions, "// NPCs use fortify potions in combat.\n"
																											"// Potions are used based on the equipped weapons and spells.");
	ini.SetBoolValue("FortifyPotions", "DontUseWithWeaponsSheathed", Settings::fortifyPotions._DontUseWithWeaponsSheathed, "// When weapons are sheathed, no fortify potions will be used.");

	ini.SetDoubleValue("FortifyPotions", "EnemyLevelScalePlayerLevelFortify", Settings::fortifyPotions._EnemyLevelScalePlayerLevelFortify, "// If the enemy they are facing has a level greater equal \n"
																																	"// 'this value' * PlayerLevel followers use fortify potions.");
	ini.SetLongValue("FortifyPotions", "FightingNPCsNumberThresholdFortify", Settings::fortifyPotions._EnemyNumberThresholdFortify, "// When the number of NPCs in a fight is at least at this value, followers \n"
																															"// start to use fortify potions regardless of the enemies level.\n"
																															"// This includes hostile and non-hostile NPCs.");
	ini.SetLongValue("FortifyPotions", "UseFortifyPotionChance", Settings::fortifyPotions._UseFortifyPotionChance, "// Chance that an NPC will use a fortify potion if they can. They can use a \n"
																											"// potion, if they have one in their inventory, and its effects are \n"
																											"// beneficial to their current choice of weapons.\n"
																											"// An NPC will not use a Fortify Light Armor potion if they are wearing \n"
																											"// Heavy Armor, for instance.\n");

	ini.SetValue("FortifyPotions", "FortifyProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersFortify).c_str(),
		"// Global probability adjusters for fortify potion effects");


	// food
	ini.SetBoolValue("Food", "EnableFoodUsage", Settings::food._enableFood, "// Allows NPCs to use food items, to gain beneficial effects.");
	ini.SetBoolValue("Food", "AllowDetrimentalEffects", Settings::food._AllowDetrimentalEffects, "// This allows NPCs to use food that has detrimental effects.");
	ini.SetBoolValue("Food", "OnlyAllowFoodAtCombatStart", Settings::food._RestrictFoodToCombatStart, "// NPCs will only eat food at the beginning of combat. This is the way \n"
																							"// it worked until version 3.0.0\n"
																							"// If disabled, NPCs will try to eat more food as soon as an existing \n"
																							"// food buff runs out.");
	ini.SetBoolValue("Food", "DisableFollowers", Settings::food._DisableFollowers, "// Disables food usage for followers only. You can use this to prevent your followers\n"
																			"// from eating food, if you are using survival mods, without impacting other NPCs.");
	ini.SetBoolValue("Food", "DontUseWithWeaponsSheathed", Settings::food._DontUseWithWeaponsSheathed, "// When weapons are sheathed food will not be used.");
	ini.SetValue("Food", "ProhibitedEffects", Settings::food._prohibitedEffects.string().c_str(), "// Items with the listed effects will not be distributed to NPCs.");

	ini.SetValue("Food", "FoodProbabilityAdjusters", Utility::WriteAdjusters(&Distribution::_probabilityAdjustersFood).c_str(), 
		"// Global probability adjusters for food effects");


	// player
	ini.SetBoolValue("Player", "EnablePlayerPotions", Settings::player._playerPotions, "// All activated restoration features are applied to the player, while \n"
																				"// they are in Combat.");
	ini.SetBoolValue("Player", "EnablePlayerPoisonUsage", Settings::player._playerPoisons, "// Player will automatically use poisons.");
	ini.SetBoolValue("Player", "EnablePlayerFortifyPotionUsage", Settings::player._playerFortifyPotions, "// Player will use fortify potions the way followers do.");
	ini.SetBoolValue("Player", "EnablePlayerFoodUsage", Settings::player._playerFood, "// Player will use food the way npcs do.");

	ini.SetBoolValue("Player", "UseFavoritedItemsOnly", Settings::player._UseFavoritedItemsOnly, "// Player will only use items that have been added to the favorites.");
	ini.SetBoolValue("Player", "DontUseFavoritedItems", Settings::player._DontUseFavoritedItems, "// Player will not use any items that have been added to the favorites.\n"
																						"// If both UseFavoritedItemsOnly and DontUseFavoritedItems are set to true\n"
																						"// UseFavoritedItemsOnly = false will be set automatically upon loading the game.");
	ini.SetBoolValue("Player", "DontEatRawFood", Settings::player._DontEatRawFood, "// If enabled, the player will not consume food with the Keyword VendorItemFoodRaw");
	ini.SetBoolValue("Player", "DontDrinkAlcohol", Settings::player._DontDrinkAlcohol, "// If enabled, the player will not consume anything regarded as alcohol");
	

	// usage
	ini.SetBoolValue("Usage", "DisableItemUsageWhileStaggered", Settings::usage._DisableItemUsageWhileStaggered, "// NPCs that are staggered, unconcious, ragdolling or in a kill-move aren't able to use any potions and poisons.");
	ini.SetBoolValue("Usage", "DisableItemUsageWhileFlying", Settings::usage._DisableItemUsageWhileFlying, "// NPCs that are in mid-air or flying aren't able to use any potions and poisons.");
	ini.SetBoolValue("Usage", "DisableItemUsageWhileBleedingOut", Settings::usage._DisableItemUsageWhileBleedingOut, "// NPCs that are bleeding-out aren't able to use any potions and poisons.");
	ini.SetBoolValue("Usage", "DisableItemUsageWhileSleeping", Settings::usage._DisableItemUsageWhileSleeping, "// NPCs that are sleeping aren't able to use any potions and poisons.");
	ini.SetBoolValue("Usage", "DisableNonFollowerNPCs", Settings::usage._DisableNonFollowerNPCs, "// NPCs that are not currently followers of the player won't use potions, etc.");
	ini.SetBoolValue("Usage", "DisableOutOfCombatProcessing", Settings::usage._DisableOutOfCombatProcessing, "// NPCs are only handled when they are fighting -> Old handling method \n"
																								"// until version 3.\n"
																								"// If disabled, NPCs will use Health potions outside of combat. \n"
																								"// For instance, if they run into traps.");
	ini.SetBoolValue("Usage", "DisableItemUsageForExcludedNPCs", Settings::usage._DisableItemUsageForExcludedNPCs, "// NPCs that are excluded from item distribution, will not use \n// any Potions, Fortifypotions, Poisons or Food from any other in-game source.");
	ini.SetLongValue("Usage", "GlobalItemCooldown", Settings::usage._globalCooldown, "// Cooldown in milliseconds for item usage (potions, poisons, food, etc.).\n"
																			"// 0 means that items will be used according to the \n"
																			"// CycleWaitTime defined in [System] (one potion and one poison per cycle)");


	// distribution
	ini.SetBoolValue("Distribution", "DistributePotions", Settings::distr._DistributePotions, "// NPCs are given potions.");
	ini.SetBoolValue("Distribution", "DistributePoisons", Settings::distr._DistributePoisons, "// NPCs are give poisons.");
	ini.SetBoolValue("Distribution", "DistributeFood", Settings::distr._DistributeFood, "// NPCs are given food items.");
	ini.SetBoolValue("Distribution", "DistributeFortifyPotions", Settings::distr._DistributeFortifyPotions, "// NPCs are given fortify potions.");
	ini.SetBoolValue("Distribution", "DistributeCustomItems", Settings::distr._DistributeCustomItems, "// NPCs are given custom items definable with rules. This does not \n"
																								"// affect custom potions, poisons, fortify potions and food. \n"
																								"// They are affected by the above settings.");

	ini.SetLongValue("Distribution", "LevelEasy", Settings::distr._LevelEasy, "// NPC lower or equal this level are considered weak.");
	ini.SetLongValue("Distribution", "LevelNormal", Settings::distr._LevelNormal, "// NPC lower or equal this level are considered normal in terms of strength.");
	ini.SetLongValue("Distribution", "LevelDifficult", Settings::distr._LevelDifficult, "// NPC lower or equal this level are considered difficult.");
	ini.SetLongValue("Distribution", "LevelInsane", Settings::distr._LevelInsane, "// NPC lower or equal this level are considered insane. \n"
																			"// Everything above this is always treated as a boss.");

	ini.SetBoolValue("Distribution", "GameDifficultyScaling", Settings::distr._GameDifficultyScaling, "// Disables NPC level scaling, but scales distribution according \n"
																								"// to game difficulty.");

	ini.SetLongValue("Distribution", "MaxMagnitudeWeak", Settings::distr._MaxMagnitudeWeak, "// Items with this or lower magnitude*duration are considered weak.");
	ini.SetLongValue("Distribution", "MaxMagnitudeStandard", Settings::distr._MaxMagnitudeStandard, "// Items with this or lower magnitude*duration are considered normal.");
	ini.SetLongValue("Distribution", "MaxMagnitudePotent", Settings::distr._MaxMagnitudePotent, "// Items with this or lower magnitude*duration are considered potent. \n"
																						"// Everything above this is considered Insane tier.");

	ini.SetDoubleValue("Distribution", "StyleScalingPrimary", Settings::distr._StyleScalingPrimary, "// Scaling for the weight of different alchemic effects for the \n"
																							"// distribution of potions, poison, fortify potions and food \n"
																							"// according to the primary combat type of an npc.");
	ini.SetDoubleValue("Distribution", "StyleScalingSecondary", Settings::distr._StyleScalingSecondary, "// Scaling for the weight of different alchemic effects for the \n"
																								"// distribution of potions, poison, fortify potions and food \n"
																								"// according to the secondary combat type of an npc.");
	ini.SetDoubleValue("Distribution", "ProbabilityScaling", Settings::distr._ProbabilityScaling,
		"// Modifies the chances for all items distributed to npcs.\n"
		"// This does not really affect the number of item (potions) distributed,\n"
		"// just the chances for the first 4 potions, poisons and fortify potions. \n"
		"// Even though an overall increase in items is possible, it is incredibly \n"
		"// unlikely due to the small base-probabilities for anything beyond 4 items.\n"
		"// Anything value around 2.0 might guarantee 3 or 4 potions and poisons for most npcs.");

	ini.SetBoolValue("Distribution", "DoNotDistributeMixedInvisPotions", Settings::distr._DoNotDistributeMixedInvisPotions,
		"// Potions with Invisibility as one of their effects may not be distributed\n"
		"// when they are chosen for one of their other effects.\n"
		"// Example: [Setting: false] An NPC will be given a health potion. A health potion with\n"
		"// the secondary effect Invisibility might be chosen and given to the player.\n"
		"// [Setting: true] An NPC will be given a health potion. A health potion with\n"
		"// with arbitrary secondary effects except Invisibility might be chosen.");

	// removal
	ini.SetBoolValue("Removal", "RemoveItemsOnDeath", Settings::removal._RemoveItemsOnDeath, "// Remove items from NPCs after they died.");
	ini.SetLongValue("Removal", "ChanceToRemoveItem", Settings::removal._ChanceToRemoveItem, "// Chance to remove items on death of NPC. (range: 0 to 100)");
	ini.SetLongValue("Removal", "MaxItemsLeftAfterRemoval", Settings::removal._MaxItemsLeft, "// Maximum number of items chances are rolled for during removal. \n"
																					"// Everything that goes above this value is always removed.");


	// whitelist mode
	ini.SetBoolValue("Whitelist Mode", "EnableWhitelistItems", Settings::whitelist.EnabledItems, "// Enables the whitelist mode for items. Items that shall be \n"
																						"// distributed must be explicitly whitelisted in the rules. \n"
																						"// This is the opposite to the standard (blacklist) behaviour.");
	ini.SetBoolValue("Whitelist Mode", "EnableWhitelistNPCs", Settings::whitelist.EnabledNPCs, "// Enables the whitelist mode for NPCs. NPCs that shall be \n"
																						"// given potions, etc. and shall use potions, etc. MUST be \n"
																						"// explicitly whitelisted in the rules. This is the opposite \n"
																						"// of the standard (blacklist) behaviour.");


	// fixes
	ini.SetBoolValue("Fixes", "ApplySkillBoostPerks", Settings::fixes._ApplySkillBoostPerks, "// Distributes the two Perks AlchemySkillBoosts and PerkSkillBoosts \n"
																					"// to npcs which are needed for fortify etc. potions to apply.");
	ini.SetBoolValue("Fixes", "ForceFixPotionSounds", Settings::fixes._ForceFixPotionSounds, "// Forcefully fixes all sounds used by consumables in the game \n"
																					"// without regard for other mods changes. If deactivate the changes \n"
																					"// of other mods that should have the same effect are respected.");


	// system
	ini.SetLongValue("System", "CycleWaitTime", Settings::system._cycletime, "// Time between two periods in milliseconds.\n"
																	"// Set to smaller values to increase reactivity. Set to larger \n"
																	"// values to decrease performance impact.");
	ini.SetLongValue("System", "MaxDuration", Settings::system._MaxDuration, "// Maximum duration restoration potions (health, magicka, stamina) are accounted for, idependent from their actual duration.");
	ini.SetLongValue("System", "MaxFortifyDuration", Settings::system._MaxFortifyDuration, "// Maximum duration fortification potions are accounted for, idependent from their actual duration.");
	//if (Settings::system._alternateNPCRegistration)
	//	ini.SetBoolValue("System", "AlternateNPCRegistration", Settings::system._alternateNPCRegistration, "Switches NPC registration from immediate, to registration on next cycle.");
	ini.SetBoolValue("System", "KillSwitch", Settings::system._killSwitch, "Switches actors handling to main thread");


	// compatibility
	ini.SetBoolValue("Compatibility", "DisableCreaturesWithoutRules", Settings::compatibility._DisableCreaturesWithoutRules, "// Disables item distribution and item usage for NPCs that do not\n"
																													"// not have any distribution rule and have the ActorTypeAnimal or\n"
																													"// ActorTypeCreature keywords. The Skill Boost Perks are also not\n"
																													"// distributed to these NPCs.\n"
																													"// \n"
																													"// If you experience problems with your game CTDing, try to enable this. \n"
																													"// Some CTDs are caused by the Skill Boost perks being added to certain creatures.\n"
																													"// If your game stops CTDing afterwards, the issue was caused by some creature.\n"
																													"// You can also enable this if you see lots of mod added animals using potions. \n"
																													"// \n"
																													"// Many NPCs and creatures are in normal NPC factions \n"
																													"// (for instance FalmerFaction), and will not be excluded by this setting.\n"
																													"// \n"
																													"// Be aware that this may also exclude NPCs that should not be \n"
																													"// excluded, due to many NPCs being assigned the creature keyword even \n"
																													"// though they should not have them.\n"
																													"// Passively disables custom item distribution for those npcs.");


	// compatibility animated poisons
	ini.SetBoolValue("Compatibility: Animated Poisons", "EnableAnimatedPoisons", Settings::compatibility.animatedPoisons._Enable, "// Enables the automatic usage of poison animations for npcs.");
	ini.SetBoolValue("Compatibility: Animated Poisons", "UseAnimatedPoisonsDosageSystem", Settings::compatibility.animatedPoisons._UsePoisonDosage,
		"// Uses the dosage system introduced by Animated Poisons, over \n"
																																			"// the dosage system of this mod.");

	// compatibility animated potions
	ini.SetBoolValue("Compatibility: Animated Potions", "EnableAnimatedPotions", Settings::compatibility.animatedPotions._Enable, "// Enables the automatic usage of potion animations for npcs.");

	
	// debug
	ini.SetBoolValue("Debug", "EnableLogging", Settings::debug.EnableLog, "// Enables logging output. Use with care as logs may get very large.");
	ini.SetBoolValue("Debug", "EnableLoadLogging", Settings::debug.EnableLoadLog, "// Enables logging output for plugin load, use if you want to \n"
																		 "// log rule issues");
	ini.SetLongValue("Debug", "LogLevel", Settings::debug.LogLevel, "// 1 - layer 0 log entries, 2 - layer 1 log entries, 3 - layer 3 log entries, \n"
														   "// 4 - layer 4 log entries. Affects which functions write log entries, \n"
														   "// as well as what is written by those functions. ");
	ini.SetBoolValue("Debug", "EnableProfiling", Settings::debug.EnableProfiling, "// Enables profiling output.");
	ini.SetLongValue("Debug", "ProfileLevel", Settings::debug.ProfileLevel, "// 1 - only highest level functions write their executions times to \n"
																	"// the profile log, 2 - lower level functions are written, 3 - lowest level \n"
																	"// functions are written. Be aware that not all functions are supported \n"
																	"// as Profiling costs execution time.");

	ini.SetBoolValue("Debug", "CheckActorWithoutRules", Settings::debug._CheckActorsWithoutRules, "// Checks all actors in the game on game start whether they are applied the \n"
																							"// default distribution rule.");
	ini.SetBoolValue("Debug", "CalculateCellRules", Settings::debug._CalculateCellRules, "// When entering a new cell in game, all distribution rules are calculatet once.\n"
																				"// The result of the evaluation is written to a csv file, for rule debugging");
	ini.SetBoolValue("Debug", "CalculateAllCellOnStartup", Settings::debug._Test, "// 10 seconds after loading a save game the function for \"CalculateCellRules\" \n"
																			"// is applied to all cells in the game");

	ini.SetBoolValue("Debug", "RemoveItemsBeforeDist", Settings::debug._CompatibilityRemoveItemsBeforeDist, "// Removes all distributables items from npcs before distributing new items.");
	ini.SetBoolValue("Debug", "RemoveItemsStartup", Settings::debug._CompatibilityRemoveItemsStartup, "// Removes all distributables items from npcs 5 seconds after loading a game");
	ini.SetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", Settings::debug._CompatibilityRemoveItemsStartup_OnlyExcluded, "// Removes only excluded items from npcs");

	ini.SaveFile(path);
}
