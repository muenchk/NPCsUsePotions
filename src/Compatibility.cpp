#include <semaphore>

#include "Compatibility.h"
#include "Data.h"
#include "Game.h"
#include "Utility.h"

Compatibility* Compatibility::GetSingleton()
{
	static Compatibility singleton;
	return std::addressof(singleton);
}

std::binary_semaphore sem{ 1 };

void Compatibility::Load()
{
	// get lock to avoid deadlocks (should not occur, since the functions should not be called simultaneously
	sem.acquire();
	Data* data = Data::GetSingleton();
	RE::TESDataHandler* datahandler = RE::TESDataHandler::GetSingleton();

	// NPCsUsePotions
	if (const uint32_t index = Utility::Mods::GetPluginIndex(NPCsUsePotions); index != 0x1) {
		LOG_1("Found plugin NPCsUsePotions.esp.");
		_loadedNPCsUsePotions = true;

		NUP_IgnoreItem = datahandler->LookupForm<RE::BGSKeyword>(0xD64, NPCsUsePotions);
		NUP_ExcludeItem = datahandler->LookupForm<RE::BGSKeyword>(0xD65, NPCsUsePotions);
	}

	// apothecary
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Apothecary); index != 0x1) {
		LOG_1("Found plugin Apothecary.esp.");
		_loadedApothecary = true;
	}

	// gourmet
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Gourmet); index != 0x1) {
		LOG_1("Found plugin Gourmet.esp.");
		Gour_FoodTypeAle = datahandler->LookupForm<RE::BGSKeyword>(0xA4B, Gourmet);
		Gour_FoodTypeWine = datahandler->LookupForm<RE::BGSKeyword>(0xA4C, Gourmet);
		Gour_FoodTypeDrugs = datahandler->LookupForm<RE::BGSKeyword>(0xA4D, Gourmet);

		LOG_1("[Gour] {}", Utility::PrintForm(Gour_FoodTypeAle));
		LOG_1("[Gour] {}", Utility::PrintForm(Gour_FoodTypeWine));
		LOG_1("[Gour] {}", Utility::PrintForm(Gour_FoodTypeDrugs));

		if (Gour_FoodTypeAle &&
			Gour_FoodTypeDrugs &&
			Gour_FoodTypeWine)
			_loadedGourmet = true;
	}

	// caco
	if (const uint32_t index = Utility::Mods::GetPluginIndex(CACO); index != 0x1) {
		LOG_1("Found plugin Complete Alchemy & Cooking Overhaul.esp");
		CACO_VendorItemDrinkAlcohol = RE::TESForm::LookupByID<RE::BGSKeyword>(0x01AF101A);
		LOG_1("[CACO] {}", Utility::PrintForm(CACO_VendorItemDrinkAlcohol));
		if (CACO_VendorItemDrinkAlcohol) {
			_loadedCACO = true;
		}
	}

	// animated poisons

	// search for AnimatedPoisons.esp
	if (const uint32_t index = Utility::Mods::GetPluginIndex(AnimatedPoisons); index != 0x1) {
		AnPois_Version = 32;
		LOG_1("Found plugin AnimatedPoisons.esp.");
	}
	if (const uint32_t index = Utility::Mods::GetPluginIndex(AnimatedPoisons_5); index != 0x1) {
		AnPois_Version = 50;
		LOG_1("Found plugin Animated Poisons.esp.");
	}

	switch (AnPois_Version) {
	case 32:
		{
			AnPois_DaedricArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000139C0);
			AnPois_DraugrArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x00034182);
			AnPois_DwarvenArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000139BC);
			AnPois_EbonyArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000139BF);
			AnPois_ElvenArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000139BD);
			AnPois_FalmerArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x00038341);
			AnPois_ForswornArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000CEE9E);
			AnPois_GlassArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000139BE);
			AnPois_IronArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x0001397D);
			AnPois_NordHeroArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000EAFDF);
			AnPois_OrcishArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x000139BB);
			AnPois_SteelArrow = RE::TESForm::LookupByID<RE::TESAmmo>(0x0001397F);

			AnPois_DaedricArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x84E, "AnimatedPoisons.esp");
			AnPois_DraugrArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x84F, "AnimatedPoisons.esp");
			AnPois_DwarvenArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x850, "AnimatedPoisons.esp");
			AnPois_EbonyArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x851, "AnimatedPoisons.esp");
			AnPois_ElvenArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x852, "AnimatedPoisons.esp");
			AnPois_FalmerArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x853, "AnimatedPoisons.esp");
			AnPois_ForswornArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x854, "AnimatedPoisons.esp");
			AnPois_GlassArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x855, "AnimatedPoisons.esp");
			AnPois_IronArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x856, "AnimatedPoisons.esp");
			AnPois_NordHeroArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x857, "AnimatedPoisons.esp");
			AnPois_OrcishArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x858, "AnimatedPoisons.esp");
			AnPois_SteelArrowAOSP = datahandler->LookupForm<RE::SpellItem>(0x859, "AnimatedPoisons.esp");

			AnPois_SlowEffectSP = datahandler->LookupForm<RE::SpellItem>(0x867, "AnimatedPoisons.esp");

			AnPois_FakeItem = datahandler->LookupForm<RE::TESObjectARMO>(0xDB8, "AnimatedPoisons.esp");

			AnPois_ToggleStopSprint = datahandler->LookupForm<RE::TESGlobal>(0xDDD, "AnimatedPoisons.esp");
			AnPois_ToggleForceThirdPerson = datahandler->LookupForm<RE::TESGlobal>(0xDDe, "AnimatedPoisons.esp");
			AnPois_ToggleForceCloseMenu = datahandler->LookupForm<RE::TESGlobal>(0xDDF, "AnimatedPoisons.esp");
			AnPois_TogglePlayerSlowEffect = datahandler->LookupForm<RE::TESGlobal>(0xDE0, "AnimatedPoisons.esp");
			AnPois_TogglePlayerSound = datahandler->LookupForm<RE::TESGlobal>(0xDE1, "AnimatedPoisons.esp");
			AnPois_PlayerStaggerToggle = datahandler->LookupForm<RE::TESGlobal>(0xE03, "AnimatedPoisons.esp");
			AnPois_PlayerStopAnimation = datahandler->LookupForm<RE::TESGlobal>(0xE04, "AnimatedPoisons.esp");

			AnPois_SlowEffectItem = datahandler->LookupForm<RE::TESObjectMISC>(0x868, "AnimatedPoisons.esp");

			//AnPois_List = datahandler->LookupForm<RE::BGSListForm>(, "AnimatedPoisons.esp");

			AnPois_PoisonSound = datahandler->LookupForm<RE::BGSSoundDescriptorForm>(0x806, "AnimatedPoisons.esp");

			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_DaedricArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_DraugrArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_DwarvenArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_EbonyArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ElvenArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_FalmerArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ForswornArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_GlassArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_IronArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_NordHeroArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_OrcishArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_SteelArrow));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_DaedricArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_DraugrArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_DwarvenArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_EbonyArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ElvenArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_FalmerArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ForswornArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_GlassArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_IronArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_NordHeroArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_OrcishArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_SteelArrowAOSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_SlowEffectSP));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_FakeItem));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ToggleStopSprint));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ToggleForceThirdPerson));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_ToggleForceCloseMenu));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_TogglePlayerSlowEffect));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_TogglePlayerSound));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_PlayerStaggerToggle));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_PlayerStopAnimation));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_SlowEffectItem));
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_PoisonSound));

			if (AnPois_DaedricArrow &&
				AnPois_DraugrArrow &&
				AnPois_DwarvenArrow &&
				AnPois_EbonyArrow &&
				AnPois_ElvenArrow &&
				AnPois_FalmerArrow &&
				AnPois_ForswornArrow &&
				AnPois_GlassArrow &&
				AnPois_IronArrow &&
				AnPois_NordHeroArrow &&
				AnPois_OrcishArrow &&
				AnPois_SteelArrow &&

				AnPois_DaedricArrowAOSP &&
				AnPois_DraugrArrowAOSP &&
				AnPois_DwarvenArrowAOSP &&
				AnPois_EbonyArrowAOSP &&
				AnPois_ElvenArrowAOSP &&
				AnPois_FalmerArrowAOSP &&
				AnPois_ForswornArrowAOSP &&
				AnPois_GlassArrowAOSP &&
				AnPois_IronArrowAOSP &&
				AnPois_NordHeroArrowAOSP &&
				AnPois_OrcishArrowAOSP &&
				AnPois_SteelArrowAOSP &&

				AnPois_SlowEffectSP &&

				AnPois_FakeItem &&

				AnPois_ToggleStopSprint &&
				AnPois_ToggleForceThirdPerson &&
				AnPois_ToggleForceCloseMenu &&
				AnPois_TogglePlayerSlowEffect &&
				AnPois_TogglePlayerSound &&
				AnPois_PlayerStaggerToggle &&
				AnPois_PlayerStopAnimation &&

				AnPois_SlowEffectItem &&

				// AnPois_List &&

				AnPois_PoisonSound) {
				_loadedAnimatedPoisons = true;
				if (Settings::Compatibility::AnimatedPoisons::_Enable) {
					RE::DebugNotification("NPCsUsePotions enabled Animated Poisons compatibility", 0, false);
					LOG_1("Enabled Animated Poisons.");
				}
			}
		}
	case 50:
		{
			AnPois_AnimatedPoisonsMCM = datahandler->LookupForm<RE::TESQuest>(0xE52, AnimatedPoisons_5);
			LOG_1("[AnPois] {}", Utility::PrintForm(AnPois_AnimatedPoisonsMCM));
			if (AnPois_AnimatedPoisonsMCM) {
				_loadedAnimatedPoisons = true;
				if (Settings::Compatibility::AnimatedPoisons::_Enable) {
					RE::DebugNotification("NPCsUsePotions enabled Animated Poisons 5 compatibility", 0, false);
					LOG_1("Enabled Animated Poisons.");
				}
			}
		}
	}

	// animated potions

	// search for AnimatedPotions.esp
	if (const uint32_t index = Utility::Mods::GetPluginIndex(AnimatedPotions_4_4); index != 0x1) {
		AnPoti_Version = 44;
		loginfo("Found plugin Animated Potions.esp and activated compatibility mode");
	}
	if (const uint32_t index = Utility::Mods::GetPluginIndex(AnimatedPotions_4_3); index != 0x1) {
		AnPoti_Version = 43;
		loginfo("Found plugin AnimatedPotions.esp and activated compatibility mode");
	}

	switch (AnPoti_Version) {
	case 44:  // Version 4.4
		AnPoti_TogglePlayerPotionAnimation = datahandler->LookupForm<RE::TESGlobal>(0x8C8, AnimatedPotions_4_4);
		LOG_1("[AnPoti] {}", Utility::PrintForm(AnPoti_TogglePlayerPotionAnimation));
		break;
	case 43:  // version 4.3
		AnPoti_TogglePlayerPotionAnimation = datahandler->LookupForm<RE::TESGlobal>(0x8C8, AnimatedPotions_4_3);
		LOG_1("[AnPoti] {}", Utility::PrintForm(AnPoti_TogglePlayerPotionAnimation));
	}

	if (AnPoti_TogglePlayerPotionAnimation) {
		_loadedAnimatedPotions = true;
		if (Settings::Compatibility::AnimatedPotions::_Enable) {
			RE::DebugNotification("NPCsUsePotions enabled Animated Potions compatibility", 0, false);
			LOG_1("Enabled Animated Potions.");
		}
	}

	// ZUPA
	if (Utility::Mods::GetPluginIndex("zxlice's ultimate potion animation.esp") != 0x1)
		_loadedZUPA = true;

	// Sacrosanct

	Sac_MockeryOfLife = datahandler->LookupForm<RE::EffectSetting>(0x0D592E, Sacrosanct);
	if (Sac_MockeryOfLife != nullptr) {
		_loadedSacrosanct = true;
		RE::DebugNotification("NPCsUsePotions enabled Sacrosanct compatibility", 0, false);
		LOG_1("Enabled Sacrosanct.");
	}

	// Ultimate Animated Potions

	auto findPlugin = [](std::string pluginname) {
		std::wstring wstr = std::wstring(pluginname.begin(), pluginname.end());
		auto pluginHandle = GetModuleHandle(wstr.c_str());
		if (pluginHandle)
			return true;
		return false;
	};

	_loadedUltimatePotions = findPlugin("UAPNG.dll");
	if (_loadedUltimatePotions)
	{
		RE::DebugNotification("NPCsUsePotions found UAPNG", 0, false);
		LOG_1("Found Ultimate Animted Potions.");
	}

	// ordinator
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Ordinator); index != 0x1) {
		LOG_1("Found plugin Ordinator - Perks of Skyrim.esp.");
		_loadedApothecary = true;
	}

	// vokrii
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Vokrii); index != 0x1) {
		LOG_1("Found plugin Vokrii - Minimalistic Perks of Skyrim.esp.");
		_loadedApothecary = true;
	}

	// adamant
	if (const uint32_t index = Utility::Mods::GetPluginIndex(Adamant); index != 0x1) {
		LOG_1("Found plugin Adamant.esp.");
		_loadedApothecary = true;
	}

	// global

	_globalCooldown = std::max((long)_globalCooldown, Settings::Usage::_globalCooldown);
	_globalCooldownPotions = _globalCooldown;
	_globalCooldownPoisons = _globalCooldown;
	_globalCooldownFood = _globalCooldown;
	if (_loadedAnimatedPoisons) {
		_globalCooldownPoisons = std::max(_globalCooldownPoisons, AnPois_GlobalCooldown);
		_disableParalyzedItems = true;
	}
	if (_loadedAnimatedPotions) {
		_globalCooldownPotions = std::max(_globalCooldownPotions, AnPoti_GlobalCooldown);
		_disableParalyzedItems = true;
	}
	if (_loadedZUPA) {
		_globalCooldownPotions = std::max(_globalCooldownPotions, ZUPA_GlobalCooldown);
		_disableParalyzedItems = true;
	}
	if (_loadedUltimatePotions) {
		_globalCooldownPotions = std::max(_globalCooldownPotions, Ult_GlobalCooldown);
	}

	LOG_1("GlobalCooldown set to {}ms", std::to_string(_globalCooldown));

	sem.release();
}

