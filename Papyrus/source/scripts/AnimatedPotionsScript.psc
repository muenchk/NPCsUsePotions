Scriptname AnimatedPotionsScript extends ReferenceAlias

Import po3_SKSEFunctions
Import po3_Events_Alias

Actor property PlayerRef auto
Armor property FakeItem auto
Sound property PotionSM auto
Sound property FastPotionSM auto
FormList property CorkFormList auto
FormList property CorkHandFormList auto
FormList property BlackListedPotions auto
GlobalVariable property ToggleStopSprint auto
GlobalVariable property ToggleForceThirdPerson auto
GlobalVariable property ToggleForceCloseMenu auto
GlobalVariable property TogglePlayerSlowEffect auto
GlobalVariable property TogglePlayerStagger auto
GlobalVariable property TogglePlayerStopAnimation auto
GlobalVariable property ToggleDisableDuringCombat auto
GlobalVariable property ToggleWaitForMenuClose auto
GlobalVariable property ToggleFallbackFunction auto
GlobalVariable property ToggleSheathingDrinking auto
GlobalVariable property ToggleRequireKeyword auto
Spell property SlowEffectSP auto
form LeftItem
Spell LeftSpell
bool Busy
bool LeftHand
int PotionsUsed
int InstanceID
string AnimationEventstring

Event OnInit()
    if PlayerRef.GetItemCount(FakeItem) == 0
        PlayerRef.AddItem(FakeItem, 1, true)
    endif
EndEvent


Event OnPLayerLoadGame()
    if PlayerRef.GetItemCount(FakeItem) == 0
        PlayerRef.AddItem(FakeItem, 1, true)
    endif
    Busy = false
    LeftItem = none
    LeftSpell = none
    PotionsUsed = Game.QueryStat("Potions Used")
    RegisterForModEvent("NPCsUsePotions_AnimatedPotionsEvent", "NPCSUsePotions_ItemRemoved")
    RegisterForModEvent("NPCsUsePotions_AnimatedPotionsHitEvent", "NPCsUsePotions_PotionsOnHit")
EndEvent

