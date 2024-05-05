Scriptname NPCsUsePotions_MCM extends SKI_ConfigBase

Event OnConfigInit()
    Pages = new string[11]
    Pages[0] = "General" ; general, system, removal, fixes
    Pages[1] = "Potions"
    Pages[2] = "Poisons"
    Pages[3] = "Fortify Potions"
    Pages[4] = "Food"
    Pages[5] = "Player"
    Pages[6] = "Distribution"
    Pages[7] = "Whitelist"
    Pages[8] = "Compatbility"
    Pages[9] = "Debug"
    Pages[10] = "Statistics"
EndEvent

;event OnGameReload()
;    InitPages()
;endEvent

; ----------- MCM Properties -----------
; general
int G_MaxDuration
int G_MaxFortifyDuration
int G_CycleTime
int G_GlobalCooldown
int G_DisableItemUsageWhileStaggered
int G_DisableNonFollowerNPCs
int G_DisableOutOfCombatProcessing
int G_DisableItemUsageForExcludedNPCs
int G_Remove
int G_RemoveChance
int G_RemoveMaxLeft
int G_ApplySkillBoost
int G_ForceFixSounds
; potions
int PO_EnableMagicka
int PO_EnableStamina
int PO_EnableHealth
int PO_AllowDetrimental
int PO_SheathedAsOOC
int PO_HealthThreshold
int PO_MagickaThreshold
int PO_StaminaThreshold
int PO_Chance
; poisons
int PI_EnablePoisons
int PI_AllowPositive
int PI_WeaponsSheathed
int PI_PoisonResist
int PI_LevelScale
int PI_NumberThreshold
int PI_PoisonChance
int PI_Dosage
; fortify potions
int FO_EnableFortiy
int FO_WeaponsSheathed
int FO_LevelScale
int FO_NumberThreshold
int FO_Chance
; food
int F_EnableFood
int F_AllowDetrimental
int F_RestrictCS
int F_DisableFollowers
int F_WeaponsSheathed
; player
int PL_PlayerPotions
int PL_PlayerPoisons
int PL_PLayerFortify
int PL_PLayerFood
int PL_UseFavoritedOnly
int PL_DontUseFavorited
int PL_DontEatRawFood
int PL_DontDrinkAlcohol
; distribution
int D_Poisons
int D_Potions
int D_Fortify
int D_Food
int D_CustomItems
int D_LevelEasy
int D_LevelNormal
int D_LevelDifficult
int D_Levelnsane
int D_GameDifficulty
int D_MagWeak
int D_MagStandard
int D_MagPotent
int D_StylePrimary
int D_StyleSecondary
int D_Prohib_Menu
int D_Prohib_Menu_Selection = 0 ; [0] = Potions, [1] = Poisons, [2] = Food
string[] D_Prohib_Menu_Options
int[] D_Prohib_options
; whitelist
int W_EnableItems
int W_EnableNPCs
; compatbility
int C_DisableCreatures
int C_PFX_Comp
int C_CACO_Comp
int C_APO_Comp
int C_AnPois_Comp
int C_AnPois_Enable
int C_AnPoti_Comp
int C_AnPoti_Enable
; debug
int D_EnableLog
int D_EnableLoadLog
int D_LogLevel
int D_ProfileLevel
int D_EnableProfiling
; statistics
int S_ActorsSaved
int S_ActorsRead
int S_PotionsUsed
int S_PoisonsUsed
int S_FoodUsed
int S_EventsHandled
int S_ActorsHandledTotal
int S_ActorsHandled

