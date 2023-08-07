#pragma once
#include "Settings.h"
#include <unordered_map>

class Compatibility
{
public:
	// apothecary
	static inline std::string Apothecary = "Apothecary.esp";
	RE::BGSKeyword* Apot_SH_AlcoholDrinkKeyword = nullptr;

	// caco
	static inline std::string CACO = "Complete Alchemy & Cooking Overhaul.esp";
	RE::BGSKeyword* CACO_VendorItemDrinkAlcohol = nullptr;

	// animated poisons
	static inline std::string AnimatedPoisons = "AnimatedPoisons.esp";
	RE::TESAmmo* AnPois_DaedricArrow = nullptr;
	RE::TESAmmo* AnPois_DraugrArrow = nullptr;
	RE::TESAmmo* AnPois_DwarvenArrow = nullptr;
	RE::TESAmmo* AnPois_EbonyArrow = nullptr;
	RE::TESAmmo* AnPois_ElvenArrow = nullptr;
	RE::TESAmmo* AnPois_FalmerArrow = nullptr;
	RE::TESAmmo* AnPois_ForswornArrow = nullptr;
	RE::TESAmmo* AnPois_GlassArrow = nullptr;
	RE::TESAmmo* AnPois_IronArrow = nullptr;
	RE::TESAmmo* AnPois_NordHeroArrow = nullptr;
	RE::TESAmmo* AnPois_OrcishArrow = nullptr;
	RE::TESAmmo* AnPois_SteelArrow = nullptr;

	RE::SpellItem* AnPois_DaedricArrowAOSP = nullptr;
	RE::SpellItem* AnPois_DraugrArrowAOSP = nullptr;
	RE::SpellItem* AnPois_DwarvenArrowAOSP = nullptr;
	RE::SpellItem* AnPois_EbonyArrowAOSP = nullptr;
	RE::SpellItem* AnPois_ElvenArrowAOSP = nullptr;
	RE::SpellItem* AnPois_FalmerArrowAOSP = nullptr;
	RE::SpellItem* AnPois_ForswornArrowAOSP = nullptr;
	RE::SpellItem* AnPois_GlassArrowAOSP = nullptr;
	RE::SpellItem* AnPois_IronArrowAOSP = nullptr;
	RE::SpellItem* AnPois_NordHeroArrowAOSP = nullptr;
	RE::SpellItem* AnPois_OrcishArrowAOSP = nullptr;
	RE::SpellItem* AnPois_SteelArrowAOSP = nullptr;

	RE::SpellItem* AnPois_SlowEffectSP = nullptr;

	RE::TESObjectARMO* AnPois_FakeItem = nullptr;

	RE::TESGlobal* AnPois_ToggleStopSprint = nullptr;
	RE::TESGlobal* AnPois_ToggleForceThirdPerson = nullptr;
	RE::TESGlobal* AnPois_ToggleForceCloseMenu = nullptr;
	RE::TESGlobal* AnPois_TogglePlayerSlowEffect = nullptr;
	RE::TESGlobal* AnPois_TogglePlayerSound = nullptr;
	RE::TESGlobal* AnPois_PlayerStaggerToggle = nullptr;
	RE::TESGlobal* AnPois_PlayerStopAnimation = nullptr;

	RE::TESObjectMISC* AnPois_SlowEffectItem = nullptr;

	//RE::BGSListForm* AnPois_List = nullptr;

	RE::BGSSoundDescriptorForm* AnPois_PoisonSound = nullptr;

	std::string AnPois_POiSON_DOSE_LiST_FiLENAME = "PoisonDoseList";
	std::string AnPois_POiSON_LiST_FiLENAME = "PoisonList";

	int AnPois_GlobalCooldown = 4000;

	// animated potions
	static inline std::string AnimatedPotions_4_4 = "Animated Potions.esp";
	static inline std::string AnimatedPotions_4_3 = "AnimatedPotions.esp";

	int AnPoti_Version = 44;

	RE::TESGlobal* AnPoti_TogglePlayerPotionAnimation = nullptr;

	int AnPoti_GlobalCooldown = 3000;

	// ZUPA
	int ZUPA_GlobalCooldown = 2500;