Event NPCSUsePotions_ItemRemoved(string eventName, string strArg, float numArg, Form sender)
    if (eventName == "NPCsUsePotions_AnimatedPotionsEvent" && strArg != "" && sender != None)
        Actor act = sender as Actor
        if (act && act != PlayerRef)
            form akBaseItem = Game.GetForm(strArg as Int)

            if BlackListedPotions.Find(akBaseItem) != -1
                NPCsUsePotions_Potions.AnimatedPotions_Abort(act)
                return
            endif
            if TargetConditionCheck(act) == false
                NPCsUsePotions_Potions.AnimatedPotions_Abort(act)
                return
            endif
            if ToggleDisableDuringCombat.GetValueInt() == 1 && act.IsInCombat() == true
                NPCsUsePotions_Potions.AnimatedPotions_Abort(act)
                return
            endif
            if akBaseItem.HasKeywordString("vendoritempotion") == false && ToggleRequireKeyword.GetValueInt() == 1
                NPCsUsePotions_Potions.AnimatedPotions_Abort(act)
                return
            endif

            string Animationstring = ""

            Animationstring = GetFormEditorID(akBaseItem)
            if StringUtil.Find(Animationstring, "mag_", 0) >= 0
                Animationstring = StringUtil.Substring(Animationstring, 4)
            endif
            if StringUtil.Find(Animationstring, "CACO_", 0) >= 0
                Animationstring = StringUtil.Substring(Animationstring, 5)
            endif
            if StringUtil.Find(Animationstring, "_KRY", 0) >= 0
                int Lenght = StringUtil.GetLength(Animationstring)
                int Cut_Length = Lenght - 4
                Animationstring = StringUtil.Substring(Animationstring, 0, Cut_Length)
            endif
            if ToggleFallbackFunction.GetValueInt() == 1
                if JsonUtil.StringListHas("potionlist", "potions", Animationstring) == false
                    Animationstring = "restorehealth01"
                endif
            endif
            if StringUtil.Find(Animationstring, "FortifySkill", 0) >= 0 \
            || StringUtil.Find(Animationstring, "TGPotion", 0) >= 0
                Animationstring = "fortifyskill01"
            endif

            if StringUtil.Find(Animationstring, "MS12", 0) >= 0
                Animationstring = "ms12whitephialrepaired"
            endif

            if AnimationString == ""
                AnimationString = "restorehealth01"
            endif

            act.SetAnimationVariablebool("bSprintOK", false)
            ;ForceCloseMenu(act)
            ;WaitForMenuClose()
            ;StopSprinting(act)
            Spell Lefts = None
            Form Lefti = None
            bool left = false
            if act.GetEquippeditemType(0) == 9; Left hand magic spell
                Lefts = act.GetEquippedSpell(0)
                act.UnequipSpell(Lefts, 0)
                left = true
            elseif HasLefthandeditem(act) == true
                Lefti = act.GetEquippedObject(0) as Form
                act.Unequipitem(Lefti, false, true)
                left = true
            elseif act.GetEquippeditemType(0) == 7; Has Bow
                if act.IsWeaponDrawn() == true
                    act.SheatheWeapon()
                    While act.GetAnimationVariablebool("IsUnEquipping") == true
                        Utility.wait(0.1)
                    endWhile
                endif
                left = false
            else
                left = false
            endif
            bool slow = false
            if TogglePlayerSlowEffect.GetValueint() == 1
                SlowEffectSP.RemoteCast(act, none)
                slow = true
            else
                slow = false
            endif
            RegisterForHitEventEx(self)

            if act.IsInCombat() || act.IsWeaponDrawn() == true ; Plays fast potion drink if player is in combat or weapon drawn
                if ToggleSheathingDrinking.GetValueInt() == 1
                    if act.IsWeaponDrawn() == true
                        act.SheatheWeapon()
                        While act.GetAnimationVariablebool("IsUnEquipping") == true
                            Utility.wait(0.1)
                        endWhile
                    endif
                    ;SlowDrinking()
                    NPCsUsePotions_Potions.AnimatedPotions_Callback(act)
                    Debug.SendAnimationEvent(act, Animationstring)
                    NPCsUsePotions_SlowPotionFunctions(act)
                    if left == true
                        if Lefti != none
                            act.equipItemEx(Lefti, 2)
                        elseif LeftSpell != none
                            act.EquipSpell(Lefts, 0)
                        endif
                    endif
                    if act.IsWeaponDrawn() == false
                        act.DrawWeapon()
                        While act.GetAnimationVariablebool("IsEquipping") == true
                            Utility.wait(0.01)
                        EndWhile
                    endif
                else
                    ;FastDrinking()
                    Animationstring = "Fast"+Animationstring
                    NPCsUsePotions_Potions.AnimatedPotions_Callback(act)
                    Debug.SendAnimationEvent(act, Animationstring)
                    Utility.Wait(2.4)
                    if Lefti != none
                        act.equipitemEx(Lefti, 2)
                    elseif LeftSpell != none
                        act.EquipSpell(Lefts, 0)
                    elseif HasTwoHandedMelee(act) == true
                        act.SheatheWeapon()
                        act.DrawWeapon()
                    elseif act.GetEquippeditemType(0) == 7; Has bow
                        act.DrawWeapon()
                    elseif HasRightOneHandedMelee(act) == true
                        act.equipitem(Fakeitem, false, true) ;Equip and unequip invisible shield to skip one handed redrawing
                        Utility.wait(0.05)
                        act.Unequipitem(Fakeitem, false, true)
                    else ;precaution
                        act.SheatheWeapon()
                        act.DrawWeapon()
                    endif
                endif
            else
                ;SlowDrinking()
                NPCsUsePotions_Potions.AnimatedPotions_Callback(act)
                Debug.SendAnimationEvent(act, Animationstring)
                NPCsUsePotions_SlowPotionFunctions(act)
                if left == true
                    if Lefti != none
                        act.equipItemEx(Lefti, 2)
                    elseif LeftSpell != none
                        act.EquipSpell(Lefts, 0)
                    endif
                endif
            endif

            Lefti = act.GetEquippedWeapon(false)
            Lefts = act.GetEquippedSpell(1)
            if (Lefti)
                act.UnequipItemEx(Lefti, 1)
                act.EquipItemEx(Lefti, 1)
            elseif (Lefts)
                act.UnequipSpell(Lefts, 1)
                act.EquipSpell(Lefts, 1)
            endif

            NPCsUsePotions_Potions.AnimatedPotions_RestorePoison(act)

            act.SetAnimationVariablebool("bSprintOK", true)
            UnregisterForHitEventEx(self)
        endif
    endif
