#include <Settings.h>


void Settings::Load()
{
	constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

	bool Ultimateoptions = false;

	CSimpleIniA ini;

	ini.SetUnicode();
	ini.LoadFile(path);

	// Features
	_featMagickaRestoration = ini.GetValue("Features", "EnableMagickaRestoration") ? ini.GetBoolValue("Features", "EnableMagickaRestoration") : true;
	logger::info("[SETTINGS] {} {}", "EnableMagickaRestoration", std::to_string(_featMagickaRestoration));
	_featStaminaRestoration = ini.GetValue("Features", "EnableStaminaRestoration") ? ini.GetBoolValue("Features", "EnableStaminaRestoration") : true;
	logger::info("[SETTINGS] {} {}", "EnableStaminaRestoration", std::to_string(_featStaminaRestoration));
	_featHealthRestoration = ini.GetValue("Features", "EnableHealthRestoration") ? ini.GetBoolValue("Features", "EnableHealthRestoration") : true;
	logger::info("[SETTINGS] {} {}", "EnableHealthRestoration", std::to_string(_featHealthRestoration));
	_featUsePoisons = ini.GetValue("Features", "EnablePoisonUsage") ? ini.GetBoolValue("Features", "EnablePoisonUsage") : true;
	logger::info("[SETTINGS] {} {}", "EnablePoisonUsage", std::to_string(_featUsePoisons));
	_featUseFortifyPotions = ini.GetValue("Features", "EnableFortifyPotionUsage") ? ini.GetBoolValue("Features", "EnableFortifyPotionUsage") : true;
	logger::info("[SETTINGS] {} {}", "EnableFortifyPotionUsage", std::to_string(_featUseFortifyPotions));
	_featUseFood = ini.GetValue("Features", "EnableFoodUsage") ? ini.GetBoolValue("Features", "EnableFoodUsage") : true;
	logger::info("[SETTINGS] {} {}", "EnableFoodUsage", std::to_string(_featUseFood));

	_playerRestorationEnabled = ini.GetValue("Features", "EnablePlayerRestoration") ? ini.GetBoolValue("Features", "EnablePlayerRestoration") : false;
	logger::info("[SETTINGS] {} {}", "EnablePlayerRestoration", std::to_string(_playerRestorationEnabled));
	_playerUsePoisons = ini.GetValue("Features", "EnablePlayerPoisonUsage") ? ini.GetBoolValue("Features", "EnablePlayerPoisonUsage") : false;
	logger::info("[SETTINGS] {} {}", "EnablePlayerPoisonUsage", std::to_string(_playerUsePoisons));
	_playerUseFortifyPotions = ini.GetValue("Features", "EnablePlayerFortifyPotionUsage") ? ini.GetBoolValue("Features", "EnablePlayerFortifyPotionUsage") : false;
	logger::info("[SETTINGS] {} {}", "EnablePlayerFortifyPotionUsage", std::to_string(_playerUseFortifyPotions));

	_featDistributePotions = ini.GetValue("Features", "DistributePotions") ? ini.GetBoolValue("Features", "DistributePotions") : true;
	logger::info("[SETTINGS] {} {}", "DistributePotions", std::to_string(_featDistributePotions));
	_featDistributePoisons = ini.GetValue("Features", "DistributePoisons") ? ini.GetBoolValue("Features", "DistributePoisons") : true;
	logger::info("[SETTINGS] {} {}", "DistributePoisons", std::to_string(_featDistributePoisons));
	_featDistributeFood = ini.GetValue("Features", "DistributeFood") ? ini.GetBoolValue("Features", "DistributeFood") : true;
	logger::info("[SETTINGS] {} {}", "DistributeFood", std::to_string(_featDistributeFood));
	_featDistributeFortifyPotions = ini.GetValue("Features", "DistributeFortifyPotions") ? ini.GetBoolValue("Features", "DistributeFortifyPotions") : true;
	logger::info("[SETTINGS] {} {}", "DistributeFortifyPotions", std::to_string(_featDistributeFortifyPotions));

	_featRemoveItemsOnDeath = ini.GetValue("Features", "RemoveItemsOnDeath") ? ini.GetBoolValue("Features", "RemoveItemsOnDeath") : true;
	logger::info("[SETTINGS] {} {}", "RemoveItemsOnDeath", std::to_string(_featRemoveItemsOnDeath));

	_featDisableItemUsageWhileStaggered = ini.GetValue("Features", "DisableItemUsageWhileStaggered") ? ini.GetBoolValue("Features", "DisableItemUsageWhileStaggered") : false;
	logger::info("[SETTINGS] {} {}", "DisableItemUsageWhileStaggered", std::to_string(_featDisableItemUsageWhileStaggered));

	// fixes
	_ApplySkillBoostPerks = ini.GetBoolValue("Fixes", "ApplySkillBoostPerks", true);
	logger::info("[SETTINGS] {} {}", "ApplySkillBoostPerks", std::to_string(_ApplySkillBoostPerks));

	// compatibility
	_CompatibilityPotionAnimation = ini.GetValue("Compatibility", "UltimatePotionAnimation") ? ini.GetBoolValue("Compatibility", "UltimatePotionAnimation") : false;
	logger::info("[SETTINGS] {} {}", "UltimatePotionAnimation", std::to_string(_CompatibilityPotionAnimation));
	_CompatibilityPotionAnimationFortify = ini.GetValue("Compatibility", "UltimatePotionAnimationFortify") ? ini.GetBoolValue("Compatibility", "UltimatePotionAnimationFortify") : false;
	logger::info("[SETTINGS] {} {}", "UltimatePotionAnimationFortify", std::to_string(_CompatibilityPotionAnimationFortify));
	// get wether zxlice's Ultimate Potion Animation is present
	//auto constexpr folder = R"(Data\SKSE\Plugins\)";
	//for (const auto& entry : std::filesystem::directory_iterator(folder)) {
	//	if (entry.exists() && !entry.path().empty() && entry.path().filename() == "zxlice's ultimate potion animation.dll") {
	//		Ultimateoptions = true;
	//		logger::info("[SETTINGS] zxlice's Ultimate Potion Animation has been detected");
	//	}
	//}
	_CompatibilityMode = ini.GetValue("Compatibility", "Compatibility") ? ini.GetBoolValue("Compatibility", "Compatibility") : false;
	logger::info("[SETTINGS] {} {}", "Compatibility", std::to_string(_CompatibilityMode));
	_CompatibilityDisableAutomaticAdjustments = ini.GetValue("Compatibility", "DisableAutomaticAdjustments") ? ini.GetBoolValue("Compatibility", "DisableAutomaticAdjustments") : false;
	logger::info("[SETTINGS] {} {}", "DisableAutomaticAdjustments", std::to_string(_CompatibilityDisableAutomaticAdjustments));

	_CompatibilityWhitelist = ini.GetBoolValue("Compatibility", "WhitelistMode", false);
	logger::info("[SETTINGS] {} {}", "WhitelistMode", std::to_string(_CompatibilityWhitelist));

	// distribution
	_LevelEasy = ini.GetValue("Distribution", "LevelEasy") ? ini.GetLongValue("Distribution", "LevelEasy") : _LevelEasy;
	logger::info("[SETTINGS] {} {}", "LevelEasy", std::to_string(_LevelEasy));
	_LevelNormal = ini.GetValue("Distribution", "LevelNormal") ? ini.GetLongValue("Distribution", "LevelNormal") : _LevelNormal;
	logger::info("[SETTINGS] {} {}", "LevelNormal", std::to_string(_LevelNormal));
	_LevelDifficult = ini.GetValue("Distribution", "LevelDifficult") ? ini.GetLongValue("Distribution", "LevelDifficult") : _LevelDifficult;
	logger::info("[SETTINGS] {} {}", "LevelDifficult", std::to_string(_LevelDifficult));
	_LevelInsane = ini.GetValue("Distribution", "LevelInsane") ? ini.GetLongValue("Distribution", "LevelInsane") : _LevelInsane;
	logger::info("[SETTINGS] {} {}", "LevelInsane", std::to_string(_LevelInsane));

	_GameDifficultyScaling = ini.GetValue("Distribution", "GameDifficultyScaling") ? ini.GetBoolValue("Distribution", "GameDifficultyScaling") : false;
	logger::info("[SETTINGS] {} {}", "GameDifficultyScaling", std::to_string(_GameDifficultyScaling));

	_MaxMagnitudeWeak = ini.GetValue("Distribution", "MaxMagnitudeWeak") ? ini.GetLongValue("Distribution", "MaxMagnitudeWeak") : _MaxMagnitudeWeak;
	logger::info("[SETTINGS] {} {}", "MaxMagnitudeWeak", std::to_string(_MaxMagnitudeWeak));
	_MaxMagnitudeStandard = ini.GetValue("Distribution", "MaxMagnitudeStandard") ? ini.GetLongValue("Distribution", "MaxMagnitudeStandard") : _MaxMagnitudeStandard;
	logger::info("[SETTINGS] {} {}", "MaxMagnitudeStandard", std::to_string(_MaxMagnitudeStandard));
	_MaxMagnitudePotent = ini.GetValue("Distribution", "MaxMagnitudePotent") ? ini.GetLongValue("Distribution", "MaxMagnitudePotent") : _MaxMagnitudePotent;
	logger::info("[SETTINGS] {} {}", "MaxMagnitudePotent", std::to_string(_MaxMagnitudePotent));

	// Restoration Thresholds
	_healthThreshold = ini.GetValue("Restoration", "HealthThresholdPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdPercent")) : _healthThreshold;
	_healthThreshold = ini.GetValue("Restoration", "HealthThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "HealthThresholdLowerPercent")) : _healthThreshold;
	if (_healthThreshold > 0.95f)
		_healthThreshold = 0.95f;
	logger::info("[SETTINGS] {} {}", "HealthThresholdPercent", std::to_string(_healthThreshold));
	_magickaThreshold = ini.GetValue("Restoration", "MagickaThresholdPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdPercent")) : _magickaThreshold;
	_magickaThreshold = ini.GetValue("Restoration", "MagickaThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "MagickaThresholdLowerPercent")) : _magickaThreshold;
	if (_magickaThreshold > 0.95f)
		_magickaThreshold = 0.95f;
	logger::info("[SETTINGS] {} {}", "MagickaThresholdPercent", std::to_string(_magickaThreshold));
	_staminaThreshold = ini.GetValue("Restoration", "StaminaThresholdPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdPercent")) : _staminaThreshold;
	_staminaThreshold = ini.GetValue("Restoration", "StaminaThresholdLowerPercent") ? static_cast<float>(ini.GetDoubleValue("Restoration", "StaminaThresholdLowerPercent")) : _staminaThreshold;
	if (_staminaThreshold > 0.95f)
		_staminaThreshold = 0.95f;
	logger::info("[SETTINGS] {} {}", "StaminaThresholdPercent", std::to_string(_staminaThreshold));
	_UsePotionChance = ini.GetValue("Restoration", "UsePotionChance") ? static_cast<int>(ini.GetLongValue("Restoration", "UsePotionChance")) : _UsePotionChance;
	logger::info("[SETTINGS] {} {}", "UsePotionChance", std::to_string(_UsePotionChance));

	// Poisonusage options
	_EnemyLevelScalePlayerLevel = ini.GetValue("Poisons", "EnemyLevelScalePlayerLevel") ? static_cast<float>(ini.GetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel")) : _EnemyLevelScalePlayerLevel;
	logger::info("[SETTINGS] {} {}", "EnemyLevelScalePlayerLevel", std::to_string(_EnemyLevelScalePlayerLevel));
	_EnemyNumberThreshold = ini.GetValue("Poisons", "FightingNPCsNumberThreshold") ? ini.GetLongValue("Poisons", "FightingNPCsNumberThreshold") : _EnemyNumberThreshold;
	logger::info("[SETTINGS] {} {}", "FightingNPCsNumberThreshold", std::to_string(_EnemyNumberThreshold));
	_UsePoisonChance = ini.GetValue("Poisons", "UsePoisonChance") ? static_cast<int>(ini.GetLongValue("Poisons", "UsePoisonChance")) : _UsePoisonChance;
	logger::info("[SETTINGS] {} {}", "UsePoisonChance", std::to_string(_UsePoisonChance));

	// fortify options
	_EnemyLevelScalePlayerLevelFortify = ini.GetValue("Fortify", "EnemyLevelScalePlayerLevelFortify") ? static_cast<float>(ini.GetDoubleValue("Fortify", "EnemyLevelScalePlayerLevelFortify")) : _EnemyLevelScalePlayerLevelFortify;
	logger::info("[SETTINGS] {} {}", "EnemyLevelScalePlayerLevelFortify", std::to_string(_EnemyLevelScalePlayerLevelFortify));
	_EnemyNumberThresholdFortify = ini.GetValue("Fortify", "FightingNPCsNumberThresholdFortify") ? ini.GetLongValue("Fortify", "FightingNPCsNumberThresholdFortify") : _EnemyNumberThresholdFortify;
	logger::info("[SETTINGS] {} {}", "FightingNPCsNumberThresholdFortify", std::to_string(_EnemyNumberThresholdFortify));
	_UseFortifyPotionChance = ini.GetValue("Fortify", "UseFortifyPotionChance") ? static_cast<int>(ini.GetLongValue("Fortify", "UseFortifyPotionChance")) : _UseFortifyPotionChance;
	logger::info("[SETTINGS] {} {}", "UseFortifyPotionChance", std::to_string(_UseFortifyPotionChance));

	// removal options
	_ChanceToRemoveItem = ini.GetValue("Removal", "ChanceToRemoveItem") ? ini.GetLongValue("Removal", "ChanceToRemoveItem") : _ChanceToRemoveItem;
	logger::info("[SETTINGS] {} {}", "ChanceToRemoveItem", std::to_string(_ChanceToRemoveItem));
	_MaxItemsLeft = ini.GetValue("Removal", "MaxItemsLeftAfterRemoval") ? ini.GetLongValue("Removal", "MaxItemsLeftAfterRemoval") : _MaxItemsLeft;
	logger::info("[SETTINGS] {} {}", "MaxItemsLeftAfterRemoval", std::to_string(_MaxItemsLeft));

	// general
	_maxPotionsPerCycle = ini.GetValue("General", "MaxPotionsPerCycle") ? ini.GetLongValue("General", "MaxPotionsPerCycle", 1) : 1;
	logger::info("[SETTINGS] {} {}", "MaxPotionsPerCycle", std::to_string(_maxPotionsPerCycle));
	_cycletime = ini.GetValue("General", "CycleWaitTime") ? ini.GetLongValue("General", "CycleWaitTime", 1000) : 1000;
	logger::info("[SETTINGS] {} {}", "CycleWaitTime", std::to_string(_cycletime));
	_DisableEquipSounds = ini.GetValue("General", "DisableEquipSounds") ? ini.GetBoolValue("General", "DisableEquipSounds", false) : false;
	logger::info("[SETTINGS] {} {}", "DisableEquipSounds", std::to_string(_DisableEquipSounds));

	// Debugging
	EnableLog = ini.GetValue("Debug", "EnableLogging") ? ini.GetBoolValue("Debug", "EnableLogging") : false;
	Logging::EnableLog = EnableLog;
	logger::info("[SETTINGS] {} {}", "EnableLogging", std::to_string(EnableLog));
	LogLevel = ini.GetValue("Debug", "LogLevel") ? ini.GetLongValue("Debug", "LogLevel") : 0;
	Logging::LogLevel = LogLevel;
	logger::info("[SETTINGS] {} {}", "LogLevel", std::to_string(LogLevel));
	EnableProfiling = ini.GetValue("Debug", "EnableProfiling") ? ini.GetBoolValue("Debug", "EnableProfiling") : false;
	Logging::EnableProfiling = EnableProfiling;
	logger::info("[SETTINGS] {} {}", "EnableProfiling", std::to_string(EnableProfiling));
	ProfileLevel = ini.GetValue("Debug", "ProfileLevel") ? ini.GetLongValue("Debug", "ProfileLevel") : 0;
	Logging::ProfileLevel = ProfileLevel;
	logger::info("[SETTINGS] {} {}", "ProfileLevel", std::to_string(LogLevel));

	_CheckActorsWithoutRules = ini.GetBoolValue("Debug", "CheckActorWithoutRules", false);
	logger::info("[SETTINGS] {} {}", "CheckActorWithoutRules", std::to_string(_CheckActorsWithoutRules));

	_CalculateCellRules = ini.GetBoolValue("Debug", "CalculateCellRules", false);
	logger::info("[SETTINGS] {} {}", "CalculateCellRules", std::to_string(_CalculateCellRules));
	_Test = ini.GetBoolValue("Debug", "CalculateAllCellOnStartup", false);
	logger::info("[SETTINGS] {} {}", "CalculateAllCellOnStartup", std::to_string(_Test));
	if (_CalculateCellRules && _Test == false) {
		std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv", std::ofstream::out);
		out << "CellName;RuleApplied;PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
	}

	_CompatibilityRemoveItemsBeforeDist = ini.GetBoolValue("Debug", "RemoveItemsBeforeDist", false);
	logger::info("[SETTINGS] {} {}", "RemoveItemsBeforeDist", std::to_string(_CompatibilityRemoveItemsBeforeDist));
	_CompatibilityRemoveItemsStartup = ini.GetBoolValue("Debug", "RemoveItemsStartup", false);
	logger::info("[SETTINGS] {} {}", "RemoveItemsStartup", std::to_string(_CompatibilityRemoveItemsStartup));
	_CompatibilityRemoveItemsStartup_OnlyExcluded = ini.GetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", false);
	logger::info("[SETTINGS] {} {}", "RemoveItemsStartup_OnlyExcluded", std::to_string(_CompatibilityRemoveItemsStartup_OnlyExcluded));

	// save user settings, before applying adjustments
	Save();

	// apply settings for ultimate potions
	if (Ultimateoptions) {
		logger::info("[SETTINGS] [OVERRIDE] Adapting Settings for zxlice's Ultimate Potion Animation");
		_CompatibilityPotionAnimation = true;
		logger::info("[SETTINGS] [OVERRIDE] Compatibility - {} hase been overwritten and set to true", "UltimatePotionAnimation");
		if (_cycletime < 2500) {
			if (!_CompatibilityDisableAutomaticAdjustments && _playerRestorationEnabled) {
				_cycletime = 2500;
				logger::info(
					"[SETTINGS] [OVERRIDE] General - {} has been set to 2500, to avoid spamming potions while in animation. (get those nasty soudns off off me)",
					"CycleWaitTime");
			} else
				logger::info("[SETTINGS] [OVERRIDE] General - {} has NOT been adjusted, either due to adjustment policy or player features being disabled.", "CycleWaitTime");
		}
		if (_maxPotionsPerCycle > 1) {
			_maxPotionsPerCycle = 1;
			logger::info("[SETTINGS] [OVERRIDE] General - {} has been set to 1, since only one potion can be applied at a time.", "MaxPotionsPerCycle");
		}
	}
	auto datahandler = RE::TESDataHandler::GetSingleton();
	logger::info("[SETTINGS] checking for plugins");

	// search for PotionAnimatedFx.esp for compatibility
	if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ Compatibility::Plugin_PotionAnimatedfx }); plugin) {
		_CompatibilityPotionAnimatedFx = true;
		logger::info("[SETTINGS] Found plugin PotionAnimatedfx.esp and activated compatibility mode");
	} else {
		// if we cannot find the plugin then we need to disable all related compatibility options, otherwise we WILL get CTDs
	}
	logger::info("[SETTINGS] checking for plugins2");

	// plugin check
	if (_CompatibilityMode) {
		if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ PluginName }); plugin) {
			logger::info("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
		} else {
			logger::info("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, all use of potions, poisons and food is effectively disabled, except you have another sink for the papyrus events. Distribution is not affected");
		}
	} else if (_CompatibilityPotionAnimation) {
		if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ PluginName }); plugin) {
			logger::info("[SETTINGS] NPCsUsePotions.esp is loaded, Your good to go!");
		} else {
			logger::info("[SETTINGS] [WARNING] NPCsUsePotions.esp was not loaded, Potion drinking will be effectively disabled, except you have another plugin that listens to the Papyrus Mod Events. Other functionality is not affected");
		}
	}
	if (_CompatibilityPotionAnimation && _CompatibilityPotionAnimatedFx) {
		_CompatibilityPotionAnimatedFx = false;
		logger::info("[SETTINGS] [WARNING] Compatibility modes for zxlice's Ultimate Potion Animation and PotionAnimatedfx.esp have been activated simultaneously. To prevent issues the Compatibility mode for PotionAnimatedfx.esp has been deactivated.");
	}
	// Check for CACO
	{
		if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ "Complete Alchemy & Cooking Overhaul.esp" }); plugin) {
			logger::info("[SETTINGS] Complete Alchemy & Cooking Overhaul.esp is loaded, activating compatibility mode!");
			_CompatibilityCACO = true;
		}
	}
	// Check for Apothecary
	{
		if (const RE::TESFile* plugin = datahandler->LookupModByName(std::string_view{ "Apothecary.esp" }); plugin) {
			logger::info("[SETTINGS] Apothecary.esp is loaded, activating compatibility mode!");
			_CompatibilityApothecary = true;
		}
	}
	logger::info("[SETTINGS] checking for plugins end");

	// change potion sound output model to not always play on the player
	{
		RE::TESForm* SOM_player1st = RE::TESForm::LookupByID(0xb4058);
		RE::TESForm* SOM_verb = RE::TESForm::LookupByID(0xd78b4);
		if (SOM_player1st && SOM_verb) {
			RE::BGSSoundOutput* SOMMono01400_verb = SOM_verb->As<RE::BGSSoundOutput>();
			RE::BGSSoundOutput* SOMMono01400Player1st = SOM_player1st->As<RE::BGSSoundOutput>();
			// ITMPotionUse
			RE::TESForm* PotionUseF = RE::TESForm::LookupByID(0xB6435);
			PotionUse = nullptr;
			if (PotionUseF)
				PotionUse = PotionUseF->As<RE::BGSSoundDescriptorForm>();
			if (PotionUse) {
				RE::BGSSoundDescriptor* PotionUseSD = PotionUse->soundDescriptor;
				RE::BGSStandardSoundDef* PotionUseOM = (RE::BGSStandardSoundDef*)PotionUseSD;
				//LOG1_1("{}{}", (PotionUseOM->outputModel->GetFormID()));
				if (PotionUseOM->outputModel->GetFormID() == SOMMono01400Player1st->GetFormID()) {
					PotionUseOM->outputModel = SOMMono01400_verb;
					logger::info("[SETTINGS] changed output model for ITMPotionUse sound effect");
				}
				if (PotionUseOM->outputModel->GetFormID() != SOMMono01400Player1st->GetFormID()) {
					FixedPotionUse = true;
					logger::info("[SETTINGS] enabled sound playing for ITMPotionUse");
				}
			}

			//----Pickup Sounds only----
			//// ITMPotionDownSD
			//RE::TESForm* PotionDownF = RE::TESForm::LookupByID(0x3EDC0);
			//RE::BGSSoundDescriptorForm* PotionDown = nullptr;
			//if (PotionDownF)
			//	PotionDown = PotionDownF->As<RE::BGSSoundDescriptorForm>();
			//if (PotionDown) {
			//	RE::BGSSoundDescriptor* PotionDownSD = PotionDown->soundDescriptor;
			//	RE::BGSStandardSoundDef* PotionDownOM = (RE::BGSStandardSoundDef*)PotionDownSD;
			//	if (PotionDownOM->outputModel->GetFormID() == SOMMono01400Player1st->GetFormID()) {
			//		PotionDownOM->outputModel = SOMMono01400_verb;
			//		logger::info("[SETTINGS] changed output model for ITMPotionDownSD sound effect");
			//	}
			//}
			//// ITMPotionUpSD
			//RE::TESForm* PotionUpF = RE::TESForm::LookupByID(0x3EDBD);
			//RE::BGSSoundDescriptorForm* PotionUp = nullptr;
			//if (PotionUpF)
			//	PotionUp = PotionUpF->As<RE::BGSSoundDescriptorForm>();
			//if (PotionUp) {
			//	RE::BGSSoundDescriptor* PotionUpSD = PotionUp->soundDescriptor;
			//	RE::BGSStandardSoundDef* PotionUpOM = (RE::BGSStandardSoundDef*)PotionUpSD;
			//	if (PotionUpOM->outputModel->GetFormID() == SOMMono01400Player1st->GetFormID()) {
			//		PotionUpOM->outputModel = SOMMono01400_verb;
			//		logger::info("[SETTINGS] changed output model for ITMPotionUpSD sound effect");
			//	}
			//}
			// -------------------------

			// remove poisons, since they aren't "used", but applied and then removed from inventory, so no sounds would play anyway

			// ITMPoisonUse
			RE::TESForm* PoisonUseF = RE::TESForm::LookupByID(0x106614);
			PoisonUse = nullptr;
			if (PoisonUseF)
				PoisonUse = PoisonUseF->As<RE::BGSSoundDescriptorForm>();
			if (PoisonUse) {
				RE::BGSSoundDescriptor* PoisonUseSD = PoisonUse->soundDescriptor;
				RE::BGSStandardSoundDef* PoisonUseOM = (RE::BGSStandardSoundDef*)PoisonUseSD;
				if (PoisonUseOM->outputModel->GetFormID() == SOMMono01400Player1st->GetFormID()) {
					PoisonUseOM->outputModel = SOMMono01400_verb;
					FixedPoisonUse = true;
					logger::info("[SETTINGS] changed output model for ITMPoisonUse sound effect");
				}
				if (PoisonUseOM->outputModel->GetFormID() != SOMMono01400Player1st->GetFormID()) {
					FixedPoisonUse = true;
					logger::info("[SETTINGS] enabled sound playing for ITMPoisonUse");
				}
			}
			// ITMFoodEat
			RE::TESForm* FoodEatF = RE::TESForm::LookupByID(0xCAF94);
			FoodEat = nullptr;
			if (FoodEatF)
				FoodEat = FoodEatF->As<RE::BGSSoundDescriptorForm>();
			if (FoodEat) {
				RE::BGSSoundDescriptor* FoodEatSD = FoodEat->soundDescriptor;
				RE::BGSStandardSoundDef* FoodEatOM = (RE::BGSStandardSoundDef*)FoodEatSD;
				if (FoodEatOM->outputModel->GetFormID() == SOMMono01400Player1st->GetFormID()) {
					FoodEatOM->outputModel = SOMMono01400_verb;
					logger::info("[SETTINGS] changed output model for ITMFoodEat sound effect");
				}
				if (FoodEatOM->outputModel->GetFormID() != SOMMono01400Player1st->GetFormID()) {
					FixedFoodEat = true;
					logger::info("[SETTINGS] enabled sound playing for ITMFoodEat");
				}
			}
		}
	}
}


