#include "Settings.h"
#include "Logging.h"
#include "Utility.h"
#include "Compatibility.h"

using Comp = Compatibility;

std::unordered_map<RE::FormID, RE::BGSSoundDescriptorForm*> soundmap;
RE::BGSSoundOutput* Copy_SOMMono01400_verb = nullptr;

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

	if (!Settings::Fixes::_ForceFixPotionSounds)
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

	

	const auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSSoundOutput>();
	const auto factorydescform = RE::IFormFactory::GetConcreteFormFactoryByType<RE::BGSSoundDescriptorForm>();
	LOGL1_4("{}[Settings] [FixConsumables] factory {}", Utility::GetHex((uint64_t)(intptr_t)factory));

	if (SOM_player1st && SOM_verb) {
		if (Copy_SOMMono01400_verb == nullptr) {
			RE::BGSSoundOutput* SOMMono01400_verb = SOM_verb->As<RE::BGSSoundOutput>();
			Copy_SOMMono01400_verb = factory->Create();
			Copy_SOMMono01400_verb->data.reverbSendPct = 100;
			Copy_SOMMono01400_verb->attenuation = RE::malloc<RE::BGSSoundOutput::DynamicAttenuationCharacteristics>();
			Copy_SOMMono01400_verb->attenuation->data.minDistance = 150;
			Copy_SOMMono01400_verb->attenuation->data.maxDistance = 1400;
			Copy_SOMMono01400_verb->attenuation->data.curve[0] = 100;
			Copy_SOMMono01400_verb->attenuation->data.curve[1] = 50;
			Copy_SOMMono01400_verb->attenuation->data.curve[2] = 20;
			Copy_SOMMono01400_verb->attenuation->data.curve[3] = 5;
			Copy_SOMMono01400_verb->attenuation->data.curve[4] = 0;
			Copy_SOMMono01400_verb->type = SOMMono01400_verb->type;
			Copy_SOMMono01400_verb->data.flags = SOMMono01400_verb->data.flags;
			Copy_SOMMono01400_verb->data.unk1 = SOMMono01400_verb->data.unk1;
			Copy_SOMMono01400_verb->data.unk2 = SOMMono01400_verb->data.unk2;
			Copy_SOMMono01400_verb->data.reverbSendPct = SOMMono01400_verb->data.reverbSendPct;

			if (SOMMono01400_verb->speakerOutputs != nullptr) {
				Copy_SOMMono01400_verb->speakerOutputs = RE::malloc<RE::BGSSoundOutput::SpeakerArrays>();
				Copy_SOMMono01400_verb->speakerOutputs->channels[0] = SOMMono01400_verb->speakerOutputs->channels[0];
				Copy_SOMMono01400_verb->speakerOutputs->channels[1] = SOMMono01400_verb->speakerOutputs->channels[1];
				Copy_SOMMono01400_verb->speakerOutputs->channels[2] = SOMMono01400_verb->speakerOutputs->channels[2];
			}
			LOGL1_4("{}[Settings] [FixConsumables] attenuation {} ", Utility::GetHex((uint64_t)(intptr_t)Copy_SOMMono01400_verb->attenuation));
		}
		RE::BGSSoundOutput* SOMMono01400Player1st = SOM_player1st->As<RE::BGSSoundOutput>();

		RE::BGSSoundDescriptor* sounddesc = nullptr;
		RE::BGSStandardSoundDef* soundOM = nullptr;
		RE::BGSStandardSoundDef* soundOMCopy = nullptr;

		// get all alchemyitems and then fix their consumption sounds
		auto datahandler = RE::TESDataHandler::GetSingleton();
		auto alchs = datahandler->GetFormArray<RE::AlchemyItem>();
		for (auto& alch : alchs) {
			if (alch && alch->GetFormID() != 0x7) {
				if (alch->data.consumptionSound) {
					if (alch->IsPoison() && exlsounds.contains(alch->data.consumptionSound->GetFormID())) {
						alch->data.consumptionSound = PoisonUse;
						LOGL1_4("{}[Settings] [FixConsumables] changed consumption sound for {} to ITMPotionUse", Utility::PrintForm(alch));
					} else if (alch->IsFood() && exlsounds.contains(alch->data.consumptionSound->GetFormID())) {
						alch->data.consumptionSound = FoodEat;
						LOGL1_4("{}[Settings] [FixConsumables] changed consumption sound for {} to ITMPotionUse", Utility::PrintForm(alch));
					} else if (exlsounds.contains(alch->data.consumptionSound->GetFormID())) {
						alch->data.consumptionSound = PotionUse;
						LOGL1_4("{}[Settings] [FixConsumables] changed consumption sound for {} to ITMPotionUse", Utility::PrintForm(alch));
					} else {
						// find copied sound
						auto itr = soundmap.find(alch->data.consumptionSound->GetFormID());
						if (itr != soundmap.end() && itr->second != nullptr) {
							// get sound from list and assign it
							alch->data.consumptionSound = itr->second;
						} else {
							// copy the unknown sound
							//RE::TESForm* copy = //alch->data.consumptionSound->CreateDuplicateForm(false, nullptr);
							RE::BGSSoundDescriptorForm* snd = factorydescform->Create();  //copy->As<RE::BGSSoundDescriptorForm>();
							LOGL1_4("{}[Settings] [FixConsumables] snddesc {}", Utility::GetHex((uint64_t)(intptr_t)snd->soundDescriptor));
							soundmap.insert_or_assign(alch->data.consumptionSound->GetFormID(), snd);
							LOGL1_4("{}[Settings] [FixConsumables] orig {} ", Utility::GetHex((uint64_t)(intptr_t)alch->data.consumptionSound));
							LOGL1_4("{}[Settings] [FixConsumables] snd {} ", Utility::GetHex((uint64_t)(intptr_t)snd));
							// get sound descriptor
							soundOM = (RE::BGSStandardSoundDef*)alch->data.consumptionSound->soundDescriptor;
							// allocate new sound descriptor
							soundOMCopy = RE::malloc<RE::BGSStandardSoundDef>();
							soundOMCopy->category = soundOM->category;
							soundOMCopy->alternateSoundFormID = soundOM->alternateSoundFormID;
							soundOMCopy->pad14 = soundOM->pad14;
							soundOMCopy->soundFiles.resize(soundOM->soundFiles.size());
							for (int i = 0; i < (int)soundOM->soundFiles.size(); i++)
								soundOMCopy->soundFiles[i] = soundOM->soundFiles[i];
							soundOMCopy->soundCharacteristics.frequencyShift = soundOMCopy->soundCharacteristics.frequencyShift;
							soundOMCopy->soundCharacteristics.frequencyVariance = soundOMCopy->soundCharacteristics.frequencyVariance;
							soundOMCopy->soundCharacteristics.priority = soundOMCopy->soundCharacteristics.priority;
							soundOMCopy->soundCharacteristics.dbVariance = soundOMCopy->soundCharacteristics.dbVariance;
							soundOMCopy->soundCharacteristics.staticAttenuation = soundOMCopy->soundCharacteristics.staticAttenuation;
							soundOMCopy->soundCharacteristics.pad0E = soundOMCopy->soundCharacteristics.pad0E;
							soundOMCopy->conditions = soundOM->conditions;
							soundOMCopy->lengthCharacteristics.looping = soundOM->lengthCharacteristics.looping;
							soundOMCopy->lengthCharacteristics.rumbleSendValue = soundOM->lengthCharacteristics.rumbleSendValue;
							soundOMCopy->lengthCharacteristics.unk0 = soundOM->lengthCharacteristics.unk0;
							soundOMCopy->lengthCharacteristics.unk2 = soundOM->lengthCharacteristics.unk2;
							soundOMCopy->unk54 = soundOM->unk54;

							LOGL1_4("{}[Settings] [FixConsumables] soundOM {} ", Utility::GetHex((uint64_t)(intptr_t)soundOM));
							LOGL1_4("{}[Settings] [FixConsumables] soundOMCopy {} ", Utility::GetHex((uint64_t)(intptr_t)soundOMCopy));

							soundOMCopy->outputModel = Copy_SOMMono01400_verb;
							snd->soundDescriptor = soundOMCopy;
							LOGL2_4("{}[Settings] [FixConsumables] forcefully set output model for sound {} used by item {}", Utility::PrintForm(alch->data.consumptionSound), Utility::PrintForm(alch));

							alch->data.consumptionSound = snd;
						}
					}
				}
			}
		}
	}
}

