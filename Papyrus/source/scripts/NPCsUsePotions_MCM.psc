Scriptname NPCsUsePotions_MCM extends SKI_ConfigBase

Event OnConfigInit()
    Pages = new string[11]
    Pages[0] = "$NUP_PaGeneral" ; general, system, removal, fixes
    Pages[1] = "$NUP_PaPotions"
    Pages[2] = "$NUP_PaPoisons"
    Pages[3] = "$NUP_PaFortifyPotions"
    Pages[4] = "$NUP_PaFood"
    Pages[5] = "$NUP_PaPlayer"
    Pages[6] = "$NUP_PaDistribution"
    Pages[7] = "$NUP_PaWhitelist"
    Pages[8] = "$NUP_PaCompatibility"
    Pages[9] = "$NUP_PaDebug"
    Pages[10] = "$NUP_PaStatistics"
EndEvent

;event OnGameReload()
;    InitPages()
;endEvent

int ENUMPotion = 2
int ENUMFortify = 8
int ENUMPoison = 1
int ENUMFood = 4

; ----------- MCM Properties -----------
; general
int G_MaxDuration
int G_MaxFortifyDuration
int G_CycleTime
int G_GlobalCooldown
int G_DisableItemUsageWhileStaggered
int G_DisableItemUsageWhileFlying
int G_DisableItemUsageWhileBleedingOut
int G_DisableItemUsageWhileSleeping
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

int PO_Prob_Add
int PO_Prob_Selec = 0
int[] PO_Prob_Act
int[] PO_Prob_Ind
int PO_Prob_act_maxindex
; poisons
int PI_EnablePoisons
int PI_AllowPositive
int PI_WeaponsSheathed
int PI_PoisonResist
int PI_LevelScale
int PI_NumberThreshold
int PI_PoisonChance
int PI_Dosage

int PI_Prob_Add
int PI_Prob_Selec = 0
int[] PI_Prob_Act
int[] PI_Prob_Ind
int PI_Prob_act_maxindex
; fortify potions
int FO_EnableFortiy
int FO_WeaponsSheathed
int FO_LevelScale
int FO_NumberThreshold
int FO_Chance

int FO_Prob_Add
int FO_Prob_Selec = 0
int[] FO_Prob_Act
int[] FO_Prob_Ind
int FO_Prob_act_maxindex
; food
int F_EnableFood
int F_AllowDetrimental
int F_RestrictCS
int F_DisableFollowers
int F_WeaponsSheathed

int F_Prob_Add
int F_Prob_Selec = 0
int[] F_Prob_Act
int[] F_Prob_Ind
int F_Prob_act_maxindex
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
int D_ProbScaling
int D_DoNotMixed
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


string[] EffectOptionsPotion
int[] EffectOptionsPotionIndex
string[] EffectOptions

