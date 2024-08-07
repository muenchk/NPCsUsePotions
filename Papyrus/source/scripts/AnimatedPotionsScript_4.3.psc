Scriptname AnimatedPotionsScript extends ReferenceAlias

Import po3_SKSEFunctions
Import po3_Events_Alias

Actor property PlayerRef auto
Armor property FakeItem auto
Sound property PotionSound auto
Sound property FastPotionSound auto
FormList property CorkFormList auto
FormList property CorkHandFormList auto
GlobalVariable property ToggleStopSprint auto
GlobalVariable property ToggleForceThirdPerson auto
GlobalVariable property ToggleForceCloseMenu auto
GlobalVariable property TogglePlayerSlowEffect auto
GlobalVariable property TogglePlayerSound auto
GlobalVariable property PlayerStaggerToggle auto
GlobalVariable property PlayerStopAnimation auto
MiscObject property SlowEffectItem auto
Spell property SlowEffectSP auto
FormList property List auto
form LeftItem
Spell LeftSpell
bool Busy
int PotionsUsed
int InstanceID
string property POTION_LIST_FILENAME = "PotionList" autoReadOnly
string property CORK_LIST_FILENAME = "CorkList" autoReadOnly


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
    LeftItem = None
    LeftSpell = None
    PotionsUsed = Game.QueryStat("Potions Used")
    RegisterForModEvent("NPCsUsePotions_AnimatedPotionsEvent", "NPCSUsePotions_ItemRemoved")
    RegisterForModEvent("NPCsUsePotions_AnimatedPotionsHitEvent", "NPCsUsePotions_PotionsOnHit")
EndEvent

Event NPCSUsePotions_ItemRemoved(string eventName, string strArg, float numArg, Form sender)
    if (eventName == "NPCsUsePotions_AnimatedPotionsEvent" && strArg != "" && sender != None)
        Actor act = sender as Actor
        if (act && act != PlayerRef)
            if TargetConditionCheck(act) == true
                form akBaseItem = Game.GetForm(strArg as Int)
                string PotionKeystring = GetFormEditorID(akBaseItem)
                string AnimationEventstring = FetchAnimationEventstring(PotionKeystring, akBaseItem, POTION_LIST_FILENAME)
                int CorkSpellIndex = JsonUtil.intListGet(CORK_LIST_FILENAME, AnimationEventstring, 0)
                if EmptyAnimationEventstring(AnimationEventstring) == false
                    act.SetAnimationVariablebool("bSprintOK", false)
                    bool LeftHand = LeftHandCheck(act)

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

                    StopSprinting(act)

                    bool slow = false
                    if TogglePlayerSlowEffect.GetValueint() == 1
                        SlowEffectSP.RemoteCast(act, none)
                        Utility.wait(0.01)
                        act.AddItem(SlowEffectItem, 1, true)
                        slow = true
                    endif

                    if act.IsInCombat() || act.IsWeaponDrawn() == true ; Plays fast potion drink if player is in combat or weapon drawn
                        AnimationEventstring = "Fast"+AnimationEventstring
                        ;PlaySound(act, FastPotionSound)
                        NPCsUsePotions_Potions.AnimatedPotions_Callback(act)
                        Debug.SendAnimationEvent(act, AnimationEventstring)
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
                    else
                        PlaySound(act, PotionSound)
                        Debug.SendAnimationEvent(act, AnimationEventstring)
                        NPCsUsePotions_Potions.AnimatedPotions_Callback(act)
                        SlowPotionFunctions(act, CorkSpellIndex, CorkFormList, CorkHandFormList)
                        if left == true
                            if Lefti != none
                                act.equipItemEx(Lefti, 2)
                            elseif LeftSpell != none
                                act.EquipSpell(Lefts, 0)
                            endif
                        endif
                    endif
                    if slow == true
                        act.DispelSpell(SlowEffectSP)
                        Utility.wait(0.01)
                        act.RemoveItem(SlowEffectItem, 1, true)
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
                endif
            endif
        endif
    endif
