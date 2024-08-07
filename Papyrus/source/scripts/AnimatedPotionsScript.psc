Scriptname AnimatedPotionsScript extends ReferenceAlias

;Changelog:
; 5.0.0
; - Cleaned code for better readability and using better practices for more stable code.
; - Force close menu removed, because its implementation may interfere with other mods using the same function.
; - refactored some code to use local variables instead of always getting the value again and again by calling the function each time.
; this should free up computing time.
; - Fixed remote cast not casting Actor as ObjectReference, which lead to stack errors.
; - Fixed debug.sendanimationevent not casting Actor as ObjectReference. (This hasn't printed any errors but is good to change.)
; - Implemented some checks to functions that prevent getting stuck in a loop etc.
; - Added ReverBackToFPS that will revert camera back to first person if person was in 1st person before applying poison and ToggleForceThirdPerson was on.
; - Removed left hand item handling. It was pretty janky, so I'm currenty researching for skse plugin alternative for this. This also removes the use of temp ite.
; - Fixed nesting in main function by swapping it with a function call so I can use returns. ( Can't return in events )
; - Fixed ToggleSound not being checked, which lead to sound always firing.
; - Fixed sprint stop not checking if sprinting was already prevented, so that we don't interfere with some other mechanic that implemented that.
; - Added 'as ObjectReference' conversions to animation variable getters.
; - Added GetAnimationVariablebool("bIsSynced ") == false to check whether target is not mounting a horse or trying to sit in/get off a chair
; - Added wait for menu close function.
; - Added potion splashing effect
; - Added spawning empty bottles to the ground after potion use.
; - Update PlaySound() function.

Import po3_SKSEFunctions
Import po3_Events_Alias

Sound property PotionSM auto
Sound property FastPotionSM auto
Sound property potionDropSound auto
FormList property CorkFormList auto
FormList property CorkHandFormList auto
FormList property BlackListedPotions auto
GlobalVariable property ToggleStopSprint auto
GlobalVariable property ToggleForceThirdPerson auto
GlobalVariable property TogglePlayerSlowEffect auto
GlobalVariable property TogglePlayerStagger auto
GlobalVariable property TogglePlayerStopAnimation auto
GlobalVariable property ToggleDisableDuringCombat auto
GlobalVariable property ToggleWaitForMenuClose auto
GlobalVariable property ToggleRequireKeyword auto
GlobalVariable property toggleSpawnEmptyBottles auto
GlobalVariable property togglePotionSplash auto
GlobalVariable property ToggleReverBackToFPS auto
GlobalVariable property TogglePlayerSound auto
Spell property SlowEffectSP auto
Spell property FastPotionSplashSP auto
Spell property SlowPotionSplashSP auto
bool bBusy
int iPotionsUsed
int iPotionSoundInstance
Actor akPlayer
MiscObject property emptybottle auto


Event OnPLayerLoadGame()
    bBusy = false
    iPotionsUsed = Game.QueryStat("Potions Used")
    RegisterForModEvent("NPCsUsePotions_AnimatedPotionsEvent", "NPCSUsePotions_ItemRemoved")
    RegisterForModEvent("NPCsUsePotions_AnimatedPotionsHitEvent", "NPCsUsePotions_PotionsOnHit")
EndEvent

Event NPCSUsePotions_ItemRemoved(string eventName, string strArg, float numArg, Form sender)
    if (akPlayer == None)
        akPlayer = Game.GetPlayer()
    endif
    if (eventName == "NPCsUsePotions_AnimatedPotionsEvent" && strArg != "" && sender != None)
        Actor akTarget = sender as Actor
        if (akTarget && akTarget != akPlayer)
            form akBaseItem = Game.GetForm(strArg as Int)

            if BlackListedPotions.Find(akBaseItem) != -1
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif
            if ToggleDisableDuringCombat.GetValueInt() == 1 && akTarget.IsInCombat() == true
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif
            if TargetConditionCheck(akTarget, akBaseItem) == false
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif
            if akBaseItem.HasKeywordString("vendoritempotion") == false && ToggleRequireKeyword.GetValueInt() == 1
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif

            if WaitForMenuClose(60.0) == false; Wait for menu to be closed. Returning if we exceed the 60 second time limit that prevents getting stuck here.
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif

            bool bDrawSuccess = false
            bool bSheatheSuccess = false
            bool bFastDrinking = false

            if akTarget.IsInCombat() == true || akTarget.IsWeaponDrawn() == true
                bFastDrinking = true
            endif

            if bFastDrinking == true
                bDrawSuccess = DrawWeaponAndWait(akTarget,6)
            else
                bSheatheSuccess = SheatheWeaponAndWait(akTarget,6)
            endif

            if bDrawSuccess == false && bSheatheSuccess == false; Failing to draw or sheathe in 6s fails the poison apply. For example target gets staggered or yeeted to Oblivion.
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif

            bool bSlowEffect = CastSlowEffectSP(akTarget); Running helper functions
            bool bStopSprint = StopSprint(akTarget)

            string AnimationEventstring
            AnimationEventstring = GetFormEditorID(akBaseItem)
            AnimationEventstring = ModPrefixRevert(AnimationEventstring)
            AnimationEventstring = StringManipulation(AnimationEventstring)

            string path = akBaseitem.GetWorldModelPath(); update empty bottle mesh
            emptybottle.SetWorldModelPath(path)

            if bFastDrinking == true
                NPCsUsePotions_Potions.AnimatedPotions_Callback(aktarget)
                Debug.SendAnimationEvent(akTarget as ObjectReference, "Fast"+AnimationEventstring)
                if togglePotionSplash.GetValue() == 1
                    FastPotionSplashSP.Cast(akTarget as ObjectReference, none)
                endif
                Utility.Wait(2.4)
            else
                NPCsUsePotions_Potions.AnimatedPotions_Callback(aktarget)
                Debug.SendAnimationEvent(akTarget as ObjectReference, AnimationEventstring)
                int CorkSpellIndex = 0
                if StringUtil.Find(AnimationEventstring, "01", 0) >= 0
                    CorkSpellIndex = 0
                ElseIf StringUtil.Find(AnimationEventstring, "03", 0) >= 0
                    CorkSpellIndex = 1
                ElseIf StringUtil.Find(AnimationEventstring, "02", 0) >= 0
                    CorkSpellIndex = 2
                ElseIf StringUtil.Find(AnimationEventstring, "04", 0) >= 0
                    CorkSpellIndex = 3
                elseif StringUtil.Find(AnimationEventstring, "FortifySkill", 0) >= 0
                    CorkSpellIndex = 4
                elseif StringUtil.Find(AnimationEventstring, "blood", 0) >= 0
                    CorkSpellIndex = 4
                elseif StringUtil.Find(AnimationEventstring, "25", 0) >= 0 \
                || StringUtil.Find(AnimationEventstring, "50", 0) >= 0 \
                || StringUtil.Find(AnimationEventstring, "75", 0) >= 0
                    CorkSpellIndex = 4
                elseif StringUtil.Find(AnimationEventstring, "100", 0) >= 0
                    CorkSpellIndex = 4
                elseif StringUtil.Find(AnimationEventstring, "sleepingtreesap", 0) >= 0
                    CorkSpellIndex = 4
                elseif StringUtil.Find(AnimationEventstring, "ms12", 0) >= 0
                    CorkSpellIndex = 4
                endif
                Spell CorkSpell = CorkFormList.GetAt(CorkSpellIndex) as Spell
                Spell CorkHandSpell = CorkHandFormList.GetAt(CorkSpellIndex) as Spell
                ;Cork in bottle
                (CorkSpell).RemoteCast(akTarget as ObjectReference, none)
                ;Cork from bottle to hand
                Utility.Wait(0.56)
                (CorkHandSpell).RemoteCast(akTarget as ObjectReference, none)
                akTarget.DispelSpell(CorkSpell)
                ;Cork from hand to bottle
                if togglePotionSplash.GetValue() == 1
                    SlowPotionSplashSP.Cast(akTarget as ObjectReference, none)
                endif
                Utility.Wait(2.26)
                (CorkSpell).RemoteCast(akTarget as ObjectReference, none)
                akTarget.DispelSpell(CorkHandSpell)
                ;Bottle disappear with cork
                Utility.Wait(0.52)
                akTarget.DispelSpell(CorkSpell)
            endif

            if toggleSpawnEmptyBottles.GetValue() == 1 && bFastDrinking == true; Spawn empty bottle at calculated distance from target and play sound effect of bottle hitting the ground
                float distance = 100.0
                float zAngle = akTarget.GetAngleZ()
                float markerX = distance * math.sin(zAngle+300)
                float markerY = distance * math.cos(zAngle)
                float markerZ = 15
                ObjectReference bottleref = akTarget.PlaceAtMe(emptybottle as Form)
                bottleref.MoveTo(akTarget, markerX, markerY, markerZ)
                PlaySound(potionDropSound, akTarget)
            endif

            if toggleSpawnEmptyBottles.GetValue() == 1 && bFastDrinking == false
                akTarget.AddItem(emptybottle as Form, 1)
            endif

            if bStopSprint == true; Allow sprinting again
                (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", true)
            endif

            if bDrawSuccess == true; if we were fast drinking, draw weapon
                Form formItem = akTarget.GetEquippedObject(1); This is to fix the animation state getting stuck in non-drawn state while target thinks he's in drawn state
                akTarget.UnequipItem(formItem, false, true)
                akTarget.EquipItem(formItem, false, true)
                NPCsUsePotions_Potions.AnimatedPotions_RestorePoison(akTarget)
            endif
        endif
    endif
EndEvent

Event NPCsUsePotions_PotionsOnHit(string eventName, string strArg, float numArg, Form sender)
    Actor act = sender as Actor
    if (act)
        if TogglePlayerStopAnimation.GetValueint() == 1
            NPCsUsePotions_Potions.AnimatedPotions_Abort(act)
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "offsetstop")
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "AnimObjectUnequip")
        endif
        if TogglePlayerStagger.GetValueint() == 1
            Debug.SendAnimationEvent(act as ObjectReference, "StaggerStart")
        endif
    endif
EndEvent

Event OnItemRemoved(Form akBaseItem, int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)
    int EventPotionCount = Game.QueryStat("Potions Used")
    if EventPotionCount > iPotionsUsed
        iPotionsUsed = EventPotionCount
        akPlayer = Game.GetPlayer()
        PotionDrinking(akBaseitem, akPlayer)
    endif
EndEvent

bool Function PotionDrinking(Form akBaseitem, Actor akTarget)
    if BlackListedPotions.Find(akBaseItem) != -1
        return false
    endif

    if ToggleDisableDuringCombat.GetValueInt() == 1 && akTarget.IsInCombat() == true
        return false
    endif

    if TargetConditionCheck(akTarget, akBaseitem) == false
        return false
    endif

    if akBaseItem.HasKeywordString("vendoritempotion") == false && ToggleRequireKeyword.GetValueInt() == 1
        return false
    endif

    bBusy = true; Keeps track whether we are already drinking potion

    if WaitForMenuClose(60) == false; if for some reason we exited the menu but the script didn't notice, we don't get stuck here.
        bBusy = false
        return false
    endif

    bool bDrawSuccess = false
    bool bSheatheSuccess = false
    bool bFastDrinking = false
    if akTarget.IsInCombat() == true || akTarget.IsWeaponDrawn() == true
        bFastDrinking = true
    endif

    if bFastDrinking == true
        bDrawSuccess = DrawWeaponAndWait(akTarget,6)
    else
        bSheatheSuccess = SheatheWeaponAndWait(akTarget,6)
    endif

    if bDrawSuccess == false && bSheatheSuccess == false; Failing to draw or sheathe in 6s fails the poison apply. For example target gets staggered or yeeted to Oblivion.
        bBusy = false
        return false
    endif

    bool bSlowEffect = CastSlowEffectSP(akTarget); Running helper functions
    bool bStopSprint = StopSprint(akTarget)
    bool bFirstPerson = ForceThirdPerson(akTarget)

    string AnimationEventstring
    AnimationEventstring = GetFormEditorID(akBaseItem)
    AnimationEventstring = ModPrefixRevert(AnimationEventstring)
    AnimationEventstring = StringManipulation(AnimationEventstring)

    string path = akBaseitem.GetWorldModelPath(); update empty bottle mesh
    emptybottle.SetWorldModelPath(path)

    if bFastDrinking == true
        FastDrinking(AnimationEventstring ,akTarget)
    else
        SlowDrinking(AnimationEventstring, akTarget)
    endif



    if toggleSpawnEmptyBottles.GetValue() == 1 && bFastDrinking == true; Spawn empty bottle at calculated distance from target and play sound effect of bottle hitting the ground
        float distance = 100.0
        float zAngle = akTarget.GetAngleZ()
        float markerX = distance * math.sin(zAngle+300)
        float markerY = distance * math.cos(zAngle)
        float markerZ = 15
        ObjectReference bottleref = akTarget.PlaceAtMe(emptybottle as Form)
        bottleref.MoveTo(akTarget, markerX, markerY, markerZ)
        PlaySound(potionDropSound, akTarget)
    endif

    if toggleSpawnEmptyBottles.GetValue() == 1 && bFastDrinking == false
        akTarget.AddItem(emptybottle as Form, 1)
    endif

    if bFirstPerson == true && ToggleReverBackToFPS.GetValue() == 1; Bring target back to 1st person
        Game.ForceFirstPerson()
    endif

    if bStopSprint == true; Allow sprinting again
        (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", true)
    endif

    if bDrawSuccess == true; if we were fast drinking, draw weapon
        Form formItem = akTarget.GetEquippedObject(1); This is to fix the animation state getting stuck in non-drawn state while target thinks he's in drawn state
        akTarget.UnequipItem(formItem, false, true)
        akTarget.EquipItem(formItem, false, true)
    endif

    bBusy = false
    return true
EndFunction


Event OnHit(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
    if bBusy == true
        if TogglePlayerStopAnimation.GetValueint() == 1
            Sound.StopInstance(iPotionSoundInstance)
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "offsetstop")
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "AnimObjectUnequip")
        endif
        if TogglePlayerStagger.GetValueint() == 1
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "StaggerStart")
        endif
    endif