std::binary_semaphore actorpoisonlock{ 1 };
std::binary_semaphore actorpotionlock{ 1 };

void Compatibility::Clear()
{
	// get lock to avoid deadlocks (should not occur, since the functions should not be called simultaneously
	sem.acquire();

	// NPCsUsePotions
	_loadedNPCsUsePotions = false;

	// apothecary
	_loadedApothecary = false;

	// gourmet
	_loadedGourmet = false;
	Gour_FoodTypeAle = nullptr;
	Gour_FoodTypeDrugs = nullptr;
	Gour_FoodTypeWine = nullptr;

	// caco
	_loadedCACO = false;

	CACO_VendorItemDrinkAlcohol = nullptr;

	// animated poisons
	_loadedAnimatedPoisons = false;

	AnPois_DaedricArrow = nullptr;
	AnPois_DraugrArrow = nullptr;
	AnPois_DwarvenArrow = nullptr;
	AnPois_EbonyArrow = nullptr;
	AnPois_ElvenArrow = nullptr;
	AnPois_FalmerArrow = nullptr;
	AnPois_ForswornArrow = nullptr;
	AnPois_GlassArrow = nullptr;
	AnPois_IronArrow = nullptr;
	AnPois_NordHeroArrow = nullptr;
	AnPois_OrcishArrow = nullptr;
	AnPois_SteelArrow = nullptr;

	AnPois_DaedricArrowAOSP = nullptr;
	AnPois_DraugrArrowAOSP = nullptr;
	AnPois_DwarvenArrowAOSP = nullptr;
	AnPois_EbonyArrowAOSP = nullptr;
	AnPois_ElvenArrowAOSP = nullptr;
	AnPois_FalmerArrowAOSP = nullptr;
	AnPois_ForswornArrowAOSP = nullptr;
	AnPois_GlassArrowAOSP = nullptr;
	AnPois_IronArrowAOSP = nullptr;
	AnPois_NordHeroArrowAOSP = nullptr;
	AnPois_OrcishArrowAOSP = nullptr;
	AnPois_SteelArrowAOSP = nullptr;

	AnPois_SlowEffectSP = nullptr;

	AnPois_FakeItem = nullptr;

	AnPois_ToggleStopSprint = nullptr;
	AnPois_ToggleForceThirdPerson = nullptr;
	AnPois_ToggleForceCloseMenu = nullptr;
	AnPois_TogglePlayerSlowEffect = nullptr;
	AnPois_TogglePlayerSound = nullptr;
	AnPois_PlayerStaggerToggle = nullptr;
	AnPois_PlayerStopAnimation = nullptr;

	AnPois_SlowEffectItem = nullptr;

	AnPois_PoisonSound = nullptr;

	// animated potions
	_loadedAnimatedPotions = false;

	AnPoti_TogglePlayerPotionAnimation = nullptr;

	// ZUPA
	_loadedZUPA = false;

	// sacrosanct
	Sac_MockeryOfLife = nullptr;
	_loadedSacrosanct = false;

	// Ultimate Animated Potions
	_loadedUltimatePotions = false;

	// ordinator
	_loadedOrdinator = false;

	// vokrii
	_loadedVokrii = false;

	// adamant
	_loadedAdamant = false;

	// global
	_globalCooldown = 0;
	_AnPois_ActorPoisonMap.clear();
	_disableParalyzedItems = false;


	// clear poison list
	actorpoisonlock.acquire();
	_AnPois_ActorPoisonMap.clear();
	actorpoisonlock.release();

	// clear potion list
	actorpotionlock.acquire();
	_AnPoti_ActorPotionMap.clear();
	actorpotionlock.release();

	sem.release();
}