Event OnPageReset(string page)
    Pages = new string[11]
    Pages[0] = "$NUP_PaGeneral" ; general, system, removal, fixes
    Pages[1] = "$NUP_PaPotions"
    Pages[2] = "$NUP_PaPoisons"
    Pages[3] = "$NUP_PaFortifyPotions"
    Pages[4] = "$NUP_PaFood"
    Pages[5] = "$NUP_PaPlayer"
    Pages[6] = "$NUP_PaDistribution"
    Pages[7] = "$NUP_PaWhitelist"
    Pages[8] = "$NUP_PaCompatibility"
    Pages[9] = "$NUP_PaDebug"
    Pages[10] = "$NUP_PaStatistics"

    ENUMPotion = 2
    ENUMFortify = 8
    ENUMPoison = 1
    ENUMFood = 4

    EffectOptions = new string[64]
    int ii = 0
    int iden = 1
    while (ii < 64)
        EffectOptions[ii] = ToStringAlchemicEffect(ii + 1)
        ii = ii + 1
    endwhile

    EffectOptionsPotion = new string[5]
    EffectOptionsPotionIndex = new int[5]
    EffectOptionsPotion[0] = ToStringAlchemicEffect(1) ; Health
    EffectOptionsPotionIndex[0] = 1
    EffectOptionsPotion[1] = ToStringAlchemicEffect(2) ; Magicka
    EffectOptionsPotionIndex[1] = 2
    EffectOptionsPotion[2] = ToStringAlchemicEffect(3) ; Stamina
    EffectOptionsPotionIndex[2] = 3
    EffectOptionsPotion[3] = ToStringAlchemicEffect(45) ; CurePoison
    EffectOptionsPotionIndex[3] = 45
    EffectOptionsPotion[4] = ToStringAlchemicEffect(30) ; Invisibility
    EffectOptionsPotionIndex[4] = 30

    PO_Prob_Act = new int[64]
    PO_Prob_Ind = new int[64]
    PO_Prob_act_maxindex = 0
    PI_Prob_Act = new int[64]
    PI_Prob_Ind = new int[64]
    PI_Prob_act_maxindex = 0
    FO_Prob_Act = new int[64]
    FO_Prob_Ind = new int[64]
    FO_Prob_act_maxindex = 0
    F_Prob_Act = new int[64]
    F_Prob_Ind = new int[64]
    F_Prob_act_maxindex = 0

    if (page == Pages[0]) ; general
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HCooldownOption")
        G_GlobalCooldown = AddSliderOption("$NUP_SGlobalCooldown", Usage_GetGlobalCooldown())
        AddTextOption("$NUP_TEffectivePotionCooldown", Usage_GetEffectiveGlobalCooldownPotions())
        AddTextOption("$NUP_TEffectivePoisonCooldown", Usage_GetEffectiveGlobalCooldownPoisons())
        AddTextOption("$NUP_TEffectiveFoodCooldown", Usage_GetEffectiveGlobalCooldownFood())
        G_MaxDuration = AddSliderOption("$NUP_SMaxDurationForRestPotions", GetMaxDuration())
        G_MaxFortifyDuration = AddSliderOption("$NUP_SMaxDurationForFortPotions", GetMaxFortifyDuration())
        AddHeaderOption("$NUP_HSystem")
        G_CycleTime = AddSliderOption("$NUP_SCycleTime", GetCycleTime())
        SetCursorPosition(1)
        AddHeaderOption("$NUP_HGeneralOptions")
        G_DisableNonFollowerNPCs = AddToggleOption("$NUP_CDisableNonFollowerNPCs", Usage_GetDisableNonFollowerNPCs())
        G_DisableOutOfCombatProcessing = AddToggleOption("$NUP_CDisableOOCHealth", Usage_GetDisableOutOfCombatProcessing())
        G_DisableItemUsageWhileStaggered = AddToggleOption("$NUP_CDisableItemStaggered", Usage_GetDisableItemUsageWhileStaggered())
        G_DisableItemUsageWhileFlying = AddToggleOption("$NUP_CDisableItemFlying", Usage_GetDisableItemUsageWhileFlying())
        G_DisableItemUsageWhileBleedingOut = AddToggleOption("$NUP_CDisableItemBleedingOut", Usage_GetDisableItemUsageWhileBleedingOut())
        G_DisableItemUsageWhileSleeping = AddToggleOption("$NUP_CDisableItemSleeping", Usage_GetDisableItemUsageWhileSleeping())
        G_DisableItemUsageForExcludedNPCs = AddToggleOption("$NUP_CDisableExcluded", Usage_GetDisableItemUsageForExcludedNPCs())
        AddHeaderOption("$NUP_HRemovalOptions")
        G_Remove = AddToggleOption("$NUP_CRemoveItemsOfDeath", Removal_GetRemoveItemsOnDeath())
        G_RemoveChance = AddSliderOption("NUP_SChanceToRemoveItem",Removal_GetChanceToRemoveItem())
        G_RemoveMaxLeft = AddSliderOption("$NUP_SMaxItemsLeft", Removal_GetMaxItemsLeft())
        AddHeaderOption("$NUP_HFixes")
        G_ApplySkillBoost = AddToggleOption("$NUP_CApplySkillBosstPerks", Fixes_GetApplySkillBoostPerks())
        G_ForceFixSounds = AddToggleOption("$NUP_CForceFixPotionSounds", Fixes_GetForceFixPotionSounds())
    elseif (page == Pages[1]) ; potions
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HPotionOptions")
        PO_EnableHealth = AddToggleOption("$NUP_CEnableHealthPotions", Potions_GetEnableHealthRestoration())
        PO_HealthThreshold = AddSliderOption("$NUP_SHealthThreshold", Potions_GetHealthThreshold(), "{2}")
        PO_EnableMagicka = AddToggleOption("$NUP_CEnableMagickaPotions", Potions_GetEnableMagickaRestoration())
        PO_MagickaThreshold = AddSliderOption("$NUP_SMagickaThreshold", Potions_GetMagickaThreshold(), "{2}")
        PO_EnableStamina = AddToggleOption("$NUP_CEnableStaminaPotions", Potions_GetEnableStaminaRestoration())
        PO_StaminaThreshold = AddSliderOption("$NUP_SStaminathreshold", Potions_GetStaminaThreshold(), "{2}")
        PO_AllowDetrimental = AddToggleOption("$NUP_CAllowPotionsWithDetrimentalEffects", Potions_GetAllowDetrimentalEffects())
        PO_SheathedAsOOC = AddToggleOption("$NUP_CTreatWeaponsSheathedAsOOC", Potions_GetHandleWeaponSheathedAsOutOfCombat())
        PO_Chance = AddSliderOption("$NUP_SChanceToUsePotions", Potions_GetUsePotionChance())
        SetCursorPosition(1)
        AddHeaderOption("$NUP_HProbabilityAdjusters")
        PO_Prob_Add = AddMenuOption("$NUP_MAddEntry", "")
        int i = 0
        while (i < 5)
            float val = Distr_GetProbabilityAdjuster(EffectOptionsPotionIndex[i], ENUMPotion)
            if (val != 1.0)
                PO_Prob_Act[PO_Prob_act_maxindex] = AddSliderOption(ToStringAlchemicEffect(EffectOptionsPotionIndex[i]), val, "{2}")
                PO_Prob_Ind[PO_Prob_act_maxindex] = EffectOptionsPotionIndex[i]
                PO_Prob_act_maxindex = PO_Prob_act_maxindex + 1
            endif
            i = i + 1
        endwhile
    elseif (page == Pages[2]) ; poisons
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        PI_EnablePoisons = AddToggleOption("$NUP_CEnablePoisons", Poisons_GetEnablePoisons())
        PI_AllowPositive = AddToggleOption("$NUP_CAllowPositiveEffects", Poisons_GetAllowPositiveEffects())
        PI_WeaponsSheathed = AddToggleOption("$NUP_CDontUseWithWeaponsSheathed", Poisons_GetDontUseWithWeaponsSheathed())
        PI_PoisonResist = AddToggleOption("$NUP_CDontUseAgainstEnemiesWithFullPoisonResist", Poisons_GetDontUseAgainst100PoisonResist())
        PI_LevelScale = AddSliderOption("$NUP_SEnemyLevelScaleCondition", Poisons_GetEnemyLevelScalePlayerLevel(), "{2}")
        PI_NumberThreshold = AddSliderOption("$NUP_SEnemyNumberCondition", Poisons_GetEnemyNumberThreshold())
        PI_PoisonChance = AddSliderOption("$NUP_SChanceToUsePoisons", Poisons_GetUsePoisonChance())
        AddHeaderOption("$NUP_HDosage")
        PI_Dosage = AddSliderOption("$NUP_SBasePoisonDosage", Poisons_GetDosage())
        SetCursorPosition(1)
        AddHeaderOption("$NUP_HProbabilityAdjusters")
        PI_Prob_Add = AddMenuOption("$NUP_MAddEntry", "")
        int i = 0
        while (i < 64)
            float val = Distr_GetProbabilityAdjuster(i + 1, ENUMPoison)
            if (val != 1.0)
                PI_Prob_Act[PI_Prob_act_maxindex] = AddSliderOption(ToStringAlchemicEffect(i + 1), val, "{2}")
                PI_Prob_Ind[PI_Prob_act_maxindex] = i+1
                PI_Prob_act_maxindex = PI_Prob_act_maxindex + 1
            endif
            i = i + 1
        endwhile
    elseif (page == Pages[3]) ; fortify potions
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        FO_EnableFortiy = AddToggleOption("$NUP_CEnableFortify", Fortify_GetEnableFortifyPotions())
        FO_WeaponsSheathed = AddToggleOption("$NUP_CDontUseWithWeaponsSheathed", Fortify_GetDontUseWithWeaponsSheathed())
        FO_LevelScale = AddSliderOption("$NUP_SEnemyLevelScaleCondition", Fortify_GetEnemyLevelScalePlayerLevelFortify(), "{2}")
        FO_NumberThreshold = AddSliderOption("$NUP_SEnemyNumberCondition", Fortify_GetEnemyNumberThresholdFortify())
        FO_Chance = AddSliderOption("$NUP_SChanceToUseFortifyPotions", Fortify_GetUseFortifyPotionChance())
        SetCursorPosition(1)
        AddHeaderOption("$NUP_HProbabilityAdjusters")
        FO_Prob_Add = AddMenuOption("$NUP_MAddEntry", "")
        int i = 0
        while (i < 64)
            float val = Distr_GetProbabilityAdjuster(i + 1, ENUMFortify)
            if (val != 1.0)
                FO_Prob_Act[FO_Prob_act_maxindex] = AddSliderOption(ToStringAlchemicEffect(i + 1), val, "{2}")
                FO_Prob_Ind[FO_Prob_act_maxindex] = i+1
                FO_Prob_act_maxindex = FO_Prob_act_maxindex + 1
            endif
            i = i + 1
        endwhile
    elseif (page == Pages[4]) ; food
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        F_EnableFood = AddToggleOption("$NUP_CEnableFood", Food_GetEnableFood())
        F_AllowDetrimental = AddToggleOption("$NUP_CAllowFoodWithDetrimentalEffects", Food_GetAllowDetrimentalEffects())
        F_RestrictCS = AddToggleOption("$NUP_CRestrictFoodUsageToCombatStart", Food_GetRestrictFoodToCombatStart())
        F_DisableFollowers = AddToggleOption("$NUP_CDisableFoodForFollowers", Food_GetDisableFollowers())
        F_WeaponsSheathed = AddToggleOption("$NUP_CDontUseWithWeaponsSheathed", Food_GetDontUseWithWeaponsSheathed())
        SetCursorPosition(1)
        AddHeaderOption("$NUP_HProbabilityAdjusters")
        F_Prob_Add = AddMenuOption("$NUP_MAddEntry", "")
        int i = 0
        while (i < 64)
            float val = Distr_GetProbabilityAdjuster(i + 1, ENUMFood)
            if (val != 1.0)
                F_Prob_Act[F_Prob_act_maxindex] = AddSliderOption(ToStringAlchemicEffect(i + 1), val, "{2}")
                F_Prob_Ind[F_Prob_act_maxindex] = i+1
                F_Prob_act_maxindex = F_Prob_act_maxindex + 1
            endif
            i = i + 1
        endwhile
    elseif (page == Pages[5]) ; player
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HPlayerOptions")
        PL_PlayerPotions = AddToggleOption("$NUP_CUsePotions", Player_GetPlayerPotions())
        PL_PlayerPoisons = AddToggleOption("$NUP_CUsePoisons", Player_GetPlayerPoisons())
        PL_PLayerFortify = AddToggleOption("$NUP_CUseFortifyPotions", Player_GetPlayerFortify())
        PL_PLayerFood = AddToggleOption("$NUP_CUseFood", Player_GetPlayerFood())
        PL_UseFavoritedOnly = AddToggleOption("$NUP_COnlyUseFavoritedItems", Player_GetUseFavoritedItemsOnly())
        PL_DontUseFavorited = AddToggleOption("$NUP_CDontUseFavoritedItems", Player_GetDontUseFavoritedItems())
        PL_DontEatRawFood = AddToggleOption("$NUP_CDontEatRawFood", Player_GetDontEatRawFood())
        PL_DontDrinkAlcohol = AddToggleOption("$NUP_CDontDrinkAlcoholicBeverages", Player_GetDontDrinkAlcohol())
    elseif (page == Pages[6]) ; distribution
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HGeneralDistributionOptions")
        D_Potions = AddToggleOption("$NUP_CDistributePotions", Distr_GetDistributePotions())
        D_Poisons = AddToggleOption("$NUP_CDistributePoisons", Distr_GetDistributePoisons())
        D_Fortify = AddToggleOption("$NUP_CDistributeFortifyPotions", Distr_GetDistributeFortify())
        D_Food = AddToggleOption("$NUP_CDistributeFood", Distr_GetDistributeFood())
        D_CustomItems = AddToggleOption("$NUP_CDistributeCustomItems", Distr_GetDistributeCustomItems())
        AddHeaderOption("$NUP_HDifficultyOptions")
        D_GameDifficulty = AddToggleOption("$NUP_CUseGameDifficulty", Distr_GetGameDifficultyScaling())
        D_LevelEasy = AddSliderOption("$NUP_SLevelEasy", Distr_GetLevelEasy())
        D_LevelNormal = AddSliderOption("$NUP_SLevelNormal", Distr_GetLevelNormal())
        D_LevelDifficult = AddSliderOption("$NUP_SLevelDifficult", Distr_GetLevelDifficult())
        D_Levelnsane = AddSliderOption("$NUP_SLevelInsane", Distr_GetLevelInsane())
        AddHeaderOption("$NUP_HItemMagnitudeOptions")
        D_MagWeak = AddSliderOption("$NUP_SMaxMagnitudeForWeakItems", Distr_GetMaxMagnitudeWeak())
        D_MagStandard = AddSliderOption("$NUP_SMaxMagnitudeForStandardItems", Distr_GetMaxMagnitudeStandard())
        D_MagPotent = AddSliderOption("$NUP_SMaxMagnitudeForPotentItems", Distr_GetMaxMagnitudePotent())
        AddHeaderOption("$NUP_HStyleScaling")
        D_StylePrimary = AddSliderOption("$NUP_SPrimary", Distr_GetStyleScalingPrimary(), "{2}")
        D_StyleSecondary = AddSliderOption("$NUP_SSecondary", Distr_GetStyleScalingSecondary(), "{2}")
        AddHeaderOption("$NUP_HProbabilityModifiers")
        D_ProbScaling = AddSliderOption("$NUP_SItemChanceMultiplier", Distr_GetProbabilityScaling(), "{2}")
        AddHeaderOption("$NUP_HMiscSettings")
        D_DoNotMixed = AddToggleOption("$NUP_CDoNotDistributeMixedInvisPotions",Distr_GetDoNotDistributeMixedInvisPotions())

        SetCursorPosition(1)
        AddHeaderOption("$NUP_HProhibitedEffects")
        D_Prohib_Menu_Options = new string[3]
        D_Prohib_Menu_Options[0] = "$NUP_TPotions"
        D_Prohib_Menu_Options[1] = "$NUP_TPoisons"
        D_Prohib_Menu_Options[2] = "$NUP_TFood"
        D_Prohib_Menu = AddMenuOption("$NUP_MItemType", D_Prohib_Menu_Options[D_Prohib_Menu_Selection])
        AddTextOption("$NUP_TEffectsThatCanBeProhibited", "")
        int i = 1
        D_prohib_options = new int[65]
        while (i < 65)
            D_Prohib_options[i] = AddToggleOption(ToStringAlchemicEffect(i), IsEffectProhibited(i))
            i = i + 1
        endwhile
    elseif (page == Pages[7]) ; whitelist
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HWhitelistOptions")
        W_EnableItems = AddToggleOption("$NUP_EnableItems", Whitelist_GetEnabledItems())
        W_EnableNPCs = AddToggleOption("$NUP_CEnableNPCs", Whitelist_GetEnabledNPCs())
    elseif (page == Pages[8]) ; compatbility
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HGeneralCompatibilityOptions")
        C_DisableCreatures = AddToggleOption("$NUP_CDisableCreaturesWithoutRules", Comp_GetDisableCreaturesWhitoutRules())
        AddHeaderOption("$NUP_HAnimatedPoisons")
        C_AnPois_Enable = AddToggleOption("$NUP_CEnableAnimations", Comp_AnimatedPoisons_GetEnabled())
        AddTextOption("$NUP_TLoaded", Comp_AnimatedPoisons_Loaded())
        AddHeaderOption("$NUP_HAnimatedPotions")
        C_AnPoti_Enable = AddToggleOption("$NUP_CEnableAnimations", Comp_AnimatedPotions_GetEnabled())
        AddTextOption("$NUP_TLoaded", Comp_AnimatedPotions_Loaded())
        AddHeaderOption("$NUP_HZUPA")
        AddTextOption("$NUP_TLoaded", Comp_ZUPA_Loaded())
        AddHeaderOption("$NUP_HSacrosanct")
        AddTextOption("$NUP_TLoaded", Comp_Sacrosanct_Loaded())
        AddHeaderOption("$NUP_HUltimatePotionsNG")
        AddTextOption("$NUP_TLoaded", Comp_UltimatePotions_Loaded())


        SetCursorPosition(1)
        AddHeaderOption("$NUP_HCACO")
        AddTextOption("$NUP_TLoaded", Comp_CACO_Loaded())
        AddHeaderOption("$NUP_HApothecary")
        AddTextOption("$NUP_TLoaded", Comp_Apothecary_Loaded())
    elseif (page == Pages[9]) ; debug
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        AddHeaderOption("$NUP_HDebugOptions")
        D_EnableLog = AddToggleOption("$NUP_CEnableLogging", Debug_GetEnableLog())
        D_EnableLoadLog = AddToggleOption("$NUP_CEnableLoadLogging", Debug_GetEnableLoadLog())
        D_LogLevel = AddSliderOption("$NUP_SLogDetailLevel", Debug_GetLogLevel())
        D_EnableProfiling = AddToggleOption("$NUP_CEnableProfiling", Debug_GetEnableProfiling())
        D_ProfileLevel = AddSliderOption("$NUP_SProfilingDetailLevel", Debug_GetProfileLevel())
    elseif (page == Pages[10])
        SetCursorFillMode(TOP_TO_BOTTOM)
        SetCursorPosition(0)
        S_ActorsSaved = AddTextOption("$NUP_TActorsSaved", Stats_ActorsSaved())
        S_ActorsRead = AddTextOption("$NUP_TActorsRead", Stats_ActorsRead())
        S_PotionsUsed = AddTextOption("$NUP_TPotionsUsed", Stats_PotionsUsed())
        S_PoisonsUsed = AddTextOption("$NUP_TPoisonsUsed", Stats_PoisonsUsed())
        S_FoodUsed = AddTextOption("$NUP_TFoodUsed", Stats_FoodUsed())
        S_EventsHandled = AddTextOption("$NUP_TEventsHandled", Stats_EventsHandled())
        S_ActorsHandled = AddTextOption("$NUP_TActorsHandled", Stats_ActorsHandled())
        S_ActorsHandledTotal = AddTextOption("$NUP_TTotalActorsHandled", Stats_ActorsHandledTotal())
    endif