EndEvent


;Functions
Function FastDrinking(string eventstring, Actor akTarget)
    iPotionSoundInstance = PlaySound(FastPotionSM, akTarget)
    Debug.SendAnimationEvent(akTarget as ObjectReference, "Fast"+eventstring)
    if togglePotionSplash.GetValue() == 1
        FastPotionSplashSP.Cast(akTarget as ObjectReference, none)
    endif
    Utility.Wait(2.4)
EndFunction


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
Function SlowDrinking(string eventstring, Actor akTarget)
    iPotionSoundInstance = PotionSM.Play(akTarget)
    Debug.SendAnimationEvent(akTarget as ObjectReference, eventstring)
    int CorkSpellIndex = 0
    if StringUtil.Find(eventstring, "01", 0) >= 0
        CorkSpellIndex = 0
    ElseIf StringUtil.Find(eventstring, "03", 0) >= 0
        CorkSpellIndex = 1
    ElseIf StringUtil.Find(eventstring, "02", 0) >= 0
        CorkSpellIndex = 2
    ElseIf StringUtil.Find(eventstring, "04", 0) >= 0
        CorkSpellIndex = 3
    elseif StringUtil.Find(eventstring, "FortifySkill", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(eventstring, "blood", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(eventstring, "25", 0) >= 0 \
    || StringUtil.Find(eventstring, "50", 0) >= 0 \
    || StringUtil.Find(eventstring, "75", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(eventstring, "100", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(eventstring, "sleepingtreesap", 0) >= 0
        CorkSpellIndex = 4
    elseif StringUtil.Find(eventstring, "ms12", 0) >= 0
        CorkSpellIndex = 4
    endif
    Spell CorkSpell = CorkFormList.GetAt(CorkSpellIndex) as Spell
    Spell CorkHandSpell = CorkHandFormList.GetAt(CorkSpellIndex) as Spell
    ;Cork in bottle
    (CorkSpell).RemoteCast(akTarget as ObjectReference, none)
    ;Cork from bottle to hand
    Utility.Wait(0.56)
    (CorkHandSpell).RemoteCast(akTarget as ObjectReference, none)
    akTarget.DispelSpell(CorkSpell)
    ;Cork from hand to bottle
    if togglePotionSplash.GetValue() == 1
        SlowPotionSplashSP.Cast(akTarget as ObjectReference, none)
    endif
    Utility.Wait(2.26)
    (CorkSpell).RemoteCast(akTarget as ObjectReference, none)
    akTarget.DispelSpell(CorkHandSpell)
    ;Bottle disappear with cork
    Utility.Wait(0.52)
    akTarget.DispelSpell(CorkSpell)
EndFunction


; For some specific mods returns a specific string. Returns unchanged parameter string if nothing done.
string Function StringManipulation(string eventstring)
    if StringUtil.Find(eventstring, "FortifySkill", 0) >= 0 \
    || StringUtil.Find(eventstring, "TGPotion", 0) >= 0
        return "fortifyskill01"
    endif

    if StringUtil.Find(eventstring, "MS12", 0) >= 0
        return "ms12whitephialrepaired"
    endif
    return eventstring
EndFunction



; Removes any formID changes mods have made and returns it. Returns unchanged parameter string if nothing done.
string Function ModPrefixRevert(string eventstring)
    if StringUtil.Find(eventstring, "mag_", 0) >= 0
        return StringUtil.Substring(eventstring, 4)
    endif
    if StringUtil.Find(eventstring, "CACO_", 0) >= 0
        return StringUtil.Substring(eventstring, 5)
    endif
    if StringUtil.Find(eventstring, "_KRY", 0) >= 0
        int Lenght = StringUtil.GetLength(eventstring)
        int Cut_Length = Lenght - 4
        return StringUtil.Substring(eventstring, 0, Cut_Length)
    endif
    return eventstring
EndFunction


; Plays sound at target if TogglePlayerSound is enabled.
;
; Parameters:
; - TypeActor: The actor to play sound at.
;
; Return Value:
; - TypeBool: Returns true if the TogglePlayerSound was enabled; otherwise, false.
int Function PlaySound(Sound sfx, Actor akTarget)
    if TogglePlayerSound.GetValue() == 1
        return sfx.Play(akTarget as ObjectReference)
    endif
    return -1
EndFunction


; Checks if the target has enabled stop sprinting and if so, will stop and prevent sprinting for
; the duration of applying poison. Also checks if sprinting was allowed before applying poison to
; not unintentionally release target from some other sprint lock.
;
; Parameters:
; - TypeActor: The actor to stop and prevent sprinting.
;
; Return Value:
; - TypeBool: Returns true if the ToggleStopSprint was enabled; otherwise, false.
bool Function stopSprint(Actor akTarget)
    if ToggleStopSprint.GetValue() == 1 && (akTarget as ObjectReference).GetAnimationVariablebool("bSprintOK") == true
        (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", false)
        if akTarget.IsSprinting()
            akTarget.SetDontMove(true)
            utility.wait(0.5)
            akTarget.SetDontMove(false)
        endif
        return true
    endif

    return false
EndFunction


; Waits for a menu to close, up to a specified maximum wait time.
;
; Parameters:
; - TypeFloat: maxWaitTime - The maximum amount of time (in seconds) to wait for the menu to close.
;
; Return Value:
; - TypeBool: True if the menu closes within the specified time, false otherwise.
bool Function WaitForMenuClose(float maxWaitTime)
    float waitTime = 0
    if toggleWaitForMenuClose.GetValue() == 0
        return true
    endif
    while Utility.isinMenuMode() && waitTime < maxWaitTime
        Utility.wait(0.1)
        waitTime += 0.1
    endwhile
    if waitTime >= maxWaitTime
        return false
    endif

    return true
EndFunction


; Casts a slow effect on the specified target actor if ToggleSlowEffect is enabled.
;
; Parameters:
; - akTarget: Actor - The target actor on which to cast the slow effect.
;
; Return Value:
; - Bool: True if ToggleSlowEffect was enabled; otherwise, false.
bool Function CastSlowEffectSP(Actor akTarget)
    if TogglePlayerSlowEffect.GetValueint() == 1
        SlowEffectSP.RemoteCast(akTarget as ObjectReference,none)
        return true
    endif

    return false
EndFunction


; Forces the player into third-person view if is in 1st person and has ToggleForceThirdPerson enabled.
;
; Parameters:
; - TypeActor: Actor - The target actor for which to force third-person view.
;
; Return Value:
; - TypeBool: True if the conditions are met and third-person view is forced, false otherwise.
bool Function ForceThirdPerson(Actor akTarget)
    if (akTarget as ObjectReference).GetAnimationVariableint("i1stPerson") == 1 && ToggleForceThirdPerson.GetValueint() == 1
        Game.ForceThirdPerson()
        return true
    endif

    return false
EndFunction


; Checks whether the target is not:
; - Already applying poison
; - Riding a horse
; - Swimming
; - Staggered
; - in bAnimationDriven state
; - in a killmove
; - Bleeding out
; - Ragdolling
; - Not mounting or standing on/off chairs
; - Additional check for knockstate from PO3's functions (maybe unnescessary?)
;
; Additionally checks that target is in any of the following menus:
; - Favorite
; - Inventory
; - HUD menu
;
; Parameters:
; - TypeActor: The actor to be checked.
;
; Return Value:
; - TypeBool: Returns true if the target passes the checks; otherwise, returns false.
bool Function TargetConditionCheck(Actor akTarget, Form consumedPoison)
    if bBusy == false && (akTarget as ObjectReference).GetAnimationVariablebool("bIsRiding") == false && akTarget.IsSwimming() == false \
    && (akTarget as ObjectReference).GetAnimationVariablebool("isStaggering") == false && (akTarget as ObjectReference).GetAnimationVariablebool("bAnimationDriven") == false \
    && akTarget.IsInKillMove() == false && (akTarget as ObjectReference).GetAnimationVariablebool("IsBleedingOut") == false && (akTarget as ObjectReference).GetAnimationVariablebool("bIsSynced") == false \
    && GetActorKnockState(akTarget) == 0 && akTarget.GetMass() == 0
    ; For GetMass: "The object has to be controlled by Havok ("ragdolling") or this function will always return 0" which is how we get ragdoll state of target.
    ; While sitting on a chair bAnimationDriven is true
    ; While getting on a horse bIsSynced is true;/  && consumedPoison.HasKeywordString("VendorItemPoison") == true /; ;Things like frostbite venom don't have this tag so we don't use it.
        if UI.IsMenuOpen("InventoryMenu") == true || UI.IsMenuOpen("FavoritesMenu") == true || UI.IsMenuOpen("HUD Menu") == true
            Return true
        endif
    endif
    Return false
EndFunction


; Draws the target's weapon and waits until the target has finished equipping.
; Waits for a certain of maximum of seconds.
;
; Parameters:
; - TypeActor: The actor to draw their weapon.
; - TypeFloat: Maximun time in seconds that the function will wait.
;
; Return Value:
; - TypeBool: True if the target successfully drawed their weapon on time; otherwise, false.
bool Function DrawWeaponAndWait(Actor akTarget, float fMaxWaitTime)
    if akTarget.IsWeaponDrawn() == false
        akTarget.DrawWeapon()
        float fWaitTime = 0.0
        while (akTarget as ObjectReference).GetAnimationVariablebool("IsEquipping") == true && fWaitTime < fMaxWaitTime
            Utility.wait(0.1)
            fWaitTime += 0.1
        endWhile

        if fWaitTime >= fMaxWaitTime
            return false
        endif
    endif
    return true
EndFunction


; Sheathes the target's weapon and waits until the target has finished unequipping.
; Waits for a certain of maximum of seconds.
;
; Parameters:
; - TypeActor: The actor to sheathe their weapon.
; - TypeFloat: Maximun time in seconds that the function will wait.
;
; Return Value:
; - TypeBool: True if the target successfully sheathed their weapon on time; otherwise, false.
bool Function SheatheWeaponAndWait(Actor akTarget, float fMaxWaitTime)
    if akTarget.isWeaponDrawn() == true
        akTarget.SheatheWeapon()
        float fWaitTime = 0.0
        while (akTarget as ObjectReference).GetAnimationVariablebool("isUnEquipping") == true && fWaitTime < fMaxWaitTime
            Utility.wait(0.1)
            fWaitTime += 0.1
        endwhile

        if fWaitTime >= fMaxWaitTime
            return false
        endif
    endif
    return true
EndFunction