Event OnPageReset(string page)
    Pages = new string[11]
    Pages[0] = "General" ; general, system, removal, fixes
    Pages[1] = "Potions"
    Pages[2] = "Poisons"
    Pages[3] = "Fortify Potions"
    Pages[4] = "Food"
    Pages[5] = "Player"
    Pages[6] = "Distribution"
    Pages[7] = "Whitelist"
    Pages[8] = "Compatbility"
    Pages[9] = "Debug"
    Pages[10] = "Statistics"

    if (page == Pages[0]) ; general
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("Cooldown Options")
        G_GlobalCooldown = AddSliderOption("Global Cooldown", Usage_GetGlobalCooldown())
        AddTextOption("Effective Potion Cooldown", Usage_GetEffectiveGlobalCooldownPotions())
        AddTextOption("Effective Poison Cooldown", Usage_GetEffectiveGlobalCooldownPoisons())
        AddTextOption("Effective Food Cooldown", Usage_GetEffectiveGlobalCooldownFood())
        G_MaxDuration = AddSliderOption("Max Duration for restoration potions", GetMaxDuration())
        G_MaxFortifyDuration = AddSliderOption("Max Duration for fortificaion potions", GetMaxFortifyDuration())
        AddHeaderOption("System")
        G_CycleTime = AddSliderOption("Cycle Time", GetCycleTime())
        SetCursorPosition(1)
        AddHeaderOption("General Options")
        G_DisableNonFollowerNPCs = AddToggleOption("Disable Non-Follower NPCs", Usage_GetDisableNonFollowerNPCs())
        G_DisableOutOfCombatProcessing = AddToggleOption("Disable Out-Of-Combat Health potion usage", Usage_GetDisableOutOfCombatProcessing())
        G_DisableItemUsageWhileStaggered = AddToggleOption("Disable item usage while staggered", Usage_GetDisableItemUsageWhileStaggered())
        G_DisableItemUsageForExcludedNPCs = AddToggleOption("Disable item usage for excluded npcs", Usage_GetDisableItemUsageForExcludedNPCs())

        SetCursorPosition(1)
        AddHeaderOption("Removal Options")
        G_Remove = AddToggleOption("Remove Items on Death", Removal_GetRemoveItemsOnDeath())
        G_RemoveChance = AddSliderOption("Chance to Remove Item",Removal_GetChanceToRemoveItem())
        G_RemoveMaxLeft = AddSliderOption("Max Items Left", Removal_GetMaxItemsLeft())
        AddHeaderOption("Fixes")
        G_ApplySkillBoost = AddToggleOption("Apply Skill Boost Perks", Fixes_GetApplySkillBoostPerks())
        G_ForceFixSounds = AddToggleOption("Force Fix Potion Sounds", Fixes_GetForceFixPotionSounds())
    elseif (page == Pages[1]) ; potions
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        PO_EnableHealth = AddToggleOption("Enable Health Potions", Potions_GetEnableHealthRestoration())
        PO_HealthThreshold = AddSliderOption("Health Threshold", Potions_GetHealthThreshold(), "{2}")
        PO_EnableMagicka = AddToggleOption("Enable Magicka Potions", Potions_GetEnableMagickaRestoration())
        PO_MagickaThreshold = AddSliderOption("Magicka Threshold", Potions_GetMagickaThreshold(), "{2}")
        PO_EnableStamina = AddToggleOption("Enable Stamina Potions", Potions_GetEnableStaminaRestoration())
        PO_StaminaThreshold = AddSliderOption("Stamina Threshold", Potions_GetStaminaThreshold(), "{2}")
        PO_AllowDetrimental = AddToggleOption("Allow Potions with Detrimental Effects", Potions_GetAllowDetrimentalEffects())
        PO_SheathedAsOOC = AddToggleOption("Treat Weapons Sheathed as Out-Of-Combat", Potions_GetHandleWeaponSheathedAsOutOfCombat())
        PO_Chance = AddSliderOption("Chance to use Potions", Potions_GetUsePotionChance())
    elseif (page == Pages[2]) ; poisons
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        PI_EnablePoisons = AddToggleOption("Enable Poisons", Poisons_GetEnablePoisons())
        PI_AllowPositive = AddToggleOption("Allow Positive Effects", Poisons_GetAllowPositiveEffects())
        PI_WeaponsSheathed = AddToggleOption("Don't Use With Weapons Sheathed", Poisons_GetDontUseWithWeaponsSheathed())
        PI_PoisonResist = AddToggleOption("Don't Use Against Enemies with 100 PR", Poisons_GetDontUseAgainst100PoisonResist())
        PI_LevelScale = AddSliderOption("Enemy Level Scale Condition", Poisons_GetEnemyLevelScalePlayerLevel(), "{2}")
        PI_NumberThreshold = AddSliderOption("Enemy Number Condition", Poisons_GetEnemyNumberThreshold())
        PI_PoisonChance = AddSliderOption("Chance to use Poisons", Poisons_GetUsePoisonChance())
        SetCursorPosition(1)
        AddHeaderOption("Dosage")
        PI_Dosage = AddSliderOption("Base Poison Dosage", Poisons_GetDosage())
    elseif (page == Pages[3]) ; fortify potions
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        FO_EnableFortiy = AddToggleOption("Enable Fortify Potions", Fortify_GetEnableFortifyPotions())
        FO_WeaponsSheathed = AddToggleOption("Don't use with Weapons Sheathed", Fortify_GetDontUseWithWeaponsSheathed())
        FO_LevelScale = AddSliderOption("Enemy Level Scale Condition", Fortify_GetEnemyLevelScalePlayerLevelFortify(), "{2}")
        FO_NumberThreshold = AddSliderOption("Enemy Number Condition", Fortify_GetEnemyNumberThresholdFortify())
        FO_Chance = AddSliderOption("Chance to use Fortify Potions", Fortify_GetUseFortifyPotionChance())
    elseif (page == Pages[4]) ; food
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        F_EnableFood = AddToggleOption("Enable Food", Food_GetEnableFood())
        F_AllowDetrimental = AddToggleOption("Allow Food with Detrimental Effects", Food_GetAllowDetrimentalEffects())
        F_RestrictCS = AddToggleOption("Restrict Food Usage to Combat Start", Food_GetRestrictFoodToCombatStart())
        F_DisableFollowers = AddToggleOption("Disable Food for Followers", Food_GetDisableFollowers())
        F_WeaponsSheathed = AddToggleOption("Dont Use Food With Sheathed Weapons", Food_GetDontUseWithWeaponsSheathed())
    elseif (page == Pages[5]) ; player
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("Player Options")
        PL_PlayerPotions = AddToggleOption("Use Potions", Player_GetPlayerPotions())
        PL_PlayerPoisons = AddToggleOption("Use Poisons", Player_GetPlayerPoisons())
        PL_PLayerFortify = AddToggleOption("Use Fortify Potions", Player_GetPlayerFortify())
        PL_PLayerFood = AddToggleOption("Use Food", Player_GetPlayerFood())
        PL_UseFavoritedOnly = AddToggleOption("Only Use Favorited Items", Player_GetUseFavoritedItemsOnly())
        PL_DontUseFavorited = AddToggleOption("Don't Use Favorited Items", Player_GetDontUseFavoritedItems())
        PL_DontEatRawFood = AddToggleOption("Don't Eat Raw Food", Player_GetDontEatRawFood())
        PL_DontDrinkAlcohol = AddToggleOption("Don't Drink Alcoholic Beverages", Player_GetDontDrinkAlcohol())
    elseif (page == Pages[6]) ; distribution
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("General Distribution Options")
        D_Potions = AddToggleOption("Distribute Potions", Distr_GetDistributePotions())
        D_Poisons = AddToggleOption("Distribute Poisons", Distr_GetDistributePoisons())
        D_Fortify = AddToggleOption("Distribute Fortify Potions", Distr_GetDistributeFortify())
        D_Food = AddToggleOption("Distribute Food", Distr_GetDistributeFood())
        D_CustomItems = AddToggleOption("Distribute Custom Items", Distr_GetDistributeCustomItems())
        AddHeaderOption("Dfficulty Options")
        D_GameDifficulty = AddToggleOption("Use Game Difficulty", Distr_GetGameDifficultyScaling())
        D_LevelEasy = AddSliderOption("Level Easy", Distr_GetLevelEasy())
        D_LevelNormal = AddSliderOption("Level Normal", Distr_GetLevelNormal())
        D_LevelDifficult = AddSliderOption("Level Difficult", Distr_GetLevelDifficult())
        D_Levelnsane = AddSliderOption("Level Insane", Distr_GetLevelInsane())
        AddHeaderOption("Item magnitude options")
        D_MagWeak = AddSliderOption("Max magnitude for weak items", Distr_GetMaxMagnitudeWeak())
        D_MagStandard = AddSliderOption("Max magnitude for standard items", Distr_GetMaxMagnitudeStandard())
        D_MagPotent = AddSliderOption("Max magnitude for potent items", Distr_GetMaxMagnitudePotent())
        AddHeaderOption("Style Scaling")
        D_StylePrimary = AddSliderOption("Primary", Distr_GetStyleScalingPrimary(), "{2}")
        D_StyleSecondary = AddSliderOption("Secondary", Distr_GetStyleScalingSecondary(), "{2}")

        SetCursorPosition(1)
        AddHeaderOption("Prohibited Effects")
        D_Prohib_Menu_Options = new string[3]
        D_Prohib_Menu_Options[0] = "Potions"
        D_Prohib_Menu_Options[1] = "Poisons"
        D_Prohib_Menu_Options[2] = "Food"
        D_Prohib_Menu = AddMenuOption("ItemType", "Potions")
        AddTextOption("Effects that can be prohibited", "")
        int i = 1
        D_prohib_options = new int[65]
        while (i < 65)
            D_Prohib_options[i] = AddToggleOption(ToStringAlchemicEffect(i), IsEffectProhibited(i))
            i = i + 1
        endwhile
    elseif (page == Pages[7]) ; whitelist
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("Whitelist Options")
        W_EnableItems = AddToggleOption("Enable Items", Whitelist_GetEnabledItems())
        W_EnableNPCs = AddToggleOption("Enable NPCs", Whitelist_GetEnabledNPCs())
    elseif (page == Pages[8]) ; compatbility
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("General Compatbility Options")
        C_DisableCreatures = AddToggleOption("Disable Creatures Without Rules", Comp_GetDisableCreaturesWhitoutRules())
        AddHeaderOption("Animated Poisons")
        C_AnPois_Enable = AddToggleOption("Enable Animations", Comp_AnimatedPoisons_GetEnabled())
        AddTextOption("Loaded", Comp_AnimatedPoisons_Loaded())
        AddHeaderOption("Animated Potions")
        C_AnPoti_Enable = AddToggleOption("Enable Animations", Comp_AnimatedPotions_GetEnabled())
        AddTextOption("Loaded", Comp_AnimatedPotions_Loaded())
        AddHeaderOption("ZUPA")
        AddTextOption("Loaded", Comp_ZUPA_Loaded())
        AddHeaderOption("Sacrosanct")
        AddTextOption("Loaded", Comp_Sacrosanct_Loaded())
        AddHeaderOption("Ultimate Potions NG")
        AddTextOption("Loaded", Comp_UltimatePotions_Loaded())


        SetCursorPosition(1)
        AddHeaderOption("CACO")
        AddTextOption("Loaded", Comp_CACO_Loaded())
        AddHeaderOption("Apothecary")
        AddTextOption("Loaded", Comp_Apothecary_Loaded())
    elseif (page == Pages[9]) ; debug
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("Debug options")
        D_EnableLog = AddToggleOption("Enable Logging", Debug_GetEnableLog())
        D_EnableLoadLog = AddToggleOption("Enable Load Logging", Debug_GetEnableLoadLog())
        D_LogLevel = AddSliderOption("Log detail level", Debug_GetLogLevel())
        D_EnableProfiling = AddToggleOption("Enable Profiling", Debug_GetEnableProfiling())
        D_ProfileLevel = AddSliderOption("Profiling detail level", Debug_GetProfileLevel())
    elseif (page == Pages[10])
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        S_ActorsSaved = AddTextOption("Actors Saved", Stats_ActorsSaved())
        S_ActorsRead = AddTextOption("Actors Read", Stats_ActorsRead())
        S_PotionsUsed = AddTextOption("Potions Used", Stats_PotionsUsed())
        S_PoisonsUsed = AddTextOption("Poisons Used", Stats_PoisonsUsed())
        S_FoodUsed = AddTextOption("Food Used", Stats_FoodUsed())
        S_EventsHandled = AddTextOption("Events Handled", Stats_EventsHandled())
        S_ActorsHandled = AddTextOption("Actors Handled", Stats_ActorsHandled())
        S_ActorsHandledTotal = AddTextOption("Total Actors Handled", Stats_ActorsHandledTotal())
    endif
EndEvent