	// potion animated fx
	static inline std::string PotionAnimatedfx = "PotionAnimatedfx.esp";
	RE::EffectSetting* PAF_NPCDrinkingCoolDownEffect = nullptr;
	RE::SpellItem* PAF_NPCDrinkingCoolDownSpell = nullptr;
	std::string PAF_NPCDrinkingCoolDownEffect_name = "PAF_NPCDrinkingCoolDownEffect";
	std::string PAF_NPCDrinkingCoolDownSpell_name = "PAF_NPCDrinkingCoolDownSpell";

	// general section
private:
	/// <summary>
	/// Whether all objects for Apothecary have been found
	/// </summary>
	bool _loadedApothecary = false;
	/// <summary>
	/// Whether all objects for Complete Alchemy and Cooking Overhaul have been found
	/// </summary>
	bool _loadedCACO = false;
	/// <summary>
	/// Whether all objects for Animated Poisons have been found
	/// </summary>
	bool _loadedAnimatedPoisons = false;
	/// <summary>
	/// Whether all objects for Animated Potions have been found
	/// </summary>
	bool _loadedAnimatedPotions = false;
	/// <summary>
	/// Whether all objects for Potion Animated Fx have been found
	/// </summary>
	bool _loadedPotionAnimatedFx = false;
	/// <summary>
	/// Whether all objects for zxlice's Ultimate Potion Animation have been found
	/// </summary>
	bool _loadedZUPA = false;

	/// <summary>
	/// Global cooldown applied
	/// </summary>
	int _globalCooldown = 0;

	/// <summary>
	/// Global cooldown used for potions
	/// </summary>
	int _globalCooldownPotions = 0;
	/// <summary>
	/// Global cooldown used for poisons
	/// </summary>
	int _globalCooldownPoisons = 0;
	/// <summary>
	/// Global cooldown used for food
	/// </summary>
	int _globalCooldownFood = 0;

	/// <summary>
	/// whether item usage while paralyzed is disabled
	/// </summary>
	bool _disableParalyzedItems = false;

	/// <summary>
	/// Maps actorIDs to poisons that should be applied
	/// </summary>
	std::unordered_map<RE::FormID, std::tuple<RE::AlchemyItem*, RE::FormID>> _AnPois_ActorPoisonMap;

	/// <summary>
	/// Maps actorIDs to potions that should be applied
	/// </summary>
	std::unordered_map<RE::FormID, std::tuple<RE::AlchemyItem*, RE::FormID>> _AnPoti_ActorPotionMap;
	/// <summary>
	/// Maps actorIDs to saved poisons that should be reapplied
	/// </summary>
	std::unordered_map<RE::FormID, std::tuple<RE::AlchemyItem*, RE::FormID, int>> _AnPoti_ActorPoisonMap;

public:
	/// <summary>
	/// Returns a static Compatibility object
	/// </summary>
	/// <returns></returns>
	static Compatibility* GetSingleton();

	/// <summary>
	/// Loads all game items for the mods, and verifies that compatibility has been established
	/// </summary>
	void Load();

	/// <summary>
	/// Clears all loaded data from memory
	/// </summary>
	void Clear();

	bool CanApplyPoisonToLeftHand()
	{
		// all anmiation mods must be false, then we may use left hand
		return !(!LoadedAnimatedPoisons() && !LoadedAnimatedPotionFx() && !LoadedAnimatedPotions() && !LoadedZUPA());
	}

	/// <summary>
	/// returns whether compatibility for apothecary is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedApothecary()
	{
		return Settings::Compatibility::Apothecary::_CompatibilityApothecary && _loadedApothecary;
	}

	/// <summary>
	/// returns whether compatibility for caco is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedCACO()
	{
		return Settings::Compatibility::CACO::_CompatibilityCACO && _loadedCACO;
	}

	/// <summary>
	/// returns whether compatibility for animated potions is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedAnimatedPotions()
	{
		return Settings::Compatibility::AnimatedPotions::_CompatibilityAnimatedPotions && Settings::Compatibility::AnimatedPotions::_Enable &&
		       _loadedAnimatedPotions &&
		       (AnPoti_TogglePlayerPotionAnimation && AnPoti_TogglePlayerPotionAnimation->value == 1);
	}

	/// <summary>
	/// returns whether compatibility for animated poisons is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedAnimatedPoisons()
	{
		return Settings::Compatibility::AnimatedPoisons::_CompatibilityAnimatedPoisons && Settings::Compatibility::AnimatedPoisons::_Enable && _loadedAnimatedPoisons;
	}