RE::AlchemyItem* Compatibility::AnPois_FindActorPoison(RE::FormID actor)
{
	if (actor == 0)
		return nullptr;
	RE::AlchemyItem* ret = nullptr;
	actorpoisonlock.acquire();
	auto itr = _AnPois_ActorPoisonMap.find(actor);
	if (itr != _AnPois_ActorPoisonMap.end()) {
		ret = std::get<0>(itr->second);
	}
	actorpoisonlock.release();
	return ret;
}
void Compatibility::AnPois_AddActorPoison(RE::FormID actor, RE::AlchemyItem* poison)
{
	if (actor == 0 || Utility::ValidateForm(poison) == false)
		return;
	actorpoisonlock.acquire();
	_AnPois_ActorPoisonMap.insert_or_assign(actor, std::tuple<RE::AlchemyItem*,RE::FormID>{ poison, poison->GetFormID() });
	actorpoisonlock.release();
}
void Compatibility::AnPois_DeleteActorPoison(RE::FormID form)
{
	if (form == 0)
		return;
	actorpoisonlock.acquire();
	_AnPois_ActorPoisonMap.erase(form);
	auto itr = _AnPois_ActorPoisonMap.begin();
	while (itr != _AnPois_ActorPoisonMap.end()) {
		if (std::get<1>(itr->second) == form) {
			_AnPois_ActorPoisonMap.erase(itr);
			itr--;
		}
		itr++;
	}
	actorpoisonlock.release();
}
void Compatibility::AnPois_RemoveActorPoison(RE::FormID actor)
{
	if (actor == 0)
		return;
	actorpoisonlock.acquire();
	_AnPois_ActorPoisonMap.erase(actor);
	actorpoisonlock.release();
}

