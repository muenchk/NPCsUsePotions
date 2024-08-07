Scriptname AnimatedPoisonsScript extends ReferenceAlias

;Changelog:
; 5.1.1
; - Fixed wait for menu close never returning false if timer went over because of missing "=" in waitTime >= maxWaitTime
; - Fixed GetAnimationVariablebool("bIsSynced ") whitespace that caused stack errors
; - Added 'as ObjectReference' conversions to animation variable getters.
; 5.1.0
; - Fixed loop stuck check timer
; - Added GetAnimationVariablebool("bIsSynced ") == false to check whether target is not mounting a horse or trying to sit in/get off a chair
; - Checking ragdoll state with GetMass() == 0 instead with GetKnockedState() which will let me remove three mod depencies as PO3's custom scripts are no longer needed
; - Changed OnHitEx to OnHit as PO3's scripts are no longer used. Should be pretty much the same as this script is only on player.
; - Update PlaySound() function.
; - Added spawning empty bottles to the ground after poison use.
; - Added poison splashing effect
; - Fixed on hit not checking that we were applying poison
; - Added wait for menu close function.

; 5.0.0
; - Cleaned code for better readability and using better practices for more stable code.
; - Force close menu removed, because its implementation may interfere with other mods using the same function.
; - Using formlists in parallel to drive logic for picking right ArtObject and altering the MagicEffect's HitEffectArt accordingly.
; This way we only need one magic effect for bottle ArtObjects and one for arrow ArtObjects. This also takes care of mods changing
; EditorIDs for items, so we don't have to check that. This also removes the need of having to make a uniques animation instance for each
; poison. Now instead of 188 animations we only have one.
; - refactored some code to use local variables instead of always getting the value again and again by calling the function each time.
; this should free up computing time.
; - Fixed remote cast not casting Actor as ObjectReference, which lead to stack errors.
; - Fixed debug.sendanimationevent not casting Actor as ObjectReference. (This hasn't printed any errors but is good to change.)
; - Implemented some checks to functions that prevent getting stuck in a loop etc.
; - Added ReverBackToFPS that will revert camera back to first person if person was in 1st person before applying poison and ToggleForceThirdPerson was on.
; - Removed poison dose mechanic as it was out of scope for this mod. maybe separate mod for it if there's demand.
; - Removed left hand item handling. It was pretty janky, so I'm currenty researching for skse plugin alternative for this. This also removes the use of temp ite.
; - Fixed nesting in main function by swapping it with a function call so I can use returns. ( Can't return in events )
; - Added redraw bow after poisoning arrows.
; - Fixed ToggleSound not being checked, which lead to sound always firing.
; - Fixed sprint stop not checking if sprinting was already prevented, so that we don't interfere with some other mechanic that implemented that.


GlobalVariable property toggleStopSprint auto
GlobalVariable property toggleSlowEffect auto
GlobalVariable property toggleStagger auto
GlobalVariable property toggleHitStopsAnimation auto
GlobalVariable property toggleForceThirdPerson auto
GlobalVariable property toggleReverBackToFPS auto
GlobalVariable property toggleSound auto
GlobalVariable property toggleRedrawBow auto
GlobalVariable property toggleWaitForMenuClose auto
GlobalVariable property toggleSpawnEmptyBottles auto
GlobalVariable property togglePoisonSplash auto
Spell property PoisonAOSP auto
Spell property ArrowAOSP auto
Spell property SlowEffectSP auto
Spell property PoisonSplashSP auto
Sound property poisonSound auto
Sound property poisonDropSound auto
FormList Property arrowAOList auto
FormList Property ArrowList auto
FormList Property PoisonList auto
FormList Property poisonAOList auto
int iPoisonsUsed
int iPoisonSoundInstance
bool bBusy = false
Actor akPlayer
MiscObject property emptybottle auto


Event OnPLayerLoadGame()
    bBusy = false
    iPoisonsUsed = Game.QueryStat("Poisons Used")
    RegisterForModEvent("NPCsUsePotions_AnimatedPoisonsEvent", "NPCsUsePotions_ItemRemoved")
    RegisterForModEvent("NPCsUsePotions_AnimatedPoisonsHitEvent", "NPCsUsePotions_PoisonsOnHit")
EndEvent

Function NPCsUsePotions_ItemRemoved(string eventName, string strArg, float numArg, Form sender)
    if (eventName == "NPCsUsePotions_AnimatedPoisonsEvent" && strArg != "" && sender != None)
        Actor akTarget = sender as Actor
        if (akTarget)
            form akBaseItem = Game.GetForm(strArg as Int)
            if TargetConditionCheck(akTarget, akBaseitem) == false
                NPCsUsePotions_Poisons.AnimatedPoisons_Abort(akTarget)
                return
            endif

            if WaitForMenuClose(60) == false
                NPCsUsePotions_Poisons.AnimatedPoisons_Abort(akTarget)
                return
            endif

            int iItemType = akTarget.GetEquippeditemType(0)
            bool bHasBow = false
            if ( iItemType == 7 || iItemType == 12)
                bHasBow = true
            endif

            bool bDrawSuccess = true
            bool bSheatheSuccess = true

            if bHasBow == true; Sheathe (cross)bow or draw weapon
                bSheatheSuccess = SheatheWeaponAndWait(akTarget,6)
            else
                bDrawSuccess = DrawWeaponAndWait(akTarget,6)
            endif

            if bDrawSuccess == false || bSheatheSuccess == false
                NPCsUsePotions_Poisons.AnimatedPoisons_Abort(akTarget)
                return
            endif

            bool bSlowEffect = CastSlowEffectSP(akTarget); Running helper functions
            bool bStopSprint = StopSprint(akTarget)

            if bHasBow == true; Get correct ArtObject for arrowAOSP
                int iArrowListSize = ArrowList.GetSize()
                int index = -1
                int arrowIndex = -1

                while index < iArrowListSize; Detemine what arrows we have equipped. arrowIndex stays at -1 if none matching arrows found (mod added arrows for example).
                    index += 1
                    if akTarget.IsEquipped(ArrowList.GetAt(index)) == true
                        arrowIndex = index; used to get arrow Art Object of the formlist
                        index = iArrowListSize; break the while loop
                    endif
                endwhile

                if arrowIndex < 0; Display first arrow of the list if none found matching (mod added arrows for example).
                    arrowIndex = 0
                endif

                MagicEffect[] ArrowAOSPeffects = ArrowAOSP.GetMagicEffects(); Changing Hit Effect Art to correspond to the arrow mesh
                MagicEffect ArrowAOeffect = ArrowAOSPeffects[0]
                Art ArrowAO = arrowAOList.GetAt(arrowIndex) as Art
                ArrowAOeffect.SetHitEffectArt(ArrowAO)
            endif

            int poisonIndex = poisonList.Find(akBaseItem)

            if poisonIndex < 0
                poisonIndex = 0 ; Display first poison of the list if none found matching (mod added poisons for example).
            endif

            MagicEffect[] PoisonAOSPeffects = PoisonAOSP.GetMagicEffects(); Changing Hit Effect Art to correspond to the poison mesh
            MagicEffect PoisonAOeffect = PoisonAOSPeffects[0]
            Art poisonAO = poisonAOList.GetAt(poisonIndex) as Art
            PoisonAOeffect.SetHitEffectArt(poisonAO)

            if bHasBow == true; Display arrows if target has (cross)bow
                ArrowAOSP.RemoteCast(akTarget as ObjectReference, none)
            endif

            string path = akBaseitem.GetWorldModelPath(); update empty bottle mesh
            emptybottle.SetWorldModelPath(path)

            PoisonAOSP.RemoteCast(akTarget as ObjectReference, none); Display poison, play poison sound and play the animation
            Debug.SendAnimationEvent(akTarget as ObjectReference, "genericpoison")
            NPCsUsePotions_Poisons.AnimatedPoisons_PlaySound(akTarget)

            Utility.wait(0.8)

            if togglePoisonSplash.GetValue() == 1
                PoisonSplashSP.Cast(akTarget as ObjectReference, none)
            endif

            Utility.wait(1.2)

            if toggleSpawnEmptyBottles.GetValue() == 1; Spawn empty bottle at calculated distance from target and play sound effect of bottle hitting the ground
                ObjectReference bottleref = akTarget.PlaceAtMe(emptybottle as Form)
                float distance = 100.0
                float zAngle = akTarget.GetAngleZ()
                float markerX = distance * math.sin(zAngle)
                float markerY = distance * math.cos(zAngle)
                float markerZ = 10
                bottleref.MoveTo(akTarget, markerX, markerY, markerZ)
                PlaySound(poisonDropSound, akTarget)
            endif

            Utility.wait(0.6)

            if bStopSprint == true; Allow sprinting again
                (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", true)
            endif

            Form formItem = akTarget.GetEquippedObject(1); This is to fix the animation state getting stuck in non-drawn state while target thinks he's in drawn state
            akTarget.UnequipItem(formItem, false, true)
            akTarget.EquipItem(formItem, false, true)

            if bHasBow == true && ToggleRedrawBow.GetValue() == 1; Draw bow if target poisoned arrows and ToggleRedrawBow is enabled
                akTarget.DrawWeapon()
            endif

            NPCsUsePotions_Poisons.AnimatedPoisons_Callback(akTarget, 1)
        endif
    endif
EndFunction

Event NPCsUsePotions_PoisonsOnHit(string eventName, string strArg, float numArg, Form sender)
    Actor act = sender as Actor
    if (act)
        if ToggleHitStopsAnimation.GetValueint() == 1
            NPCsUsePotions_Poisons.AnimatedPoisons_Abort(act)
            Debug.SendAnimationEvent(act, "offsetstop")
        endif
        if ToggleStagger.GetValueint() == 1
            Debug.SendAnimationEvent(act, "StaggerStart")
        endif
    endif
EndEvent


; Whenever player removes item we check whether a game stat "Poisons Used" has increased which will tell if player consumed an poison item.
Event OnitemRemoved(Form akBaseitem, int aiitemCount, ObjectReference akitemReference, ObjectReference akDestContainer)
    int iEventPoisonCount = Game.QueryStat("Poisons Used")
    if iEventPoisonCount > iPoisonsUsed
        iPoisonsUsed = iEventPoisonCount
        akPlayer = Game.GetPlayer()
        PoisonApply(akBaseitem, akPlayer)
    endif
EndEvent


; Handles all the logic for poison applying animation and helper functions.
bool Function PoisonApply(Form akBaseitem, Actor akTarget)
    if TargetConditionCheck(akTarget, akBaseitem) == false
        return false
    endif

    bBusy = true; Keeps track whether we are already applying poison

    if WaitForMenuClose(60) == false; if for some reason we exited the menu but the script didn't notice, we don't get stuck here.
        bBusy = false
        return false
    endif

    int iItemType = akTarget.GetEquippeditemType(0); Check for equipped bow or crossbow as they are sheathed rather than drawn while poisoning.
    bool bHasBow = false
    if ( iItemType == 7 || iItemType == 12)
        bHasBow = true
    endif

    bool bDrawSuccess = true
    bool bSheatheSuccess = true

    if bHasBow == true; Sheathe (cross)bow or draw weapon
        bSheatheSuccess = SheatheWeaponAndWait(akTarget,6)
    else
        bDrawSuccess = DrawWeaponAndWait(akTarget,6)
    endif

    if bDrawSuccess == false || bSheatheSuccess == false; Failing to draw or sheathe in 6s fails the poison apply. For example target gets staggered or yeeted to Oblivion.
        bBusy = false
        return false
    endif

    bool bSlowEffect = CastSlowEffectSP(akTarget); Running helper functions
    bool bStopSprint = StopSprint(akTarget)
    bool bFirstPerson = ForceThirdPerson(akTarget)

    if bHasBow == true; Get correct ArtObject for arrowAOSP
        int iArrowListSize = ArrowList.GetSize()
        int index = -1
        int arrowIndex = -1

        while index < iArrowListSize; Detemine what arrows we have equipped. arrowIndex stays at -1 if none matching arrows found (mod added arrows for example).
            index += 1
            if akTarget.IsEquipped(ArrowList.GetAt(index)) == true
                arrowIndex = index; used to get arrow Art Object of the formlist
                index = iArrowListSize; break the while loop
            endif
        endwhile

        if arrowIndex < 0; Display first arrow of the list if none found matching (mod added arrows for example).
            arrowIndex = 0
        endif

        MagicEffect[] ArrowAOSPeffects = ArrowAOSP.GetMagicEffects(); Changing Hit Effect Art to correspond to the arrow mesh
        MagicEffect ArrowAOeffect = ArrowAOSPeffects[0]
        Art ArrowAO = arrowAOList.GetAt(arrowIndex) as Art
        ArrowAOeffect.SetHitEffectArt(ArrowAO)
    endif

    int poisonIndex = poisonList.Find(akBaseItem); Changing Hit Effect Art to correspond to the poison mesh

    if poisonIndex < 0
        poisonIndex = 0 ; Display first poison of the list if none found matching (mod added poisons for example).
    endif

    MagicEffect[] PoisonAOSPeffects = PoisonAOSP.GetMagicEffects(); Changing Hit Effect Art to correspond to the poison mesh
    MagicEffect PoisonAOeffect = PoisonAOSPeffects[0]
    Art poisonAO = poisonAOList.GetAt(poisonIndex) as Art
    PoisonAOeffect.SetHitEffectArt(poisonAO)

    if bHasBow == true; Display arrows if target has (cross)bow
        ArrowAOSP.RemoteCast(akTarget as ObjectReference, none)
    endif

    string path = akBaseitem.GetWorldModelPath(); update empty bottle mesh
    emptybottle.SetWorldModelPath(path)

    PoisonAOSP.RemoteCast(akTarget as ObjectReference, none); Display poison, play poison sound and play the animation
    iPoisonSoundInstance = PlaySound(poisonSound, akTarget)
    Debug.SendAnimationEvent(akTarget as ObjectReference, "genericpoison")

    Utility.wait(0.8); (total wait 2.6) After this time the bottle is being tilted so that the poison is "pouring out"

    if togglePoisonSplash.GetValue() == 1
        PoisonSplashSP.Cast(akTarget as ObjectReference, none)
    endif

    Utility.wait(1.2); After this the bottle is about to hit ground

    if toggleSpawnEmptyBottles.GetValue() == 1; Spawn empty bottle at calculated distance from target and play sound effect of bottle hitting the ground
        ObjectReference bottleref = akTarget.PlaceAtMe(emptybottle as Form)
        float distance = 100.0
        float zAngle = akTarget.GetAngleZ()
        float markerX = distance * math.sin(zAngle)
        float markerY = distance * math.cos(zAngle)
        float markerZ = 10
        bottleref.MoveTo(akTarget, markerX, markerY, markerZ)
        PlaySound(poisonDropSound, akTarget)
    endif

    Utility.wait(0.6); After this the animation is about to end

    if bFirstPerson == true && ToggleReverBackToFPS.GetValue() == 1; Bring target back to 1st person
        Game.ForceFirstPerson()
    endif

    if bStopSprint == true; Allow sprinting again
        (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", true)
    endif

    Form formItem = akTarget.GetEquippedObject(1); This is to fix the animation state getting stuck in non-drawn state while target thinks he's in drawn state
    akTarget.UnequipItem(formItem, false, true)
    akTarget.EquipItem(formItem, false, true)

    if bHasBow == true && ToggleRedrawBow.GetValue() == 1; Draw bow if target poisoned arrows and ToggleRedrawBow is enabled
        akTarget.DrawWeapon()
    endif

    bBusy = false
    return true
EndFunction


; Hit event handling.
; Stops sound and animation if hit and ToggleHitStopsAnimation is enabled.
; Also staggers target if hit and ToggleStagger is enabled.
Event OnHit(ObjectReference akAggressor, Form akSource, Projectile akProjectile, bool abPowerAttack, bool abSneakAttack, bool abBashAttack, bool abHitBlocked)
    if bBusy == true
        if ToggleHitStopsAnimation.GetValueint() == 1
            Sound.Stopinstance(iPoisonSoundInstance)
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "offsetstop")
        endif

        if ToggleStagger.GetValueint() == 1
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "StaggerStart")
        endif
    endif
EndEvent


; Plays sound at target if TogglePlayerSound is enabled.
;
; Parameters:
; - TypeActor: The actor to play sound at.
;
; Return Value:
; - TypeBool: Returns true if the TogglePlayerSound was enabled; otherwise, false.
int Function PlaySound(Sound sfx, Actor akTarget)
    if ToggleSound.GetValue() == 1
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
    if ToggleSlowEffect.GetValueint() == 1
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
;
; Additionally, it verifies if the target has a poisonable weapon equipped in right hand
; and is in any of the following menus:
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
    && HasSuitableWeaponEquipped(akTarget) == true && akTarget.GetMass() == 0
    ; For GetMass: "The object has to be controlled by Havok ("ragdolling") or this function will always return 0" which is how we get ragdoll state of target.
    ; While sitting on a chair bAnimationDriven is true
    ; While getting on a horse bIsSynced is true;/  && consumedPoison.HasKeywordString("VendorItemPoison") == true /; ;Things like frostbite venom don't have this tag so we don't use it.
        if UI.IsMenuOpen("InventoryMenu") == true || UI.IsMenuOpen("FavoritesMenu") == true || UI.IsMenuOpen("HUD Menu") == true
            Return true
        endif
    endif
    Return false
EndFunction


; Checks if the target has any of the following weapons equipped:
; - One-handed sword (right hand)
; - One-handed dagger (right hand)
; - One-handed axe (right hand)
; - One-handed mace (right hand)
; - Two-handed sword
; - Two-handed axe
; - Bow
; - Crossbow
;
; Parameters:
; - TypeActor: The actor from whom the equipped item types will be checked.
;
; Return Value:
; - TypeBool: Returns true if the target has any of the specified weapons equipped; otherwise, false.
bool Function HasSuitableWeaponEquipped(Actor akTarget)
    int equippedItemType = akTarget.GetEquippedItemType(1)
    if equippedItemType == 1 || equippedItemType == 2 || equippedItemType == 3 || equippedItemType == 4 || equippedItemType == 5 || equippedItemType == 6 || equippedItemType == 7 || equippedItemType == 12
        return true
    endif
    return false
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