EndEvent

Event OnOptionMenuOpen(int option)
    if (option == 0)

    elseif (option == D_Prohib_Menu)
        SetMenuDialogStartIndex(D_Prohib_Menu_Selection)
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(D_Prohib_Menu_Options)
    elseif (option == PO_Prob_Add)
        SetMenuDialogStartIndex(PO_Prob_Selec)
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(EffectOptionsPotion)
    elseif (option == PI_Prob_Add)
        SetMenuDialogStartIndex(PI_Prob_Selec)
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(EffectOptions)
    elseif (option == FO_Prob_Add)
        SetMenuDialogStartIndex(FO_Prob_Selec)
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(EffectOptions)
    elseif (option == F_Prob_Add)
        SetMenuDialogStartIndex(F_Prob_Selec)
        SetMenuDialogDefaultIndex(0)
        SetMenuDialogOptions(EffectOptions)
    endif
EndEvent

Event OnOptionMenuAccept(int option, int index)
    if (option == 0)

    ElseIf(option == PO_Prob_Add)
        PO_Prob_Selec = index
        Distr_SetProbabilityAdjuster(EffectOptionsPotionIndex[index], ENUMPotion, 1.01)
    ElseIf(option == PI_Prob_Add)
        PI_Prob_Selec = index
        Distr_SetProbabilityAdjuster(index + 1, ENUMPoison, 1.01)
    ElseIf(option == FO_Prob_Add)
        FO_Prob_Selec = index
        Distr_SetProbabilityAdjuster(index + 1, ENUMFortify, 1.01)
    ElseIf(option == F_Prob_Add)
        F_Prob_Selec = index
        Distr_SetProbabilityAdjuster(index + 1, ENUMFood, 1.01)
    elseif (option == D_Prohib_Menu)
        D_Prohib_Menu_Selection = index
        SetMenuOptionValue(D_Prohib_Menu, D_Prohib_Menu_Options[D_Prohib_Menu_Selection], true)
    endif
    ForcePageReset()
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
    elseif (option == D_DoNotMixed)
        Distr_SetDoNotDistributeMixedInvisPotions(!Distr_GetDoNotDistributeMixedInvisPotions())
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
    elseif (option == G_DisableItemUsageWhileFlying)
        Usage_SetDisableItemUsageWhileFlying(!Usage_GetDisableItemUsageWhileFlying())
    elseif (option == G_DisableItemUsageWhileBleedingOut)
        Usage_SetDisableItemUsageWhileBleedingOut(!Usage_GetDisableItemUsageWhileBleedingOut())
    elseif (option == G_DisableItemUsageWhileSleeping)
        Usage_SetDisableItemUsageWhileSleeping(!Usage_GetDisableItemUsageWhileSleeping())
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
    elseif (option == D_ProbScaling)
        SetSliderDialogDefaultValue(1.0)
        SetSliderDialogStartValue(Distr_GetProbabilityScaling())
        SetSliderDialogRange(0.1, 3.0)
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

    int i = 0
    while (i < PO_Prob_act_maxindex)
        if (option == PO_Prob_Act[i])
            SetSliderDialogDefaultValue(1)
            SetSliderDialogStartValue(Distr_GetProbabilityAdjuster(PO_Prob_Ind[i], ENUMPotion))
            SetSliderDialogRange(0, 20)
            SetSliderDialogInterval(0.01)
        endif
        i = i + 1
    endwhile
    i = 0
    while (i < PI_Prob_act_maxindex)
        if (option == PI_Prob_Act[i])
            SetSliderDialogDefaultValue(1)
            SetSliderDialogStartValue(Distr_GetProbabilityAdjuster(PI_Prob_Ind[i], ENUMPoison))
            SetSliderDialogRange(0, 20)
            SetSliderDialogInterval(0.01)
        endif
        i = i + 1
    endwhile
    i = 0
    while (i < FO_Prob_act_maxindex)
        if (option == FO_Prob_Act[i])
            SetSliderDialogDefaultValue(1)
            SetSliderDialogStartValue(Distr_GetProbabilityAdjuster(FO_Prob_Ind[i], ENUMFortify))
            SetSliderDialogRange(0, 20)
            SetSliderDialogInterval(0.01)
        endif
        i = i + 1
    endwhile
    i = 0
    while (i < F_Prob_act_maxindex)
        if (option == F_Prob_Act[i])
            SetSliderDialogDefaultValue(1)
            SetSliderDialogStartValue(Distr_GetProbabilityAdjuster(F_Prob_Ind[i], ENUMFood))
            SetSliderDialogRange(0, 20)
            SetSliderDialogInterval(0.01)
        endif
        i = i + 1
    endwhile
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
    elseif (option == D_ProbScaling)
        Distr_SetProbabilityScaling(value)
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

    int i = 0
    while (i < PO_Prob_act_maxindex)
        if (option == PO_Prob_Act[i])
            Distr_SetProbabilityAdjuster(PO_Prob_Ind[i], ENUMPotion, value)
        endif
        i = i + 1
    endwhile
    i = 0
    while (i < PI_Prob_act_maxindex)
        if (option == PI_Prob_Act[i])
            Distr_SetProbabilityAdjuster(PI_Prob_Ind[i], ENUMPoison, value)
        endif
        i = i + 1
    endwhile
    i = 0
    while (i < FO_Prob_act_maxindex)
        if (option == FO_Prob_Act[i])
            Distr_SetProbabilityAdjuster(FO_Prob_Ind[i], ENUMFortify, value)
        endif
        i = i + 1
    endwhile
    i = 0
    while (i < F_Prob_act_maxindex)
        if (option == F_Prob_Act[i])
            Distr_SetProbabilityAdjuster(F_Prob_Ind[i], ENUMFood, value)
        endif
        i = i + 1
    endwhile

    ForcePageReset()
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
    elseif (option == D_ProbScaling)
        Distr_SetProbabilityScaling(1.0)
    elseif (option == D_DoNotMixed)
        Distr_SetDoNotDistributeMixedInvisPotions(false)
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
    elseif (option == G_DisableItemUsageWhileFlying)
        Usage_SetDisableItemUsageWhileFlying(false)
    elseif (option == G_DisableItemUsageWhileBleedingOut)
        Usage_SetDisableItemUsageWhileBleedingOut(false)
    elseif (option == G_DisableItemUsageWhileSleeping)
        Usage_SetDisableItemUsageWhileSleeping(false)
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
        SetInfoText("$NUP_Help_ActorsSaved")
    elseif (option == S_ActorsRead)
        SetInfoText("$NUP_Help_ActorsRead")
    elseif (option == S_PotionsUsed)
        SetInfoText("$NUP_Help_PotionsUsed")
    elseif (option == S_PoisonsUsed)
        SetInfoText("$NUP_Help_PoisonsUsed")
    elseif (option == S_FoodUsed)
        SetInfoText("$NUP_Help_FoodUsed")
    elseif (option == S_EventsHandled)
        SetInfoText("$NUP_Help_EventsHandled")
    elseif (option == S_ActorsHandled)
        SetInfoText("$NUP_Help_ActorsHandled")
    elseif (option == S_ActorsHandledTotal)
        SetInfoText("$NUP_Help_ActorsHandledTotal")
    elseif (option == C_DisableCreatures)
        SetInfoText("$NUP_Help_DisableCreatures")
    elseif (option == C_AnPois_Enable)
        SetInfoText("$NUP_Help_AnimatedPoisonsEnable")
    elseif (option == C_AnPoti_Enable)
        SetInfoText("$NUP_Help_AnimatedPotionsEnable")
    elseif (option == W_EnableItems)
        SetInfoText("$NUP_Help_WhitelistEnableItems")
    elseif (option == W_EnableNPCs)
        SetInfoText("$NUP_Help_WhitelistEnableNPCs")
    elseif (option == G_Remove)
        SetInfoText("$NUP_Help_RemoveItems")
    elseif (option == G_RemoveChance)
        SetInfoText("$NUP_Help_ChanceRemoveItems")
    elseif (option == G_RemoveMaxLeft)
        SetInfoText("$NUP_Help_RemoveMaxLeft")
    elseif (option == G_ApplySkillBoost)
        SetInfoText("$NUP_Help_ApplySkillBoostPerks")
    elseif (option == G_ForceFixSounds)
        SetInfoText("$NUP_Help_FixSounds")
    elseif (option == D_Poisons)
        SetInfoText("$NUP_Help_DistrPoisons")
    elseif (option == D_Potions)
        SetInfoText("$NUP_Help_DistrPotions")
    elseif (option == D_Fortify)
        SetInfoText("$NUP_Help_DistrFortify")
    elseif (option == D_Food)
        SetInfoText("$NUP_Help_DistrFood")
    elseif (option == D_CustomItems)
        SetInfoText("$NUP_Help_DistrCustomItems")
    elseif (option == D_LevelEasy)
        SetInfoText("$NUP_Help_LevelEasy")
    elseif (option == D_LevelNormal)
        SetInfoText("$NUP_Help_LevelNormal")
    elseif (option == D_LevelDifficult)
        SetInfoText("$NUP_Help_LevelDifficult")
    elseif (option == D_Levelnsane)
        SetInfoText("$NUP_Help_LevelInsane")
    elseif (option == D_GameDifficulty)
        SetInfoText("$NUP_Help_GameDifficulty")
    elseif (option == D_MagWeak)
        SetInfoText("$NUP_Help_MagWeak")
    elseif (option == D_MagStandard)
        SetInfoText("$NUP_Help_MagStandard")
    elseif (option == D_MagPotent)
        SetInfoText("$NUP_Help_MagPotent")
    elseif (option == D_StylePrimary)
        SetInfoText("$NUP_Help_StylePrimary")
    elseif (option == D_StyleSecondary)
        SetInfoText("$NUP_Help_StyeleSecondary")
    elseif (option == D_ProbScaling)
        SetInfoText("$NUP_Help_ProbabilityScaling")
    elseif (option == D_DoNotMixed)
        SetInfoText("$NUP_Help_DoNotDistributeMixedPotions")
    elseif (option == F_EnableFood)
        SetInfoText("$NUP_Help_EnableFood")
    elseif (option == F_AllowDetrimental)
        SetInfoText("$NUP_Help_FoodAllowDetrimental")
    elseif (option == F_RestrictCS)
        SetInfoText("$NUP_Help_FoodRestrictToCombatStart")
    elseif (option == F_DisableFollowers)
        SetInfoText("$NUP_Help_FoodDisableFollowers")
    elseif (option == F_WeaponsSheathed)
        SetInfoText("$NUP_Help_FoodWeaponsSheathed")
    elseif (option == FO_EnableFortiy)
        SetInfoText("$NUP_Help_EnableFortify")
    elseif (option == FO_WeaponsSheathed)
        SetInfoText("$NUP_Help_FortifyWeaponsSheatzed")
    elseif (option == FO_LevelScale)
        SetInfoText("$NUP_Help_FortifyLevelScaling")
    elseif (option == FO_NumberThreshold)
        SetInfoText("$NUP_Help_FortifyNumberThreshold")
    elseif (option == FO_Chance)
        SetInfoText("$NUP_Help_ForifyChance")
    elseif (option == PI_EnablePoisons)
        SetInfoText("$NUP_Help_EnablePoisons")
    elseif (option == PI_AllowPositive)
        SetInfoText("$NUP_Help_PoisonsAllowPositive")
    elseif (option == PI_WeaponsSheathed)
        SetInfoText("$NUP_Help_PoisonsWeaponsSheathed")
    elseif (option == PI_PoisonResist)
        SetInfoText("$NUP_Help_PoisonsDontUseAgainstFullPoisonResist")
    elseif (option == PI_LevelScale)
        SetInfoText("$NUP_Help_PoisonsLevelScaling.")
    elseif (option == PI_NumberThreshold)
        SetInfoText("$NUP_Help_PoisonsNumberThreshold")
    elseif (option == PI_PoisonChance)
        SetInfoText("$NUP_Help_PoisonsChanceToUse")
    elseif (option == PI_Dosage)
        SetInfoText("$NUP_Help_PoisonDosage")
    elseif (option == PO_EnableMagicka)
        SetInfoText("$NUP_Help_PotionsEnableMagicka")
    elseif (option == PO_EnableStamina)
        SetInfoText("$NUP_Help_PotionsEnableStamina")
    elseif (option == PO_EnableHealth)
        SetInfoText("$NUP_Help_PotionsEnableHealth")
    elseif (option == PO_AllowDetrimental)
        SetInfoText("$NUP_Help_PotionsAllowDetrimental")
    elseif (option == PO_SheathedAsOOC)
        SetInfoText("$NUP_Help_PotionsTreatSheathedAsOOC")
    elseif (option == PO_HealthThreshold)
        SetInfoText("$NUP_Help_PotionsHealthThreshold")
    elseif (option == PO_MagickaThreshold)
        SetInfoText("$NUP_Help_PotionsMagickaThreshold")
    elseif (option == PO_StaminaThreshold)
        SetInfoText("$NUP_Help_PotionsStaminaThreshold)
    elseif (option == PO_Chance)
        SetInfoText("$NUP_Help_PotionsChance")
    elseif (option == PL_PlayerPotions)
        SetInfoText("$NUP_Help_PlayerPotions")
    elseif (option == PL_PlayerPoisons)
        SetInfoText("$NUP_Help_PlayerPoisons")
    elseif (option == PL_PLayerFortify)
        SetInfoText("$NUP_Help_PlayerFortify")
    elseif (option == PL_PLayerFood)
        SetInfoText("$NUP_Help_PlayerFood")
    elseif (option == PL_UseFavoritedOnly)
        SetInfoText("$NUP_Help_PlayerFavoritedOnly.")
    elseif (option == PL_DontUseFavorited)
        SetInfoText("$NUP_Help_PlayerDontUseFavorited")
    elseif (option == PL_DontEatRawFood)
        SetInfoText("$NUP_Help_PlayerNoRawFood")
    elseif (option == PL_DontDrinkAlcohol)
        SetInfoText("$NUP_Help_PlayerNoAlcohol")
    elseif (option == G_GlobalCooldown)
        SetInfoText("$NUP_Help_GlobalCooldown")
    elseif (option == G_MaxDuration)
        SetInfoText("$NUP_Help_MaxDuration")
    elseif (option == G_MaxFortifyDuration)
        SetInfoText("$NUP_Help_MaxFortifyDuration")
    elseif (option == G_CycleTime)
        SetInfoText("$NUP_Help_CycleTime")
    elseif (option == G_DisableNonFollowerNPCs)
        SetInfoText("$NUP_Help_DisableNonFollowerNPCs")
    elseif (option == G_DisableOutOfCombatProcessing)
        SetInfoText("$NUP_Help_DisableOOCProcessing")
    elseif (option == G_DisableItemUsageWhileStaggered)
        SetInfoText("$NUP_Help_DisableWhileStaggered"9)
    elseif (option == G_DisableItemUsageWhileFlying)
        SetInfoText("$NUP_Help_DisableWhileFlying")
    elseif (option == G_DisableItemUsageWhileBleedingOut)
        SetInfoText(""$NUP_Help_DisableWhileBleedingOut)
    elseif (option == G_DisableItemUsageWhileSleeping)
        SetInfoText("$NUP_Help_DisableWhileSleeping")
    elseif (option == G_DisableItemUsageForExcludedNPCs)
        SetInfoText("$NUP_Help_DisableForExcludedNPCs")
    elseif (option == D_EnableLog)
        SetInfoText("$NUP_Help_EnableLog")
    elseif (option == D_EnableLoadLog)
        SetInfoText("$NUP_Help_EnableLoadLog")
    elseif (option == D_LogLevel)
        SetInfoText("$NUP_Help_LogLevel")
    elseif (option == D_EnableProfiling)
        SetInfoText("$NUP_Help_EnableProfiling")
    elseif (option == D_ProfileLevel)
        SetInfoText("$NUP_Help_ProfileLevel")
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

; returns whether flying npcs should not use items
bool Function Usage_GetDisableItemUsageWhileFlying() global native
; sets whether flying npcs should not use items
Function Usage_SetDisableItemUsageWhileFlying(bool disable) global native

; returns whether bleeding npcs should not use items
bool Function Usage_GetDisableItemUsageWhileBleedingOut() global native
; sets whether staggered npcs should not use items
Function Usage_SetDisableItemUsageWhileBleedingOut(bool disable) global native

; returns whether sleeping npcs should not use items
bool Function Usage_GetDisableItemUsageWhileSleeping() global native
; sets whether sleeping npcs should not use items
Function Usage_SetDisableItemUsageWhileSleeping(bool disable) global native

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

; Returns the scale of the distribution probability for items matching the combat styles secondary combat modifiers
float Function Distr_GetStyleScalingSecondary() global native
; Sets the scale of the distribution probability for items matching the combat styles secondary combat modifiers
Function Distr_SetStyleScalingSecondary(float value) global native

; Returns the general probability scaling
float Function Distr_GetProbabilityScaling() global native
; Sets the general probability scaling
Function Distr_SetProbabilityScaling(float value) global native

; Returns wether potions mixed with invisibility are allowed
bool Function Distr_GetDoNotDistributeMixedInvisPotions() global native
; Sets wether potions mixed with invisibility are allowed
Function Distr_SetDoNotDistributeMixedInvisPotions(bool value) global native

; Returns the probability adjuster for the given effect and item type. If the adjuster doesn't exist returns 1
float Function Distr_GetProbabilityAdjuster(int effect, int itemtype) global native

; Sets the probability adjuster for the given effect and item type. If the value is one, the adjuster is deleted
Function Distr_SetProbabilityAdjuster(int effect, int itemtype, float value) global native

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
