Scriptname AnimatedPoisonsScript extends ReferenceAlias

import PO3_SKSEFunctions
import po3_Events_Alias

Actor property PlayerRef auto
Armor property Fakeitem auto
Sound property PoisonSound auto
Ammo property DaedricArrow auto
Ammo property DraugrArrow auto
Ammo property DwarvenArrow auto
Ammo property EbonyArrow auto
Ammo property ElvenArrow auto
Ammo property FalmerArrow auto
Ammo property ForswornArrow auto
Ammo property GlassArrow auto
Ammo property ironArrow auto
Ammo property NordHeroArrow auto
Ammo property OrchishArrow auto
Ammo property SteelArrow auto
Spell property DaedricArrowAOSP auto
Spell property DraugrArrowAOSP auto
Spell property DwarvenArrowAOSP auto
Spell property EbonyArrowAOSP auto
Spell property ElvenArrowAOSP auto
Spell property FalmerArrowAOSP auto
Spell property ForswornArrowAOSP auto
Spell property GlassArrowAOSP auto
Spell property ironArrowAOSP auto
Spell property NordHeroArrowAOSP auto
Spell property OrcishArrowAOSP auto
Spell property SteelArrowAOSP auto
GlobalVariable property ToggleStopSprint auto
GlobalVariable property ToggleForceThirdPerson auto
GlobalVariable property ToggleForceCloseMenu auto
GlobalVariable property TogglePlayerSlowEffect auto
GlobalVariable property TogglePlayerSound auto
GlobalVariable property PlayerStaggerToggle auto
GlobalVariable property PlayerStopAnimation auto
MiscObject property SlowEffectitem auto
Spell property SlowEffectSP auto
FormList property List auto
form Leftitem
Spell LeftSpell
bool Busy
int PoisonsUsed
int instanceiD
string property POiSON_DOSE_LiST_FiLENAME = "PoisonDoseList" autoReadOnly
string property POiSON_LiST_FiLENAME = "PoisonList" autoReadOnly

;To-do-list
; - Remove Corks from poison that still have them
; - Hunterborn and CC patch
; - DLC arrorws

Event Oninit()
    if PlayerRef.GetitemCount(Fakeitem) == 0
        PlayerRef.Additem(Fakeitem, 1, true)
    endif
EndEvent


Event OnPLayerLoadGame()
    if PlayerRef.GetitemCount(Fakeitem) == 0
        PlayerRef.Additem(Fakeitem, 1, true)
    endif
    Busy = false
    Leftitem = none
    LeftSpell = none
    PoisonsUsed = Game.QueryStat("Poisons Used")
    RegisterForModEvent("NPCsUsePotions_AnimatedPoisonsEvent", "NPCsUsePotions_ItemRemoved")
    RegisterForModEvent("NPCsUsePotions_AnimatedPoisonsHitEvent", "NPCsUsePotions_PoisonsOnHit")
EndEvent