Event OnOptionSelect(int option)
    if (option == 0)
    elseif (option == C_DisableCreatures)
        Comp_SetDisableCreaturesWithoutRules(!Comp_GetDisableCreaturesWhitoutRules())
    elseif (option == C_AnPois_Enable)
        Comp_AnimatedPoisons_SetEnabled(!Comp_AnimatedPoisons_GetEnabled())
    elseif (option == C_AnPoti_Enable)
        Comp_AnimatedPotions_SetEnabled(!Comp_AnimatedPotions_GetEnabled())
    elseif (option == W_EnableItems)
        Whitelist_SetEnabledItems(!Whitelist_GetEnabledItems())
    elseif (option == W_EnableNPCs)
        Whitelist_SetEnabledNPCs(!Whitelist_GetEnabledNPCs())
    elseif (option == G_Remove)
        Removal_SetRemoveItemsOnDeath(!Removal_GetRemoveItemsOnDeath())
    elseif (option == G_ApplySkillBoost)
        Fixes_SetApplySkillBoostPerks(!Fixes_GetApplySkillBoostPerks())
    elseif (option == G_ForceFixSounds)
        Fixes_SetForceFixPotionSounds(!Fixes_GetForceFixPotionSounds())
    elseif (option == D_Poisons)
        Distr_SetDistributePoison(!Distr_GetDistributePoisons())
    elseif (option == D_Potions)
        Distr_SetDistributePotions(!Distr_GetDistributePotions())
    elseif (option == D_Fortify)
        Distr_SetDistributeFortify(!Distr_GetDistributeFortify())
    elseif (option == D_Food)
        Distr_SetDistributeFood(!Distr_GetDistributeFood())
    elseif (option == D_CustomItems)
        Distr_SetDistributeCustomItems(!Distr_GetDistributeCustomItems())
    elseif (option == D_GameDifficulty)
        Distr_SetGameDifficultyScaling(!Distr_GetGameDifficultyScaling())
    elseif (option == F_EnableFood)
        Food_SetEnableFood(!Food_GetEnableFood())
    elseif (option == F_AllowDetrimental)
        Food_SetAllowDetrimentalEffects(!Food_GetAllowDetrimentalEffects())
    elseif (option == F_RestrictCS)
        Food_SetRestrictFoodToCombatStart(!Food_GetRestrictFoodToCombatStart())
    elseif (option == F_DisableFollowers)
        Food_SetDisableFollowers(!Food_GetDisableFollowers())
    elseif (option == F_WeaponsSheathed)
        Food_SetDontUseWithWeaponsSheathed(!Food_GetDontUseWithWeaponsSheathed())
    elseif (option == FO_EnableFortiy)
        Fortify_SetEnableFortifyPotions(!Fortify_GetEnableFortifyPotions())
    elseif (option == FO_WeaponsSheathed)
        Fortify_SetDontUseWithWeaponsSheathed(!Fortify_GetDontUseWithWeaponsSheathed())
    elseif (option == PI_EnablePoisons)
        Poisons_SetEnablePoison(!Poisons_GetEnablePoisons())
    elseif (option == PI_AllowPositive)
        Poisons_SetAllowPositiveEffects(!Poisons_GetAllowPositiveEffects())
    elseif (option == PI_WeaponsSheathed)
        Poisons_SetDontUseWithWeaponsSheathed(!Poisons_GetDontUseWithWeaponsSheathed())
    elseif (option == PI_PoisonResist)
        Poisons_SetDontUseAgainst100PoisonResist(!Poisons_GetDontUseAgainst100PoisonResist())
    elseif (option == PO_EnableMagicka)
        Potions_SetEnableMagickaRestoration(!Potions_GetEnableMagickaRestoration())
    elseif (option == PO_EnableStamina)
        Potions_SetEnableStaminaRestoration(!Potions_GetEnableStaminaRestoration())
    elseif (option == PO_EnableHealth)
        Potions_SetEnableHealthRestoration(!Potions_GetEnableHealthRestoration())
    elseif (option == PO_AllowDetrimental)
        Potions_SetAllowDetrimentalEffects(!Potions_GetAllowDetrimentalEffects())
    elseif (option == PO_SheathedAsOOC)
        Potions_SetHandleWeaponSheathedAsOutOfCombat(!Potions_GetHandleWeaponSheathedAsOutOfCombat())
    elseif (option == PL_PlayerPotions)
        Player_SetPlayerPotions(!Player_GetPlayerPotions())
    elseif (option == PL_PlayerPoisons)
        Player_SetPlayerPoisons(!Player_GetPlayerPoisons())
    elseif (option == PL_PLayerFortify)
        Player_SetPlayerFortify(!Player_GetPlayerFortify())
    elseif (option == PL_PLayerFood)
        Player_SetPlayerFood(!Player_GetPlayerFood())
    elseif (option == PL_UseFavoritedOnly)
        Player_SetUseFavoritedItemsOnly(!Player_GetUseFavoritedItemsOnly())
    elseif (option == PL_DontUseFavorited)
        Player_SetDontUseFavoritedItems(!Player_GetDontUseFavoritedItems())
    elseif (option == PL_DontEatRawFood)
        Player_SetDontEatRawFood(!Player_GetDontEatRawFood())
    elseif (option == PL_DontDrinkAlcohol)
        Player_SetDontDrinkAlcohol(!Player_GetDontDrinkAlcohol())
    elseif (option == G_DisableNonFollowerNPCs)
        Usage_SetDisableNonFollowerNPCs(!Usage_GetDisableNonFollowerNPCs())
    elseif (option == G_DisableOutOfCombatProcessing)
        Usage_SetDisableOutOfCombatProcessing(!Usage_GetDisableOutOfCombatProcessing())
    elseif (option == G_DisableItemUsageWhileStaggered)
        Usage_SetDisableItemUsageWhileStaggered(!Usage_GetDisableItemUsageWhileStaggered())
    elseif (option == G_DisableItemUsageForExcludedNPCs)
        Usage_SetDisableItemUsageForExcludedNPCs(!Usage_GetDisableItemUsageForExcludedNPCs())
    elseif (option == D_EnableLog)
        Debug_SetEnableLog(!Debug_GetEnableLog())
    elseif (option == D_EnableLoadLog)
        Debug_SetEnableLoadLog(!Debug_GetEnableLoadLog())
    elseif (option == D_EnableProfiling)
        Debug_SetEnableProfiling(!Debug_GetEnableProfiling())
    endif
    int i = 1
    while (i < 65)
        if (option == D_Prohib_options[i])
            InvertEffectProhibited(i)
        endif
        i = i + 1
    endwhile
    ForcePageReset()
EndEvent

Event OnOptionSliderOpen(int option)
    if (option == 0)
    elseif (option == G_RemoveChance)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Removal_GetChanceToRemoveItem())
        SetSliderDialogRange(0, 100)
        SetSliderDialogInterval(1)
    elseif (option == G_RemoveMaxLeft)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Removal_GetMaxItemsLeft())
        SetSliderDialogRange(0, 200)
        SetSliderDialogInterval(1)
    elseif (option == D_LevelEasy)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetLevelEasy())
        SetSliderDialogRange(1, 200)
        SetSliderDialogInterval(1)
    elseif (option == D_LevelNormal)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetLevelNormal())
        SetSliderDialogRange(1, 200)
        SetSliderDialogInterval(1)
    elseif (option == D_LevelDifficult)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetLevelDifficult())
        SetSliderDialogRange(1, 200)
        SetSliderDialogInterval(1)
    elseif (option == D_Levelnsane)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetLevelInsane())
        SetSliderDialogRange(1, 200)
        SetSliderDialogInterval(1)
    elseif (option == D_MagWeak)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetMaxMagnitudeWeak())
        SetSliderDialogRange(1, 500)
        SetSliderDialogInterval(1)
    elseif (option == D_MagStandard)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetMaxMagnitudeStandard())
        SetSliderDialogRange(1, 500)
        SetSliderDialogInterval(1)
    elseif (option == D_MagPotent)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetMaxMagnitudePotent())
        SetSliderDialogRange(1, 1000)
        SetSliderDialogInterval(1)
    elseif (option == D_StylePrimary)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetStyleScalingPrimary())
        SetSliderDialogRange(0.05, 20.0)
        SetSliderDialogInterval(0.05)
    elseif (option == D_StyleSecondary)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Distr_GetStyleScalingSecondary())
        SetSliderDialogRange(0.05, 20.0)
        SetSliderDialogInterval(0.05)
    elseif (option == FO_LevelScale)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Fortify_GetEnemyLevelScalePlayerLevelFortify())
        SetSliderDialogRange(0, 100)
        SetSliderDialogInterval(0.05)
    elseif (option == FO_NumberThreshold)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Fortify_GetEnemyNumberThresholdFortify())
        SetSliderDialogRange(1, 1000)
        SetSliderDialogInterval(1)
    elseif (option == FO_Chance)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Fortify_GetUseFortifyPotionChance())
        SetSliderDialogRange(0, 100)
        SetSliderDialogInterval(1)
    elseif (option == PI_LevelScale)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Poisons_GetEnemyLevelScalePlayerLevel())
        SetSliderDialogRange(0, 100)
        SetSliderDialogInterval(0.05)
    elseif (option == PI_NumberThreshold)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Poisons_GetEnemyNumberThreshold())
        SetSliderDialogRange(1, 1000)
        SetSliderDialogInterval(1)
    elseif (option == PI_PoisonChance)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Poisons_GetUsePoisonChance())
        SetSliderDialogRange(0, 100)
        SetSliderDialogInterval(1)
    elseif (option == PI_Dosage)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Poisons_GetDosage())
        SetSliderDialogRange(1, 1000)
        SetSliderDialogInterval(1)
    elseif (option == PO_HealthThreshold)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Potions_GetHealthThreshold())
        SetSliderDialogRange(0.05, 0.95)
        SetSliderDialogInterval(0.05)
    elseif (option == PO_MagickaThreshold)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Potions_GetMagickaThreshold())
        SetSliderDialogRange(0.05, 0.95)
        SetSliderDialogInterval(0.05)
    elseif (option == PO_StaminaThreshold)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Potions_GetStaminaThreshold())
        SetSliderDialogRange(0.05, 0.95)
        SetSliderDialogInterval(0.05)
    elseif (option == PO_Chance)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Potions_GetUsePotionChance())
        SetSliderDialogRange(0, 100)
        SetSliderDialogInterval(1)
    elseif (option == G_GlobalCooldown)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Usage_GetGlobalCooldown())
        SetSliderDialogRange(500, 1000000)
        SetSliderDialogInterval(100)
    elseif (option == G_MaxDuration)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(GetMaxDuration())
        SetSliderDialogRange(0, 1000000)
        SetSliderDialogInterval(1000)
    elseif (option == G_MaxFortifyDuration)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(GetMaxFortifyDuration())
        SetSliderDialogRange(0, 1000000)
        SetSliderDialogInterval(1000)
    elseif (option == G_CycleTime)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(GetCycleTime())
        SetSliderDialogRange(100, 10000)
        SetSliderDialogInterval(100)
    elseif (option == D_LogLevel)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Debug_GetLogLevel())
        SetSliderDialogRange(0, 4)
        SetSliderDialogInterval(1)
    elseif (option == D_ProfileLevel)
        SetSliderDialogDefaultValue(0)
        SetSliderDialogStartValue(Debug_GetProfileLevel())
        SetSliderDialogRange(0, 4)
        SetSliderDialogInterval(1)
    endif