EndEvent

Event NPCsUsePotions_PotionsOnHit(string eventName, string strArg, float numArg, Form sender)
    Actor act = sender as Actor
    if (act)
        if PlayerStopAnimation.GetValueint() == 1
            Debug.SendAnimationEvent(act, "offsetstop")
            Debug.SendAnimationEvent(act, "AnimObjectUnequip")
        endif
        if PlayerStaggerToggle.GetValueint() == 1
            Debug.SendAnimationEvent(act, "StaggerStart")
        endif
    endif
EndEvent

Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
    int EventPotionCount = Game.QueryStat("Potions Used")
    if EventPotionCount > PotionsUsed
        PotionsUsed = EventPotionCount
        if TargetConditionCheck(PlayerRef) == true
            if Busy == false
                string PotionKeystring = GetFormEditorID(akBaseItem)
                string AnimationEventstring = FetchAnimationEventstring(PotionKeystring, akBaseItem, POTION_LIST_FILENAME)
                int CorkSpellIndex = JsonUtil.intListGet(CORK_LIST_FILENAME, AnimationEventstring, 0)
                if EmptyAnimationEventstring(AnimationEventstring) == false
                    if UI.IsMenuOpen("InventoryMenu") == true || UI.IsMenuOpen("FavoritesMenu") == true || UI.IsMenuOpen("HUD Menu") == true
                        Busy = true
                        RegisterForHitEventEx(self)
                        PlayerRef.SetAnimationVariablebool("bSprintOK", false)
                        ForceCloseMenu(PlayerRef)
                        bool FirstPerson = ForceThirdPerson(PlayerRef)
                        bool LeftHand = LeftHandCheck(PlayerRef)
                        WaitForMenuClose()
                        StopSprinting(PlayerRef)
                        bool SlowEffect = SlowEffectCast(PlayerRef, SlowEffectItem)
                        if PlayerRef.IsInCombat() || PlayerRef.IsWeaponDrawn() == true ; Plays fast potion drink if player is in combat or weapon drawn
                            AnimationEventstring = "Fast"+AnimationEventstring
                            PlaySound(PlayerRef, FastPotionSound)
                            Debug.SendAnimationEvent(PlayerRef, AnimationEventstring)
                            Utility.Wait(2.4)
                            EquipLeftItemAndWeapon(PlayerRef)
                        else
                            PlaySound(PlayerRef, PotionSound)
                            Debug.SendAnimationEvent(PlayerRef, AnimationEventstring)
                            SlowPotionFunctions(PlayerRef, CorkSpellIndex, CorkFormList, CorkHandFormList)
                            if LeftHand == true
                                EquipLeftHand(PlayerRef)
                            endif
                        endif
                        if SlowEffect == true
                            PlayerRef.DispelSpell(SlowEffectSP)
                            Utility.wait(0.01)
                            PlayerRef.RemoveItem(SlowEffectItem, 1, true)
                        endif
                        if FirstPerson == true
                            Game.ForceFirstPerson()
                        endif
                        LeftItem = None
                        LeftSpell = None
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
            Sound.StopInstance(InstanceID)
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
    While Utility.IsInMenuMode()
        Utility.wait(0.1)
    endwhile
endFunction

; Casts slow effect on target and adds an item with weight, so engine has to refresh target speed
bool Function SlowEffectCast(Actor akTarget, MiscObject SlowEffectItem)
    if TogglePlayerSlowEffect.GetValueint() == 1
        SlowEffectSP.RemoteCast(akTarget, none)
        Utility.wait(0.01)
        akTarget.AddItem(SlowEffectItem, 1, true)
        Return true
    else
        Return false
    endif
endFunction

; Forces player to 3rd person view and returns to 1st person view if the option is enabled
bool Function ForceThirdPerson(Actor akTarget)
    if akTarget.GetAnimationVariableint("i1stPerson") == 1 && ToggleForceThirdPerson.GetValueint() == 1
        Game.ForceThirdPerson()
        Return true
    else
        Return false
    endif
