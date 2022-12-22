#pragma once
#include "Settings.h"
#include <unordered_map>

class Compatibility
{
public:
	// ZUPA

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
	static inline std::string AnimatedPotions = "AnimatedPotions.esp";

	int AnPoti_GlobalCooldown = 0;

	// ZUPA
	int ZUPA_GlobalCooldown = 3000;
	
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

	/// <summary>
	/// returns whether compatibility for apothecary is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedApothecary()
	{
		return Settings::_CompatibilityApothecary && _loadedApothecary;
	}

	/// <summary>
	/// returns whether compatibility for caco is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedCACO()
	{
		return Settings::_CompatibilityCACO && _loadedCACO;
	}
	
	/// <summary>
	/// returns whether compatibility for animated potions is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedAnimatedPotions()
	{
		return Settings::_CompatibilityAnimatedPotions && _loadedAnimatedPotions;
	}

	/// <summary>
	/// returns whether compatibility for animated poisons is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedAnimatedPoisons()
	{
		return Settings::_CompatibilityAnimatedPoisons && _loadedAnimatedPoisons;
	}

	/// <summary>
	/// returns whether the compatibility for AnimatedPotionFx is enabled
	/// </summary>
	/// <returns></returns>
	bool LoadedAnimatedPotionFx()
	{
		return Settings::_CompatibilityPotionAnimatedFx && _loadedPotionAnimatedFx;
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
		return Settings::_featDisableItemUsageWhileStaggered || _disableParalyzedItems;	
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

	static void Register();
};