EndEvent

Event OnOptionSliderAccept(int option, float value)
    int valueint = value as Int
    if (option == 0)
    elseif (option == G_RemoveChance)
        Removal_SetChanceToRemoveItem(valueint)
    elseif (option == G_RemoveMaxLeft)
        Removal_SetMaxItemsLeft(valueint)
    elseif (option == D_LevelEasy)
        Distr_SetLevelEasy(valueint)
    elseif (option == D_LevelNormal)
        Distr_SetLevelNormal(valueint)
    elseif (option == D_LevelDifficult)
        Distr_SetLevelDifficult(valueint)
    elseif (option == D_Levelnsane)
        Distr_SetLevelInsane(valueint)
    elseif (option == D_MagWeak)
        Distr_SetMaxMagnitudeWeak(valueint)
    elseif (option == D_MagStandard)
        Distr_SetMaxMagnitudeStandard(valueint)
    elseif (option == D_MagPotent)
        Distr_SetMaxMagnitudePotent(valueint)
    elseif (option == D_StylePrimary)
        Distr_SetStyleScalingPrimary(value)
    elseif (option == D_StyleSecondary)
        Distr_SetStyleScalingSecondary(value)
    elseif (option == FO_LevelScale)
        Fortify_SetEnemyLevelScalePlayerLevelFortify(value)
    elseif (option == FO_NumberThreshold)
        Fortify_SetEnemyNumberThresholdFortify(valueint)
    elseif (option == FO_Chance)
        Fortify_SetUseFortifyPotionChance(valueint)
    elseif (option == PI_LevelScale)
        Poisons_SetEnemyLevelScalePlayerLevel(value)
    elseif (option == PI_NumberThreshold)
        Poisons_SetEnemyNumberThreshold(valueint)
    elseif (option == PI_PoisonChance)
        Poisons_SetUsePoisonChance(valueint)
    elseif (option == PI_Dosage)
        Poisons_SetDosage(valueint)
    elseif (option == PO_HealthThreshold)
        Potions_SetHealthThreshold(value)
    elseif (option == PO_MagickaThreshold)
        Potions_SetMagickaThreshold(value)
    elseif (option == PO_StaminaThreshold)
        Potions_SetStaminaThreshold(value)
    elseif (option == PO_Chance)
        Potions_SetUsePotionChance(valueint)
    elseif (option == G_GlobalCooldown)
        Usage_SetGlobalCooldown(valueint)
    elseif (option == G_MaxDuration)
        SetMaxDuration(valueint)
    elseif (option == G_MaxFortifyDuration)
        SetMaxFortifyDuration(valueint)
    elseif (option == G_CycleTime)
        SetCycleTime(valueint)
    elseif (option == D_LogLevel)
        Debug_SetLogLevel(valueint)
    elseif (option == D_ProfileLevel)
        Debug_SetProfileLevel(valueint)
    endif
    ForcePageReset()
EndEvent

Event OnOptionMenuOpen(int option)
    if (option == 0)

    elseif (option == D_Prohib_Menu)
        SetMenuDialogStartIndex(D_Prohib_Menu_Selection)
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(D_Prohib_Menu_Options)
    endif
EndEvent

Event OnOptionMenuAccept(int option, int index)
    if (option == 0)

    elseif (option == D_Prohib_Menu)
        D_Prohib_Menu_Selection = index
    endif
EndEvent

Event OnOptionDefault(int option)
    if (option == 0)
    elseif (option == C_DisableCreatures)
    elseif (option == C_PFX_Comp)
    elseif (option == C_CACO_Comp)
    elseif (option == C_APO_Comp)
    elseif (option == C_AnPois_Comp)
    elseif (option == C_AnPois_Enable)
    elseif (option == C_AnPoti_Comp)
    elseif (option == C_AnPoti_Enable)
    elseif (option == W_EnableItems)
        Whitelist_SetEnabledItems(false)
    elseif (option == W_EnableNPCs)
        Whitelist_SetEnabledNPCs(false)
    elseif (option == G_Remove)
        Removal_SetRemoveItemsOnDeath(true)
    elseif (option == G_RemoveChance)
        Removal_SetChanceToRemoveItem(90)
    elseif (option == G_RemoveMaxLeft)
        Removal_SetMaxItemsLeft(2)
    elseif (option == G_ApplySkillBoost)
        Fixes_SetApplySkillBoostPerks(true)
    elseif (option == G_ForceFixSounds)
        Fixes_SetForceFixPotionSounds(true)
    elseif (option == D_Poisons)
        Distr_SetDistributePoison(true)
    elseif (option == D_Potions)
        Distr_SetDistributePotions(true)
    elseif (option == D_Fortify)
        Distr_SetDistributeFortify(true)
    elseif (option == D_Food)
        Distr_SetDistributeFood(true)
    elseif (option == D_CustomItems)
        Distr_SetDistributeCustomItems(true)
    elseif (option == D_LevelEasy)
        Distr_SetLevelEasy(20)
    elseif (option == D_LevelNormal)
        Distr_SetLevelNormal(35)
    elseif (option == D_LevelDifficult)
        Distr_SetLevelDifficult(50)
    elseif (option == D_Levelnsane)
        Distr_SetLevelInsane(70)
    elseif (option == D_GameDifficulty)
        Distr_SetGameDifficultyScaling(false)
    elseif (option == D_MagWeak)
        Distr_SetMaxMagnitudeWeak(30)
    elseif (option == D_MagStandard)
        Distr_SetMaxMagnitudeStandard(60)
    elseif (option == D_MagPotent)
        Distr_SetMaxMagnitudePotent(150)
    elseif (option == D_StylePrimary)
        Distr_SetStyleScalingPrimary(1.2)
    elseif (option == D_StyleSecondary)
        Distr_SetStyleScalingSecondary(1.1)
    elseif (option == F_EnableFood)
        Food_SetEnableFood(true)
    elseif (option == F_AllowDetrimental)
        Food_SetAllowDetrimentalEffects(false)
    elseif (option == F_RestrictCS)
        Food_SetRestrictFoodToCombatStart(false)
    elseif (option == F_DisableFollowers)
        Food_SetDisableFollowers(false)
    elseif (option == F_WeaponsSheathed)
        Food_SetDontUseWithWeaponsSheathed(false)
    elseif (option == FO_EnableFortiy)
        Fortify_SetEnableFortifyPotions(true)
    elseif (option == FO_WeaponsSheathed)
        Fortify_SetDontUseWithWeaponsSheathed(true)
    elseif (option == FO_LevelScale)
        Fortify_SetEnemyLevelScalePlayerLevelFortify(0.8)
    elseif (option == FO_NumberThreshold)
        Fortify_SetEnemyNumberThresholdFortify(5)
    elseif (option == FO_Chance)
        Fortify_SetUseFortifyPotionChance(100)
    elseif (option == PI_EnablePoisons)
        Poisons_SetEnablePoison(true)
    elseif (option == PI_AllowPositive)
        Poisons_SetAllowPositiveEffects(false)
    elseif (option == PI_WeaponsSheathed)
        Poisons_SetDontUseWithWeaponsSheathed(true)
    elseif (option == PI_PoisonResist)
        Poisons_SetDontUseAgainst100PoisonResist(true)
    elseif (option == PI_LevelScale)
        Poisons_SetEnemyLevelScalePlayerLevel(0.8)
    elseif (option == PI_NumberThreshold)
        Poisons_SetEnemyNumberThreshold(5)
    elseif (option == PI_PoisonChance)
        Poisons_SetUsePoisonChance(100)
    elseif (option == PI_Dosage)
        Poisons_SetDosage(1)
    elseif (option == PO_EnableMagicka)
        Potions_SetEnableMagickaRestoration(true)
    elseif (option == PO_EnableStamina)
        Potions_SetEnableStaminaRestoration(true)
    elseif (option == PO_EnableHealth)
        Potions_SetEnableHealthRestoration(true)
    elseif (option == PO_AllowDetrimental)
        Potions_SetAllowDetrimentalEffects(false)
    elseif (option == PO_SheathedAsOOC)
        Potions_SetHandleWeaponSheathedAsOutOfCombat(true)
    elseif (option == PO_HealthThreshold)
        Potions_SetHealthThreshold(0.5)
    elseif (option == PO_MagickaThreshold)
        Potions_SetMagickaThreshold(0.5)
    elseif (option == PO_StaminaThreshold)
        Potions_SetStaminaThreshold(0.5)
    elseif (option == PO_Chance)
        Potions_SetUsePotionChance(100)
    elseif (option == PL_PlayerPotions)
        Player_SetPlayerPotions(false)
    elseif (option == PL_PlayerPoisons)
        Player_SetPlayerPoisons(false)
    elseif (option == PL_PLayerFortify)
        Player_SetPlayerFortify(false)
    elseif (option == PL_PLayerFood)
        Player_SetPlayerFood(false)
    elseif (option == PL_UseFavoritedOnly)
        Player_SetUseFavoritedItemsOnly(false)
    elseif (option == PL_DontUseFavorited)
        Player_SetDontUseFavoritedItems(false)
    elseif (option == PL_DontEatRawFood)
        Player_SetDontEatRawFood(false)
    elseif (option == PL_DontDrinkAlcohol)
        Player_SetDontDrinkAlcohol(false)
    elseif (option == G_GlobalCooldown)
        Usage_SetGlobalCooldown(1000)
    elseif (option == G_MaxDuration)
        SetMaxDuration(10000)
    elseif (option == G_MaxFortifyDuration)
        SetMaxFortifyDuration(180000)
    elseif (option == G_CycleTime)
        SetCycleTime(1000)
    elseif (option == G_DisableNonFollowerNPCs)
        Usage_SetDisableNonFollowerNPCs(false)
    elseif (option == G_DisableOutOfCombatProcessing)
        Usage_SetDisableOutOfCombatProcessing(false)
    elseif (option == G_DisableItemUsageWhileStaggered)
        Usage_SetDisableItemUsageWhileStaggered(false)
    elseif (option == G_DisableItemUsageForExcludedNPCs)
        Usage_SetDisableItemUsageForExcludedNPCs(false)
    elseif (option == D_EnableLog)
        Debug_SetEnableLog(false)
    elseif (option == D_EnableLoadLog)
        Debug_SetEnableLoadLog(false)
    elseif (option == D_LogLevel)
        Debug_SetLogLevel(0)
    elseif (option == D_EnableProfiling)
        Debug_SetEnableProfiling(false)
    elseif (option == D_ProfileLevel)
        Debug_SetProfileLevel(0)
    endif
    int i = 1
    while (i < 65)
        if (option == D_Prohib_options[i])
            if (IsEffectProhibited(i))
                InvertEffectProhibited(i)
            endif
        endif
        i = i + 1
    endwhile
    ForcePageReset()
