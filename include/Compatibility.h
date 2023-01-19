#pragma once
#include "Settings.h"
#include <unordered_map>

class Compatibility
{
public:

	// apothecary

	// caco

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

	int AnPoti_GlobalCooldown = 3;

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
	bool _loadedApothecary = false;
	bool _loadedCACO = false;
	bool _loadedAnimatedPoisons = false;
	bool _loadedAnimatedPotions = false;
	bool _loadedPotionAnimatedFx = false;
	bool _loadedZUPA = false;

	int _globalCooldown = 0;

	bool _disableParalyzedItems = false;

	std::unordered_map<RE::FormID, std::tuple<RE::AlchemyItem*, RE::FormID>> _AnPois_ActorPoisonMap;

	std::unordered_map<RE::FormID, std::tuple<RE::AlchemyItem*, RE::FormID>> _AnPoti_ActorPotionMap;
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

	RE::AlchemyItem* AnPois_FindActorPoison(RE::FormID actor);
	void AnPois_AddActorPoison(RE::FormID actor, RE::AlchemyItem* poison);
	void AnPois_DeleteActorPoison(RE::FormID actor);
	void AnPois_RemoveActorPoison(RE::FormID actor);

	
	RE::AlchemyItem* AnPoti_FindActorPotion(RE::FormID actor);
	void AnPoti_AddActorPotion(RE::FormID actor, RE::AlchemyItem* potion);
	void AnPoti_DeleteActorPotion(RE::FormID actor);
	void AnPoti_RemoveActorPotion(RE::FormID actor);
	std::tuple<RE::AlchemyItem*, int> AnPoti_FindActorPoison(RE::FormID actor);
	void AnPoti_AddActorPoison(RE::FormID actor, RE::AlchemyItem* Poison, int count);
	void AnPoti_DeleteActorPoison(RE::FormID actor);
	void AnPoti_RemoveActorPoison(RE::FormID actor);

	static void Register();
};