endFunction

; Forcibly closes menu when player is in menu and has the option enabled
Function ForceCloseMenu(Actor akTarget)
    if UI.IsMenuOpen("InventoryMenu") == true && ToggleForceCloseMenu.GetValueint() == 1
        Game.DisablePlayerControls(false, false, false, false, false, true, false)
        utility.wait(0.1)
        Game.EnablePlayerControls(false, false, false, false, false, true, false)
    endif
endFunction

; Plays sound if the option is enabled
Function PlaySound(Actor akTarget, Sound SoundEffect)
    if  TogglePlayerSound.GetValueint() == 1
        InstanceID = SoundEffect.Play(akTarget)
    endif
endFunction

; Stops target from sprinting if the option is enabled
Function StopSprinting(Actor akTarget)
    if ToggleStopSprint.GetValueint() == 1 && akTarget.IsSprinting()
        akTarget.SetDontMove(1)
        Utility.Wait(1)
        akTarget.SetDontMove(0)
    endif
endFunction

;Returns true when AnimationEventstring is empty ("")
bool Function EmptyAnimationEventstring(string AnimationEventstring)
    if AnimationEventstring == ""
        Return true
    else
        Return false
    endif
endFunction

;Equips left item, spell or weapon.
Function EquipLeftHand(Actor akTarget)
    if LeftItem != None
        akTarget.equipItemEx(LeftItem, 2)
    elseif LeftSpell != None
        akTarget.EquipSpell(LeftSpell, 0)
    endif
endFunction

;Equips left item, spell or weapon and handles drawing them
Function EquipLeftItemAndWeapon(Actor akTarget)
    if LeftItem != None
        akTarget.equipItemEx(LeftItem, 2)
    elseif LeftSpell != None
        akTarget.EquipSpell(LeftSpell, 0)
    elseif HasTwoHandedMelee(akTarget) == true
        akTarget.SheatheWeapon()
        akTarget.DrawWeapon()
    elseif akTarget.GetEquippedItemType(0) == 7; Has bow
        akTarget.DrawWeapon()
    elseif HasRightOneHandedMelee(akTarget) == true
        akTarget.equipItem(FakeItem, false, true) ;Equip and unequip invisible shield to skip one handed redrawing
        Utility.wait(0.05)
        akTarget.UnequipItem(FakeItem, false, true)
    else ;precaution
        akTarget.SheatheWeapon()
        akTarget.DrawWeapon()
    endif
endFunction

; Returns whether target has one handed melee or staff equipped in right hand
bool Function HasRightOneHandedMelee(Actor akTarget)
    if akTarget.GetEquippedItemType(1) == 1 || akTarget.GetEquippedItemType(1) == 2 || \
    akTarget.GetEquippedItemType(1) == 3 || akTarget.GetEquippedItemType(1) == 4 || akTarget.GetEquippedItemType(1) == 8
        Return true
    else
        Return false
    endif
endFunction


; Returns whether target has two handed melee equipped
bool Function HasTwoHandedMelee(Actor akTarget)
    if akTarget.GetEquippedItemType(1) == 5 || akTarget.GetEquippedItemType(1) == 6 \
    || akTarget.GetEquippedItemType(0) == 5 || akTarget.GetEquippedItemType(0) == 6
        Return true
    else
        Return false
    endif
endFunction

; Searchs left hand for item/spell/bow.
; Unequips magic and items
; Sheathes bows if not in combat or not drawn, else save form and unequips
; Returns true if any item/weapon/spell was found.
; Bow unequipping will hold script until it's completely sheathed.
bool Function LeftHandCheck(Actor akTarget)
    if akTarget.GetEquippedItemType(0) == 9; Left hand magic spell
        LeftSpell = akTarget.GetEquippedSpell(0)
        akTarget.UnequipSpell(LeftSpell, 0)
        Return true
    elseif HasLefthandedItem(akTarget) == true
        LeftItem = akTarget.GetEquippedObject(0) as Form
        akTarget.UnequipItem(LeftItem, false, true)
        Return true
    elseif akTarget.GetEquippedItemType(0) == 7; Has Bow
        SheatheWeaponAndWait(akTarget)
    else
        Return false
    endif