void Settings::Save()
{
	constexpr auto path = L"Data/SKSE/Plugins/NPCsUsePotions.ini";

	CSimpleIniA ini;

	ini.SetUnicode();

	// features
	ini.SetBoolValue("Features", "EnableHealthRestoration", _featHealthRestoration, ";NPCs use health potions to restore their missing hp in combat.");
	ini.SetBoolValue("Features", "EnableMagickaRestoration", _featMagickaRestoration, ";NPCs use magicka potions to restore their missing magicka in combat.");
	ini.SetBoolValue("Features", "EnableStaminaRestoration", _featStaminaRestoration, ";NPCs use stamina potions to restore their missing stamina in combat.");
	ini.SetBoolValue("Features", "EnablePoisonUsage", _featUsePoisons, ";NPCs use poisons in combat.\n;Followers will use poisons only on appropiate enemies.\n;Generic NPCs will randomly use their poisons.");
	ini.SetBoolValue("Features", "EnableFortifyPotionUsage", _featUseFortifyPotions, ";NPCs use fortify potions in combat.\n;Potions are used based on the equipped weapons and spells.");
	ini.SetBoolValue("Features", "EnableFoodUsage", _featUseFood, ";Normally one would assume that NPCs eat during the day. This features simulates");

	ini.SetBoolValue("Features", "EnablePlayerRestoration", _playerRestorationEnabled, ";All activated restoration features are applied to the player, while they are in Combat.");
	ini.SetBoolValue("Features", "EnablePlayerPoisonUsage", _playerUsePoisons, ";Player will automatically use poisons.");
	ini.SetBoolValue("Features", "EnablePlayerFortifyPotionUsage", _playerUseFortifyPotions, ";Player will use fortify potions the way followers do.");

	ini.SetBoolValue("Features", "DistributePotions", _featDistributePotions, ";NPCs are given potions when they enter combat.");
	ini.SetBoolValue("Features", "DistributePoisons", _featDistributePoisons, ";NPCs are give poisons when they enter combat.");
	ini.SetBoolValue("Features", "DistributeFood", _featDistributeFood, ";NPCs are given food items when they enter combat, and will use them immediately.");
	ini.SetBoolValue("Features", "DistributeFortifyPotions", _featDistributeFortifyPotions, ";NPCs are give fortify potions when they enter combat.");
	ini.SetBoolValue("Features", "RemoveItemsOnDeath", _featRemoveItemsOnDeath, ";Remove items from NPCs after they died.");

	ini.SetBoolValue("Features", "DisableItemUsageWhileStaggered", _featDisableItemUsageWhileStaggered, ";NPCs that are staggered aren't able to use any potions and poisons.");

	// fixes
	ini.SetBoolValue("Fixes", "ApplySkillBoostPerks", _ApplySkillBoostPerks, ";Distributes the two Perks AlchemySkillBoosts and PerkSkillBoosts to npcs which are needed for fortify etc. potions to apply.");

	// compatibility
	ini.SetBoolValue("Compatibility", "UltimatePotionAnimation", _CompatibilityPotionAnimation, ";Compatibility mode for \"zxlice's ultimate potion animation\". Requires the Skyrim esp plugin. Only uses compatibility mode for Health, Stamina and Magicka Potions");
	ini.SetBoolValue("Compatibility", "UltimatePotionAnimationFortify", _CompatibilityPotionAnimationFortify, ";Compatibility mode for \"zxlice's ultimate potion animation\". Requires the Skyrim esp plugin. Uses compatibility mode for Fortify Potions");
	ini.SetBoolValue("Compatibility", "Compatibility", _CompatibilityMode, ";General Compatibility Mode. If set to true, all items will be equiped using Papyrus workaround. Requires the Skyrim esp plugin.");
	ini.SetBoolValue("Compatibility", "DisableAutomaticAdjustments", _CompatibilityDisableAutomaticAdjustments, ";Disables automatic changes made to settings, due to compatibility.\n;Not all changes can be disabled.\n;1) Changes to \"MaxPotionsPerCycle\" when using Potion Animation Mods.\n;2) Enabling of \"UltimatePotionAnimation\" if zxlice's dll is found in your plugin folder. Since it would very likely result in a crash with this option enabled.");
	ini.SetBoolValue("Compatibility", "WhitelistMode", _CompatibilityWhitelist, ";Enables the whitelist mode. Items that shall be distributed must not\n;be explicitly stated in the rules. This is the opposite to the standard (blacklist) behaviour.\n;Only use this if your loadorder causes you CTDs etc due to items being distributed that should not.\n;If you know which mod is causing the issue please do report it, such that appropriate rules can be created. This should only be a temporary solution.");

	// distribution options
	ini.SetLongValue("Distribution", "LevelEasy", _LevelEasy, ";NPC lower or equal this level are considered weak.");
	ini.SetLongValue("Distribution", "LevelNormal", _LevelNormal, ";NPC lower or equal this level are considered normal in terms of strength.");
	ini.SetLongValue("Distribution", "LevelDifficult", _LevelDifficult, ";NPC lower or equal this level are considered difficult.");
	ini.SetLongValue("Distribution", "LevelInsane", _LevelInsane, ";NPC lower or equal this level are considered insane. Everything above this is always treated as a boss.");

	ini.SetBoolValue("Distribution", "GameDifficultyScaling", _GameDifficultyScaling, ";Disables NPC level scaling, but scales chance according to game difficulty.");

	ini.SetLongValue("Distribution", "MaxMagnitudeWeak", _MaxMagnitudeWeak, ";Items with this or lower magnitude*duration are considered weak.");
	ini.SetLongValue("Distribution", "MaxMagnitudeStandard", _MaxMagnitudeStandard, ";Items with this or lower magnitude*duration are considered normal.");
	ini.SetLongValue("Distribution", "MaxMagnitudePotent", _MaxMagnitudePotent, ";Items with this or lower magnitude*duration are considered potent. Everything above this is considered Insane tier");

	// potion options
	ini.SetDoubleValue("Restoration", "HealthThresholdPercent", _healthThreshold, ";Upon reaching this threshold, NPCs will start to use health potions");
	ini.SetDoubleValue("Restoration", "MagickaThresholdPercent", _magickaThreshold, ";Upon reaching this threshold, NPCs will start to use magicka potions");
	ini.SetDoubleValue("Restoration", "StaminaThresholdPercent", _staminaThreshold, ";Upon reaching this threshold, NPCs will start to use stamina potions");
	ini.SetLongValue("Restoration", "UsePotionChance", _UsePotionChance, ";Chance that an NPC will use a potion if they can. Set to 100 to always take a potion, when appropiate.");

	// Poison usage options
	ini.SetDoubleValue("Poisons", "EnemyLevelScalePlayerLevel", _EnemyLevelScalePlayerLevel, ";Scaling factor when NPCs start using poisons on enemies.\n;If the enemy they are facing has a level greater equal 'this value' * PlayerLevel followers use poisons.");
	ini.SetLongValue("Poisons", "FightingNPCsNumberThreshold", _EnemyNumberThreshold, ";When the number of NPCs in a fight is at least at this value, followers start to use poisons regardless of the enemies level, to faster help out the player.");
	ini.SetLongValue("Poisons", "UsePoisonChance", _UsePoisonChance, ";Chance that an NPC will use a fortify potion if they can.");

	// fortify options
	ini.SetDoubleValue("Fortify", "EnemyLevelScalePlayerLevelFortify", _EnemyLevelScalePlayerLevelFortify, ";Scaling factor when NPCs start using fortify potions on enemies.\n;If the enemy they are facing has a level greater equal 'this value' * PlayerLevel followers use fortify potions.");
	ini.SetLongValue("Fortify", "FightingNPCsNumberThresholdFortify", _EnemyNumberThresholdFortify, ";When the number of NPCs in a fight is at least at this value, followers start to use fortify potions regardless of the enemies level.");
	ini.SetLongValue("Fortify", "UseFortifyPotionChance", _UseFortifyPotionChance, ";Chance that an NPC will use a potion if they can. Set to 100 to always take a potion, when appropiate.");

	// removal options
	ini.SetLongValue("Removal", "ChanceToRemoveItem", _ChanceToRemoveItem, ";Chance to remove items on death of NPC. (range: 0 to 100)");
	ini.SetLongValue("Removal", "MaxItemsLeftAfterRemoval", _MaxItemsLeft, ";Maximum number of items chances are rolled for during removal. Everything that goes above this value is always removed.");

	// general
	ini.SetLongValue("General", "MaxPotionsPerCycle", _maxPotionsPerCycle, ";Maximum number of potions NPCs can use each Period");
	//logger::info("[SETTINGS] writing {} {}", "MaxPotionsPerCycle", std::to_string(_maxPotionsPerCycle));
	ini.SetLongValue("General", "CycleWaitTime", _cycletime, ";Time between two periods in milliseconds.");
	//logger::info("[SETTINGS] writing {} {}", "CycleWaitTime", std::to_string(_cycletime));
	ini.SetLongValue("General", "DisableEquipSounds", _DisableEquipSounds, ";Disable Sounds when equipping Items.");
	//logger::info("[SETTINGS] writing {} {}", "DisableEquipSounds", std::to_string(_DisableEquipSounds));

	// debugging
	ini.SetBoolValue("Debug", "EnableLogging", EnableLog, ";Enables logging output. Use with care as log may get very large");
	ini.SetLongValue("Debug", "LogLevel", LogLevel, ";1 - layer 0 log entries, 2 - layer 1 log entries, 3 - layer 3 log entries, 4 - layer 4 log entries. Affects which functions write log entries, as well as what is written by those functions. ");
	ini.SetBoolValue("Debug", "EnableProfiling", EnableProfiling, ";Enables profiling output.");
	ini.SetLongValue("Debug", "ProfileLevel", ProfileLevel, ";1 - only highest level functions write their executions times to the log, 2 - lower level functions are written, 3 - lowest level functions are written. Be aware that not all functions are supported as Profiling costs execution time.");

	ini.SetBoolValue("Debug", "CheckActorWithoutRules", _CheckActorsWithoutRules, ";Checks all actors in the game on game start whether they are applied the default distribution rule.");
	ini.SetBoolValue("Debug", "CalculateCellRules", _CalculateCellRules, ";When entering a new cell in game, all distribution rules are calculatet once.\n;The result of the evaluation is written to a csv file, for rule debugging");
	ini.SetBoolValue("Debug", "CalculateAllCellOnStartup", _Test, ";10 seconds after loading a save game the function for \"CalculateAllCellRules\" is applied to all cells in the game");

	ini.SetBoolValue("Debug", "RemoveItemsBeforeDist", _CompatibilityRemoveItemsBeforeDist, ";Removes all distributables items from npcs before distributing new items.");
	ini.SetBoolValue("Debug", "RemoveItemsStartup", _CompatibilityRemoveItemsStartup, ";Removes all distributables items from npcs 5 seconds after loading a game");
	ini.SetBoolValue("Debug", "RemoveItemsStartup_OnlyExcluded", _CompatibilityRemoveItemsStartup_OnlyExcluded, ";Removes only excluded items from npcs");

	ini.SaveFile(path);
}