RE::AlchemyItem* Compatibility::AnPoti_FindActorPotion(RE::FormID actor)
{
	if (actor == 0)
		return nullptr;
	RE::AlchemyItem* ret = nullptr;
	actorpotionlock.acquire();
	auto itr = _AnPoti_ActorPotionMap.find(actor);
	if (itr != _AnPoti_ActorPotionMap.end()) {
		ret = std::get<0>(itr->second);
	}
	actorpotionlock.release();
	return ret;
}
void Compatibility::AnPoti_AddActorPotion(RE::FormID actor, RE::AlchemyItem* potion)
{
	if (actor == 0 || Utility::ValidateForm(potion) == false)
		return;
	actorpotionlock.acquire();
	_AnPoti_ActorPotionMap.insert_or_assign(actor, std::tuple<RE::AlchemyItem*, RE::FormID>{ potion, potion->GetFormID() });
	actorpotionlock.release();
}
void Compatibility::AnPoti_DeleteActorPotion(RE::FormID form)
{
	if (form == 0)
		return;
	actorpotionlock.acquire();
	_AnPoti_ActorPotionMap.erase(form);
	auto itr = _AnPoti_ActorPotionMap.begin();
	while (itr != _AnPoti_ActorPotionMap.end()) {
		if (std::get<1>(itr->second) == form) {
			_AnPoti_ActorPotionMap.erase(itr);
			itr--;
		}
		itr++;
	}
	actorpotionlock.release();
}
void Compatibility::AnPoti_RemoveActorPotion(RE::FormID actor)
{
	if (actor == 0)
		return;
	actorpotionlock.acquire();
	_AnPoti_ActorPotionMap.erase(actor);
	actorpotionlock.release();
}
std::tuple<RE::AlchemyItem*, int> Compatibility::AnPoti_FindActorPoison(RE::FormID actor)
{
	if (actor == 0)
		return { nullptr, 0 };
	std::tuple<RE::AlchemyItem*, int> ret;
	actorpotionlock.acquire();
	auto itr = _AnPoti_ActorPoisonMap.find(actor);
	if (itr != _AnPoti_ActorPoisonMap.end()) {
		ret = { std::get<0>(itr->second), std::get<2>(itr->second) };
	}
	actorpotionlock.release();
	return ret;
}
void Compatibility::AnPoti_AddActorPoison(RE::FormID actor, RE::AlchemyItem* poison, int count)
{
	if (actor == 0 || Utility::ValidateForm(poison) == false)
		return;
	actorpotionlock.acquire();
	_AnPoti_ActorPoisonMap.insert_or_assign(actor, std::tuple<RE::AlchemyItem*, RE::FormID, int>{ poison, poison->GetFormID(), count });
	actorpotionlock.release();
}
void Compatibility::AnPoti_DeleteActorPoison(RE::FormID form)
{
	if (form == 0)
		return;
	actorpotionlock.acquire();
	_AnPoti_ActorPoisonMap.erase(form);
	auto itr = _AnPoti_ActorPoisonMap.begin();
	while (itr != _AnPoti_ActorPoisonMap.end()) {
		if (std::get<1>(itr->second) == form) {
			_AnPoti_ActorPoisonMap.erase(itr);
			itr--;
		}
		itr++;
	}
	actorpotionlock.release();
}
void Compatibility::AnPoti_RemoveActorPoison(RE::FormID actor)
{
	if (actor == 0)
		return;
	actorpotionlock.acquire();
	_AnPoti_ActorPoisonMap.erase(actor);
	actorpotionlock.release();
}

void SaveGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
{
	
}

void LoadGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
{
	LOG_1("");
	Compatibility::GetSingleton()->Load();
}

void RevertGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
{
	LOG_1("");
	Compatibility::GetSingleton()->Clear();
}

void Compatibility::Register()
{
	Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000100, LoadGameCallback);
	LOG_1("Registered {}", typeid(LoadGameCallback).name());
	Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000200, RevertGameCallback);
	LOG_1("Registered {}", typeid(RevertGameCallback).name());
	Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000300, SaveGameCallback);
	LOG_1("Registered {}", typeid(SaveGameCallback).name());
}

bool Compatibility::CannotRestoreHealth(std::shared_ptr<ActorInfo> acinfo)
{
	bool res = false;
	if (_loadedSacrosanct)
		res |= acinfo->HasMagicEffect(Sac_MockeryOfLife);

	return res;
}

bool Compatibility::CannotRestoreMagicka(std::shared_ptr<ActorInfo> acinfo)
{
	return false;
}

bool Compatibility::CannotRestoreStamina(std::shared_ptr<ActorInfo> acinfo)
{
	return false;
}