endFunction

; Returns true whether target is not currently riding, swimming, staggering, in a killmove or bleeding out
bool Function TargetConditionCheck(Actor akTarget)
    if akTarget.GetAnimationVariablebool("bIsRiding") == false && akTarget.IsSwimming() == false && akTarget.GetAnimationVariablebool("isStaggering") == false \
    && akTarget.GetAnimationVariablebool("bAnimationDriven") == false && akTarget.IsInKillMove() == false && akTarget.GetAnimationVariablebool("IsBleedingOut") == false \
    && GetActorKnockState(akTarget) == 0
        Return true
    else
        Return false
    endif
endFunction

; Returns whether target has one handed melee, staff, shield or torch equipped in left hand
bool Function HasLefthandedItem(Actor akTarget)
    if akTarget.GetEquippedItemType(0) == 1 || akTarget.GetEquippedItemType(0) == 2 || akTarget.GetEquippedItemType(0) == 3 || akTarget.GetEquippedItemType(0) == 4 \
    || akTarget.GetEquippedItemType(0) == 8 || akTarget.GetEquippedItemType(0) == 10  || akTarget.GetEquippedItemType(0) == 11
        Return true
    else
        Return false
    endif
endFunction

; Draws currently equipped weapon and waits until drawing animation has finished
Function DrawWeaponAndWait(Actor akTarget)
    akTarget.DrawWeapon()
    While akTarget.GetAnimationVariablebool("IsEquipping") == true
        Utility.wait(0.1)
    endWhile
endFunction

; Sheaths currently equipped weapon and waits until sheathing animation has finished. Returns true if weapon was sheathed and false if it already was.
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

; Timed cork art object spell casting. Cork spell is picked via formlist with an index from Json
;Json cork spell numbers:
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
Function SlowPotionFunctions(Actor akTarget, int CorkSpellIndex, FormList ListofCorks, FormList ListofHandCorks)
    Spell CorkSpell = ListofCorks.GetAt(CorkSpellIndex) as Spell
    Spell CorkHandSpell = ListofHandCorks.GetAt(CorkSpellIndex) as Spell
    ;Cork in bottle
    (CorkSpell).RemoteCast(akTarget, None)
    ;Cork from bottle to hand
    Utility.Wait(0.56)
    (CorkHandSpell).RemoteCast(akTarget, None)
    akTarget.DispelSpell(CorkSpell)
    ;Cork from hand to bottle
    Utility.Wait(2.26)
    (CorkSpell).RemoteCast(akTarget, None)
    akTarget.DispelSpell(CorkHandSpell)
    ;Bottle disappear with cork
    Utility.Wait(0.52)
    akTarget.DispelSpell(CorkSpell)
endFunction

; Returns animation event string from a Json with given key
string Function FetchAnimationEventstring(string KeyName, Form akBaseItem , string JsonFileName)
    string Fetchedstring = JsonUtil.stringListGet(JsonFileName, KeyName, 0) ;if not found returns ""
    if Fetchedstring == "" && akBaseItem.HasKeywordstring("VendorItemPotion") ;Fallback system
        Fetchedstring = "PotionHealthLesser"
    endif
    Return Fetchedstring
endFunction

;Populating Json
Function stringListPopulating()
    int Quantity = List.GetSize()
    int Count = 0
    While Quantity >= Count
        string Value = GetFormEditorID(List.GetAt(Count))
        JsonUtil.stringListAdd("PotionListApothecary", Value, "", false)
        Count += 1
    endWhile
endFunction