EndEvent

Event OnOptionHighlight(int option)
    if (option == 0)
    elseif (option == S_ActorsSaved)
        SetInfoText("The number of actors saved in the last savegame.")
    elseif (option == S_ActorsRead)
        SetInfoText("The number of actors read from the last savegame.")
    elseif (option == S_PotionsUsed)
        SetInfoText("The number of potions used this game session.")
    elseif (option == S_PoisonsUsed)
        SetInfoText("The number of poisons used this game session.")
    elseif (option == S_FoodUsed)
        SetInfoText("The number of food eaten this game session.")
    elseif (option == S_EventsHandled)
        SetInfoText("The number of game events handled this game session.")
    elseif (option == S_ActorsHandled)
        SetInfoText("The number of actors handled in the last cycle.")
    elseif (option == S_ActorsHandledTotal)
        SetInfoText("The number of actors that have been handled this game session.")
    elseif (option == C_DisableCreatures)
        SetInfoText("Disables item distribution and item usage for NPCs that do not not have any distribution rule and have the ActorTypeAnimal or ActorTypeCreature keywords. The Skill Boost Perks are also not distributed to these NPCs. If you experience problems with your game CTDing, try to enable this. Some CTDs are caused by the Skill Boost perks being added to certain creatures. If your game stops CTDing afterwards, the issue was caused by some creature. You can also enable this if you see lots of mod added animals using potions. Many NPCs and creatures are in normal NPC factions (for instance FalmerFaction), and will not be excluded by this setting. Be aware that this may also exclude NPCs that should not be excluded, due to many NPCs being assigned the creature keyword even though they should not have them. Passively disables custom item distribution for those npcs.")
    elseif (option == C_AnPois_Enable)
        SetInfoText("Enables the automatic usage of poison animations for npcs.")
    elseif (option == C_AnPoti_Enable)
        SetInfoText("Enables the automatic usage of potion animations for npcs.")
    elseif (option == W_EnableItems)
        SetInfoText("Enables the whitelist mode for items. Items that shall be distributed must be explicitly whitelisted in the rules. This is the opposite to the standard (blacklist) behaviour.")
    elseif (option == W_EnableNPCs)
        SetInfoText("Enables the whitelist mode for NPCs. NPCs that shall be given potions, etc. and shall use potions, etc. MUST be explicitly whitelisted in the rules. This is the opposite of the standard (blacklist) behaviour.")
    elseif (option == G_Remove)
        SetInfoText("Remove items from NPCs after they died.")
    elseif (option == G_RemoveChance)
        SetInfoText("Chance to remove items on death of NPC. (range: 0 to 100)")
    elseif (option == G_RemoveMaxLeft)
        SetInfoText("Maximum number of items chances are rolled for during removal. Everything that goes above this value is always removed.")
    elseif (option == G_ApplySkillBoost)
        SetInfoText("Distributes the two Perks AlchemySkillBoosts and PerkSkillBoosts to npcs which are needed for fortify etc. potions to apply. [Restart your game to apply changes]")
    elseif (option == G_ForceFixSounds)
        SetInfoText("Forcefully fixes all sounds used by consumables in the game without regard for other mods changes. If deactivate the changes of other mods that should have the same effect are respected. [Restart your game to apply changes]")
    elseif (option == D_Poisons)
        SetInfoText("NPCs are given poisons.")
    elseif (option == D_Potions)
        SetInfoText("NPCs are given potions.")
    elseif (option == D_Fortify)
        SetInfoText("NPCs are given fortify potions.")
    elseif (option == D_Food)
        SetInfoText("NPCs are given food items.")
    elseif (option == D_CustomItems)
        SetInfoText("NPCs are given custom items definable with rules. This does not affect custom potions, poisons, fortify potions and food. They are affected by the above settings.")
    elseif (option == D_LevelEasy)
        SetInfoText("NPC lower or equal this level are considered weak.")
    elseif (option == D_LevelNormal)
        SetInfoText("NPC lower or equal this level are considered normal in terms of strength.")
    elseif (option == D_LevelDifficult)
        SetInfoText("NPC lower or equal this level are considered difficult.")
    elseif (option == D_Levelnsane)
        SetInfoText("NPC lower or equal this level are considered insane. Everything above this is always treated as a boss.")
    elseif (option == D_GameDifficulty)
        SetInfoText("Disables NPC level scaling, but scales distribution according to game difficulty.")
    elseif (option == D_MagWeak)
        SetInfoText("Items with this or lower magnitude*duration are considered weak.")
    elseif (option == D_MagStandard)
        SetInfoText("Items with this or lower magnitude*duration are considered normal.")
    elseif (option == D_MagPotent)
        SetInfoText("Items with this or lower magnitude*duration are considered potent. Everything above this is considered Insane tier.")
    elseif (option == D_StylePrimary)
        SetInfoText("Scaling for the weight of different alchemic effects for the distribution of potions, poison, fortify potions and food according to the primary combat type of an npc.")
    elseif (option == D_StyleSecondary)
        SetInfoText("Scaling for the weight of different alchemic effects for the distribution of potions, poison, fortify potions and food according to the secondary combat type of an npc.")
    elseif (option == F_EnableFood)
        SetInfoText("Allows NPCs to use food items, to gain beneficial effects.")
    elseif (option == F_AllowDetrimental)
        SetInfoText("This allows NPCs to use food that has detrimental effects.")
    elseif (option == F_RestrictCS)
        SetInfoText("NPCs will only eat food at the beginning of combat. This is the way it worked until version 3.0.0 If disabled, NPCs will try to eat more food as soon as an existing food buff runs out.")
    elseif (option == F_DisableFollowers)
        SetInfoText("Disables food usage for followers only. You can use this to prevent your followers from eating food, if you are using survival mods, without impacting other NPCs.")
    elseif (option == F_WeaponsSheathed)
        SetInfoText("When weapons are sheathed food will not be used.")
    elseif (option == FO_EnableFortiy)
        SetInfoText("NPCs use fortify potions in combat. Potions are used based on the equipped weapons and spells.")
    elseif (option == FO_WeaponsSheathed)
        SetInfoText("When weapons are sheathed, no fortify potions will be used.")
    elseif (option == FO_LevelScale)
        SetInfoText("If the enemy they are facing has a level greater equal 'this value' * PlayerLevel followers use fortify potions.")
    elseif (option == FO_NumberThreshold)
        SetInfoText("When the number of NPCs in a fight is at least at this value, followers start to use fortify potions regardless of the enemies level. This includes hostile and non-hostile NPCs.")
    elseif (option == FO_Chance)
        SetInfoText("Chance that an NPC will use a fortify potion if they can. They can use a potion, if they have one in their inventory, and its effects are beneficial to their current choice of weapons. An NPC will not use a Fortify Light Armor potion if they are wearing Heavy Armor, for instance.")
    elseif (option == PI_EnablePoisons)
        SetInfoText("NPCs use appropiate poisons in combat. Poisons are considered appropiate, if they can harm the enemy. For instance, damaging Magicka of an enemy that does not use spells, is not appropiate.")
    elseif (option == PI_AllowPositive)
        SetInfoText("This allows NPCs to use poisons that apply positive effects to their opponents.")
    elseif (option == PI_WeaponsSheathed)
        SetInfoText("If the weapons are sheathed, poisons will not be used.")
    elseif (option == PI_PoisonResist)
        SetInfoText("NPCs and the player will not use Poisons against those that have 100% poison resistance anymore.")
    elseif (option == PI_LevelScale)
        SetInfoText("If the enemy they are facing has a level greater equal 'this value' * PlayerLevel followers use poisons.")
    elseif (option == PI_NumberThreshold)
        SetInfoText("When the number of NPCs in a fight is at least at this value, followers start to use poisons regardless of the enemies level, to faster help out the player. This includes hostile and non-hostile NPCs.")
    elseif (option == PI_PoisonChance)
        SetInfoText("Chance that an NPC will use a poison if they possess one with an \n// appropiate effect.")
    elseif (option == PI_Dosage)
        SetInfoText("// The dosage describes the number of hits a poison lasts on your weapons. The dosage for specific poisons, or alchemic effects can also be defined by distribution rules and may outrule this setting.")
    elseif (option == PO_EnableMagicka)
        SetInfoText("NPCs use magicka potions to restore their missing magicka in combat.")
    elseif (option == PO_EnableStamina)
        SetInfoText("NPCs use stamina potions to restore their missing stamina in combat.")
    elseif (option == PO_EnableHealth)
        SetInfoText("NPCs use health potions to restore their missing hp in combat.")
    elseif (option == PO_AllowDetrimental)
        SetInfoText("If this is enabled NPCs will use potions that contain detrimental effects. For instance, impure potions, that restore health and damage magicka. !!!This setting also affects fortify potions.")
    elseif (option == PO_SheathedAsOOC)
        SetInfoText("")
    elseif (option == PO_HealthThreshold)
        SetInfoText("Upon reaching this threshold, NPCs will start to use health potions")
    elseif (option == PO_MagickaThreshold)
        SetInfoText("Upon reaching this threshold, NPCs will start to use magicka potions")
    elseif (option == PO_StaminaThreshold)
        SetInfoText("Upon reaching this threshold, NPCs will start to use stamina potions")
    elseif (option == PO_Chance)
        SetInfoText("Chance that an NPC will use a potion if they can. Set to 100 to always take a potion, when appropiate. An NPC can use a potion if they (1) have a potion in their inventory, and (2) when the respective value falls below the threshold.")
    elseif (option == PL_PlayerPotions)
        SetInfoText("All activated restoration features are applied to the player, while they are in Combat.")
    elseif (option == PL_PlayerPoisons)
        SetInfoText("Player will automatically use poisons.")
    elseif (option == PL_PLayerFortify)
        SetInfoText("Player will use fortify potions the way followers do.")
    elseif (option == PL_PLayerFood)
        SetInfoText("Player will use food the way npcs do.")
    elseif (option == PL_UseFavoritedOnly)
        SetInfoText("Player will only use items that have been added to the favorites.")
    elseif (option == PL_DontUseFavorited)
        SetInfoText("Player will not use any items that have been added to the favorites. If both UseFavoritedItemsOnly and DontUseFavoritedItems are set to true UseFavoritedItemsOnly = false will be set automatically upon loading the game.")
    elseif (option == PL_DontEatRawFood)
        SetInfoText("If enabled, the player will not consume food with the Keyword VendorItemFoodRaw.")
    elseif (option == PL_DontDrinkAlcohol)
        SetInfoText("If enabled, the player will not consume anything regarded as alcohol")
    elseif (option == G_GlobalCooldown)
        SetInfoText("Cooldown in milliseconds for item usage (potions, poisons, food, etc.). 0 means that items will be used according to the CycleWaitTime defined in [System] (one potion and one poison per cycle)")
    elseif (option == G_MaxDuration)
        SetInfoText("Maximum duration restoration potions (health, magicka, stamina) are accounted for, idependent from their actual duration.")
    elseif (option == G_MaxFortifyDuration)
        SetInfoText("Maximum duration fortification potions are accounted for, idependent from their actual duration.")
    elseif (option == G_CycleTime)
        SetInfoText("Time between two periods in milliseconds. Set to smaller values to increase reactivity. Set to larger values to decrease performance impact.")
    elseif (option == G_DisableNonFollowerNPCs)
        SetInfoText("NPCs that are not currently followers of the player won't use potions, etc.")
    elseif (option == G_DisableOutOfCombatProcessing)
        SetInfoText("PCs are only handled when they are fighting -> Old handling method until version 3. If disabled, NPCs will use Health potions outside of combat. For instance, if they run into traps.")
    elseif (option == G_DisableItemUsageWhileStaggered)
        SetInfoText("NPCs that are staggered, in mid-air, flying, unconcious, bleeding-out, ragdolling or in a kill-move aren't able to use any potions and poisons.")
    elseif (option == G_DisableItemUsageForExcludedNPCs)
        SetInfoText("NPCs that are excluded from item distribution, will not use any Potions, Fortifypotions, Poisons or Food from any other in-game source.")
    elseif (option == D_EnableLog)
        SetInfoText("Enables logging output. Use with care as logs may get very large.")
    elseif (option == D_EnableLoadLog)
        SetInfoText("Enables logging output for plugin load, use if you want to log rule issues")
    elseif (option == D_LogLevel)
        SetInfoText("0 - layer 0 log entries, 1 - layer 1 log entries, 2 - layer 3 log entries, 3 - layer 4 log entries. Affects which functions write log entries, as well as what is written by those functions.")
    elseif (option == D_EnableProfiling)
        SetInfoText("Enables profiling output.")
    elseif (option == D_ProfileLevel)
        SetInfoText("0 - only highest level functions write their executions times to the profile log, 1 - lower level functions are written, 2 - lowest level functions are written. Be aware that not all functions are supported as Profiling costs execution time.")
    else
        SetInfoText("")
    endif
