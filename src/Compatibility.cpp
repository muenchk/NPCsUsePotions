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

	// apothecary
	if (Settings::Compatibility::Apothecary::_CompatibilityApothecary)
		_loadedApothecary = true;

	// caco
	if (Settings::Compatibility::CACO::_CompatibilityCACO)
		_loadedCACO = true;

	// animated poisons
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

	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_DaedricArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_DraugrArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_DwarvenArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_EbonyArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ElvenArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_FalmerArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ForswornArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_GlassArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_IronArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_NordHeroArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_OrcishArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_SteelArrow));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_DaedricArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_DraugrArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_DwarvenArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_EbonyArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ElvenArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_FalmerArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ForswornArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_GlassArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_IronArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_NordHeroArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_OrcishArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_SteelArrowAOSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_SlowEffectSP));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_FakeItem));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ToggleStopSprint));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ToggleForceThirdPerson));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_ToggleForceCloseMenu));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_TogglePlayerSlowEffect));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_TogglePlayerSound));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_PlayerStaggerToggle));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_PlayerStopAnimation));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_SlowEffectItem));
	LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPois_PoisonSound));

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

		AnPois_PoisonSound
		) {
		_loadedAnimatedPoisons = true;
		if (Settings::Compatibility::AnimatedPoisons::_CompatibilityAnimatedPoisons && Settings::Compatibility::AnimatedPoisons::_Enable)
			RE::DebugNotification("NPCsUsePotions enabled Animated Poisons compatibility", 0, false);
	}

	// animated potions
	
	switch (AnPoti_Version) {
	case 44: // Version 4.4
		AnPoti_TogglePlayerPotionAnimation = datahandler->LookupForm<RE::TESGlobal>(0x8C8, AnimatedPotions_4_4);
		LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPoti_TogglePlayerPotionAnimation));
		break;
	case 43: // version 4.3
		AnPoti_TogglePlayerPotionAnimation = datahandler->LookupForm<RE::TESGlobal>(0x8C8, AnimatedPotions_4_3);
		LOG1_1("{}[Compatibility] [Load] {}", Utility::PrintForm(AnPoti_TogglePlayerPotionAnimation));
	}

	if (AnPoti_TogglePlayerPotionAnimation) {
		_loadedAnimatedPotions = true;
		if (Settings::Compatibility::AnimatedPotions::_CompatibilityAnimatedPotions && Settings::Compatibility::AnimatedPotions::_Enable)
			RE::DebugNotification("NPCsUsePotions enabled Animated Potions compatibility", 0, false);
	}
	

	// potion animated fx
	PAF_NPCDrinkingCoolDownEffect = RE::TESForm::LookupByEditorID<RE::EffectSetting>(std::string_view{ PAF_NPCDrinkingCoolDownEffect_name });
	PAF_NPCDrinkingCoolDownSpell = RE::TESForm::LookupByEditorID<RE::SpellItem>(std::string_view{ PAF_NPCDrinkingCoolDownSpell_name });
	if (PAF_NPCDrinkingCoolDownEffect && PAF_NPCDrinkingCoolDownSpell) {
		_loadedPotionAnimatedFx = true;
		if (Settings::Compatibility::PotionAnimatedFx::_CompatibilityPotionAnimatedFx)
			RE::DebugNotification("NPCsUsePotions enabled AnimatedPotionFx compatibility",0, false);
	}

	// ZUPA
	if (Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimation || Settings::Compatibility::UltimatePotionAnimation::_CompatibilityPotionAnimationFortify)
		_loadedZUPA = true;

	// global

	_globalCooldown = max(_globalCooldown, Settings::Usage::_globalCooldown);
	if (_loadedAnimatedPoisons) {
		_globalCooldown = max(_globalCooldown, AnPois_GlobalCooldown);
		_disableParalyzedItems = true;
	}
	if (_loadedAnimatedPotions) {
		_globalCooldown = max(_globalCooldown, AnPoti_GlobalCooldown);
		_disableParalyzedItems = true;
	}
	if (_loadedZUPA) {
		_globalCooldown = max(_globalCooldown, ZUPA_GlobalCooldown);
		_disableParalyzedItems = true;
	}
	if (_loadedPotionAnimatedFx) {
		_disableParalyzedItems = true;
	}



	sem.release();
}

std::binary_semaphore actorpoisonlock{ 1 };
std::binary_semaphore actorpotionlock{ 1 };

void Compatibility::Clear()
{
	// get lock to avoid deadlocks (should not occur, since the functions should not be called simultaneously
	sem.acquire();
	// apothecary
	_loadedApothecary = false;

	// caco
	_loadedCACO = false;

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

	// potion animated fx
	_loadedPotionAnimatedFx = false;

	PAF_NPCDrinkingCoolDownEffect = nullptr;
	PAF_NPCDrinkingCoolDownSpell = nullptr;

	// ZUPA
	_loadedZUPA = false;

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
	LOG_1("{}[Compatibility] [LoadGameCallback]");
	Compatibility::GetSingleton()->Load();
}

void RevertGameCallback(SKSE::SerializationInterface* /*a_intfc*/)
{
	LOG_1("{}[Compatibility] [RevertGameCallback]");
	Compatibility::GetSingleton()->Clear();
}

void Compatibility::Register()
{
	Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000100, LoadGameCallback);
	LOG1_1("{}Registered {}", typeid(LoadGameCallback).name());
	Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000200, RevertGameCallback);
	LOG1_1("{}Registered {}", typeid(RevertGameCallback).name());
	Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000300, SaveGameCallback);
	LOG1_1("{}Registered {}", typeid(SaveGameCallback).name());
}