	/// <summary>
	/// returns whether the compatibility for AnimatedPotionFx is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedAnimatedPotionFx()
	{
		return Settings::Compatibility::PotionAnimatedFx::_CompatibilityPotionAnimatedFx && _loadedPotionAnimatedFx;
	}

	/// <summary>
	/// returns whether the compatibility for ZUPA is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedZUPA()
	{
		return _loadedZUPA;
	}

	/// <summary>
	/// returns whether item usage should be disabled while an actor is paralyzed, considering the settings and the loaded plugins
	/// </summary>
	/// <returns></returns>
	bool DisableItemUsageWhileParalyzed()
	{
		return Settings::Usage::_DisableItemUsageWhileStaggered || _disableParalyzedItems;
	}

	/// <summary>
	/// Returns the global cooldown for item usage
	/// </summary>
	/// <returns></returns>
	int GetGlobalCooldown()
	{
		return _globalCooldown;
	}

	/// <summary>
	/// Returns the global cooldown for potion usage
	/// </summary>
	/// <returns></returns>
	int GetGlobalCooldownPotions()
	{
		return _globalCooldownPotions;
	}

	/// <summary>
	/// Returns the global cooldown for poison usage
	/// </summary>
	/// <returns></returns>
	int GetGlobalCooldownPoisons()
	{
		return _globalCooldownPoisons;
	}

	/// <summary>
	/// Returns the global cooldown for food usage
	/// </summary>
	/// <returns></returns>
	int GetGlobalCooldownFood()
	{
		return _globalCooldownFood;
	}

	/// <summary>
	/// [AnimatedPoisons] Finds the poison that should be applied to [actor]'s weapons
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	RE::AlchemyItem* AnPois_FindActorPoison(RE::FormID actor);
	/// <summary>
	/// [AnimatedPoisons] Saves a [poison] that should be applied to [actor]'s weapons
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="poison"></param>
	void AnPois_AddActorPoison(RE::FormID actor, RE::AlchemyItem* poison);
	/// <summary>
	/// [AnimatedPoisons] Deletes a Form from the actor poison map [CTD FAILSAVE]
	/// </summary>
	/// <param name="actor"></param>
	void AnPois_DeleteActorPoison(RE::FormID form);
	/// <summary>
	/// [AnimatedPoisons] removes a saved poison that should be applied to [actor]
	/// </summary>
	/// <param name="actor"></param>
	void AnPois_RemoveActorPoison(RE::FormID actor);

	/// <summary>
	/// [AnimatedPotions] Finds the potion that should be applied to [actor]
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	RE::AlchemyItem* AnPoti_FindActorPotion(RE::FormID actor);
	/// <summary>
	/// [AnimatedPotions] Saves a [potion] that should be applied to [actor]
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="potion"></param>
	void AnPoti_AddActorPotion(RE::FormID actor, RE::AlchemyItem* potion);
	/// <summary>
	/// [AnimatedPotions] Deletes a form from the actor potion map [CTD FAILSAVE]
	/// </summary>
	/// <param name="actor"></param>
	void AnPoti_DeleteActorPotion(RE::FormID form);
	/// <summary>
	/// [AnimatedPotions] Removes a saved potion that should be used by [actor]
	/// </summary>
	/// <param name="actor"></param>
	void AnPoti_RemoveActorPotion(RE::FormID actor);
	/// <summary>
	/// [AnimatedPotions] Finds the poison that should be reapplied to an actors weapons
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	std::tuple<RE::AlchemyItem*, int> AnPoti_FindActorPoison(RE::FormID actor);
	/// <summary>
	/// [AnimatedPotions] Saves a [poison] that should be reapplied to [actor]'s weapons
	/// </summary>
	/// <param name="actor"></param>
	/// <param name="poison"></param>
	void AnPoti_AddActorPoison(RE::FormID actor, RE::AlchemyItem* Poison, int count);
	/// <summary>
	/// [AnimatedPotions] Deletes a Form from the actor poison map [CTD FAILSAVE]
	/// </summary>
	/// <param name="actor"></param>
	void AnPoti_DeleteActorPoison(RE::FormID form);
	/// <summary>
	/// [AnimatedPotions] removes a saves poison that should be reapplied to [actor]
	/// </summary>
	/// <param name="actor"></param>
	void AnPoti_RemoveActorPoison(RE::FormID actor);

	/// <summary>
	/// Registers Game Callbacks
	/// </summary>
	static void Register();
};