EndEvent

Event NPCsUsePotions_PotionsOnHit(string eventName, string strArg, float numArg, Form sender)
    Actor act = sender as Actor
    if (act)
        if TogglePlayerStopAnimation.GetValueint() == 1
            NPCsUsePotions_Potions.AnimatedPotions_Abort(act)
            Debug.SendAnimationEvent(act, "offsetstop")
            Debug.SendAnimationEvent(act, "AnimObjectUnequip")
        endif
        if TogglePlayerStagger.GetValueint() == 1
            Debug.SendAnimationEvent(act, "StaggerStart")
        endif
    endif
EndEvent

; Timed cork art object spell casting. Cork spell is picked by AnimationEventString suffix or prefix.
;Cork numbers:
; 0 for lesser potion corks
; 1 for extra potion corks
; 2 for great potion corks
; 3 for extreme potion corks
; 4 for skill potion corks
; 5 for blood potion corks
; 6 for resist 50 corks
; 7 for resist 100 corks
; 8 for SleepingTreeSap cork
; 9 for whitephial cork
; Some potions are in different order, meaning extra is after great or vice versa, so this is not ideal.
Function NPCsUsePotions_SlowPotionFunctions(Actor akTarget)
    int CorkSpellIndex = 0
    if StringUtil.Find(AnimationEventString, "01", 0) >= 0
        CorkSpellIndex = 0
    ElseIf StringUtil.Find(AnimationEventString, "03", 0) >= 0
        CorkSpellIndex = 1
    ElseIf StringUtil.Find(AnimationEventString, "02", 0) >= 0
        CorkSpellIndex = 2
    ElseIf StringUtil.Find(AnimationEventString, "04", 0) >= 0
        CorkSpellIndex = 3
    elseif StringUtil.Find(AnimationEventString, "FortifySkill", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "blood", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "25", 0) >= 0 \
    || StringUtil.Find(AnimationEventString, "50", 0) >= 0 \
    || StringUtil.Find(AnimationEventString, "75", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "100", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "sleepingtreesap", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "ms12", 0) >= 0
        CorkSpellIndex = 4
    endif
    Spell CorkSpell = CorkFormList.GetAt(CorkSpellIndex) as Spell
    Spell CorkHandSpell = CorkHandFormList.GetAt(CorkSpellIndex) as Spell
    ;Cork in bottle
    (CorkSpell).RemoteCast(akTarget, none)
    ;Cork from bottle to hand
    Utility.Wait(0.56)
    (CorkHandSpell).RemoteCast(akTarget, none)
    akTarget.DispelSpell(CorkSpell)
    ;Cork from hand to bottle
    Utility.Wait(2.26)
    (CorkSpell).RemoteCast(akTarget, none)
    akTarget.DispelSpell(CorkHandSpell)
    ;Bottle disappear with cork
    Utility.Wait(0.52)
    akTarget.DispelSpell(CorkSpell)