EndEvent











bool Function IsEffectProhibited(int value)
    if (D_Prohib_Menu_Selection == 0)
        return Potions_IsEffectProhibited(value)
    elseif (D_Prohib_Menu_Selection == 1)
        return Poisons_IsEffectProhibited(value)
    elseif (D_Prohib_Menu_Selection == 2)
        return Food_IsEffectProhibited(value)
    endif
EndFunction

Function InvertEffectProhibited(int value)
    if (D_Prohib_Menu_Selection == 0)
        Potions_InvertEffectProhibited(value)
    elseif (D_Prohib_Menu_Selection == 1)
        Poisons_InvertEffectProhibited(value)
    elseif (D_Prohib_Menu_Selection == 2)
        Food_InvertEffectProhibited(value)
    endif
endfunction


; ------------------ GLOBAL ------------------

; --------- general ---------

; returns the max duration accounted for for restoration potions
int Function GetMaxDuration() global native
; sets the max durartion accounted for for restoration potions
Function SetMaxDuration(int milliseconds) global native

; returns the max duration accounted for for fortification potions
int Function GetMaxFortifyDuration() global native
; sets the max duration accounted for for fortification potions
Function SetMaxFortifyDuration(int milliseconds) global native

string Function ToStringAlchemicEffect(int value) global native

; --------- System ---------

; returns the current time between cycles in milliseconds
int Function GetCycleTime() global native
; sets the time between cycles in milliseconds
Function SetCycleTime(int milliseconds) global native

; --------- Usage ---------

; returns the global coolwon applied to potion, poison, and food usage in milliseconds
int Function Usage_GetGlobalCooldown() global native
; sets the global cooldown applied to potion, poison, and food usage in milliseconds
Function Usage_SetGlobalCooldown(int milliseconds) global native
; returns the effective global cooldown applied to potions, considering mod compatibility
int Function Usage_GetEffectiveGlobalCooldownPotions() global native
; returns the effective global cooldown applied to poisons, considering mod compatibility
int Function Usage_GetEffectiveGlobalCooldownPoisons() global native
; returns the effective global cooldown applied to food, considering mod compatibility
int Function Usage_GetEffectiveGlobalCooldownFood() global native

; returns whether staggered npcs should not use items
bool Function Usage_GetDisableItemUsageWhileStaggered() global native
; sets whether staggered npcs should not use items
Function Usage_SetDisableItemUsageWhileStaggered(bool disable) global native

; returns whether processing of NPCs that are not active followers of the player is disabled
bool Function Usage_GetDisableNonFollowerNPCs() global native
; sets whether processing of NPCs that are not active follower of the player is disabled
Function Usage_SetDisableNonFollowerNPCs(bool disabled) global native

; returns whether out-of-combat processing is disabled
bool Function Usage_GetDisableOutOfCombatProcessing() global native
; sets whether out-of-combat processing is disabled
Function Usage_SetDisableOutOfCombatProcessing(bool disabled) global native

; returns whether item usage is disabled for npcs that are excluded from item distribution
bool Function Usage_GetDisableItemUsageForExcludedNPCs() global native
; sets whether item usage is disabled for npcs that are excluded from iten distribution
Function Usage_SetDisableItemUsageForExcludedNPCs(bool disabled) global native

; --------- Potions ---------

; Returns whether the suage of magicka potions is enabled
bool Function Potions_GetEnableMagickaRestoration() global native
; Sets whether the usage of magicka potions is enabled
Function Potions_SetEnableMagickaRestoration(bool enabled) global native