void Settings::ResetConsumables()
{
	soundmap.clear();
	Copy_SOMMono01400_verb = nullptr;
}

void Settings::Load()
{
	constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

	bool Ultimateoptions = false;

	CSimpleIniA ini;

	ini.SetUnicode();
	ini.LoadFile(path);

	// old settings order version 2.4.x
	{
		// Features
		Potions::_enableMagickaRestoration = ini.GetBoolValue("Features", "EnableMagickaRestoration", Potions::_enableMagickaRestoration);
		Potions::_enableStaminaRestoration = ini.GetBoolValue("Features", "EnableStaminaRestoration", Potions::_enableStaminaRestoration);
		Potions::_enableHealthRestoration = ini.GetBoolValue("Features", "EnableHealthRestoration", Potions::_enableHealthRestoration);
		Poisons::_enablePoisons = ini.GetBoolValue("Features", "EnablePoisonUsage", Poisons::_enablePoisons);
		FortifyPotions::_enableFortifyPotions = ini.GetBoolValue("Features", "EnableFortifyPotionUsage", FortifyPotions::_enableFortifyPotions);
		Food::_enableFood = ini.GetBoolValue("Features", "EnableFoodUsage", Food::_enableFood);

		Player::_playerPotions = ini.GetBoolValue("Features", "EnablePlayerRestoration", Player::_playerPotions);
		Player::_playerPoisons = ini.GetBoolValue("Features", "EnablePlayerPoisonUsage", Player::_playerPoisons);
		Player::_playerFortifyPotions = ini.GetBoolValue("Features", "EnablePlayerFortifyPotionUsage", Player::_playerFortifyPotions);
		Player::_playerFood = ini.GetBoolValue("Features", "EnablePlayerFoodUsage", Player::_playerFood);

		Distr::_DistributePotions = ini.GetBoolValue("Features", "DistributePotions", Distr::_DistributePotions);
		Distr::_DistributePoisons = ini.GetBoolValue("Features", "DistributePoisons", Distr::_DistributePoisons);
		Distr::_DistributeFood = ini.GetBoolValue("Features", "DistributeFood", Distr::_DistributeFood);
		Distr::_DistributeFortifyPotions = ini.GetBoolValue("Features", "DistributeFortifyPotions", Distr::_DistributeFortifyPotions);
		Distr::_DistributeCustomItems = ini.GetBoolValue("Features", "DistributeCustomItems", Distr::_DistributeCustomItems);

		Removal::_RemoveItemsOnDeath = ini.GetBoolValue("Features", "RemoveItemsOnDeath", Removal::_RemoveItemsOnDeath);

		Usage::_DisableItemUsageWhileStaggered = ini.GetBoolValue("Features", "DisableItemUsageWhileStaggered", Usage::_DisableItemUsageWhileStaggered);

		Usage::_DisableNonFollowerNPCs = ini.GetBoolValue("Features", "DisableNonFollowerNPCs", Usage::_DisableNonFollowerNPCs);
		Usage::_DisableOutOfCombatProcessing = ini.GetBoolValue("Features", "DisableOutOfCombatProcessing", Usage::_DisableOutOfCombatProcessing);

		// fixes
		Fixes::_ApplySkillBoostPerks = ini.GetBoolValue("Fixes", "ApplySkillBoostPerks", Fixes::_ApplySkillBoostPerks);
		Fixes::_ForceFixPotionSounds = ini.GetBoolValue("Fixes", "ForceFixPotionSounds", Fixes::_ForceFixPotionSounds);

		// compatibility
		Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation = ini.GetBoolValue("Compatibility", "UltimatePotionAnimation", Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation);
		Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify = ini.GetBoolValue("Compatibility", "UltimatePotionAnimationFortify", Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify);

		Compatibility::_CompatibilityMode = ini.GetBoolValue("Compatibility", "Compatibility", Compatibility::_CompatibilityMode);

		Whitelist::EnabledItems = ini.GetBoolValue("Compatibility", "WhitelistMode", Whitelist::EnabledItems);

		Compatibility::_DisableCreaturesWithoutRules = ini.GetBoolValue("Compatibility", "DisableCreaturesWithoutRules", Compatibility::_DisableCreaturesWithoutRules);

		// distribution
		Distr::_LevelEasy = ini.GetLongValue("Distribution", "LevelEasy", Distr::_LevelEasy);
		Distr::_LevelNormal = ini.GetLongValue("Distribution", "LevelNormal", Distr::_LevelNormal);
		Distr::_LevelDifficult = ini.GetLongValue("Distribution", "LevelDifficult", Distr::_LevelDifficult);
		Distr::_LevelInsane = ini.GetLongValue("Distribution", "LevelInsane", Distr::_LevelInsane);

		Distr::_GameDifficultyScaling = ini.GetBoolValue("Distribution", "GameDifficultyScaling", Distr::_GameDifficultyScaling);

		Distr::_MaxMagnitudeWeak = ini.GetLongValue("Distribution", "MaxMagnitudeWeak", Distr::_MaxMagnitudeWeak);
		Distr::_MaxMagnitudeStandard = ini.GetLongValue("Distribution", "MaxMagnitudeStandard", Distr::_MaxMagnitudeStandard);
		Distr::_MaxMagnitudePotent = ini.GetLongValue("Distribution", "MaxMagnitudePotent", Distr::_MaxMagnitudePotent);

		Distr::_StyleScalingPrimary = (float)ini.GetDoubleValue("Distribution", "StyleScalingPrimary", Distr::_StyleScalingPrimary);
		Distr::_StyleScalingSecondary = (float)ini.GetDoubleValue("Distribution", "StyleScalingSecondary", Distr::_StyleScalingSecondary);

		// Restoration Thresholds
		Potions::_healthThreshold = static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdPercent", Potions::_healthThreshold));
		Potions::_healthThreshold = static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdLowerPercent", Potions::_healthThreshold));
		if (Potions::_healthThreshold > 0.95f)
			Potions::_healthThreshold = 0.95f;
		Potions::_magickaThreshold = static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdPercent", Potions::_magickaThreshold));
		Potions::_magickaThreshold = static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdLowerPercent", Potions::_magickaThreshold));
		if (Potions::_magickaThreshold > 0.95f)
			Potions::_magickaThreshold = 0.95f;
		Potions::_staminaThreshold = static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdPercent", Potions::_staminaThreshold));
		Potions::_staminaThreshold = static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdLowerPercent", Potions::_staminaThreshold));
		if (Potions::_staminaThreshold > 0.95f)
			Potions::_staminaThreshold = 0.95f;
		Potions::_UsePotionChance = static_cast<int>(ini.GetLongValue("Restoration", "UsePotionChance", Potions::_UsePotionChance));

		// Poisonusage options
		Poisons::_EnemyLevelScalePlayerLevel = static_cast<float>(ini.GetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", Poisons::_EnemyLevelScalePlayerLevel));
		Poisons::_EnemyNumberThreshold = ini.GetLongValue("Poisons", "FightingNPCsNumberThreshold", Poisons::_EnemyNumberThreshold);
		Poisons::_UsePoisonChance = static_cast<int>(ini.GetLongValue("Poisons", "UsePoisonChance", Poisons::_UsePoisonChance));

		// fortify options
		FortifyPotions::_EnemyLevelScalePlayerLevelFortify = static_cast<float>(ini.GetDoubleValue("Fortify", "EnemyLevelScalePlayerLevelFortify", FortifyPotions::_EnemyLevelScalePlayerLevelFortify));
		FortifyPotions::_EnemyNumberThresholdFortify = ini.GetLongValue("Fortify", "FightingNPCsNumberThresholdFortify", FortifyPotions::_EnemyNumberThresholdFortify);
		FortifyPotions::_UseFortifyPotionChance = static_cast<int>(ini.GetLongValue("Fortify", "UseFortifyPotionChance", FortifyPotions::_UseFortifyPotionChance));

		// removal options
		Removal::_ChanceToRemoveItem = ini.GetLongValue("Removal", "ChanceToRemoveItem", Removal::_ChanceToRemoveItem);
		Removal::_MaxItemsLeft = ini.GetLongValue("Removal", "MaxItemsLeftAfterRemoval", Removal::_MaxItemsLeft);

		// general
		System::_cycletime = ini.GetLongValue("General", "CycleWaitTime", System::_cycletime);

		// Debugging
		Debug::EnableLog = ini.GetBoolValue("Debug", "EnableLogging", Debug::EnableLog);
		Logging::EnableLog = Debug::EnableLog;
		Debug::EnableLoadLog = ini.GetBoolValue("Debug", "EnableLoadLogging", Debug::EnableLoadLog);
		Logging::EnableLoadLog = Debug::EnableLoadLog;
		Debug::LogLevel = ini.GetLongValue("Debug", "LogLevel", Debug::LogLevel);
		Logging::LogLevel = Debug::LogLevel;
		Debug::EnableProfiling = ini.GetBoolValue("Debug", "EnableProfiling", Debug::EnableProfiling);
		Logging::EnableProfiling = Debug::EnableProfiling;
		Debug::ProfileLevel = ini.GetLongValue("Debug", "ProfileLevel", Debug::ProfileLevel);
		Logging::ProfileLevel = Debug::ProfileLevel;

		Debug::_CheckActorsWithoutRules = ini.GetBoolValue("Debug", "CheckActorWithoutRules", Debug::_CheckActorsWithoutRules);

		Debug::_CalculateCellRules = ini.GetBoolValue("Debug", "CalculateCellRules", Debug::_CalculateCellRules);
		Debug::_Test = ini.GetBoolValue("Debug", "CalculateAllCellOnStartup", Debug::_Test);
		if (Debug::_CalculateCellRules && Debug::_Test == false) {
			std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv", std::ofstream::out);
			out << "CellName;RuleApplied;PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
		}

		Debug::_CompatibilityRemoveItemsBeforeDist = ini.GetBoolValue("Debug", "RemoveItemsBeforeDist", Debug::_CompatibilityRemoveItemsBeforeDist);
		Debug::_CompatibilityRemoveItemsStartup = ini.GetBoolValue("Debug", "RemoveItemsStartup", Debug::_CompatibilityRemoveItemsStartup);
		Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded = ini.GetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded);

	}

	

	// new settings order version 3.x+
	{
		// potions
		Potions::_enableHealthRestoration = ini.GetBoolValue("Potions", "EnableHealthRestoration", Potions::_enableHealthRestoration);
		loginfo("[SETTINGS] {} {}", "Potions:           EnableHealthRestoration", std::to_string(Potions::_enableHealthRestoration));
		Potions::_enableMagickaRestoration = ini.GetBoolValue("Potions", "EnableMagickaRestoration", Potions::_enableMagickaRestoration);
		loginfo("[SETTINGS] {} {}", "Potions:           EnableMagickaRestoration", std::to_string(Potions::_enableMagickaRestoration));
		Potions::_enableStaminaRestoration = ini.GetBoolValue("Potions", "EnableStaminaRestoration", Potions::_enableStaminaRestoration);
		loginfo("[SETTINGS] {} {}", "Potions:           EnableStaminaRestoration", std::to_string(Potions::_enableStaminaRestoration));
		Potions::_AllowDetrimentalEffects = static_cast<int>(ini.GetLongValue("Potions", "AllowDetrimentalEffects", Potions::_AllowDetrimentalEffects));
		loginfo("[SETTINGS] {} {}", "Potions:           AllowDetrimentalEffects", std::to_string(Potions::_AllowDetrimentalEffects));
		Potions::_HandleWeaponSheathedAsOutOfCombat = static_cast<int>(ini.GetLongValue("Potions", "HandleWeaponSheathedAsOutOfCombat", Potions::_HandleWeaponSheathedAsOutOfCombat));
		loginfo("[SETTINGS] {} {}", "Potions:           HandleWeaponSheathedAsOutOfCombat", std::to_string(Potions::_HandleWeaponSheathedAsOutOfCombat));

		Potions::_healthThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "HealthThresholdPercent", Potions::_healthThreshold));
		Potions::_healthThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "HealthThresholdLowerPercent", Potions::_healthThreshold));
		if (Potions::_healthThreshold > 0.95f)
			Potions::_healthThreshold = 0.95f;
		loginfo("[SETTINGS] {} {}", "Potions:           HealthThresholdPercent", std::to_string(Potions::_healthThreshold));
		Potions::_magickaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "MagickaThresholdPercent", Potions::_magickaThreshold));
		Potions::_magickaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "MagickaThresholdLowerPercent", Potions::_magickaThreshold));
		if (Potions::_magickaThreshold > 0.95f)
			Potions::_magickaThreshold = 0.95f;
		loginfo("[SETTINGS] {} {}", "Potions:           MagickaThresholdPercent", std::to_string(Potions::_magickaThreshold));
		Potions::_staminaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "StaminaThresholdPercent", Potions::_staminaThreshold));
		Potions::_staminaThreshold = static_cast<float>(ini.GetDoubleValue("Potions", "StaminaThresholdLowerPercent", Potions::_staminaThreshold));
		if (Potions::_staminaThreshold > 0.95f)
			Potions::_staminaThreshold = 0.95f;
		loginfo("[SETTINGS] {} {}", "Potions:           StaminaThresholdPercent", std::to_string(Potions::_staminaThreshold));
		Potions::_UsePotionChance = static_cast<int>(ini.GetLongValue("Potions", "UsePotionChance", Potions::_UsePotionChance));
		loginfo("[SETTINGS] {} {}", "Potions:           UsePotionChance", std::to_string(Potions::_UsePotionChance));


		// poisons
		Poisons::_enablePoisons = ini.GetBoolValue("Poisons", "EnablePoisonUsage", Poisons::_enablePoisons);
		loginfo("[SETTINGS] {} {}", "Poisons:           EnablePoisonUsage", std::to_string(Poisons::_enablePoisons));
		Poisons::_AllowPositiveEffects = static_cast<int>(ini.GetLongValue("Poisons", "AllowPositiveEffects", Poisons::_AllowPositiveEffects));
		loginfo("[SETTINGS] {} {}", "Poisons:           AllowPositiveEffects", std::to_string(Poisons::_AllowPositiveEffects));
		Poisons::_DontUseWithWeaponsSheathed = static_cast<int>(ini.GetLongValue("Poisons", "DontUseWithWeaponsSheathed", Poisons::_DontUseWithWeaponsSheathed));
		loginfo("[SETTINGS] {} {}", "Poisons:           DontUseWithWeaponsSheathed", std::to_string(Poisons::_DontUseWithWeaponsSheathed));
		Poisons::_EnemyLevelScalePlayerLevel = static_cast<float>(ini.GetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", Poisons::_EnemyLevelScalePlayerLevel));
		loginfo("[SETTINGS] {} {}", "Poisons:           EnemyLevelScalePlayerLevel", std::to_string(Poisons::_EnemyLevelScalePlayerLevel));
		Poisons::_EnemyNumberThreshold = ini.GetLongValue("Poisons", "FightingNPCsNumberThreshold", Poisons::_EnemyNumberThreshold);
		loginfo("[SETTINGS] {} {}", "Poisons:           FightingNPCsNumberThreshold", std::to_string(Poisons::_EnemyNumberThreshold));
		Poisons::_UsePoisonChance = static_cast<int>(ini.GetLongValue("Poisons", "UsePoisonChance", Poisons::_UsePoisonChance));
		loginfo("[SETTINGS] {} {}", "Poisons:           UsePoisonChance", std::to_string(Poisons::_UsePoisonChance));
		Poisons::_Dosage = static_cast<int>(ini.GetLongValue("Poisons", "Dosage", Poisons::_Dosage));
		loginfo("[SETTINGS] {} {}", "Poisons:           Dosage", std::to_string(Poisons::_Dosage));
		

		// fortify potions
		FortifyPotions::_enableFortifyPotions = ini.GetBoolValue("FortifyPotions", "EnableFortifyPotionUsage", FortifyPotions::_enableFortifyPotions);
		loginfo("[SETTINGS] {} {}", "Fortify Potions:   EnableFortifyPotionUsage", std::to_string(FortifyPotions::_enableFortifyPotions));
		FortifyPotions::_DontUseWithWeaponsSheathed = ini.GetBoolValue("FortifyPotions", "DontUseWithWeaponsSheathed", FortifyPotions::_DontUseWithWeaponsSheathed);
		loginfo("[SETTINGS] {} {}", "Fortify Potions:   DontUseWithWeaponsSheathed", std::to_string(FortifyPotions::_DontUseWithWeaponsSheathed));
		FortifyPotions::_EnemyLevelScalePlayerLevelFortify = static_cast<float>(ini.GetDoubleValue("FortifyPotions", "EnemyLevelScalePlayerLevelFortify", FortifyPotions::_EnemyLevelScalePlayerLevelFortify));
		loginfo("[SETTINGS] {} {}", "Fortify Potions:   EnemyLevelScalePlayerLevelFortify", std::to_string(FortifyPotions::_EnemyLevelScalePlayerLevelFortify));
		FortifyPotions::_EnemyNumberThresholdFortify = ini.GetLongValue("FortifyPotions", "FightingNPCsNumberThresholdFortify", FortifyPotions::_EnemyNumberThresholdFortify);
		loginfo("[SETTINGS] {} {}", "Fortify Potions:   FightingNPCsNumberThresholdFortify", std::to_string(FortifyPotions::_EnemyNumberThresholdFortify));
		FortifyPotions::_UseFortifyPotionChance = static_cast<int>(ini.GetLongValue("FortifyPotions", "UseFortifyPotionChance", FortifyPotions::_UseFortifyPotionChance));
		loginfo("[SETTINGS] {} {}", "Fortify Potions:   UseFortifyPotionChance", std::to_string(FortifyPotions::_UseFortifyPotionChance));


		// food
		Food::_enableFood = ini.GetBoolValue("Food", "EnableFoodUsage", Food::_enableFood);
		loginfo("[SETTINGS] {} {}", "Food:              EnableFoodUsage", std::to_string(Food::_enableFood));
		Food::_AllowDetrimentalEffects = static_cast<int>(ini.GetLongValue("Food", "AllowDetrimentalEffects", Food::_AllowDetrimentalEffects));
		loginfo("[SETTINGS] {} {}", "Food:              AllowDetrimentalEffects", std::to_string(Food::_AllowDetrimentalEffects));
		Food::_RestrictFoodToCombatStart = ini.GetBoolValue("Food", "OnlyAllowFoodAtCombatStart", Food::_RestrictFoodToCombatStart);
		loginfo("[SETTINGS] {} {}", "Food:              OnlyAllowFoodAtCombatStart", std::to_string(Food::_RestrictFoodToCombatStart));
		Food::_DisableFollowers = ini.GetBoolValue("Food", "DisableFollowers", Food::_DisableFollowers);
		loginfo("[SETTINGS] {} {}", "Food:              DisableFollowers", std::to_string(Food::_DisableFollowers));
		Food::_DontUseWithWeaponsSheathed = ini.GetBoolValue("Food", "DontUseWithWeaponsSheathed", Food::_DontUseWithWeaponsSheathed);
		loginfo("[SETTINGS] {} {}", "Food:              DontUseWithWeaponsSheathed", std::to_string(Food::_DontUseWithWeaponsSheathed));

		// player
		Player::_playerPotions = ini.GetBoolValue("Player", "EnablePlayerPotions", Player::_playerPotions);
		loginfo("[SETTINGS] {} {}", "Player:            EnablePlayerPotions", std::to_string(Player::_playerPotions));
		Player::_playerPoisons = ini.GetBoolValue("Player", "EnablePlayerPoisonUsage", Player::_playerPoisons);
		loginfo("[SETTINGS] {} {}", "Player:            EnablePlayerPoisonUsage", std::to_string(Player::_playerPoisons));
		Player::_playerFortifyPotions = ini.GetBoolValue("Player", "EnablePlayerFortifyPotionUsage", Player::_playerFortifyPotions);
		loginfo("[SETTINGS] {} {}", "Player:            EnablePlayerFortifyPotionUsage", std::to_string(Player::_playerFortifyPotions));
		Player::_playerFood = ini.GetBoolValue("Player", "EnablePlayerFoodUsage", Player::_playerFood);
		loginfo("[SETTINGS] {} {}", "Player:            EnablePlayerFoodUsage", std::to_string(Player::_playerFood));

		Player::_UseFavoritedItemsOnly = ini.GetBoolValue("Player", "UseFavoritedItemsOnly", Player::_UseFavoritedItemsOnly);
		loginfo("[SETTINGS] {} {}", "Player:            UseFavoritedItemsOnly", std::to_string(Player::_UseFavoritedItemsOnly));
		Player::_DontUseFavoritedItems = ini.GetBoolValue("Player", "DontUseFavoritedItems", Player::_DontUseFavoritedItems);
		loginfo("[SETTINGS] {} {}", "Player:            DontUseFavoritedItems", std::to_string(Player::_DontUseFavoritedItems));
		if (Player::_UseFavoritedItemsOnly && Player::_DontUseFavoritedItems)
			Player::_UseFavoritedItemsOnly = false;
		Player::_DontEatRawFood = ini.GetBoolValue("Player", "DontEatRawFood", Player::_DontEatRawFood);
		loginfo("[SETTINGS] {} {}", "Player:            DontEatRawFood", std::to_string(Player::_DontEatRawFood));
		Player::_DontDrinkAlcohol = ini.GetBoolValue("Player", "DontDrinkAlcohol", Player::_DontDrinkAlcohol);
		loginfo("[SETTINGS] {} {}", "Player:            DontDrinkAlcohol", std::to_string(Player::_DontDrinkAlcohol));


		// usage
		Usage::_DisableItemUsageWhileStaggered = ini.GetBoolValue("Usage", "DisableItemUsageWhileStaggered", Usage::_DisableItemUsageWhileStaggered);
		loginfo("[SETTINGS] {} {}", "usage:             DisableItemUsageWhileStaggered", std::to_string(Usage::_DisableItemUsageWhileStaggered));
		Usage::_DisableNonFollowerNPCs = ini.GetBoolValue("Usage", "DisableNonFollowerNPCs", Usage::_DisableNonFollowerNPCs);
		loginfo("[SETTINGS] {} {}", "usage:             DisableNonFollowerNPCs", std::to_string(Usage::_DisableNonFollowerNPCs));
		Usage::_DisableOutOfCombatProcessing = ini.GetBoolValue("Usage", "DisableOutOfCombatProcessing", Usage::_DisableOutOfCombatProcessing);
		loginfo("[SETTINGS] {} {}", "usage:             DisableOutOfCombatProcessing", std::to_string(Usage::_DisableOutOfCombatProcessing));
		Usage::_DisableItemUsageForExcludedNPCs = ini.GetBoolValue("Usage", "DisableItemUsageForExcludedNPCs", Usage::_DisableItemUsageForExcludedNPCs);
		loginfo("[SETTINGS] {} {}", "usage:             DisableItemUsageForExcludedNPCs", std::to_string(Usage::_DisableItemUsageForExcludedNPCs));
		Usage::_globalCooldown = ini.GetLongValue("Usage", "GlobalItemCooldown", Usage::_globalCooldown);
		loginfo("[SETTINGS] {} {}", "usage:             GlobalItemCooldown", std::to_string(Usage::_globalCooldown));


		// distribution

		Distr::_DistributePotions = ini.GetBoolValue("Distribution", "DistributePotions", Distr::_DistributePotions);
		loginfo("[SETTINGS] {} {}", "Distribution:      DistributePotions", std::to_string(Distr::_DistributePotions));
		Distr::_DistributePoisons = ini.GetBoolValue("Distribution", "DistributePoisons", Distr::_DistributePoisons);
		loginfo("[SETTINGS] {} {}", "Distribution:      DistributePoisons", std::to_string(Distr::_DistributePoisons));
		Distr::_DistributeFood = ini.GetBoolValue("Distribution", "DistributeFood", Distr::_DistributeFood);
		loginfo("[SETTINGS] {} {}", "Distribution:      DistributeFood", std::to_string(Distr::_DistributeFood));
		Distr::_DistributeFortifyPotions = ini.GetBoolValue("Distribution", "DistributeFortifyPotions", Distr::_DistributeFortifyPotions);
		loginfo("[SETTINGS] {} {}", "Distribution:      DistributeFortifyPotions", std::to_string(Distr::_DistributeFortifyPotions));
		Distr::_DistributeCustomItems = ini.GetBoolValue("Distribution", "DistributeCustomItems", Distr::_DistributeCustomItems);
		loginfo("[SETTINGS] {} {}", "Distribution:      DistributeCustomItems", std::to_string(Distr::_DistributeCustomItems));

		Distr::_LevelEasy = ini.GetLongValue("Distribution", "LevelEasy", Distr::_LevelEasy);
		loginfo("[SETTINGS] {} {}", "Distribution:      LevelEasy", std::to_string(Distr::_LevelEasy));
		Distr::_LevelNormal = ini.GetLongValue("Distribution", "LevelNormal", Distr::_LevelNormal);
		loginfo("[SETTINGS] {} {}", "Distribution:      LevelNormal", std::to_string(Distr::_LevelNormal));
		Distr::_LevelDifficult = ini.GetLongValue("Distribution", "LevelDifficult", Distr::_LevelDifficult);
		loginfo("[SETTINGS] {} {}", "Distribution:      LevelDifficult", std::to_string(Distr::_LevelDifficult));
		Distr::_LevelInsane = ini.GetLongValue("Distribution", "LevelInsane", Distr::_LevelInsane);
		loginfo("[SETTINGS] {} {}", "Distribution:      LevelInsane", std::to_string(Distr::_LevelInsane));

		Distr::_GameDifficultyScaling = ini.GetBoolValue("Distribution", "GameDifficultyScaling", Distr::_GameDifficultyScaling);
		loginfo("[SETTINGS] {} {}", "Distribution:      GameDifficultyScaling", std::to_string(Distr::_GameDifficultyScaling));

		Distr::_MaxMagnitudeWeak = ini.GetLongValue("Distribution", "MaxMagnitudeWeak", Distr::_MaxMagnitudeWeak);
		loginfo("[SETTINGS] {} {}", "Distribution:      MaxMagnitudeWeak", std::to_string(Distr::_MaxMagnitudeWeak));
		Distr::_MaxMagnitudeStandard = ini.GetLongValue("Distribution", "MaxMagnitudeStandard", Distr::_MaxMagnitudeStandard);
		loginfo("[SETTINGS] {} {}", "Distribution:      MaxMagnitudeStandard", std::to_string(Distr::_MaxMagnitudeStandard));
		Distr::_MaxMagnitudePotent = ini.GetLongValue("Distribution", "MaxMagnitudePotent", Distr::_MaxMagnitudePotent);
		loginfo("[SETTINGS] {} {}", "Distribution:      MaxMagnitudePotent", std::to_string(Distr::_MaxMagnitudePotent));

		Distr::_StyleScalingPrimary = (float)ini.GetDoubleValue("Distribution", "StyleScalingPrimary", Distr::_StyleScalingPrimary);
		loginfo("[SETTINGS] {} {}", "Distribution:      StyleScalingPrimary", std::to_string(Distr::_StyleScalingPrimary));
		Distr::_StyleScalingSecondary = (float)ini.GetDoubleValue("Distribution", "StyleScalingSecondary", Distr::_StyleScalingSecondary);
		loginfo("[SETTINGS] {} {}", "Distribution:      StyleScalingSecondary", std::to_string(Distr::_StyleScalingSecondary));


		// removal
		Removal::_RemoveItemsOnDeath = ini.GetBoolValue("Removal", "RemoveItemsOnDeath", Removal::_RemoveItemsOnDeath);
		loginfo("[SETTINGS] {} {}", "Removal:           RemoveItemsOnDeath", std::to_string(Removal::_RemoveItemsOnDeath));
		Removal::_ChanceToRemoveItem = ini.GetLongValue("Removal", "ChanceToRemoveItem", Removal::_ChanceToRemoveItem);
		loginfo("[SETTINGS] {} {}", "Removal:           ChanceToRemoveItem", std::to_string(Removal::_ChanceToRemoveItem));
		Removal::_MaxItemsLeft = ini.GetLongValue("Removal", "MaxItemsLeftAfterRemoval", Removal::_MaxItemsLeft);
		loginfo("[SETTINGS] {} {}", "Removal:           MaxItemsLeftAfterRemoval", std::to_string(Removal::_MaxItemsLeft));


		// whitelist mode
		Whitelist::EnabledItems = ini.GetBoolValue("Whitelist Mode", "EnableWhitelistItems", Whitelist::EnabledItems);
		loginfo("[SETTINGS] {} {}", "Whitelist Mode:    EnableWhitelistItems", std::to_string(Whitelist::EnabledItems));
		Whitelist::EnabledNPCs = ini.GetBoolValue("Whitelist Mode", "EnableWhitelistNPCs", Whitelist::EnabledNPCs);
		loginfo("[SETTINGS] {} {}", "Whitelist Mode:    EnableWhitelistNPCs", std::to_string(Whitelist::EnabledNPCs));


		// fixes
		Fixes::_ApplySkillBoostPerks = ini.GetBoolValue("Fixes", "ApplySkillBoostPerks", Fixes::_ApplySkillBoostPerks);
		loginfo("[SETTINGS] {} {}", "Fixes:             ApplySkillBoostPerks", std::to_string(Fixes::_ApplySkillBoostPerks));
		Fixes::_ForceFixPotionSounds = ini.GetBoolValue("Fixes", "ForceFixPotionSounds", Fixes::_ForceFixPotionSounds);
		loginfo("[SETTINGS] {} {}", "Fixes:             ForceFixPotionSounds", std::to_string(Fixes::_ForceFixPotionSounds));


		// system
		System::_cycletime = ini.GetLongValue("System", "CycleWaitTime", System::_cycletime);
		loginfo("[SETTINGS] {} {}", "System:            CycleWaitTime", std::to_string(System::_cycletime));

		
		// compatibility
		Compatibility::_DisableCreaturesWithoutRules = ini.GetBoolValue("Compatibility", "DisableCreaturesWithoutRules", Compatibility::_DisableCreaturesWithoutRules);
		loginfo("[SETTINGS] {} {}", "Compatibility:     DisableCreaturesWithoutRules", std::to_string(Compatibility::_DisableCreaturesWithoutRules));
		Compatibility::_CompatibilityMode = ini.GetBoolValue("Compatibility", "Compatibility", Compatibility::_CompatibilityMode);
		loginfo("[SETTINGS] {} {}", "Compatibility:     Compatibility", std::to_string(Compatibility::_CompatibilityMode));

		// compatibility zxlice's Ultimate Potions Animation
		Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation = ini.GetBoolValue("Compatibility: zxliceUltimatePotions", "EnablePotionCompatibility", Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation);
		loginfo("[SETTINGS] {} {}", "Compatibility:     EnablePotionCompatibility", std::to_string(Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation));
		Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify = ini.GetBoolValue("Compatibility: zxliceUltimatePotions", "EnableFortifyPotionCompatibility", Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify);
		loginfo("[SETTINGS] {} {}", "Compatibility:     EnableFortifyPotionCompatibility", std::to_string(Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify));

		// compatibility animated poisons
		Compatibility::AnimatedPoisons::_Enable = ini.GetBoolValue("Compatibility: Animated Poisons", "EnableAnimatedPoisons", Compatibility::AnimatedPoisons::_Enable);
		loginfo("[SETTINGS] {} {}", "Compatibility:     EnableAnimatedPoisons", std::to_string(Compatibility::AnimatedPoisons::_Enable));
		Compatibility::AnimatedPoisons::_UsePoisonDosage = ini.GetBoolValue("Compatibility: Animated Poisons", "UseAnimatedPoisonsDosageSystem", Compatibility::AnimatedPoisons::_UsePoisonDosage);
		loginfo("[SETTINGS] {} {}", "Compatibility:     UseAnimatedPoisonsDosageSystem", std::to_string(Compatibility::AnimatedPoisons::_UsePoisonDosage));

		// compatibility animated potions
		Compatibility::AnimatedPotions::_Enable = ini.GetBoolValue("Compatibility: Animated Potions", "EnableAnimatedPotions", Compatibility::AnimatedPotions::_Enable);
		loginfo("[SETTINGS] {} {}", "Compatibility:     EnableAnimatedPotions", std::to_string(Compatibility::AnimatedPotions::_Enable));


		// debug
		Debug::EnableLog = ini.GetBoolValue("Debug", "EnableLogging", Debug::EnableLog);
		Logging::EnableLog = Debug::EnableLog;
		loginfo("[SETTINGS] {} {}", "Debug:             EnableLogging", std::to_string(Debug::EnableLog));
		Debug::EnableLoadLog = ini.GetBoolValue("Debug", "EnableLoadLogging", Debug::EnableLoadLog);
		Logging::EnableLoadLog = Debug::EnableLoadLog;
		loginfo("[SETTINGS] {} {}", "Debug:             EnableLoadLogging", std::to_string(Debug::EnableLoadLog));
		Debug::LogLevel = ini.GetLongValue("Debug", "LogLevel", Debug::LogLevel);
		Logging::LogLevel = Debug::LogLevel;
		loginfo("[SETTINGS] {} {}", "Debug:             LogLevel", std::to_string(Debug::LogLevel));
		Debug::EnableProfiling = ini.GetBoolValue("Debug", "EnableProfiling", Debug::EnableProfiling);
		Logging::EnableProfiling = Debug::EnableProfiling;
		loginfo("[SETTINGS] {} {}", "Debug:             EnableProfiling", std::to_string(Debug::EnableProfiling));
		Debug::ProfileLevel = ini.GetLongValue("Debug", "ProfileLevel", Debug::ProfileLevel);
		Logging::ProfileLevel = Debug::ProfileLevel;
		loginfo("[SETTINGS] {} {}", "Debug:             ProfileLevel", std::to_string(Debug::LogLevel));

		Debug::_CheckActorsWithoutRules = ini.GetBoolValue("Debug", "CheckActorWithoutRules", Debug::_CheckActorsWithoutRules);
		loginfo("[SETTINGS] {} {}", "Debug:             CheckActorWithoutRules", std::to_string(Debug::_CheckActorsWithoutRules));

		Debug::_CalculateCellRules = ini.GetBoolValue("Debug", "CalculateCellRules", Debug::_CalculateCellRules);
		loginfo("[SETTINGS] {} {}", "Debug:             CalculateCellRules", std::to_string(Debug::_CalculateCellRules));
		Debug::_Test = ini.GetBoolValue("Debug", "CalculateAllCellOnStartup", Debug::_Test);
		loginfo("[SETTINGS] {} {}", "Debug:             CalculateAllCellOnStartup", std::to_string(Debug::_Test));
		if (Debug::_CalculateCellRules && Debug::_Test == false) {
			std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv", std::ofstream::out);
			out << "CellName;RuleApplied;PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
		}

		Debug::_CompatibilityRemoveItemsBeforeDist = ini.GetBoolValue("Debug", "RemoveItemsBeforeDist", Debug::_CompatibilityRemoveItemsBeforeDist);
		loginfo("[SETTINGS] {} {}", "Debug:             RemoveItemsBeforeDist", std::to_string(Debug::_CompatibilityRemoveItemsBeforeDist));
		Debug::_CompatibilityRemoveItemsStartup = ini.GetBoolValue("Debug", "RemoveItemsStartup", Debug::_CompatibilityRemoveItemsStartup);
		loginfo("[SETTINGS] {} {}", "Debug:             RemoveItemsStartup", std::to_string(Debug::_CompatibilityRemoveItemsStartup));
		Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded = ini.GetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded);
		loginfo("[SETTINGS] {} {}", "Debug:             RemoveItemsStartup_OnlyExcluded", std::to_string(Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded));

	}





	// save user settings, before applying adjustments
	Save();

	// apply settings for ultimate potions
	if (Ultimateoptions) {
		Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation = true;
		loginfo("[SETTINGS] [OVERRIDE] Compatibility - {} hase been overwritten and set to true", "UltimatePotionAnimation");
	}
	loginfo("[SETTINGS] checking for plugins");

	// search for PotionAnimatedFx.esp for compatibility
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Comp::PotionAnimatedfx); index != 0x1) {
		// only activate compatibility mode for potion animated fx, if compatibility plugin has been installed.
		if (const uint32_t idx = Utility::Mods::GetPluginIndex(PluginName); idx != 0x1) {
			Compatibility::PotionAnimatedFx::_CompatibilityPotionAnimatedFx = true;
			loginfo("[SETTINGS] Found plugin PotionAnimatedfx.esp and activated compatibility mode");
		} else {
			loginfo("[SETTINGS] Compatibility mode has not been activated. Compatibility for Potion Animated Fx has not been activated");
		}
	} else {
		// if we cannot find the plugin then we need to disable all related compatibility options, otherwise we WILL get CTDs
	}

	// search for AnimatedPoisons.esp
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Comp::AnimatedPoisons); index != 0x1) {
		Compatibility::AnimatedPoisons::_CompatibilityAnimatedPoisons = true;
		loginfo("[SETTINGS] Found plugin AnimatedPoisons.esp and activated compatibility mode");
	}

	// search for AnimatedPotions.esp
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Comp::AnimatedPotions_4_4); index != 0x1) {
		Compatibility::AnimatedPotions::_CompatibilityAnimatedPotions = true;
		Comp::GetSingleton()->AnPoti_Version = 44;
		loginfo("[SETTINGS] Found plugin AnimatedPotions.esp and activated compatibility mode");
	}
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Comp::AnimatedPotions_4_3); index != 0x1) {
		Compatibility::AnimatedPotions::_CompatibilityAnimatedPotions = true;
		Comp::GetSingleton()->AnPoti_Version = 43;
		loginfo("[SETTINGS] Found plugin AnimatedPotions.esp and activated compatibility mode");
	}

	// plugin check
	if (Compatibility::_CompatibilityMode) {
		if (const uint32_t index = Utility::Mods::GetPluginIndex(PluginName); index != 0x1) {
			loginfo("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
		} else {
			loginfo("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, all use of potions, poisons and food is effectively disabled, except you have another sink for the papyrus events. Distribution is not affected");
		}
	} else if (Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation) {
		if (const uint32_t index = Utility::Mods::GetPluginIndex(PluginName); index != 0x1) {
			loginfo("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
		} else {
			loginfo("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, Potion drinking will be effectively disabled, except you have another plugin that listens to the Papyrus Mod Events. Other functionality is not affected");
		}
	}
	// Check for CACO
	{
		if (const uint32_t index = Utility::Mods::GetPluginIndex(Comp::CACO); index != 0x1) {
			loginfo("[SETTINGS] Complete Alchemy & Cooking Overhaul.esp is loaded, activating compatibility mode!");
			Compatibility::CACO::_CompatibilityCACO = true;
		}
	}
	// Check for Apothecary
	{
		if (const uint32_t index = Utility::Mods::GetPluginIndex(Comp::Apothecary); index != 0x1) {
			loginfo("[SETTINGS] Apothecary.esp is loaded, activating compatibility mode!");
			Compatibility::Apothecary::_CompatibilityApothecary = true;
		}
	}
	loginfo("[SETTINGS] checking for plugins end");

	//FixConsumables();
}


void Settings::Save()
{
	constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

	CSimpleIniA ini;

	ini.SetUnicode();

	// potions
	ini.SetBoolValue("Potions", "EnableHealthRestoration", Potions::_enableHealthRestoration, "// NPCs use health potions to restore their missing hp in combat.");
	ini.SetBoolValue("Potions", "EnableMagickaRestoration", Potions::_enableMagickaRestoration, "// NPCs use magicka potions to restore their missing magicka in combat.");
	ini.SetBoolValue("Potions", "EnableStaminaRestoration", Potions::_enableStaminaRestoration, "// NPCs use stamina potions to restore their missing stamina in combat.");
	ini.SetBoolValue("Potions", "AllowDetrimentalEffects", Potions::_AllowDetrimentalEffects, "// If this is enabled NPCs will use potions that contain detrimental\n"
																								"// effects. For instance, impure potions, that restore health and damage magicka.\n"
																								"// !!!This setting also affects fortify potions");
	ini.SetBoolValue("Potions", "HandleWeaponSheathedAsOutOfCombat", Potions::_HandleWeaponSheathedAsOutOfCombat, "// If weapons are not drawn in combat, it will be treated as Out-of-Combat.\n"
																													"// This currently means only health potions will be used.");

	ini.SetDoubleValue("Potions", "HealthThresholdPercent", Potions::_healthThreshold, "// Upon reaching this threshold, NPCs will start to use health potions");
	ini.SetDoubleValue("Potions", "MagickaThresholdPercent", Potions::_magickaThreshold, "// Upon reaching this threshold, NPCs will start to use magicka potions");
	ini.SetDoubleValue("Potions", "StaminaThresholdPercent", Potions::_staminaThreshold, "// Upon reaching this threshold, NPCs will start to use stamina potions");
	ini.SetLongValue("Potions", "UsePotionChance", Potions::_UsePotionChance, "// Chance that an NPC will use a potion if they can. Set to 100 to always \n"
																				"// take a potion, when appropiate.\n"
																				"// An NPC can use a potion if they (1) have a potion in their inventory, \n"
																				"// and (2) when the respective value falls below the threshold.");


	// poisons
	ini.SetBoolValue("Poisons", "EnablePoisonUsage", Poisons::_enablePoisons, "// NPCs use appropiate poisons in combat. Poisons are considered appropiate,\n"
																				"// if they can harm the enemy. For instance, damaging Magicka of an enemy \n"
																				"// that does not use spells, is not appropiate.");
	ini.SetBoolValue("Poisons", "AllowPositiveEffects", Poisons::_AllowPositiveEffects, "// This allows NPCs to use poisons that apply positive effects to their opponents");
	ini.SetBoolValue("Poisons", "DontUseWithWeaponsSheathed", Poisons::_DontUseWithWeaponsSheathed, "// If the weapons are sheathed, poisons will not be used.");
	ini.SetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", Poisons::_EnemyLevelScalePlayerLevel, "// If the enemy they are facing has a level greater equal \n"
																										"// 'this value' * PlayerLevel followers use poisons.");
	ini.SetLongValue("Poisons", "FightingNPCsNumberThreshold", Poisons::_EnemyNumberThreshold, "// When the number of NPCs in a fight is at least at this value, followers\n"
																								"// start to use poisons regardless of the enemies level, to faster help out \n"
																								"// the player.\n"
																								"// This includes hostile and non-hostile NPCs.");
	ini.SetLongValue("Poisons", "UsePoisonChance", Poisons::_UsePoisonChance, "// Chance that an NPC will use a poison if they possess one with an \n// appropiate effect.");
	ini.SetLongValue("Poisons", "Dosage", Poisons::_Dosage, "// The dosage describes the number of hits a poison lasts on your weapons.\n"
															"// The dosage for specific poisons, or alchemic effects can also be defined \n"
															"// by distribution rules and may outrule this setting.");


	// fortify potions
	ini.SetBoolValue("FortifyPotions", "EnableFortifyPotionUsage", FortifyPotions::_enableFortifyPotions, "// NPCs use fortify potions in combat.\n"
																											"// Potions are used based on the equipped weapons and spells.");
	ini.SetBoolValue("FortifyPotions", "DontUseWithWeaponsSheathed", FortifyPotions::_DontUseWithWeaponsSheathed, "// When weapons are sheathed, no fortify potions will be used.");

	ini.SetDoubleValue("FortifyPotions", "EnemyLevelScalePlayerLevelFortify", FortifyPotions::_EnemyLevelScalePlayerLevelFortify, "// If the enemy they are facing has a level greater equal \n"
																																	"// 'this value' * PlayerLevel followers use fortify potions.");
	ini.SetLongValue("FortifyPotions", "FightingNPCsNumberThresholdFortify", FortifyPotions::_EnemyNumberThresholdFortify, "// When the number of NPCs in a fight is at least at this value, followers \n"
																															"// start to use fortify potions regardless of the enemies level.\n"
																															"// This includes hostile and non-hostile NPCs.");
	ini.SetLongValue("FortifyPotions", "UseFortifyPotionChance", FortifyPotions::_UseFortifyPotionChance, "// Chance that an NPC will use a fortify potion if they can. They can use a \n"
																											"// potion, if they have one in their inventory, and its effects are \n"
																											"// beneficial to their current choice of weapons.\n"
																											"// An NPC will not use a Fortify Light Armor potion if they are wearing \n"
																											"// Heavy Armor, for instance.\n");


	// food
	ini.SetBoolValue("Food", "EnableFoodUsage", Food::_enableFood, "// Allows NPCs to use food items, to gain beneficial effects.");
	ini.SetBoolValue("Food", "AllowDetrimentalEffects", Food::_AllowDetrimentalEffects, "// This allows NPCs to use food that has detrimental effects.");
	ini.SetBoolValue("Food", "OnlyAllowFoodAtCombatStart", Food::_RestrictFoodToCombatStart, "// NPCs will only eat food at the beginning of combat. This is the way \n"
																							"// it worked until version 3.0.0\n"
																							"// If disabled, NPCs will try to eat more food as soon as an existing \n"
																							"// food buff runs out.");
	ini.SetBoolValue("Food", "DisableFollowers", Food::_DisableFollowers, "// Disables food usage for followers only. You can use this to prevent your followers\n"
																			"// from eating food, if you are using survival mods, without impacting other NPCs.");
	ini.SetBoolValue("Food", "DontUseWithWeaponsSheathed", Food::_DontUseWithWeaponsSheathed, "// When weapons are sheathed food will not be used.");


	// player
	ini.SetBoolValue("Player", "EnablePlayerPotions", Player::_playerPotions, "// All activated restoration features are applied to the player, while \n"
																				"// they are in Combat.");
	ini.SetBoolValue("Player", "EnablePlayerPoisonUsage", Player::_playerPoisons, "// Player will automatically use poisons.");
	ini.SetBoolValue("Player", "EnablePlayerFortifyPotionUsage", Player::_playerFortifyPotions, "// Player will use fortify potions the way followers do.");
	ini.SetBoolValue("Player", "EnablePlayerFoodUsage", Player::_playerFood, "// Player will use food the way npcs do.");

	ini.SetBoolValue("Player", "UseFavoritedItemsOnly", Player::_UseFavoritedItemsOnly, "// Player will only use items that have been added to the favorites.");
	ini.SetBoolValue("Player", "DontUseFavoritedItems", Player::_DontUseFavoritedItems, "// Player will not use any items that have been added to the favorites.\n"
																						"// If both UseFavoritedItemsOnly and DontUseFavoritedItems are set to true\n"
																						"// UseFavoritedItemsOnly = false will be set automatically upon loading the game.");
	ini.SetBoolValue("Player", "DontEatRawFood", Player::_DontEatRawFood, "// If enabled, the player will not consume food with the Keyword VendorItemFoodRaw");
	ini.SetBoolValue("Player", "DontDrinkAlcohol", Player::_DontDrinkAlcohol, "// If enabled, the player will not consume anything regarded as alcohol");
	

	// usage
	ini.SetBoolValue("Usage", "DisableItemUsageWhileStaggered", Usage::_DisableItemUsageWhileStaggered, "// NPCs that are staggered aren't able to use any potions and poisons.");
	ini.SetBoolValue("Usage", "DisableNonFollowerNPCs", Usage::_DisableNonFollowerNPCs, "// NPCs that are not currently followers of the player won't use potions, etc.");
	ini.SetBoolValue("Usage", "DisableOutOfCombatUsage", Usage::_DisableOutOfCombatProcessing, "// NPCs are only handled when they are fighting -> Old handling method \n"
																								"// until version 3.\n"
																								"// If disabled, NPCs will use Health potions outside of combat. \n"
																								"// For instance, if they run into traps.");
	ini.SetBoolValue("Usage", "DisableItemUsageForExcludedNPCs", Usage::_DisableItemUsageForExcludedNPCs, "// NPCs that are excluded from item distribution, will not use \n// any Potions, Fortifypotions, Poisons or Food from any other in-game source.");
	ini.SetLongValue("Usage", "GlobalItemCooldown", Usage::_globalCooldown, "// Cooldown in milliseconds for item usage (potions, poisons, food, etc.).\n"
																			"// 0 means that items will be used according to the \n"
																			"// CycleWaitTime defined in [System] (one potion and one poison per cycle)");


	// distribution
	ini.SetBoolValue("Distribution", "DistributePotions", Distr::_DistributePotions, "// NPCs are given potions.");
	ini.SetBoolValue("Distribution", "DistributePoisons", Distr::_DistributePoisons, "// NPCs are give poisons.");
	ini.SetBoolValue("Distribution", "DistributeFood", Distr::_DistributeFood, "// NPCs are given food items.");
	ini.SetBoolValue("Distribution", "DistributeFortifyPotions", Distr::_DistributeFortifyPotions, "// NPCs are given fortify potions.");
	ini.SetBoolValue("Distribution", "DistributeCustomItems", Distr::_DistributeCustomItems, "// NPCs are given custom items definable with rules. This does not \n"
																								"// affect custom potions, poisons, fortify potions and food. \n"
																								"// They are affected by the above settings.");

	ini.SetLongValue("Distribution", "LevelEasy", Distr::_LevelEasy, "// NPC lower or equal this level are considered weak.");
	ini.SetLongValue("Distribution", "LevelNormal", Distr::_LevelNormal, "// NPC lower or equal this level are considered normal in terms of strength.");
	ini.SetLongValue("Distribution", "LevelDifficult", Distr::_LevelDifficult, "// NPC lower or equal this level are considered difficult.");
	ini.SetLongValue("Distribution", "LevelInsane", Distr::_LevelInsane, "// NPC lower or equal this level are considered insane. \n"
																			"// Everything above this is always treated as a boss.");

	ini.SetBoolValue("Distribution", "GameDifficultyScaling", Distr::_GameDifficultyScaling, "// Disables NPC level scaling, but scales distribution according \n"
																								"// to game difficulty.");

	ini.SetLongValue("Distribution", "MaxMagnitudeWeak", Distr::_MaxMagnitudeWeak, "// Items with this or lower magnitude*duration are considered weak.");
	ini.SetLongValue("Distribution", "MaxMagnitudeStandard", Distr::_MaxMagnitudeStandard, "// Items with this or lower magnitude*duration are considered normal.");
	ini.SetLongValue("Distribution", "MaxMagnitudePotent", Distr::_MaxMagnitudePotent, "// Items with this or lower magnitude*duration are considered potent. \n"
																						"// Everything above this is considered Insane tier");

	ini.SetDoubleValue("Distribution", "StyleScalingPrimary", Distr::_StyleScalingPrimary, "// Scaling for the weight of different alchemic effects for the \n"
																							"// distribution of potions, poison, fortify potions and food \n"
																							"// according to the primary combat type of an npc.");
	ini.SetDoubleValue("Distribution", "StyleScalingSecondary", Distr::_StyleScalingSecondary, "// Scaling for the weight of different alchemic effects for the \n"
																								"// distribution of potions, poison, fortify potions and food \n"
																								"// according to the secondary combat type of an npc.");


	// removal
	ini.SetBoolValue("Removal", "RemoveItemsOnDeath", Removal::_RemoveItemsOnDeath, "// Remove items from NPCs after they died.");
	ini.SetLongValue("Removal", "ChanceToRemoveItem", Removal::_ChanceToRemoveItem, "// Chance to remove items on death of NPC. (range: 0 to 100)");
	ini.SetLongValue("Removal", "MaxItemsLeftAfterRemoval", Removal::_MaxItemsLeft, "// Maximum number of items chances are rolled for during removal. \n"
																					"// Everything that goes above this value is always removed.");


	// whitelist mode
	ini.SetBoolValue("Whitelist Mode", "EnableWhitelistItems", Whitelist::EnabledItems, "// Enables the whitelist mode for items. Items that shall be \n"
																						"// distributed must be explicitly whitelisted in the rules. \n"
																						"// This is the opposite to the standard (blacklist) behaviour.");
	ini.SetBoolValue("Whitelist Mode", "EnableWhitelistNPCs", Whitelist::EnabledNPCs, "// Enables the whitelist mode for NPCs. NPCs that shall be \n"
																						"// given potions, etc. and shall use potions, etc. MUST be \n"
																						"// explicitly whitelisted in the rules. This is the opposite \n"
																						"// of the standard (blacklist) behaviour");


	// fixes
	ini.SetBoolValue("Fixes", "ApplySkillBoostPerks", Fixes::_ApplySkillBoostPerks, "// Distributes the two Perks AlchemySkillBoosts and PerkSkillBoosts \n"
																					"// to npcs which are needed for fortify etc. potions to apply.");
	ini.SetBoolValue("Fixes", "ForceFixPotionSounds", Fixes::_ForceFixPotionSounds, "// Forcefully fixes all sounds used by consumables in the game \n"
																					"// without regard for other mods changes. If deactivate the changes \n"
																					"// of other mods that should have the same effect are respected.");


	// system
	ini.SetLongValue("System", "CycleWaitTime", System::_cycletime, "// Time between two periods in milliseconds.\n"
																	"// Set to smaller values to increase reactivity. Set to larger \n"
																	"// values to decrease performance impact.");


	// compatibility
	ini.SetBoolValue("Compatibility", "DisableCreaturesWithoutRules", Compatibility::_DisableCreaturesWithoutRules, "// Disables item distribution and item usage for NPCs that do not\n"
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
																													"// Passiveley disables custom item distribution for those npcs.");
	ini.SetBoolValue("Compatibility", "Compatibility", Compatibility::_CompatibilityMode, "// General Compatibility Mode. If set to true, all items will be\n"
																						  "// equipped using Papyrus workaround. Requires the Skyrim esp plugin.");

	// compatibility zxlices ultimate potion animation
	ini.SetBoolValue("Compatibility: zxliceUltimatePotions", "EnablePotionCompatibility", Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation, "// Compatibility mode for \"zxlice's ultimate potion animation\". \n"
																																								 "// Requires the Skyrim esp plugin. Only uses compatibility mode \n"
																																								 "// for Health, Stamina and Magicka Potions.");
	ini.SetBoolValue("Compatibility: zxliceUltimatePotions", "EnableFortifyPotionCompatibility", Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify, "// Compatibility mode for \"zxlice's ultimate potion animation\". \n"
																																											   "// Requires the Skyrim esp plugin. Uses compatibility mode for \n"
																																											   "// Fortify Potions.");

	// compatibility animated poisons
	ini.SetBoolValue("Compatibility: Animated Poisons", "EnableAnimatedPoisons", Compatibility::AnimatedPoisons::_Enable, "// Enables the automatic usage of poison animations for npcs.");
	ini.SetBoolValue("Compatibility: Animated Poisons", "UseAnimatedPoisonsDosageSystem", Compatibility::AnimatedPoisons::_UsePoisonDosage, "// Uses the dosage system introduced by Animated Poisons, over \n"
																																			"// the dosage system of this mod.");

	// compatibility animated potions
	ini.SetBoolValue("Compatibility: Animated Potions", "EnableAnimatedPotions", Compatibility::AnimatedPotions::_Enable, "// Enables the automatic usage of potion animations for npcs.");

	
	// debug
	ini.SetBoolValue("Debug", "EnableLogging", Debug::EnableLog, "// Enables logging output. Use with care as logs may get very large.");
	ini.SetBoolValue("Debug", "EnableLoadLogging", Debug::EnableLoadLog, "// Enables logging output for plugin load, use if you want to \n"
																		 "// log rule issues");
	ini.SetLongValue("Debug", "LogLevel", Debug::LogLevel, "// 1 - layer 0 log entries, 2 - layer 1 log entries, 3 - layer 3 log entries, \n"
														   "// 4 - layer 4 log entries. Affects which functions write log entries, \n"
														   "// as well as what is written by those functions. ");
	ini.SetBoolValue("Debug", "EnableProfiling", Debug::EnableProfiling, "// Enables profiling output.");
	ini.SetLongValue("Debug", "ProfileLevel", Debug::ProfileLevel, "// 1 - only highest level functions write their executions times to \n"
																	"// the profile log, 2 - lower level functions are written, 3 - lowest level \n"
																	"// functions are written. Be aware that not all functions are supported \n"
																	"// as Profiling costs execution time.");

	ini.SetBoolValue("Debug", "CheckActorWithoutRules", Debug::_CheckActorsWithoutRules, "// Checks all actors in the game on game start whether they are applied the \n"
																							"// default distribution rule.");
	ini.SetBoolValue("Debug", "CalculateCellRules", Debug::_CalculateCellRules, "// When entering a new cell in game, all distribution rules are calculatet once.\n"
																				"// The result of the evaluation is written to a csv file, for rule debugging");
	ini.SetBoolValue("Debug", "CalculateAllCellOnStartup", Debug::_Test, "// 10 seconds after loading a save game the function for \"CalculateCellRules\" \n"
																			"// is applied to all cells in the game");

	ini.SetBoolValue("Debug", "RemoveItemsBeforeDist", Debug::_CompatibilityRemoveItemsBeforeDist, "// Removes all distributables items from npcs before distributing new items.");
	ini.SetBoolValue("Debug", "RemoveItemsStartup", Debug::_CompatibilityRemoveItemsStartup, "// Removes all distributables items from npcs 5 seconds after loading a game");
	ini.SetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", Debug::_CompatibilityRemoveItemsStartup_OnlyExcluded, "// Removes only excluded items from npcs");

	ini.SaveFile(path);
}
