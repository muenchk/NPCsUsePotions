Scriptname AnimatedPotionsScript extends ReferenceAlias

;Changelog:
; 5.1.0
; - Changed fast potion drinking only happening when weapon is already drawn. Before also when in combat.
; - Removed sheathing and drawing functions as they are no longer needed.
; - Removed  PO3's knockstate check, so the mod can be used player-only with only FNIS/Nemesis + SkyUI + SKSE. Using NPC scripts require PO3's mods and its requirements.
; - Removed string manipulation functions as they are no longer needed.
; - Major changes to how the potion is displayed. We now check the path of the potion model and reroute the Art Object SPells (corks, empty bottles and the potion) mesh path to display the correct Art Object version of the mesh.
; The Art Object version of the mesh is almost the same but it has some critical changes to be able to display as Art Object (like removing collision). This way adding new potions is super easy as you just need to have the exactly same
; named Art Object version of the mesh in this mods artobjects folder. This reduces needed animations from ~212 to just 2... Also in the esp we can get rid of so many unnecessary Art Objects and Anim Objects.
; - Fixed selfmade potions to display as "PotionHealthLesser.nif" as most of the time they are that model. Maybe at higher Alchemy levels with more potent potions is the "greater" mesh but we'll fix that later.
; - Added drinking sound that can be toggled.
; 5.0.0
; - Cleaned code for better readability and using better practices for more stable code.
; - Force close menu removed, because its implementation may interfere with other mods using the same function.
; - refactored some code to use local variables instead of always getting the value again and again by calling the function each time.
; this should free up computing time.
; - Fixed remote cast not casting Actor as ObjectReference, which lead to stack errors.
; - Fixed debug.sendanimationevent not casting Actor as ObjectReference. (This hasn't printed any errors but is good to change.)
; - Implemented some checks to functions that prevent getting stuck in a loop etc.
; - Added ReverBackToFPS that will revert camera back to first person if person was in 1st person before drinking potion and ToggleForceThirdPerson was on.
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


Sound property PotionSM auto
Sound property FastPotionSM auto
Sound property potionDropSound auto
Sound property potionDrinkSound auto
MiscObject property emptybottle auto
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
GlobalVariable property TogglePlayerDrinkSound auto
Spell property SlowEffectSP auto
Spell property FastPotionSplashAOSP auto
Spell property SlowPotionSplashAOSP auto
Spell property PotionAOSP auto
Spell property CorkHandAOSP auto
Spell property CorkBottleAOSP auto
bool bBusy
int iPotionsUsed
int iPotionSoundInstance
Actor akPlayer


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
            if TargetConditionCheck(akTarget) == false
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

            if WaitForWeapon(akTarget, 6.0) == false; Wait for target to finish sheathing or drawing if consumed potion during that. Returning if we exceed the 6 second time limit that prevents getting stuck here.
                NPCsUsePotions_Potions.AnimatedPotions_Abort(akTarget)
                return
            endif

            bool bFastDrinking = false

            if akTarget.IsWeaponDrawn() == true
                bFastDrinking = true
            endif

            bool bSlowEffect = CastSlowEffectSP(akTarget); Running helper functions
            bool bStopSprint = StopSprint(akTarget)

            SetModelPaths(akBaseitem, bFastDrinking); May become ambitius when multiple actors consume different potions at the same time

            if bFastDrinking == true; Actual drinking logic happens here
                NPCsUsePotions_Potions.AnimatedPotions_Callback(aktarget)
                Debug.SendAnimationEvent(akTarget as ObjectReference, "fastgenericpotion")
                PotionAOSP.RemoteCast(akTarget as ObjectReference, none)
                if togglePotionSplash.GetValue() == 1
                    FastPotionSplashAOSP.Cast(akTarget as ObjectReference, none)
                endif
                Utility.Wait(2.4)
                akTarget.DispelSpell(PotionAOSP)
            else
                NPCsUsePotions_Potions.AnimatedPotions_Callback(aktarget)
                Debug.SendAnimationEvent(akTarget as ObjectReference, "genericpotion")
                PotionAOSP.RemoteCast(akTarget as ObjectReference, none)

                (CorkBottleAOSP).RemoteCast(akTarget as ObjectReference, none)
                Utility.Wait(0.56)
                (CorkHandAOSP).RemoteCast(akTarget as ObjectReference, none)
                akTarget.DispelSpell(CorkBottleAOSP)

                if togglePotionSplash.GetValue() == 1
                    SlowPotionSplashAOSP.Cast(akTarget as ObjectReference, none)
                endif
                Utility.Wait(0.4)
                Utility.Wait(1.86)
                (CorkBottleAOSP).RemoteCast(akTarget as ObjectReference, none)
                akTarget.DispelSpell(CorkHandAOSP)
                Utility.Wait(0.52)
                akTarget.DispelSpell(CorkBottleAOSP)
                akTarget.DispelSpell(PotionAOSP)
            endif

            SpawnEmptyBottle(akTarget, bFastDrinking)

            if bStopSprint == true; Allow sprinting again
                (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", true)
            endif

            if bFastDrinking == true; if we were fast drinking, draw weapon
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
            Debug.SendAnimationEvent(act as ObjectReference, "offsetstop")
            act.DispelSpell(FastPotionSplashAOSP)
            act.DispelSpell(SlowPotionSplashAOSP)
            act.DispelSpell(PotionAOSP)
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
        PotionDrinking(akBaseitem, akPlayer,akItemReference)
    endif
EndEvent


bool Function PotionDrinking(Form akBaseitem, Actor akTarget, ObjectReference akItemReference)
    if BlackListedPotions.Find(akBaseItem) != -1; Returning if potion is blacklisted.
        return false
    endif

    if ToggleDisableDuringCombat.GetValueInt() == 1 && akTarget.IsInCombat() == true; Returning if potion animations are disable during combat and is in combat.
        return false
    endif

    if TargetConditionCheck(akTarget) == false; Returning if fails conditioncheck.
        return false
    endif

    if akBaseItem.HasKeywordString("vendoritempotion") == false && ToggleRequireKeyword.GetValueInt() == 1; Returning if consumed potion didn't have required keyword.
        return false
    endif

    bBusy = true; Keeps track whether we are already drinking potion

    if WaitForMenuClose(60.0) == false; Wait for menu to be closed. Returning if we exceed the 60 second time limit that prevents getting stuck here.
        bBusy = false
        return false
    endif

    if WaitForWeapon(akTarget, 6.0) == false; Wait for target to finish sheathing or drawing if consumed potion during that. Returning if we exceed the 6 second time limit that prevents getting stuck here.
        bBusy = false
        return false
    endif

    bool bFastDrinking = false

    if akTarget.IsWeaponDrawn() == true
        bFastDrinking = true
    endif

    bool bSlowEffect = CastSlowEffectSP(akTarget); Running helper functions
    bool bStopSprint = StopSprint(akTarget)
    bool bFirstPerson = ForceThirdPerson(akTarget)

    SetModelPaths(akBaseitem, bFastDrinking); Update models to use correct meshes

    if bFastDrinking == true; Actual drinking logic happens here
        FastDrinking(akTarget)
    else
        SlowDrinking(akTarget)
    endif

    SpawnEmptyBottle(akTarget, bFastDrinking)

    if bFirstPerson == true && ToggleReverBackToFPS.GetValue() == 1; Bring target back to 1st person
        Game.ForceFirstPerson()
    endif

    if bStopSprint == true; Allow sprinting again
        (akTarget as ObjectReference).SetAnimationVariablebool("bSprintOK", true)
    endif

    if bFastDrinking == true; if we were fast drinking, draw weapon
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
            akPlayer.DispelSpell(FastPotionSplashAOSP)
            akPlayer.DispelSpell(SlowPotionSplashAOSP)
            akPlayer.DispelSpell(PotionAOSP)
        endif
        if TogglePlayerStagger.GetValueint() == 1
            Debug.SendAnimationEvent(akPlayer as ObjectReference, "StaggerStart")
        endif
    endif
EndEvent


bool Function WaitForWeapon(Actor akTarget, float fMaxWaitTime)
    float fWaitTime = 0.0
    while (akTarget as ObjectReference).GetAnimationVariablebool("isUnEquipping") == true && fWaitTime < fMaxWaitTime
        Utility.wait(0.1)
        fWaitTime += 0.1
    endwhile
    if fWaitTime >= fMaxWaitTime
        return false
    endif
    return true
EndFunction


Function SetModelPaths(Form akBaseitem, bool bFastDrinking)
    string path = akBaseitem.GetWorldModelPath(); Example: Clutter\Potions\PotionHealthLesser.nif
    if path == ""
        path = "Clutter\\Potions\\PotionHealthLesser.nif"; SOMETIMES selfmade potions do not have any path or the function doesn't return any path, so we fix that here sort of.
    endif

    emptybottle.SetWorldModelPath(path); Setting Empty bottle mesh to same as consumed potion mesh.

    int meshNameStartIndex
    meshNameStartIndex = StringUtil.GetLength(path) - 1

    while StringUtil.GetNthChar(path,meshNameStartIndex) != "\\" && meshNameStartIndex > -1
        meshNameStartIndex = meshNameStartIndex - 1
    endwhile

    string meshName = StringUtil.Substring(path,meshNameStartIndex); Example: \PotionHealthLesser.nif We removed the potion mesh path
    string AOmeshPath
    string CorkHandAOPath = "artobjects\\corkhand"+meshName
    string CorkBottleAOPath = "artobjects\\corkbottle"+meshName

    if bFastDrinking == false
        AOmeshPath = "artobjects"+meshName; Example: artobjects\PotionHealthLesser.nif We add artobject mesh path
    else
        AOmeshPath = "artobjects\\lefthandbottles"+meshName; Example: artobjects\lefthandbottles\PotionHealthLesser.nif We add artobject mesh path.
    endif

    MagicEffect[] potionAOSPEffects = potionAOSP.GetMagicEffects(); Set the potion Art Object to correspond the consumed potion.
    MagicEffect potionAOEffect = potionAOSPEffects[0]
    (potionAOEffect.GetHitEffectArt()).SetModelPath(AOmeshPath)

    MagicEffect[] CorkHandAOSPEffects = CorkHandAOSP.GetMagicEffects(); Set the cork that is attached to targets hand Art Object to correspond the consumed potions cork.
    MagicEffect CorkHandAOEffect = CorkHandAOSPEffects[0]
    (CorkHandAOEffect.GetHitEffectArt()).SetModelPath(CorkHandAOPath)

    MagicEffect[] CorkBottleAOSPEffects = CorkBottleAOSP.GetMagicEffects(); Set the potion Art Object to correspond the consumed potion.
    MagicEffect CorkBottleAOEffect = CorkBottleAOSPEffects[0]
    (CorkBottleAOEffect.GetHitEffectArt()).SetModelPath(CorkBottleAOPath)
EndFunction


Function SpawnEmptyBottle(Actor akTarget, bool bFastDrinking)
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
EndFunction


Function FastDrinking(Actor akTarget)
    iPotionSoundInstance = PlaySound(FastPotionSM, akTarget)
    Debug.SendAnimationEvent(akTarget as ObjectReference, "fastgenericpotion")
    PotionAOSP.RemoteCast(akTarget as ObjectReference, none)
    if togglePotionSplash.GetValue() == 1
        FastPotionSplashAOSP.Cast(akTarget as ObjectReference, none)
    endif
    Utility.Wait(0.8)
    if TogglePlayerDrinkSound.GetValue() == 1
        PlaySound(potionDrinkSound, akTarget)
    endif

    Utility.Wait(1.6)
    akTarget.DispelSpell(PotionAOSP)
EndFunction


Function SlowDrinking(Actor akTarget)
    iPotionSoundInstance = PotionSM.Play(akTarget)
    Debug.SendAnimationEvent(akTarget as ObjectReference, "genericpotion")
    PotionAOSP.RemoteCast(akTarget as ObjectReference, none)

    (CorkBottleAOSP).RemoteCast(akTarget as ObjectReference, none)
    Utility.Wait(0.56)
    (CorkHandAOSP).RemoteCast(akTarget as ObjectReference, none)
    akTarget.DispelSpell(CorkBottleAOSP)

    if togglePotionSplash.GetValue() == 1
        SlowPotionSplashAOSP.Cast(akTarget as ObjectReference, none)
    endif
    Utility.Wait(0.4)
    if TogglePlayerDrinkSound.GetValue() == 1
        PlaySound(potionDrinkSound, akTarget)
    endif
    Utility.Wait(1.86)
    (CorkBottleAOSP).RemoteCast(akTarget as ObjectReference, none)
    akTarget.DispelSpell(CorkHandAOSP)
    Utility.Wait(0.52)
    akTarget.DispelSpell(CorkBottleAOSP)
    akTarget.DispelSpell(PotionAOSP)
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
; the duration of drinking potion. Also checks if sprinting was allowed before drinking potion to
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


; Waits for a menu to close, up to a specified maximum wait time when toggleWaitForMenuClose option is enabled.
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
; - Already drinking potion
; - Riding a horse
; - Swimming
; - Staggered
; - in bAnimationDriven state
; - in a killmove
; - Bleeding out
; - Ragdolling
; - Not mounting or standing on/off chairs
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
bool Function TargetConditionCheck(Actor akTarget)
    if bBusy == false && (akTarget as ObjectReference).GetAnimationVariablebool("bIsRiding") == false && akTarget.IsSwimming() == false \
    && (akTarget as ObjectReference).GetAnimationVariablebool("isStaggering") == false && (akTarget as ObjectReference).GetAnimationVariablebool("bAnimationDriven") == false \
    && akTarget.IsInKillMove() == false && (akTarget as ObjectReference).GetAnimationVariablebool("IsBleedingOut") == false \
    && (akTarget as ObjectReference).GetAnimationVariablebool("bIsSynced") == false && akTarget.GetMass() == 0
    ; For GetMass: "The object has to be controlled by Havok ("ragdolling") or this function will always return 0" which is how we get ragdoll state of target.
    ; While sitting on a chair bAnimationDriven is true
    ; While getting on a horse bIsSynced is true;/
        if UI.IsMenuOpen("InventoryMenu") == true || UI.IsMenuOpen("FavoritesMenu") == true || UI.IsMenuOpen("HUD Menu") == true
            Return true
        endif
    endif
    Return false
EndFunction