; Returns whether the usage of stamina potions is enabled
bool Function Potions_GetEnableStaminaRestoration() global native
; Sets whether the usage of stamina potions is enabled
Function Potions_SetEnableStaminaRestoration(bool enabled) global native

; Returns whether the usage of health potions is enabled
bool Function Potions_GetEnableHealthRestoration() global native
; Sets whether the usage of health potions is enabled
Function Potions_SetEnableHealthRestoration(bool enabled) global native

; Returns whether the usage of potions with negative effects is allowed
bool Function Potions_GetAllowDetrimentalEffects() global native
; Sets whether the usage of potions with negative effects is allowed
Function Potions_SetAllowDetrimentalEffects(bool allow) global native

; Returns whether NPCs that have their weapons sheathed should be treated as out-of-combat
bool Function Potions_GetHandleWeaponSheathedAsOutOfCombat() global native
; Sets whether NPCs that have their weapons sheathed should be treated as out-of-combat
Function Potions_SetHandleWeaponSheathedAsOutOfCombat(bool enabled) global native

; Returns the threshold for the usage of health potions
float Function Potions_GetHealthThreshold() global native
; Sets the threshold for the usage of health potions
Function Potions_SetHealthThreshold(float value) global native

; Returns the threshold for the usage of magicka potions
float Function Potions_GetMagickaThreshold() global native
; Sets the threshold for the usage of magicka potions
Function Potions_SetMagickaThreshold(float value) global native

; Returns the threshold for the usage of stamina potions
float Function Potions_GetStaminaThreshold() global native
; Sets the threshold for the usage of stamina potions
Function Potions_SetStaminaThreshold(float value) global native

; Returns the chance that an NPC will use a potion if they are able to
int Function Potions_GetUsePotionChance() global native
; Sets the chance that an NPC will use a potion if they are able to
Function Potions_SetUsePotionChance(int value) global native

; Returns whether the given AlchemicEffect has been prohibited
bool Function Potions_IsEffectProhibited(int value) global native
; Inverts the current prohibition status of the given alchemic effect
Function Potions_InvertEffectProhibited(int value) global native

; --------- Poisons ---------

; Returns whether the usage of poison is enabled
bool Function Poisons_GetEnablePoisons() global native
; Sets whether the usage of poisons is enabled
Function Poisons_SetEnablePoison(bool enabled) global native

; Returns whether poisons with positive effects may be used
bool Function Poisons_GetAllowPositiveEffects() global native
; Sets whther poisons with positive effects may be used
Function Poisons_SetAllowPositiveEffects(bool allow) global native

; Returns whether the usage of poisons is disabled while weapons are sheathed
bool Function Poisons_GetDontUseWithWeaponsSheathed() global native
;  Sets whether the usage of poisons is disabled while weapons are sheathed
Function Poisons_SetDontUseWithWeaponsSheathed(bool disabled) global native

; Returns whether NPCs should not use poisons when facing an enemy with 100 poison resistance
bool Function Poisons_GetDontUseAgainst100PoisonResist() global native
; Sets whether NPCs should not use poisons when facing an enemy with 100 poison resistance
Function Poisons_SetDontUseAgainst100PoisonResist(bool disabled) global native

; Returns the level scale of enemies in regard to the player, at which the player begins using poisons [or relationship with EnemyNumberThreshold]
float Function Poisons_GetEnemyLevelScalePlayerLevel() global native
; Sets the level scale of enemies in regard to the player, at which the player begins using poisons [or relationship with EnemyNumberThreshold]
Function Poisons_SetEnemyLevelScalePlayerLevel(float value) global native

; Returns the number of enemies that have to participate in a fight, for the player to start using poisons [or relationship with EnemyLevelScalePlayerLevel]
int Function Poisons_GetEnemyNumberThreshold() global native
; Sets the number of enemies that have to participate in a fight, for the player to start using poisons [or relationship with EnemyLevelScalePlayerLevel]
Function Poisons_SetEnemyNumberThreshold(int value) global native

; Returns the chance that an NPC will use a poison if they can
int Function Poisons_GetUsePoisonChance() global native
; Sets the chance that an NPC will use a poison if they can
Function Poisons_SetUsePoisonChance(int value) global native

; Returns the base dosage applied to all poisons
int Function Poisons_GetDosage() global native
; Sets the base dosage applied to all poisons
Function Poisons_SetDosage(int value) global native

; Returns whether the given AlchemicEffect has been prohibited
bool Function Poisons_IsEffectProhibited(int value) global native
; Inverts the current prohibition status of the given alchemic effect
Function Poisons_InvertEffectProhibited(int value) global native

; --------- Fortification Potions ---------

; Returns whether the usage of fortify potions is enabled
bool Function Fortify_GetEnableFortifyPotions() global native
; Sets whether the usage of fortify potions is enabled
Function Fortify_SetEnableFortifyPotions(bool enabled) global native

; Returns whether fortification potions whould not be used when weapons are sheathed
bool Function Fortify_GetDontUseWithWeaponsSheathed() global native
; Sets whether fortification potions should not be used when weapons are sheathed
Function Fortify_SetDontUseWithWeaponsSheathed(bool disabled) global native

; Returns the level scale of enemies in regard to the player, at which the player begins using fortification potions [or relationship with EnemyNumberThreshold]
float Function Fortify_GetEnemyLevelScalePlayerLevelFortify() global native
; Sets the level scale of enemies in regard to the player, at which the player begins using fortification potions [or relationship with EnemyNumberThreshold]
Function Fortify_SetEnemyLevelScalePlayerLevelFortify(float value) global native

; Returns the number of enemies that have to participate in a fight, for the player to start using fortification potions [or relationship with EnemyLevelScalePlayerLevel]
int Function Fortify_GetEnemyNumberThresholdFortify() global native
; Sets the number of enemies that have to participate in a fight, for the player to start using fortification potions [or relationship with EnemyLevelScalePlayerLevel]
Function Fortify_SetEnemyNumberThresholdFortify(int value) global native

; Returns the chance for an NPC to use a fortification potion when they may do so
int Function Fortify_GetUseFortifyPotionChance() global native
; Sets the chance for an NPC to use a fortification potion when they may do so
Function Fortify_SetUseFortifyPotionChance(int value) global native

; --------- Food ---------

; Returns whether the usage of food is enabled
bool Function Food_GetEnableFood() global native
; Sets whether the usage of food is enabled
Function Food_SetEnableFood(bool value) global native

; Returns whether the usage of food with detrimental effects is allowed
bool Function Food_GetAllowDetrimentalEffects() global native
; Sets whether the usage of food with detrimental effects is allowed
Function Food_SetAllowDetrimentalEffects(bool allow) global native

; Returns whether to restrict food usage to the beginning of a fight
bool Function Food_GetRestrictFoodToCombatStart() global native
; Sets whether to restrict food usage to the beginning of a fight
Function Food_SetRestrictFoodToCombatStart(bool enabled) global native

; Returns whether food usage is disabled for followers
bool Function Food_GetDisableFollowers() global native
; Sets whether food usage is disabled for followers
Function Food_SetDisableFollowers(bool disabled) global native

; Returns whether food should not be used while weapons are sheathed
bool Function Food_GetDontUseWithWeaponsSheathed() global native
; Sets whether food should not be used while weapons are sheathed
Function Food_SetDontUseWithWeaponsSheathed(bool disabled) global native

; Returns whether the given AlchemicEffect has been prohibited
bool Function Food_IsEffectProhibited(int value) global native
; Inverts the current prohibition status of the given alchemic effect
Function Food_InvertEffectProhibited(int value) global native

; --------- Player ---------

; Returns whether player potion usage is enabled
bool Function Player_GetPlayerPotions() global native
; Sets whether player potion usage is enabled
Function Player_SetPlayerPotions(bool enabled) global native

; Returns whether player poison usage is enabled
bool Function Player_GetPlayerPoisons() global native
; Sets whether player poison usage is enabled
Function Player_SetPlayerPoisons(bool enabled) global native

; Returns whether player fortify potion usage is enabled
bool Function Player_GetPlayerFortify() global native
; Sets whether player fortify potion usage is enabled
Function Player_SetPlayerFortify(bool enabled) global native

; Returns whether player food usage is enabled
bool Function Player_GetPlayerFood() global native
; Sets whether player food usage is enabled
Function Player_SetPlayerFood(bool enabled) global native

; Returns whether only favorite items may be used by the player
bool Function Player_GetUseFavoritedItemsOnly() global native
; Sets whether only favorite items may be used by the player
Function Player_SetUseFavoritedItemsOnly(bool enabled) global native

; Returns whether favorite items should not be used by player
bool Function Player_GetDontUseFavoritedItems() global native
; Sets whther favorite items should not be used by player
Function Player_SetDontUseFavoritedItems(bool disabled) global native

; Returns whether player won't eat raw food
bool Function Player_GetDontEatRawFood() global native
; Sets whether player won't eat raw food
Function Player_SetDontEatRawFood(bool disabled) global native

; Returns whether the player won't drink alcohol
bool Function Player_GetDontDrinkAlcohol() global native
; Sets whether the player won't drink alcohol
Function Player_SetDontDrinkAlcohol(bool disabled) global native

