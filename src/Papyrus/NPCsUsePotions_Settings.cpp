
#include "Compatibility.h"
#include "Papyrus/NPCsUsePotions_Settings.h"
#include "Settings.h"
#include "Statistics.h"
#include "Utility.h"


namespace Papyrus
{
	namespace SettingsAPI
	{
		const std::string script = "NPCsUsePotions_MCM";

		void Register(RE::BSScript::Internal::VirtualMachine* a_vm)
		{
			// general
			a_vm->RegisterFunction(std::string("GetMaxDuration"), script, Get_MaxDuration);
			a_vm->RegisterFunction(std::string("SetMaxDuration"), script, Set_MaxDuration);
			a_vm->RegisterFunction(std::string("GetMaxFortifyDuration"), script, Get_MaxFortifyDuration);
			a_vm->RegisterFunction(std::string("SetMaxFortifyDuration"), script, Set_MaxFortifyDuration);
			a_vm->RegisterFunction(std::string("ToStringAlchemicEffect"), script, ToStringAlchemicEffect);
			// system
			a_vm->RegisterFunction(std::string("GetCycleTime"), script, System::Get_CycleTime);
			a_vm->RegisterFunction(std::string("SetCycleTime"), script, System::Set_CycleTime);
			// usage
			a_vm->RegisterFunction(std::string("Usage_GetGlobalCooldown"), script, Usage::Get_GlobalCooldown);
			a_vm->RegisterFunction(std::string("Usage_SetGlobalCooldown"), script, Usage::Set_GlobalCooldown);
			a_vm->RegisterFunction(std::string("Usage_GetEffectiveGlobalCooldownPotions"), script, Usage::Get_EffectiveGlobalCooldownPotions);
			a_vm->RegisterFunction(std::string("Usage_GetEffectiveGlobalCooldownPoisons"), script, Usage::Get_EffectiveGlobalCooldownPoisons);
			a_vm->RegisterFunction(std::string("Usage_GetEffectiveGlobalCooldownFood"), script, Usage::Get_EffectiveGlobalCooldownFood);
			a_vm->RegisterFunction(std::string("Usage_GetDisableItemUsageWhileStaggered"), script, Usage::Get_DisableItemUsageWhileStaggered);
			a_vm->RegisterFunction(std::string("Usage_SetDisableItemUsageWhileStaggered"), script, Usage::Set_DisableItemUsageWhileStaggered);
			a_vm->RegisterFunction(std::string("Usage_GetDisableItemUsageWhileFlying"), script, Usage::Get_DisableItemUsageWhileFlying);
			a_vm->RegisterFunction(std::string("Usage_SetDisableItemUsageWhileFlying"), script, Usage::Set_DisableItemUsageWhileFlying);
			a_vm->RegisterFunction(std::string("Usage_GetDisableItemUsageWhileBleedingOut"), script, Usage::Get_DisableItemUsageWhileBleedingOut);
			a_vm->RegisterFunction(std::string("Usage_SetDisableItemUsageWhileBleedingOut"), script, Usage::Set_DisableItemUsageWhileBleedingOut);
			a_vm->RegisterFunction(std::string("Usage_GetDisableItemUsageWhileSleeping"), script, Usage::Get_DisableItemUsageWhileSleeping);
			a_vm->RegisterFunction(std::string("Usage_SetDisableItemUsageWhileSleeping"), script, Usage::Set_DisableItemUsageWhileSleeping);
			a_vm->RegisterFunction(std::string("Usage_GetDisableNonFollowerNPCs"), script, Usage::Get_DisableNonFollowerNPCs);
			a_vm->RegisterFunction(std::string("Usage_SetDisableNonFollowerNPCs"), script, Usage::Set_DisableNonFollowerNPCs);
			a_vm->RegisterFunction(std::string("Usage_GetDisableOutOfCombatProcessing"), script, Usage::Get_DisableOutOfCombatProcessing);
			a_vm->RegisterFunction(std::string("Usage_SetDisableOutOfCombatProcessing"), script, Usage::Set_DisableOutOfCombatProcessing);
			a_vm->RegisterFunction(std::string("Usage_GetDisableItemUsageForExcludedNPCs"), script, Usage::Get_DisableItemUsageForExcludedNPCs);
			a_vm->RegisterFunction(std::string("Usage_SetDisableItemUsageForExcludedNPCs"), script, Usage::Set_DisableItemUsageForExcludedNPCs);
			// potions
			a_vm->RegisterFunction(std::string("Potions_GetEnableMagickaRestoration"), script, Potions::Get_Potions_EnableMagickaRestoration);
			a_vm->RegisterFunction(std::string("Potions_SetEnableMagickaRestoration"), script, Potions::Set_Potions_EnableMagickaRestoration);
			a_vm->RegisterFunction(std::string("Potions_GetEnableStaminaRestoration"), script, Potions::Get_Potions_EnableStaminaRestoration);
			a_vm->RegisterFunction(std::string("Potions_SetEnableStaminaRestoration"), script, Potions::Set_Potions_EnableStaminaRestoration);
			a_vm->RegisterFunction(std::string("Potions_GetEnableHealthRestoration"), script, Potions::Get_Potions_EnableHealthRestoration);
			a_vm->RegisterFunction(std::string("Potions_SetEnableHealthRestoration"), script, Potions::Set_Potions_EnableHealthRestoration);
			a_vm->RegisterFunction(std::string("Potions_GetAllowDetrimentalEffects"), script, Potions::Get_AllowDetrimentalEffects);
			a_vm->RegisterFunction(std::string("Potions_SetAllowDetrimentalEffects"), script, Potions::Set_AllowDetrimentalEffects);
			a_vm->RegisterFunction(std::string("Potions_GetHandleWeaponSheathedAsOutOfCombat"), script, Potions::Get_HandleWeaponSheathedAsOutOfCombat);
			a_vm->RegisterFunction(std::string("Potions_SetHandleWeaponSheathedAsOutOfCombat"), script, Potions::Set_HandleWeaponSheathedAsOutOfCombat);
			a_vm->RegisterFunction(std::string("Potions_GetHealthThreshold"), script, Potions::Get_HealthThreshold);
			a_vm->RegisterFunction(std::string("Potions_SetHealthThreshold"), script, Potions::Set_HealthThreshold);
			a_vm->RegisterFunction(std::string("Potions_GetMagickaThreshold"), script, Potions::Get_MagickaThreshold);
			a_vm->RegisterFunction(std::string("Potions_SetMagickaThreshold"), script, Potions::Set_MagickaThreshold);
			a_vm->RegisterFunction(std::string("Potions_GetStaminaThreshold"), script, Potions::Get_StaminaThreshold);
			a_vm->RegisterFunction(std::string("Potions_SetStaminaThreshold"), script, Potions::Set_StaminaThreshold);
			a_vm->RegisterFunction(std::string("Potions_GetUsePotionChance"), script, Potions::Get_UsePotionChance);
			a_vm->RegisterFunction(std::string("Potions_SetUsePotionChance"), script, Potions::Set_UsePotionChance);
			a_vm->RegisterFunction(std::string("Potions_IsEffectProhibited"), script, Potions::IsEffectProhibited);
			a_vm->RegisterFunction(std::string("Potions_InvertEffectProhibited"), script, Potions::InvertEffectProhibited);
			// poisons
			a_vm->RegisterFunction(std::string("Poisons_GetEnablePoisons"), script, Poisons::Get_EnablePoisons);
			a_vm->RegisterFunction(std::string("Poisons_SetEnablePoison"), script, Poisons::Set_EnablePoisons);
			a_vm->RegisterFunction(std::string("Poisons_GetAllowPositiveEffects"), script, Poisons::Get_AllowPositiveEffects);
			a_vm->RegisterFunction(std::string("Poisons_SetAllowPositiveEffects"), script, Poisons::Set_AllowPositiveEffects);
			a_vm->RegisterFunction(std::string("Poisons_GetDontUseWithWeaponsSheathed"), script, Poisons::Get_DontUseWithWeaponsSheathed);
			a_vm->RegisterFunction(std::string("Poisons_SetDontUseWithWeaponsSheathed"), script, Poisons::Set_DontUseWithWeaponsSheathed);
			a_vm->RegisterFunction(std::string("Poisons_GetDontUseAgainst100PoisonResist"), script, Poisons::Get_DontUseAgainst100PoisonResist);
			a_vm->RegisterFunction(std::string("Poisons_SetDontUseAgainst100PoisonResist"), script, Poisons::Set_DontUseAgainst100PoisonResist);
			a_vm->RegisterFunction(std::string("Poisons_GetEnemyLevelScalePlayerLevel"), script, Poisons::Get_EnemyLevelScalePlayerLevel);
			a_vm->RegisterFunction(std::string("Poisons_SetEnemyLevelScalePlayerLevel"), script, Poisons::Set_EnemyLevelScalePLayerLevel);
			a_vm->RegisterFunction(std::string("Poisons_GetEnemyNumberThreshold"), script, Poisons::Get_EnemyNumberThreshold);
			a_vm->RegisterFunction(std::string("Poisons_SetEnemyNumberThreshold"), script, Poisons::Set_EnemyNumberThreshold);
			a_vm->RegisterFunction(std::string("Poisons_GetUsePoisonChance"), script, Poisons::Get_UsePoisonChance);
			a_vm->RegisterFunction(std::string("Poisons_SetUsePoisonChance"), script, Poisons::Set_UsePoisonChance);
			a_vm->RegisterFunction(std::string("Poisons_GetDosage"), script, Poisons::Get_Dosage);
			a_vm->RegisterFunction(std::string("Poisons_SetDosage"), script, Poisons::Set_Dosage);
			a_vm->RegisterFunction(std::string("Poisons_IsEffectProhibited"), script, Poisons::IsEffectProhibited);
			a_vm->RegisterFunction(std::string("Poisons_InvertEffectProhibited"), script, Poisons::InvertEffectProhibited);
			// fortify potions
			a_vm->RegisterFunction(std::string("Fortify_GetEnableFortifyPotions"), script, FortifyPotions::Get_EnableFortifyPotions);
			a_vm->RegisterFunction(std::string("Fortify_SetEnableFortifyPotions"), script, FortifyPotions::Set_EnableFortifyPotions);
			a_vm->RegisterFunction(std::string("Fortify_GetDontUseWithWeaponsSheathed"), script, FortifyPotions::Get_DontUseWithWeaponsSheathed);
			a_vm->RegisterFunction(std::string("Fortify_SetDontUseWithWeaponsSheathed"), script, FortifyPotions::Set_DontUseWithWeaponsSheathed);
			a_vm->RegisterFunction(std::string("Fortify_GetEnemyLevelScalePlayerLevelFortify"), script, FortifyPotions::Get_EnemyLevelScalePlayerLevelFortify);
			a_vm->RegisterFunction(std::string("Fortify_SetEnemyLevelScalePlayerLevelFortify"), script, FortifyPotions::Set_EnemyLevelScalePlayerLevelFortify);
			a_vm->RegisterFunction(std::string("Fortify_GetEnemyNumberThresholdFortify"), script, FortifyPotions::Get_EnemyNumberThresholdFortify);
			a_vm->RegisterFunction(std::string("Fortify_SetEnemyNumberThresholdFortify"), script, FortifyPotions::Set_EnemyNumberThresholdFortify);
			a_vm->RegisterFunction(std::string("Fortify_GetUseFortifyPotionChance"), script, FortifyPotions::Get_UseFortifyPotionChance);
			a_vm->RegisterFunction(std::string("Fortify_SetUseFortifyPotionChance"), script, FortifyPotions::Set_UseFortifyPotionChance);
			// food
			a_vm->RegisterFunction(std::string("Food_GetEnableFood"), script, Food::Get_EnableFood);
			a_vm->RegisterFunction(std::string("Food_SetEnableFood"), script, Food::Set_EnableFood);
			a_vm->RegisterFunction(std::string("Food_GetAllowDetrimentalEffects"), script, Food::Get_AllowDetrimentalEffects);
			a_vm->RegisterFunction(std::string("Food_SetAllowDetrimentalEffects"), script, Food::Set_AllowDetrimentalEffects);
			a_vm->RegisterFunction(std::string("Food_GetRestrictFoodToCombatStart"), script, Food::Get_RestrictFoodToCombatStart);
			a_vm->RegisterFunction(std::string("Food_SetRestrictFoodToCombatStart"), script, Food::Set_RestrictFoodToCombatStart);
			a_vm->RegisterFunction(std::string("Food_GetDisableFollowers"), script, Food::Get_DisableFollowers);
			a_vm->RegisterFunction(std::string("Food_SetDisableFollowers"), script, Food::Set_DisableFollowers);
			a_vm->RegisterFunction(std::string("Food_GetDontUseWithWeaponsSheathed"), script, Food::Get_DontUseWithWeaponsSheathed);
			a_vm->RegisterFunction(std::string("Food_SetDontUseWithWeaponsSheathed"), script, Food::Set_DontUseWithWeaponsSheathed);
			a_vm->RegisterFunction(std::string("Food_IsEffectProhibited"), script, Food::IsEffectProhibited);
			a_vm->RegisterFunction(std::string("Food_InvertEffectProhibited"), script, Food::InvertEffectProhibited);
			// player
			a_vm->RegisterFunction(std::string("Player_GetPlayerPotions"), script, Player::Get_PlayerPotions);
			a_vm->RegisterFunction(std::string("Player_SetPlayerPotions"), script, Player::Set_PlayerPotions);
			a_vm->RegisterFunction(std::string("Player_GetPlayerPoisons"), script, Player::Get_PlayerPoisons);
			a_vm->RegisterFunction(std::string("Player_SetPlayerPoisons"), script, Player::Set_PlayerPoisons);
			a_vm->RegisterFunction(std::string("Player_GetPlayerFortify"), script, Player::Get_PlayerFortifyPotions);
			a_vm->RegisterFunction(std::string("Player_SetPlayerFortify"), script, Player::Set_PlayerFortifyPotions);
			a_vm->RegisterFunction(std::string("Player_GetPlayerFood"), script, Player::Get_PlayerFood);
			a_vm->RegisterFunction(std::string("Player_SetPlayerFood"), script, Player::Set_PlayerFood);
			a_vm->RegisterFunction(std::string("Player_GetUseFavoritedItemsOnly"), script, Player::Get_UseFavoritedItemsOnly);
			a_vm->RegisterFunction(std::string("Player_SetUseFavoritedItemsOnly"), script, Player::Set_UseFavoritedItemsOnly);
			a_vm->RegisterFunction(std::string("Player_GetDontUseFavoritedItems"), script, Player::Get_DontUseFavoritedItems);
			a_vm->RegisterFunction(std::string("Player_SetDontUseFavoritedItems"), script, Player::Set_DontUseFavoritedItems);
			a_vm->RegisterFunction(std::string("Player_GetDontEatRawFood"), script, Player::Get_DontEatRawFood);
			a_vm->RegisterFunction(std::string("Player_SetDontEatRawFood"), script, Player::Set_DontEatRawFood);
			a_vm->RegisterFunction(std::string("Player_GetDontDrinkAlcohol"), script, Player::Get_DontDrinkAlcohol);
			a_vm->RegisterFunction(std::string("Player_SetDontDrinkAlcohol"), script, Player::Set_DontDrinkAlcohol);
			// distribution
			a_vm->RegisterFunction(std::string("Distr_GetDistributePoisons"), script, Distribution::Get_DistributePoisons);
			a_vm->RegisterFunction(std::string("Distr_SetDistributePoison"), script, Distribution::Set_DistributePoisons);
			a_vm->RegisterFunction(std::string("Distr_GetDistributePotions"), script, Distribution::Get_DistributePotions);
			a_vm->RegisterFunction(std::string("Distr_SetDistributePotions"), script, Distribution::Set_DistributePotions);
			a_vm->RegisterFunction(std::string("Distr_GetDistributeFortify"), script, Distribution::Get_DistributeFortifyPotions);
			a_vm->RegisterFunction(std::string("Distr_SetDistributeFortify"), script, Distribution::Set_DistributeFortifyPotions);
			a_vm->RegisterFunction(std::string("Distr_GetDistributeFood"), script, Distribution::Get_DistributeFood);
			a_vm->RegisterFunction(std::string("Distr_SetDistributeFood"), script, Distribution::Set_DistributeFood);
			a_vm->RegisterFunction(std::string("Distr_GetDistributeCustomItems"), script, Distribution::Get_DistributeCustomItems);
			a_vm->RegisterFunction(std::string("Distr_SetDistributeCustomItems"), script, Distribution::Set_DistributeCustomItems);
			a_vm->RegisterFunction(std::string("Distr_GetLevelEasy"), script, Distribution::Get_LevelEasy);
			a_vm->RegisterFunction(std::string("Distr_SetLevelEasy"), script, Distribution::Set_LevelEasy);
			a_vm->RegisterFunction(std::string("Distr_GetLevelNormal"), script, Distribution::Get_LevelNormal);
			a_vm->RegisterFunction(std::string("Distr_SetLevelNormal"), script, Distribution::Set_LevelNormal);
			a_vm->RegisterFunction(std::string("Distr_GetLevelDifficult"), script, Distribution::Get_LevelDifficult);
			a_vm->RegisterFunction(std::string("Distr_SetLevelDifficult"), script, Distribution::Set_LevelDifficult);
			a_vm->RegisterFunction(std::string("Distr_GetLevelInsane"), script, Distribution::Get_LevelInsane);
			a_vm->RegisterFunction(std::string("Distr_SetLevelInsane"), script, Distribution::Set_LevelInsane);
			a_vm->RegisterFunction(std::string("Distr_GetGameDifficultyScaling"), script, Distribution::Get_GameDifficultyScaling);
			a_vm->RegisterFunction(std::string("Distr_SetGameDifficultyScaling"), script, Distribution::Set_GameDifficultyScaling);
			a_vm->RegisterFunction(std::string("Distr_GetMaxMagnitudeWeak"), script, Distribution::Get_MaxMagnitudeWeak);
			a_vm->RegisterFunction(std::string("Distr_SetMaxMagnitudeWeak"), script, Distribution::Set_MaxMagnitudeWeak);
			a_vm->RegisterFunction(std::string("Distr_GetMaxMagnitudeStandard"), script, Distribution::Get_MaxMagnitudeStandard);
			a_vm->RegisterFunction(std::string("Distr_SetMaxMagnitudeStandard"), script, Distribution::Set_MaxMagnitudeStandard);
			a_vm->RegisterFunction(std::string("Distr_GetMaxMagnitudePotent"), script, Distribution::Get_MaxMagnitudePotent);
			a_vm->RegisterFunction(std::string("Distr_SetMaxMagnitudePotent"), script, Distribution::Set_MaxMagnitudePotent);
			a_vm->RegisterFunction(std::string("Distr_GetStyleScalingPrimary"), script, Distribution::Get_StyleScalingPrimary);
			a_vm->RegisterFunction(std::string("Distr_SetStyleScalingPrimary"), script, Distribution::Set_StyleScalingPrimary);
			a_vm->RegisterFunction(std::string("Distr_GetStyleScalingSecondary"), script, Distribution::Get_StyleScalingSecondary);
			a_vm->RegisterFunction(std::string("Distr_SetStyleScalingSecondary"), script, Distribution::Set_StyleScalingSecondary);
			a_vm->RegisterFunction(std::string("Distr_GetProbabilityScaling"), script, Distribution::Get_ProbabilityScaling);
			a_vm->RegisterFunction(std::string("Distr_SetProbabilityScaling"), script, Distribution::Set_ProbabilityScaling);
			a_vm->RegisterFunction(std::string("Distr_GetDoNotDistributeMixedInvisPotions"), script, Distribution::Get_DoNotDistributeMixedInvisPotions);
			a_vm->RegisterFunction(std::string("Distr_SetDoNotDistributeMixedInvisPotions"), script, Distribution::Set_DoNotDistributeMixedInvisPotions);
			a_vm->RegisterFunction(std::string("Distr_GetProbabilityAdjuster"), script, Distribution::Get_ProbabilityAdjuster);
			a_vm->RegisterFunction(std::string("Distr_SetProbabilityAdjuster"), script, Distribution::Set_ProbabilityAdjuster);
			// removal
			a_vm->RegisterFunction(std::string("Removal_GetRemoveItemsOnDeath"), script, Removal::Get_RemoveItemsOnDeath);
			a_vm->RegisterFunction(std::string("Removal_SetRemoveItemsOnDeath"), script, Removal::Set_RemoveItemsOnDeath);
			a_vm->RegisterFunction(std::string("Removal_GetChanceToRemoveItem"), script, Removal::Get_ChanceToRemoveItem);
			a_vm->RegisterFunction(std::string("Removal_SetChanceToRemoveItem"), script, Removal::Set_ChanceToRemoveItem);
			a_vm->RegisterFunction(std::string("Removal_GetMaxItemsLeft"), script, Removal::Get_MaxItemsLeft);
			a_vm->RegisterFunction(std::string("Removal_SetMaxItemsLeft"), script, Removal::Set_MaxItemsLeft);
			// whitelist
			a_vm->RegisterFunction(std::string("Whitelist_GetEnabledItems"), script, Whitelist::Get_EnabledItems);
			a_vm->RegisterFunction(std::string("Whitelist_SetEnabledItems"), script, Whitelist::Set_EnabledItems);
			a_vm->RegisterFunction(std::string("Whitelist_GetEnabledNPCs"), script, Whitelist::Get_EnabledNPCs);
			a_vm->RegisterFunction(std::string("Whitelist_SetEnabledNPCs"), script, Whitelist::Set_EnabledNPCs);
			// fixes
			a_vm->RegisterFunction(std::string("Fixes_GetApplySkillBoostPerks"), script, Fixes::Get_ApplySkillBoostPerks);
			a_vm->RegisterFunction(std::string("Fixes_SetApplySkillBoostPerks"), script, Fixes::Set_ApplySkillBosstPerks);
			a_vm->RegisterFunction(std::string("Fixes_GetForceFixPotionSounds"), script, Fixes::Get_ForceFixPotionSounds);
			a_vm->RegisterFunction(std::string("Fixes_SetForceFixPotionSounds"), script, Fixes::Set_ForceFixPotionSounds);
			// compatibility
			a_vm->RegisterFunction(std::string("Comp_GetDisableCreaturesWhitoutRules"), script, Compatibility::Get_DisableCreaturesWithoutRules);
			a_vm->RegisterFunction(std::string("Comp_SetDisableCreaturesWithoutRules"), script, Compatibility::Set_DisableCreaturesWithoutRules);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPoisons_GetEnabled"), script, Compatibility::Get_AnimatedPoisons_Enabled);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPoisons_SetEnabled"), script, Compatibility::Set_AnimatedPoisons_Enabled);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPoisons_GetUsePoisonDosage"), script, Compatibility::Get_AnimatedPoisons_UsePoisonDosage);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPoisons_SetUsePoisonDosage"), script, Compatibility::Set_AnimatedPoisons_UsePoisonDosage);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPotions_GetEnabled"), script, Compatibility::Get_AnimatedPotions_Enabled);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPotions_SetEnabled"), script, Compatibility::Set_AnimatedPotions_Enable);
			a_vm->RegisterFunction(std::string("Comp_CACO_Loaded"), script, Compatibility::CACO_Loaded);
			a_vm->RegisterFunction(std::string("Comp_Apothecary_Loaded"), script, Compatibility::Apothecary_Loaded);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPoisons_Loaded"), script, Compatibility::AnimatedPoisons_Loaded);
			a_vm->RegisterFunction(std::string("Comp_AnimatedPotions_Loaded"), script, Compatibility::AnimatedPotions_Loaded);
			a_vm->RegisterFunction(std::string("Comp_ZUPA_Loaded"), script, Compatibility::ZUPA_Loaded);
			a_vm->RegisterFunction(std::string("Comp_Sacrosanct_Loaded"), script, Compatibility::Sacrosanct_Loaded);
			a_vm->RegisterFunction(std::string("Comp_UltimatePotions_Loaded"), script, Compatibility::UltimatePotions_Loaded);
			// debug
			a_vm->RegisterFunction(std::string("Debug_GetEnableLog"), script, Debug::Get_EnableLog);
			a_vm->RegisterFunction(std::string("Debug_SetEnableLog"), script, Debug::Set_EnableLog);
			a_vm->RegisterFunction(std::string("Debug_GetEnableLoadLog"), script, Debug::Get_EnableLoadLog);
			a_vm->RegisterFunction(std::string("Debug_SetEnableLoadLog"), script, Debug::Set_EnableLoadLog);
			a_vm->RegisterFunction(std::string("Debug_GetLogLevel"), script, Debug::Get_LogLevel);
			a_vm->RegisterFunction(std::string("Debug_SetLogLevel"), script, Debug::Set_LogLevel);
			a_vm->RegisterFunction(std::string("Debug_GetProfileLevel"), script, Debug::Get_ProfileLevel);
			a_vm->RegisterFunction(std::string("Debug_SetProfileLevel"), script, Debug::Set_ProfileLevel);
			a_vm->RegisterFunction(std::string("Debug_GetEnableProfiling"), script, Debug::Get_EnableProfiling);
			a_vm->RegisterFunction(std::string("Debug_SetEnableProfiling"), script, Debug::Set_EnableProfiling);
			// statistics
			a_vm->RegisterFunction(std::string("Stats_ActorsSaved"), script, Papyrus::SettingsAPI::Statistics::Get_ActorsSaved);
			a_vm->RegisterFunction(std::string("Stats_ActorsRead"), script, Papyrus::SettingsAPI::Statistics::Get_ActorsRead);
			a_vm->RegisterFunction(std::string("Stats_PotionsUsed"), script, Papyrus::SettingsAPI::Statistics::Get_PotionsUsed);
			a_vm->RegisterFunction(std::string("Stats_PoisonsUsed"), script, Papyrus::SettingsAPI::Statistics::Get_PoisonsUsed);
			a_vm->RegisterFunction(std::string("Stats_FoodUsed"), script, Papyrus::SettingsAPI::Statistics::Get_FoodUsed);
			a_vm->RegisterFunction(std::string("Stats_EventsHandled"), script, Papyrus::SettingsAPI::Statistics::Get_EventsHandled);
			a_vm->RegisterFunction(std::string("Stats_ActorsHandledTotal"), script, Papyrus::SettingsAPI::Statistics::Get_ActorsHandledTotal);
			a_vm->RegisterFunction(std::string("Stats_ActorsHandled"), script, Papyrus::SettingsAPI::Statistics::Get_ActorsHandled);
		}

		bool IsEffectProhibited(Settings::ItemType type, int value)
		{
			switch (type)
			{
			case Settings::ItemType::kPotion:
			case Settings::ItemType::kFortifyPotion:
				return (AlchemicEffect::GetFromBaseValue(value) & Settings::potions._prohibitedEffects).IsValid();
			case Settings::ItemType::kPoison:
				return (AlchemicEffect::GetFromBaseValue(value) & Settings::poisons._prohibitedEffects).IsValid();
			case Settings::ItemType::kFood:
				return (AlchemicEffect::GetFromBaseValue(value) & Settings::food._prohibitedEffects).IsValid();
			}
			return false;
		}

		void InvertEffectProhibited(Settings::ItemType type, int value)
		{
			LOG_4("begin {}", value);
			switch (type) {
			case Settings::ItemType::kPotion:
			case Settings::ItemType::kFortifyPotion:
				{
					LOG_4("potion");
					if (!Settings::potions._prohibitedEffects.HasEffect(value)) {
						Settings::potions._prohibitedEffects |= AlchemicEffect::GetFromBaseValue(value);
						LOG_4("Added effect");
					} else {
						Settings::potions._prohibitedEffects &= ~AlchemicEffect::GetFromBaseValue(value);
						LOG_4("Removed Effect");
					}
				}
				break;
			case Settings::ItemType::kPoison:
				{
					LOG_4("poison");
					if (!Settings::poisons._prohibitedEffects.HasEffect(value)) {
						Settings::poisons._prohibitedEffects |= AlchemicEffect::GetFromBaseValue(value);
						LOG_4("Added effect");
					} else {
						Settings::poisons._prohibitedEffects &= ~AlchemicEffect::GetFromBaseValue(value);
						LOG_4("Removed Effect");
					}
				}
				break;
			case Settings::ItemType::kFood:
				{
					LOG_4("food");
					if (!Settings::food._prohibitedEffects.HasEffect(value)) {
						Settings::food._prohibitedEffects |= AlchemicEffect::GetFromBaseValue(value);
						LOG_4("Added effect");
					} else {
						Settings::food._prohibitedEffects &= ~AlchemicEffect::GetFromBaseValue(value);
						LOG_4("Removed Effect");
					}
				}
				break;
			}
			Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			Settings::_updateSettings |= (uint32_t)Settings::UpdateFlag::kProhibitedEffects;
		}

		int Get_MaxDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
		{
			return Settings::_MaxDuration;
		}

		void Set_MaxDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds)
		{
			if (milliseconds < 0)
				milliseconds = 0;
			Settings::_MaxDuration = milliseconds;
			Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
		}

		int Get_MaxFortifyDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
		{
			return Settings::_MaxFortifyDuration;
		}

		void Set_MaxFortifyDuration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds)
		{
			if (milliseconds < 0)
				milliseconds = 0;
			Settings::_MaxFortifyDuration = milliseconds;
			Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
		}

		RE::BSFixedString ToStringAlchemicEffect(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
		{
			return Utility::ToStringLocalized(AlchemicEffect::GetFromBaseValue(value));
		}

		namespace System
		{
			int Get_CycleTime(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return (int)Settings::system._cycletime;
			}

			void Set_CycleTime(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds)
			{
				if (milliseconds < 100)
					milliseconds = 100;
				if (milliseconds > 10000)
					milliseconds = 10000;
				Settings::system._cycletime = milliseconds;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Usage
		{
			int Get_GlobalCooldown(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return (int)Settings::usage._globalCooldown;
			}

			void Set_GlobalCooldown(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int milliseconds)
			{
				if (milliseconds < 500)
					milliseconds = 500;
				Settings::usage._globalCooldown = milliseconds;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
				Settings::_updateSettings |= (uint32_t)Settings::UpdateFlag::kCompatibility;
			}

			int Get_EffectiveGlobalCooldownPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return (int)::Compatibility::GetSingleton()->GetGlobalCooldownPotions();
			}

			int Get_EffectiveGlobalCooldownPoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return (int)::Compatibility::GetSingleton()->GetGlobalCooldownPoisons();
			}

			int Get_EffectiveGlobalCooldownFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return (int)::Compatibility::GetSingleton()->GetGlobalCooldownFood();
			}

			bool Get_DisableItemUsageWhileStaggered(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableItemUsageWhileStaggered;
			}

			void Set_DisableItemUsageWhileStaggered(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::usage._DisableItemUsageWhileStaggered = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableItemUsageWhileFlying(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableItemUsageWhileFlying;
			}

			void Set_DisableItemUsageWhileFlying(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::usage._DisableItemUsageWhileFlying = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableItemUsageWhileBleedingOut(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableItemUsageWhileBleedingOut;
			}

			void Set_DisableItemUsageWhileBleedingOut(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::usage._DisableItemUsageWhileBleedingOut = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableItemUsageWhileSleeping(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableItemUsageWhileSleeping;
			}

			void Set_DisableItemUsageWhileSleeping(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::usage._DisableItemUsageWhileSleeping = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableNonFollowerNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableNonFollowerNPCs;
			}

			void Set_DisableNonFollowerNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disable)
			{
				Settings::usage._DisableNonFollowerNPCs = disable;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableOutOfCombatProcessing(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableOutOfCombatProcessing;
			}

			void Set_DisableOutOfCombatProcessing(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::usage._DisableOutOfCombatProcessing = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableItemUsageForExcludedNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::usage._DisableItemUsageForExcludedNPCs;
			}

			void Set_DisableItemUsageForExcludedNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::usage._DisableItemUsageForExcludedNPCs = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Potions
		{

			bool Get_Potions_EnableMagickaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._enableMagickaRestoration;
			}

			void Set_Potions_EnableMagickaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::potions._enableMagickaRestoration = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_Potions_EnableStaminaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._enableStaminaRestoration;
			}

			void Set_Potions_EnableStaminaRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::potions._enableStaminaRestoration = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_Potions_EnableHealthRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._enableHealthRestoration;
			}

			void Set_Potions_EnableHealthRestoration(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::potions._enableHealthRestoration = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._AllowDetrimentalEffects;
			}

			void Set_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool allow)
			{
				Settings::potions._AllowDetrimentalEffects = allow;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_HandleWeaponSheathedAsOutOfCombat(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._HandleWeaponSheathedAsOutOfCombat;
			}

			void Set_HandleWeaponSheathedAsOutOfCombat(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::potions._HandleWeaponSheathedAsOutOfCombat = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_HealthThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._healthThreshold;
			}

			void Set_HealthThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value > 0.95f)
					value = 0.95f;
				if (value < 0.05f)
					value = 0.05f;
				Settings::potions._healthThreshold = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_MagickaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._magickaThreshold;
			}

			void Set_MagickaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value > 0.95f)
					value = 0.95f;
				if (value < 0.05f)
					value = 0.05f;
				Settings::potions._magickaThreshold = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_StaminaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._staminaThreshold;
			}

			void Set_StaminaThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value > 0.95f)
					value = 0.95f;
				if (value < 0.05f)
					value = 0.05f;
				Settings::potions._staminaThreshold = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_UsePotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::potions._UsePotionChance;
			}

			void Set_UsePotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				Settings::potions._UsePotionChance = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool IsEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				return SettingsAPI::IsEffectProhibited(Settings::ItemType::kPotion, value);
			}

			void InvertEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				SettingsAPI::InvertEffectProhibited(Settings::ItemType::kPotion, value);
			}
		}

		namespace Poisons
		{
			bool Get_EnablePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._enablePoisons;
			}

			void Set_EnablePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::poisons._enablePoisons = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_AllowPositiveEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._AllowPositiveEffects; 
			}

			void Set_AllowPositiveEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool allow)
			{
				Settings::poisons._AllowPositiveEffects = allow;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._DontUseWithWeaponsSheathed;
			}

			void Set_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::poisons._DontUseWithWeaponsSheathed = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontUseAgainst100PoisonResist(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._DontUseAgainst100PoisonResist;
			}

			void Set_DontUseAgainst100PoisonResist(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::poisons._DontUseAgainst100PoisonResist = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_EnemyLevelScalePlayerLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._EnemyLevelScalePlayerLevel;
			}

			void Set_EnemyLevelScalePLayerLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value < 0.0f)
					value = 0.0f;
				if (value > 100.0f)
					value = 100.0f;
				Settings::poisons._EnemyLevelScalePlayerLevel = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_EnemyNumberThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._EnemyNumberThreshold;
			}

			void Set_EnemyNumberThreshold(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 1)
					value = 1;
				Settings::poisons._EnemyNumberThreshold = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_UsePoisonChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._UsePoisonChance;
			}

			void Set_UsePoisonChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 0)
					value = 0;
				if (value > 100)
					value = 100;
				Settings::poisons._UsePoisonChance = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_Dosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::poisons._Dosage;
			}

			void Set_Dosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 1)
					value = 1;
				if (value > 1000)
					value = 1000;
				Settings::poisons._Dosage = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool IsEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				return SettingsAPI::IsEffectProhibited(Settings::ItemType::kPoison, value);
			}

			void InvertEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				SettingsAPI::InvertEffectProhibited(Settings::ItemType::kPoison, value);
			}
		}

		namespace FortifyPotions
		{
			bool Get_EnableFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fortifyPotions._enableFortifyPotions;
			}

			void Set_EnableFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::fortifyPotions._enableFortifyPotions = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fortifyPotions._DontUseWithWeaponsSheathed;
			}

			void Set_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::fortifyPotions._DontUseWithWeaponsSheathed = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_EnemyLevelScalePlayerLevelFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fortifyPotions._EnemyLevelScalePlayerLevelFortify;
			}

			void Set_EnemyLevelScalePlayerLevelFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value < 0.0f)
					value = 0.0f;
				if (value > 100.0f)
					value = 100.0f;
				Settings::fortifyPotions._EnemyLevelScalePlayerLevelFortify = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_EnemyNumberThresholdFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fortifyPotions._EnemyNumberThresholdFortify;
			}

			void Set_EnemyNumberThresholdFortify(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				Settings::fortifyPotions._EnemyNumberThresholdFortify = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_UseFortifyPotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fortifyPotions._UseFortifyPotionChance;
			}

			void Set_UseFortifyPotionChance(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 0)
					value = 0;
				if (value > 100)
					value = 100;
				Settings::fortifyPotions._UseFortifyPotionChance = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Food
		{
			bool Get_EnableFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::food._enableFood;
			}

			void Set_EnableFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::food._enableFood = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::food._AllowDetrimentalEffects;
			}

			void Set_AllowDetrimentalEffects(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool allow)
			{
				Settings::food._AllowDetrimentalEffects = allow;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_RestrictFoodToCombatStart(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::food._RestrictFoodToCombatStart;
			}

			void Set_RestrictFoodToCombatStart(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::food._RestrictFoodToCombatStart = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DisableFollowers(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::food._DisableFollowers;
			}

			void Set_DisableFollowers(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::food._DisableFollowers = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::food._DontUseWithWeaponsSheathed;
			}

			void Set_DontUseWithWeaponsSheathed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::food._DontUseWithWeaponsSheathed = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool IsEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				return SettingsAPI::IsEffectProhibited(Settings::ItemType::kFood, value);
			}

			void InvertEffectProhibited(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				SettingsAPI::InvertEffectProhibited(Settings::ItemType::kFood, value);
			}
		}

		namespace Player
		{
			bool Get_PlayerPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._playerPotions;
			}

			void Set_PlayerPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::player._playerPotions = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_PlayerPoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._playerPoisons;
			}

			void Set_PlayerPoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::player._playerPoisons = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_PlayerFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._playerFortifyPotions;
			}

			void Set_PlayerFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::player._playerFortifyPotions = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_PlayerFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._playerFood;
			}

			void Set_PlayerFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::player._playerFood = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_UseFavoritedItemsOnly(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._UseFavoritedItemsOnly;
			}

			void Set_UseFavoritedItemsOnly(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::player._UseFavoritedItemsOnly = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontUseFavoritedItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._DontUseFavoritedItems;
			}

			void Set_DontUseFavoritedItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::player._DontUseFavoritedItems = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontEatRawFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._DontEatRawFood;
			}

			void Set_DontEatRawFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::player._DontEatRawFood = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DontDrinkAlcohol(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::player._DontDrinkAlcohol;
			}

			void Set_DontDrinkAlcohol(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::player._DontDrinkAlcohol = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Distribution
		{
			bool Get_DistributePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._DistributePoisons;
			}

			void Set_DistributePoisons(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::distr._DistributePoisons = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DistributePotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._DistributePotions;
			}

			void Set_DistributePotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::distr._DistributePotions = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DistributeFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._DistributeFortifyPotions;
			}

			void Set_DistributeFortifyPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::distr._DistributeFortifyPotions = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DistributeFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._DistributeFood;
			}

			void Set_DistributeFood(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::distr._DistributeFood = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DistributeCustomItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._DistributeCustomItems;
			}

			void Set_DistributeCustomItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::distr._DistributeCustomItems = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_LevelEasy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._LevelEasy;
			}

			void Set_LevelEasy(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 1)
					value = 1;
				if (value >= Settings::distr._LevelNormal)
					value = Settings::distr._LevelNormal - 1;
				Settings::distr._LevelEasy = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_LevelNormal(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._LevelNormal;
			}

			void Set_LevelNormal(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < Settings::distr._LevelEasy)
					value = Settings::distr._LevelEasy + 1;
				if (value >= Settings::distr._LevelDifficult)
					value = Settings::distr._LevelDifficult - 1;
				Settings::distr._LevelNormal = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_LevelDifficult(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._LevelDifficult;
			}

			void Set_LevelDifficult(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < Settings::distr._LevelNormal)
					value = Settings::distr._LevelNormal + 1;
				if (value >= Settings::distr._LevelInsane)
					value = Settings::distr._LevelInsane - 1;
				Settings::distr._LevelDifficult = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_LevelInsane(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._LevelInsane;
			}

			void Set_LevelInsane(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < Settings::distr._LevelDifficult)
					value = Settings::distr._LevelDifficult + 1;
				if (value > 200)
					value = 200;
				Settings::distr._LevelInsane = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_GameDifficultyScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._GameDifficultyScaling;
			}

			void Set_GameDifficultyScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::distr._GameDifficultyScaling = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_MaxMagnitudeWeak(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._MaxMagnitudeWeak;
			}

			void Set_MaxMagnitudeWeak(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 1)
					value = 1;
				if (value >= Settings::distr._MaxMagnitudeStandard)
					value = Settings::distr._MaxMagnitudeStandard - 1;
				Settings::distr._MaxMagnitudeWeak = value;
				Settings::_updateSettings &= (uint32_t)Settings::UpdateFlag::kMagnitude;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_MaxMagnitudeStandard(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._MaxMagnitudeStandard;
			}

			void Set_MaxMagnitudeStandard(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < Settings::distr._MaxMagnitudeWeak)
					value = Settings::distr._MaxMagnitudeWeak + 1;
				if (value >= Settings::distr._MaxMagnitudePotent)
					value = Settings::distr._MaxMagnitudePotent - 1;
				Settings::distr._MaxMagnitudeStandard = value;
				Settings::_updateSettings &= (uint32_t)Settings::UpdateFlag::kMagnitude;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_MaxMagnitudePotent(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._MaxMagnitudePotent;
			}

			void Set_MaxMagnitudePotent(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < Settings::distr._MaxMagnitudeStandard)
					value = Settings::distr._MaxMagnitudeStandard - 1;
				if (value > 100000)
					value = 100000;
				Settings::distr._MaxMagnitudePotent = value;
				Settings::_updateSettings &= (uint32_t)Settings::UpdateFlag::kMagnitude;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_StyleScalingPrimary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._StyleScalingPrimary;
			}

			void Set_StyleScalingPrimary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value < 0.05f)
					value = 0.05f;
				if (value > 20.0f)
					value = 20.0f;
				Settings::distr._StyleScalingPrimary = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_StyleScalingSecondary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._StyleScalingSecondary;
			}

			void Set_StyleScalingSecondary(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				if (value < 0.05f)
					value = 0.05f;
				if (value > 20.0f)
					value = 20.0f;
				Settings::distr._StyleScalingSecondary = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_ProbabilityScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._ProbabilityScaling;
			}

			void Set_ProbabilityScaling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, float value)
			{
				Settings::distr._ProbabilityScaling = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_DoNotDistributeMixedInvisPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::distr._DoNotDistributeMixedInvisPotions;
			}
			void Set_DoNotDistributeMixedInvisPotions(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool value)
			{
				Settings::distr._DoNotDistributeMixedInvisPotions = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			float Get_ProbabilityAdjuster(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int effect, int itemtype)
			{
				AlchemicEffect eff = AlchemicEffect::GetFromBaseValue(effect);
				switch (itemtype) {
				case (uint32_t)Settings::ItemType::kPotion:
					{
						auto itr = ::Distribution::probabilityAdjustersPotion()->find(eff);
						if (itr != ::Distribution::probabilityAdjustersPotion()->end())
							return itr->second;
					}
					break;
				case (uint32_t)Settings::ItemType::kFortifyPotion:
					{
						auto itr = ::Distribution::probabilityAdjustersFortify()->find(eff);
						if (itr != ::Distribution::probabilityAdjustersFortify()->end())
							return itr->second;
					}
					break;
				case (uint32_t)Settings::ItemType::kPoison:
					{
						auto itr = ::Distribution::probabilityAdjustersPoison()->find(eff);
						if (itr != ::Distribution::probabilityAdjustersPoison()->end())
							return itr->second;
					}
					break;
				case (uint32_t)Settings::ItemType::kFood:
					{
						auto itr = ::Distribution::probabilityAdjustersFood()->find(eff);
						if (itr != ::Distribution::probabilityAdjustersFood()->end())
							return itr->second;
					}
					break;
				}
				return 1.0f;
			}
			void Set_ProbabilityAdjuster(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int effect, int itemtype, float value)
			{
				if (value < 0)
					return;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;

				AlchemicEffect eff = AlchemicEffect::GetFromBaseValue(effect);
				switch (itemtype) {
				case (uint32_t)Settings::ItemType::kPotion:
					{
						if (value == 1.0f)
							::Distribution::probabilityAdjustersPotion()->erase(eff);
						else
							::Distribution::probabilityAdjustersPotion()->insert_or_assign(eff, value);
					}
					break;
				case (uint32_t)Settings::ItemType::kFortifyPotion:
					{
						if (value == 1.0f)
							::Distribution::probabilityAdjustersFortify()->erase(eff);
						else
							::Distribution::probabilityAdjustersFortify()->insert_or_assign(eff, value);
					}
					break;
				case (uint32_t)Settings::ItemType::kPoison:
					{
						if (value == 1.0f)
							::Distribution::probabilityAdjustersPoison()->erase(eff);
						else
							::Distribution::probabilityAdjustersPoison()->insert_or_assign(eff, value);
					}
					break;
				case (uint32_t)Settings::ItemType::kFood:
					{
						if (value == 1.0f)
							::Distribution::probabilityAdjustersFood()->erase(eff);
						else
							::Distribution::probabilityAdjustersFood()->insert_or_assign(eff, value);
					}
					break;
				}
			}
		}

		namespace Removal
		{
			bool Get_RemoveItemsOnDeath(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::removal._RemoveItemsOnDeath;
			}

			void Set_RemoveItemsOnDeath(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::removal._RemoveItemsOnDeath = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_ChanceToRemoveItem(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::removal._ChanceToRemoveItem;
			}

			void Set_ChanceToRemoveItem(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 0)
					value = 0;
				if (value > 100)
					value = 100;
				Settings::removal._ChanceToRemoveItem = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_MaxItemsLeft(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::removal._MaxItemsLeft;
			}

			void Set_MaxItemsLeft(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 0)
					value = 0;
				if (value > 1000)
					value = 1000;
				Settings::removal._MaxItemsLeft = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Whitelist
		{
			bool Get_EnabledItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::whitelist.EnabledItems;
			}

			void Set_EnabledItems(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::whitelist.EnabledItems = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_EnabledNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::whitelist.EnabledNPCs;
			}

			void Set_EnabledNPCs(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::whitelist.EnabledNPCs = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Fixes
		{
			bool Get_ApplySkillBoostPerks(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fixes._ApplySkillBoostPerks;
			}

			void Set_ApplySkillBosstPerks(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::fixes._ApplySkillBoostPerks = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_ForceFixPotionSounds(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::fixes._ForceFixPotionSounds;
			}

			void Set_ForceFixPotionSounds(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::fixes._ForceFixPotionSounds = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Compatibility
		{

			bool Get_DisableCreaturesWithoutRules(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::compatibility._DisableCreaturesWithoutRules;
			}

			void Set_DisableCreaturesWithoutRules(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool disabled)
			{
				Settings::compatibility._DisableCreaturesWithoutRules = disabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_AnimatedPoisons_Enabled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::compatibility.animatedPoisons._Enable;
			}

			void Set_AnimatedPoisons_Enabled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::compatibility.animatedPoisons._Enable = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
				Settings::_updateSettings |= (uint32_t)Settings::UpdateFlag::kCompatibility;
			}

			bool Get_AnimatedPoisons_UsePoisonDosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::compatibility.animatedPoisons._UsePoisonDosage;
			}

			void Set_AnimatedPoisons_UsePoisonDosage(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::compatibility.animatedPoisons._UsePoisonDosage = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_AnimatedPotions_Enabled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::compatibility.animatedPotions._Enable;
			}

			void Set_AnimatedPotions_Enable(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::compatibility.animatedPotions._Enable = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
				Settings::_updateSettings |= (uint32_t)Settings::UpdateFlag::kCompatibility;
			}

			bool CACO_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedCACO();
			}

			bool Apothecary_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedApothecary();
			}

			bool AnimatedPoisons_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedAnimatedPoisons();
			}

			bool AnimatedPotions_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedAnimatedPotions();
			}

			bool ZUPA_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedZUPA();
			}

			bool Sacrosanct_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedSacrosanct();
			}

			bool UltimatePotions_Loaded(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Compatibility::GetSingleton()->LoadedUltimatePotions();
			}
		}

		namespace Debug
		{
			bool Get_EnableLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::debug.EnableLog;
			}

			void Set_EnableLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::debug.EnableLog = enabled;
				Logging::EnableLog = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_EnableLoadLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::debug.EnableLoadLog;
			}

			void Set_EnableLoadLog(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::debug.EnableLoadLog = enabled;
				Logging::EnableLoadLog = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_LogLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::debug.LogLevel;
			}

			void Set_LogLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 0)
					value = 0;
				if (value > 5)
					value = 5;
				Settings::debug.LogLevel = value;
				Logging::LogLevel = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			int Get_ProfileLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::debug.ProfileLevel;
			}

			void Set_ProfileLevel(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, int value)
			{
				if (value < 0)
					value = 0;
				if (value > 5)
					value = 5;
				Settings::debug.ProfileLevel = value;
				Logging::ProfileLevel = value;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}

			bool Get_EnableProfiling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return Settings::debug.EnableProfiling;
			}

			void Set_EnableProfiling(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, bool enabled)
			{
				Settings::debug.EnableProfiling = enabled;
				Logging::EnableProfiling = enabled;
				Settings::_modifiedSettings = Settings::ChangeFlag::kChanged;
			}
		}

		namespace Statistics
		{
			long Get_ActorsSaved(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Storage_ActorsSavedLast;
			}

			long Get_ActorsRead(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Storage_ActorsReadLast;
			}

			long Get_PotionsUsed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Misc_PotionsAdministered;
			}

			long Get_PoisonsUsed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Misc_PoisonsUsed;
			}

			long Get_FoodUsed(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Misc_FoodEaten;
			}

			long Get_EventsHandled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Events_TESHitEvent +
				       ::Statistics::Events_TESCombatEvent +
				       ::Statistics::Events_TESDeathEvent +
				       ::Statistics::Events_BGSActorCellEvent +
				       ::Statistics::Events_TESCellAttachDetachEvent +
				       ::Statistics::Events_TESEquipEvent +
				       ::Statistics::Events_TESFormDeleteEvent +
				       ::Statistics::Events_TESContainerChangedEvent;
			}

			long Get_ActorsHandledTotal(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Misc_ActorsHandledTotal;
			}

			long Get_ActorsHandled(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*)
			{
				return ::Statistics::Misc_ActorsHandled;
			}
		}
	}
}