endFunction

Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
    int EventPotionCount = Game.QueryStat("Potions Used")
    if EventPotionCount <= PotionsUsed
        return
    endif

    PotionsUsed = EventPotionCount

    if BlackListedPotions.Find(akBaseItem) != -1
        return
    endif

    if TargetConditionCheck(PlayerRef) == false
        return
    endif

    if ToggleDisableDuringCombat.GetValueInt() == 1 && PlayerRef.IsInCombat() == true
        return
    endif

    if akBaseItem.HasKeywordString("vendoritempotion") == false && ToggleRequireKeyword.GetValueInt() == 1
        return
    endif

    AnimationEventstring = ""
    InstanceID = 0

    AnimationEventstring = GetFormEditorID(akBaseItem)
    ModPrefixRevert()
    FallBackFunction()
    StringManipulation()

    Busy = true
    PlayerRef.SetAnimationVariablebool("bSprintOK", false)
    ForceCloseMenu(PlayerRef)
    WaitForMenuClose()
    StopSprinting(PlayerRef)
    bool FirstPerson = ForceThirdPerson(PlayerRef)
    LeftHand = LeftHandCheck(PlayerRef)
    CastSlowEffectSP(PlayerRef)
    RegisterForHitEventEx(self)

    if PlayerRef.IsInCombat() || PlayerRef.IsWeaponDrawn() == true ; Plays fast potion drink if player is in combat or weapon drawn
        if ToggleSheathingDrinking.GetValueInt() == 1
            SheatheWeaponAndWait(PlayerRef)
            SlowDrinking()
            DrawWeaponAndWait(PlayerRef)
        else
            FastDrinking()
        endif
    else
        SlowDrinking()
    endif

    if FirstPerson == true
        Game.ForceFirstPerson()
    endif

    LeftItem = none
    LeftSpell = none
    Busy = false
    LeftHand = false
    PlayerRef.SetAnimationVariablebool("bSprintOK", true)
    UnregisterForHitEventEx(self)
EndEvent


Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
    if TogglePlayerStopAnimation.GetValueint() == 1
        Sound.StopInstance(InstanceID)
        Debug.SendAnimationEvent(PlayerRef, "offsetstop")
        Debug.SendAnimationEvent(PlayerRef, "AnimObjectUnequip")
    endif
    if TogglePlayerStagger.GetValueint() == 1
        Debug.SendAnimationEvent(PlayerRef, "StaggerStart")
    endif
EndEvent


;Functions
Function FastDrinking()
    InstanceID = FastPotionSM.Play(PlayerRef)
    AnimationEventstring = "Fast"+AnimationEventstring
    Debug.SendAnimationEvent(PlayerRef, AnimationEventstring)
    Utility.Wait(2.4)
    EquipLeftItemAndWeapon(PlayerRef)
EndFunction


Function SlowDrinking()
    InstanceID = PotionSM.Play(PlayerRef)
    Debug.SendAnimationEvent(PlayerRef, AnimationEventstring)
    SlowPotionFunctions(PlayerRef)
    if LeftHand == true
        EquipLeftHand(PlayerRef)
    endif
EndFunction


Function StringManipulation()
    if StringUtil.Find(AnimationEventString, "FortifySkill", 0) >= 0 \
    || StringUtil.Find(AnimationEventString, "TGPotion", 0) >= 0
        AnimationEventString = "fortifyskill01"
    endif

    if StringUtil.Find(AnimationEventString, "MS12", 0) >= 0
        AnimationEventString = "ms12whitephialrepaired"
    endif
EndFunction


Function ModPrefixRevert()
    if StringUtil.Find(AnimationEventString, "mag_", 0) >= 0
        AnimationEventString = StringUtil.Substring(AnimationEventString, 4)
    endif
    if StringUtil.Find(AnimationEventString, "CACO_", 0) >= 0
        AnimationEventString = StringUtil.Substring(AnimationEventString, 5)
    endif
    if StringUtil.Find(AnimationEventString, "_KRY", 0) >= 0
        int Lenght = StringUtil.GetLength(AnimationEventString)
        int Cut_Length = Lenght - 4
        AnimationEventString = StringUtil.Substring(AnimationEventString, 0, Cut_Length)
    endif
EndFunction


Function FallBackFunction()
    if ToggleFallbackFunction.GetValueInt() == 1
        if JsonUtil.StringListHas("potionlist", "potions", AnimationEventstring) == false
            AnimationEventstring = "restorehealth01"
        endif
    endif
EndFunction


Function WaitForMenuClose()
    if ToggleWaitForMenuClose.GetValueInt() == 1
        While Utility.IsInMenuMode()
            Utility.wait(0.1)
        endwhile
    endif