Function NPCsUsePotions_ItemRemoved(string eventName, string strArg, float numArg, Form sender)
    if (eventName == "NPCsUsePotions_AnimatedPoisonsEvent" && strArg != "" && sender != None)
        Actor act = sender as Actor
        if (act)
            form akBaseItem = Game.GetForm(strArg as Int)
            debug.trace("[NPCsAnimatedPoisons] strArg " + akBaseItem)
            string PoisonKeystring = GetFormEditoriD(akBaseItem)
            debug.trace("[NPCsAnimatedPoisons] " + PoisonKeystring)
            string AnimationEventstring = FetchAnimationEventstring(PoisonKeystring, akBaseItem, POiSON_LiST_FiLENAME);JsonUtil.stringListGet(POiSON_LiST_FiLENAME, PoisonKeystring, 0) ;Everything lowercase in Json. Ctrl + shift + p "Transfrom to lowercase"
            ;int PoisonDoseCount = JsonUtil.intListGet(POiSON_DOSE_LiST_FiLENAME, PoisonKeystring, 0)
            debug.trace("[NPCsAnimatedPoisons] " + AnimationEventstring)
            if EmptyAnimationEventstring(AnimationEventstring) == false
                form actLeftitem
                Spell actLeftSpell
                act.SetAnimationVariablebool("bSprintOK", false)
                StopSprinting(act)
                if act.GetEquippeditemType(0) != 7
                    act.DrawWeapon()
                    while act.GetAnimationVariablebool("isEquipping") == true
                        Utility.wait(0.1)
                    endwhile
                endif
                bool LeftHand = false
                ; left hand check
                if act.GetEquippeditemType(0) == 9; Left hand magic spell
                    actLeftSpell = act.GetEquippedSpell(0)
                    act.UnequipSpell(actLeftSpell, 0)
                    LeftHand = true
                elseif HasLefthandeditem(act) == true
                    actLeftitem = act.GetEquippedObject(0) as Form
                    act.Unequipitem(actLeftitem, false, true)
                    LeftHand =  true
                elseif act.GetEquippeditemType(0) == 7; Has Bow
                    SheatheWeaponAndWait(act)
                    ChooseArrowAnimObject(act)
                endif

                bool SlowEffect = SlowEffectCast(act, SlowEffectitem)
                Debug.SendAnimationEvent(act, AnimationEventstring)
                NPCsUsePotions_Poisons.AnimatedPoisons_PlaySound(act)
                Utility.wait(2.6)

                if SlowEffect == true
                    act.DispelSpell(SlowEffectSP)
                    Utility.wait(0.03)
                    act.Removeitem(SlowEffectitem, 1, true)
                endif
                ; equip left item and weapon
                if actLeftitem != none
                    act.equipitemEx(actLeftitem, 2)
                elseif actLeftSpell != none
                    act.EquipSpell(actLeftSpell, 0)
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
                endif
                if (act != PlayerRef)
                    actLeftitem = act.GetEquippedWeapon(false)
                    actLeftSpell = act.GetEquippedSpell(1)
                    if (actLeftitem)
                        act.UnequipItemEx(actLeftItem, 1)
                        act.EquipItemEx(actLeftItem, 1)
                    elseif (actLeftSpell)
                        act.UnequipSpell(actLeftSpell, 1)
                        act.EquipSpell(actleftSpell, 1)
                    endif
                else
                    act.SheatheWeapon()
                    act.DrawWeapon()
                endif
                act.SetAnimationVariablebool("bSprintOK", true)
                NPCsUsePotions_Poisons.AnimatedPoisons_Callback(act)
            endif
        endif
    endif
EndFunction

Event NPCsUsePotions_PoisonsOnHit(string eventName, string strArg, float numArg, Form sender)
    Actor act = sender as Actor
    if (act)
        if PlayerStopAnimation.GetValueint() == 1
            NPCsUsePotions_Poisons.AnimatedPoisons_Abort(act)
            Debug.SendAnimationEvent(act, "offsetstop")
            Debug.SendAnimationEvent(act, "AnimObjectUnequip")
        endif
        if PlayerStaggerToggle.GetValueint() == 1
            Debug.SendAnimationEvent(act, "StaggerStart")
        endif
    endif
EndEvent

Event OnitemRemoved(Form akBaseitem, int aiitemCount, ObjectReference akitemReference, ObjectReference akDestContainer)
    int EventPoisonCount = Game.QueryStat("Poisons Used")
    if EventPoisonCount > PoisonsUsed
        PoisonsUsed = EventPoisonCount
        if TargetConditionCheck(PlayerRef) == true
            if Busy == false
                string PoisonKeystring = GetFormEditoriD(akBaseitem)
                string AnimationEventstring = FetchAnimationEventstring(PoisonKeystring, akBaseitem, POiSON_LiST_FiLENAME);JsonUtil.stringListGet(POiSON_LiST_FiLENAME, PoisonKeystring, 0) ;Everything lowercase in Json. Ctrl + shift + p "Transfrom to lowercase"
                int PoisonDoseCount = JsonUtil.intListGet(POiSON_DOSE_LiST_FiLENAME, PoisonKeystring, 0)
                if EmptyAnimationEventstring(AnimationEventstring) == false
                    if Ui.isMenuOpen("inventoryMenu") == true || Ui.isMenuOpen("FavoritesMenu") == true || Ui.isMenuOpen("HUD Menu") == true
                        Busy = true
                        RegisterForHitEventEx(self)
                        PlayerRef.SetAnimationVariablebool("bSprintOK", false)
                        ForceCloseMenu(PlayerRef)
                        WaitForMenuClose()
                        StopSprinting(PlayerRef)
                        if PoisonDoseCount > 0
                            SetEquippedWeaponPoisonCount(PlayerRef, PoisonDoseCount, false)
                        endif
                        if PlayerRef.GetEquippeditemType(0) != 7
                            DrawWeaponAndWait(PlayerRef)
                        endif
                        bool FirstPerson = ForceThirdPerson(PlayerRef)
                        bool LeftHand = LeftHandCheck(PlayerRef)
                        bool SlowEffect = SlowEffectCast(PlayerRef, SlowEffectitem)
                        Debug.SendAnimationEvent(PlayerRef, AnimationEventstring)
                        PlaySound(PlayerRef, PoisonSound)
                        Utility.wait(2.6)
                        EquipLeftitemAndWeapon(PlayerRef)
                        if SlowEffect == true
                            PlayerRef.DispelSpell(SlowEffectSP)
                            Utility.wait(0.03)
                            PlayerRef.Removeitem(SlowEffectitem, 1, true)
                        endif
                        if FirstPerson == true
                            Game.ForceFirstPerson()
                        endif
                        Leftitem = none
                        LeftSpell = none
                        Busy = false
                        PlayerRef.SetAnimationVariablebool("bSprintOK", true)
                        UnregisterForHitEventEx(self)
                    endif
                endif
            endif
        endif
    endif