; --------- Distribution ---------

; Returns whether poison distribution is enabled
bool Function Distr_GetDistributePoisons() global native
; Sets whether poison distribution is enabled
Function Distr_SetDistributePoison(bool enabled) global native

; Returns whether potion distribution is enabled
bool Function Distr_GetDistributePotions() global native
; Sets whether potion distribution is enabled
Function Distr_SetDistributePotions(bool enabled) global native

; Returns whether fortify potion distribution is enabled
bool Function Distr_GetDistributeFortify() global native
; Sets whether fortify potion distribution is enabled
Function Distr_SetDistributeFortify(bool enabled) global native

; Returns whether food distribution is enabled
bool Function Distr_GetDistributeFood() global native
; Sets whether food distribution is enabled
Function Distr_SetDistributeFood(bool enabled) global native

; Returns whether CustomItem distribution is enabled
bool Function Distr_GetDistributeCustomItems() global native
; Sets whether CustomItem distribution is enabled
Function Distr_SetDistributeCustomItems(bool enabled) global native

; Returns the maximum level for easy NPCs
int Function Distr_GetLevelEasy() global native
; Sets the maximum level for easy NPCs
Function Distr_SetLevelEasy(int value) global native

; Returns the maximum level for normal NPCs
int Function Distr_GetLevelNormal() global native
; Sets the maximum level for normal NPCs
Function Distr_SetLevelNormal(int value) global native

; Returns the maximum level for difficult NPCs
int Function Distr_GetLevelDifficult() global native
; Sets the maximum level for difficult NPCs
Function Distr_SetLevelDifficult(int value) global native

; Returns the maximum level for Insane NPCs
int Function Distr_GetLevelInsane() global native
; Sets the maximum level for Insane NPCs, NPCs above this level are considered boss NPCs
Function Distr_SetLevelInsane(int value) global native

; Returns whether NPC difficulty is tied to Game difficulty
bool Function Distr_GetGameDifficultyScaling() global native
; Sets whether NPC difficulty is tied to Game difficulty
Function Distr_SetGameDifficultyScaling(bool enabled) global native

; Returns the maximum magnitude for weak potions
int Function Distr_GetMaxMagnitudeWeak() global native
; Sets the maximum magnitude for weak potions
Function Distr_SetMaxMagnitudeWeak(int value) global native

; Returns the maximum magnitude for standard potions
int Function Distr_GetMaxMagnitudeStandard() global native
; Sets the maximum magnitude for standard potions
Function Distr_SetMaxMagnitudeStandard(int value) global native

; Returns the maximum magnitude for potent potions
int Function Distr_GetMaxMagnitudePotent() global native
; Sets the maximum magnitude for potent potions
Function Distr_SetMaxMagnitudePotent(int value) global native

; Returns the scale of the distribution probability for items matching the combat styles primary combat modifiers
float Function Distr_GetStyleScalingPrimary() global native
; Sets the scale of the distribution probability for items matching the combat styles primary combat modifiers
Function Distr_SetStyleScalingPrimary(float value) global native

; Retruns the scale of the distribution probability for items matching the combat styles secondary combat modifiers
float Function Distr_GetStyleScalingSecondary() global native
; Sets the scale of the distribution probability for items matching the combat styles secondary combat modifiers
Function Distr_SetStyleScalingSecondary(float value) global native

; --------- Removal ---------

; Returns whether alchemy items are removed from NPCs inventory on death
bool Function Removal_GetRemoveItemsOnDeath() global native
; Sets whether alchemy items are removed from NPCs inventory on death
Function Removal_SetRemoveItemsOnDeath(bool enabled) global native

; Returns the chance for an item to be removed [calculated for all items individually]
int Function Removal_GetChanceToRemoveItem() global native
; Sets the chance for an item to be removed [calculated for all items individually]
Function Removal_SetChanceToRemoveItem(int value) global native

; Returns the maximum amount of items that may remain in NPCs inventory
int Function Removal_GetMaxItemsLeft() global native
; Sets the maximum amount of items that may remain in NPCs inventory
Function Removal_SetMaxItemsLeft(int value) global native

; --------- Whitelist ---------

; Returns whether whitelist is enabled for Items [not whitelisted items will not be distributed and used]
bool Function Whitelist_GetEnabledItems() global native
; Sets whether whitelist is enabled for items [not whitelisted items will not be distributed and used]
Function Whitelist_SetEnabledItems(bool enabled) global native

; Returns whether whitelist is enabled for NPCs [not whitelisted NPCs will not get and use items]
bool Function Whitelist_GetEnabledNPCs() global native
; Sets whether whitelist is enabled for NPCs [not whitelisted NPCs will not get and use items]
Function Whitelist_SetEnabledNPCs(bool enabled) global native

; --------- Fixes ---------

; Returns whether Skill Boost Perks are distributed to all elligible NPCs
bool Function Fixes_GetApplySkillBoostPerks() global native
; Sets whether Skill Boost Perks are distributed to all elligible NPCs [requires Restart]
Function Fixes_SetApplySkillBoostPerks(bool enabled) global native

; Returns whether the sounds of potions, poisons, and food will be fixed by modifying the Records on game load
bool Function Fixes_GetForceFixPotionSounds() global native
; Sets whether the sounds of potions, poisons, and food will be fixed by modifying the Records on game load [requires Restart]
Function Fixes_SetForceFixPotionSounds(bool enabled) global native

; --------- Compatibility ---------

; Returns whether all npcs that are in the ActorTypeCreature and/or ActorTypeAnimal faction and do not have a dedicated rule are removed from item distribution, item usage, and perk distribution
bool Function Comp_GetDisableCreaturesWhitoutRules() global native
; Sets whether all npcs that are in the ActorTypeCreature and/or ActorTypeAnimal faction and do not have a dedicated rule are removed from item distrubtion, item usage, and perk distribution
Function Comp_SetDisableCreaturesWithoutRules(bool enabled) global native

; Returns whether Animated Poisons animations should be used by NPCs
bool Function Comp_AnimatedPoisons_GetEnabled() global native
; Sets whether Animated Poisons animations should be used by NPCs
Function Comp_AnimatedPoisons_SetEnabled(bool enabled) global native

; Returns whether the poison dosage system used by Animated Poisons should be preferred over the built-in system [removed in Animated Poisons 5]
bool Function Comp_AnimatedPoisons_GetUsePoisonDosage() global native
; Sets whether the poison dosage system used by Animated Poisons should be preferred over the built-in system [removed in Animated Poisons 5]
Function Comp_AnimatedPoisons_SetUsePoisonDosage(bool enabled) global native

; Returns whether Animated Potions animations should be used by NPCs
bool Function Comp_AnimatedPotions_GetEnabled() global native
; Sets whether Animated Potions animations should be used by NPCs
Function Comp_AnimatedPotions_SetEnabled(bool enabled) global native

; Returns whether CACO is loaded
bool Function Comp_CACO_Loaded() global native
; Returns whether Apothecary is loaded
bool Function Comp_Apothecary_Loaded() global native
; Returns whether Animated Poisons is loaded
bool Function Comp_AnimatedPoisons_Loaded() global native
; Returns whether Animated Potions is loaded
bool Function Comp_AnimatedPotions_Loaded() global native
; Returns whether ZUPA is loaded
bool Function Comp_ZUPA_Loaded() global native
; Returns whetehr Sacrosanct is loaded
bool Function Comp_Sacrosanct_Loaded() global native
; Returns whether Ultimate Potions [AE] is loaded
bool Function Comp_UltimatePotions_Loaded() global native

; --------- Debug ---------

; Returns whether logging is enabled
bool Function Debug_GetEnableLog() global native
; Sets whether logging is enabled
Function Debug_SetEnableLog(bool enabled) global native

; Returns whether logging during plugin load is enabled
bool Function Debug_GetEnableLoadLog() global native
; Sets whether logging during plugin load is enabled
Function Debug_SetEnableLoadLog(bool enabled) global native

; Returns the detail of logging
int Function Debug_GetLogLevel() global native
; Sets the detail of logging
Function Debug_SetLogLevel(int value) global native

; Returns the detail of profiling
int Function Debug_GetProfileLevel() global native
; Sets the detail of profiling
Function Debug_SetProfileLevel(int value) global native

; Returns whether profiling is enabled
bool Function Debug_GetEnableProfiling() global native
; Sets whether profiling is enabled
Function Debug_SetEnableProfiling(bool enabled) global native

; --------- Statistics  ---------

; Returns the number of actors saved in the last savegame
int Function Stats_ActorsSaved() global native

; Returns the number of actors read from the last savegame
int Function Stats_ActorsRead() global native

; Returns the number of potions used this game session
int Function Stats_PotionsUsed() global native

; Returns the number of poisons used this game session
int Function Stats_PoisonsUsed() global native

; Returns the number of food eaten this game session
int Function Stats_FoodUsed() global native

; Returns the number of game events handled this game session
int Function Stats_EventsHandled() global native

; Returns the number of actors that have been handled this game session
int Function Stats_ActorsHandledTotal() global native

; Returns the number of actors handled in the last cycle
int Function Stats_ActorsHandled() global native