EndFunction


bool Function CastSlowEffectSP(Actor akTarget)
    if TogglePlayerSlowEffect.GetValueint() == 1
        SlowEffectSP.RemoteCast(akTarget, none)
        Return true
    else
        Return false
    endif
EndFunction


bool Function ForceThirdPerson(Actor akTarget)
    if akTarget.GetAnimationVariableint("i1stPerson") == 1 && ToggleForceThirdPerson.GetValueint() == 1
        Game.ForceThirdPerson()
        Return true
    else
        Return false
    endif
EndFunction


Function ForceCloseMenu(Actor akTarget)
    if Ui.isMenuOpen("inventoryMenu") == true && ToggleForceCloseMenu.GetValueint() == 1
        Game.DisablePlayerControls(false, false, false, false, false, true, false)
        utility.wait(0.05)
        Game.EnablePlayerControls(false, false, false, false, false, true, false)
    endif
EndFunction


Function StopSprinting(Actor akTarget)
    if ToggleStopSprint.GetValueint() == 1 && akTarget.IsSprinting()
        Game.DisablePlayerControls(false, false, false, false, false, true, false)
        utility.wait(0.05)
        Game.EnablePlayerControls(false, false, false, false, false, true, false)
    endif
EndFunction


Function EquipLeftHand(Actor akTarget)
    if LeftItem != none
        akTarget.equipItemEx(LeftItem, 2)
    elseif LeftSpell != none
        akTarget.EquipSpell(LeftSpell, 0)
    endif
EndFunction


Function EquipLeftitemAndWeapon(Actor akTarget)
    if Leftitem != none
        akTarget.equipitemEx(Leftitem, 2)
    elseif LeftSpell != none
        akTarget.EquipSpell(LeftSpell, 0)
    elseif HasTwoHandedMelee(akTarget) == true
        akTarget.SheatheWeapon()
        akTarget.DrawWeapon()
    elseif akTarget.GetEquippeditemType(0) == 7; Has bow
        akTarget.DrawWeapon()
    elseif HasRightOneHandedMelee(akTarget) == true
        akTarget.equipitem(Fakeitem, false, true) ;Equip and unequip invisible shield to skip one handed redrawing
        Utility.wait(0.05)
        akTarget.Unequipitem(Fakeitem, false, true)
    else ;precaution
        akTarget.SheatheWeapon()
        akTarget.DrawWeapon()
    endif
EndFunction


bool Function HasRightOneHandedMelee(Actor akTarget)
    if akTarget.GetEquippedItemType(1) == 1 \
    || akTarget.GetEquippedItemType(1) == 2 \
    || akTarget.GetEquippedItemType(1) == 3 \
    || akTarget.GetEquippedItemType(1) == 4 \
    || akTarget.GetEquippedItemType(1) == 8
        Return true
    else
        Return false
    endif
EndFunction


bool Function HasTwoHandedMelee(Actor akTarget)
    if akTarget.GetEquippedItemType(1) == 5 \
    || akTarget.GetEquippedItemType(1) == 6 \
    || akTarget.GetEquippedItemType(0) == 5 \
    || akTarget.GetEquippedItemType(0) == 6
        Return true
    else
        Return false
    endif
EndFunction


bool Function LeftHandCheck(Actor akTarget)
    if akTarget.GetEquippeditemType(0) == 9; Left hand magic spell
        LeftSpell = akTarget.GetEquippedSpell(0)
        akTarget.UnequipSpell(LeftSpell, 0)
        Return true
    elseif HasLefthandeditem(akTarget) == true
        Leftitem = akTarget.GetEquippedObject(0) as Form
        akTarget.Unequipitem(Leftitem, false, true)
        Return true
    elseif akTarget.GetEquippeditemType(0) == 7; Has Bow
        SheatheWeaponAndWait(akTarget)
    endif
    Return false
EndFunction