EndEvent


Event OnHitEx(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
    if Busy == true
        if PlayerStopAnimation.GetValueint() == 1
            Sound.Stopinstance(instanceiD)
            Debug.SendAnimationEvent(PlayerRef, "offsetstop")
            Debug.SendAnimationEvent(PlayerRef, "AnimObjectUnequip")
        endif
        if PlayerStaggerToggle.GetValueint() == 1
            Debug.SendAnimationEvent(PlayerRef, "StaggerStart")
        endif
    endif
EndEvent


;Functions

; Script waits until menu is closed.
Function WaitForMenuClose()
    while Utility.isinMenuMode()
        Utility.wait(0.1)
    endwhile
EndFunction

; Casts slow effect on target and adds an item with weight, so engine has to refresh target speed
bool Function SlowEffectCast(Actor akTarget, MiscObject SlowEffectitem)
    if TogglePlayerSlowEffect.GetValueint() == 1
        SlowEffectSP.RemoteCast(akTarget, none)
        Utility.wait(0.01)
        akTarget.Additem(SlowEffectitem, 1, true)
        Return true
    else
        Return false
    endif
EndFunction

; Forces player to 3rd person view and returns to 1st person view if the option is enabled
bool Function ForceThirdPerson(Actor akTarget)
    if akTarget.GetAnimationVariableint("i1stPerson") == 1 && ToggleForceThirdPerson.GetValueint() == 1
        Game.ForceThirdPerson()
        Return true
    else
        Return false
    endif
EndFunction

; Forcibly closes menu when player is in menu and has the option enabled
Function ForceCloseMenu(Actor akTarget)
    if Ui.isMenuOpen("inventoryMenu") == true && ToggleForceCloseMenu.GetValueint() == 1
        Game.DisablePlayerControls(false, false, false, false, false, true, false)
        utility.wait(0.1)
        Game.EnablePlayerControls(false, false, false, false, false, true, false)
    endif
EndFunction

; Plays sound if the option is enabled
Function PlaySound(Actor akTarget, Sound SoundEffect)
    if  TogglePlayerSound.GetValueint() == 1
        instanceiD = SoundEffect.Play(akTarget)
    endif
EndFunction

; Stops target from sprinting if the option is enabled
Function StopSprinting(Actor akTarget)
    if ToggleStopSprint.GetValueint() == 1 && akTarget.isSprinting()
        akTarget.SetDontMove(1)
        Utility.Wait(1)
        akTarget.SetDontMove(0)
    endif
EndFunction

;Returns true when AnimationEventstring is empty ("")
bool Function EmptyAnimationEventstring(string AnimationEventstring)
    if AnimationEventstring == ""
        Return true
    else
        Return false
    endif
EndFunction

;Equips left item, spell or weapon.
Function EquipLeftHand(Actor akTarget)
    if Leftitem != none
        akTarget.equipitemEx(Leftitem, 2)
    elseif LeftSpell != none
        akTarget.EquipSpell(LeftSpell, 0)
    endif
EndFunction

;Equips left item, spell or weapon and handles drawing them
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

; Returns whether target has one handed melee or staff equipped in right hand
bool Function HasRightOneHandedMelee(Actor akTarget)
    if akTarget.GetEquippeditemType(1) == 1 || akTarget.GetEquippeditemType(1) == 2 || \
    akTarget.GetEquippeditemType(1) == 3 || akTarget.GetEquippeditemType(1) == 4 || akTarget.GetEquippeditemType(1) == 8
        Return true
    else
        Return false
    endif
EndFunction

; Returns whether target has two handed melee equipped
bool Function HasTwoHandedMelee(Actor akTarget)
    if akTarget.GetEquippeditemType(1) == 5 || akTarget.GetEquippeditemType(1) == 6 \
    || akTarget.GetEquippeditemType(0) == 5 || akTarget.GetEquippeditemType(0) == 6
        Return true
    else
        Return false
    endif
EndFunction

; Searchs left hand for item/spell/bow.
; Unequips magic and items
; Sheathes bows if not in combat or not drawn, else save form and unequips
; Returns true if any item/weapon/spell was found.
; Bow unequipping will hold script until it's completely sheathed.
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
        ChooseArrowAnimObject(akTarget)
    else
        Return false
    endif
EndFunction

; Returns true whether target is not currently riding, swimming, staggering, in a killmove or bleeding out
bool Function TargetConditionCheck(Actor akTarget)
    if akTarget.GetAnimationVariablebool("bisRiding") == false && akTarget.isSwimming() == false && akTarget.GetAnimationVariablebool("isStaggering") == false \
    && akTarget.GetAnimationVariablebool("bAnimationDriven") == false && akTarget.isinKillMove() == false && akTarget.GetAnimationVariablebool("isBleedingOut") == false \
    && GetActorKnockState(akTarget) == 0
        Return true
    else
        Return false
    endif
EndFunction

; Returns whether target has one handed melee, staff, shield or torch equipped in left hand
bool Function HasLefthandeditem(Actor akTarget)
    if akTarget.GetEquippeditemType(0) == 1 || akTarget.GetEquippeditemType(0) == 2 || akTarget.GetEquippeditemType(0) == 3 || akTarget.GetEquippeditemType(0) == 4 \
    || akTarget.GetEquippeditemType(0) == 8 || akTarget.GetEquippeditemType(0) == 10  || akTarget.GetEquippeditemType(0) == 11
        Return true
    else
        Return false
    endif
EndFunction

; Draws currently equipped weapon and waits until drawing animation has finished
Function DrawWeaponAndWait(Actor akTarget)
    akTarget.DrawWeapon()
    while akTarget.GetAnimationVariablebool("isEquipping") == true
        Utility.wait(0.1)
    endwhile
EndFunction

; Sheaths currently equipped weapon and waits until sheathing animation has finished. Returns true if weapon was sheathed and false if it already was.
bool Function SheatheWeaponAndWait(Actor akTarget)
    if akTarget.isWeaponDrawn() == true
        akTarget.SheatheWeapon()
        while akTarget.GetAnimationVariablebool("isUnEquipping") == true
            Utility.wait(0.1)
        endwhile
        Return true
    else
        Return false
    endif
EndFunction

; Returns animation event string from a Json with given key
string Function FetchAnimationEventstring(string KeyName, Form akBaseitem , string JsonFileName)
    string Fetchedstring = JsonUtil.stringListGet(JsonFileName, KeyName, 0) ;if not found returns ""
    if Fetchedstring == "" && akBaseitem.HasKeywordstring("VendoritemPoison") ;Fallback system
        Fetchedstring = "poisondamagehealth02"
    endif
    Return Fetchedstring
EndFunction

; Checks equipped arrows and casts an art object spell accordingly.
Function ChooseArrowAnimObject(Actor akTarget)
    if akTarget.isEquipped(DaedricArrow)
        DaedricArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(DraugrArrow)
        DraugrArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(DwarvenArrow)
        DwarvenArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(EbonyArrow)
        EbonyArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(ElvenArrow)
        ElvenArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(FalmerArrow)
        FalmerArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(ForswornArrow)
        ForswornArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(GlassArrow)
        GlassArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(ironArrow)
        ironArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(NordHeroArrow)
        NordHeroArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(OrchishArrow)
        OrcishArrowAOSP.RemoteCast(akTarget, none)
    elseif akTarget.isEquipped(SteelArrow)
        SteelArrowAOSP.RemoteCast(akTarget, none)
    else
        ironArrowAOSP.RemoteCast(akTarget, none)
    endif
EndFunction

;Poison dose list populating function for when poison FormEditoriDs change.
Function PoisonJsonPopulate()
    int i
    i = 0
    while i <= 268
        string keystring = JsonUtil.stringListGet("PoisonNames", "PoisonFormEditoriDs", i)
        JsonUtil.intListAdd("PoisonDoseList", keystring, 0, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList2", keystring, 2, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList3", keystring, 3, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList4", keystring, 4, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList5", keystring, 5, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList6", keystring, 6, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList7", keystring, 7, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList8", keystring, 8, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList9", keystring, 9, true)
        Utility.wait(0.01)
        JsonUtil.intListAdd("PoisonDoseList10", keystring, 10, true)
        Utility.wait(0.01)
        i += 1
    endwhile
EndFunction