bool Function TargetConditionCheck(Actor akTarget)
    if Busy == false \
    && akTarget.GetAnimationVariablebool("bIsRiding") == false \
    && akTarget.IsSwimming() == false \
    && akTarget.GetAnimationVariablebool("isStaggering") == false \
    && akTarget.GetAnimationVariablebool("bAnimationDriven") == false \
    && akTarget.IsInKillMove() == false \
    && akTarget.GetAnimationVariablebool("IsBleedingOut") == false \
    && GetActorKnockState(akTarget) == 0
        if UI.IsMenuOpen("InventoryMenu") == true \
        || UI.IsMenuOpen("FavoritesMenu") == true \
        || UI.IsMenuOpen("HUD Menu") == true
            Return true
        endif
        Return false
    else
        Return false
    endif
EndFunction


bool Function HasLefthandedItem(Actor akTarget)
    if akTarget.GetEquippedItemType(0) == 1 || akTarget.GetEquippedItemType(0) == 2 \
    || akTarget.GetEquippedItemType(0) == 3 || akTarget.GetEquippedItemType(0) == 4 \
    || akTarget.GetEquippedItemType(0) == 8 || akTarget.GetEquippedItemType(0) == 10 \
    || akTarget.GetEquippedItemType(0) == 11
        Return true
    else
        Return false
    endif
EndFunction


bool Function DrawWeaponAndWait(Actor akTarget)
    if akTarget.IsWeaponDrawn() == false
        akTarget.DrawWeapon()
        While akTarget.GetAnimationVariablebool("IsEquipping") == true
            Utility.wait(0.01)
        EndWhile
        Return true
    else
        Return false
    endif
EndFunction


bool Function SheatheWeaponAndWait(Actor akTarget)
    if akTarget.IsWeaponDrawn() == true
        akTarget.SheatheWeapon()
        While akTarget.GetAnimationVariablebool("IsUnEquipping") == true
            Utility.wait(0.1)
        endWhile
        Return true
    else
        Return false
    endif
endFunction


; Timed cork art object spell casting. Cork spell is picked by AnimationEventString suffix or prefix.
;Cork numbers:
; 0 for lesser potion corks
; 1 for extra potion corks
; 2 for great potion corks
; 3 for extreme potion corks
; 4 for skill potion corks
; 5 for blood potion corks
; 6 for resist 50 corks
; 7 for resist 100 corks
; 8 for SleepingTreeSap cork
; 9 for whitephial cork
; Some potions are in different order, meaning extra is after great or vice versa, so this is not ideal.
Function SlowPotionFunctions(Actor akTarget)
    int CorkSpellIndex = 0
    if StringUtil.Find(AnimationEventString, "01", 0) >= 0
        CorkSpellIndex = 0
    ElseIf StringUtil.Find(AnimationEventString, "03", 0) >= 0
        CorkSpellIndex = 1
    ElseIf StringUtil.Find(AnimationEventString, "02", 0) >= 0
        CorkSpellIndex = 2
    ElseIf StringUtil.Find(AnimationEventString, "04", 0) >= 0
        CorkSpellIndex = 3
    elseif StringUtil.Find(AnimationEventString, "FortifySkill", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "blood", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "25", 0) >= 0 \
    || StringUtil.Find(AnimationEventString, "50", 0) >= 0 \
    || StringUtil.Find(AnimationEventString, "75", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "100", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "sleepingtreesap", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(AnimationEventString, "ms12", 0) >= 0
        CorkSpellIndex = 4
    endif
    Spell CorkSpell = CorkFormList.GetAt(CorkSpellIndex) as Spell
    Spell CorkHandSpell = CorkHandFormList.GetAt(CorkSpellIndex) as Spell
    ;Cork in bottle
    (CorkSpell).RemoteCast(akTarget, none)
    ;Cork from bottle to hand
    Utility.Wait(0.56)
    (CorkHandSpell).RemoteCast(akTarget, none)
    akTarget.DispelSpell(CorkSpell)
    ;Cork from hand to bottle
    Utility.Wait(2.26)
    (CorkSpell).RemoteCast(akTarget, none)
    akTarget.DispelSpell(CorkHandSpell)
    ;Bottle disappear with cork
    Utility.Wait(0.52)
    akTarget.DispelSpell(CorkSpell)
endFunction
