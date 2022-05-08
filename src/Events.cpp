#include"Events.h"
#include "Settings.h"
#include <string.h>
#include<chrono>
#include<thread>
#include <forward_list>
#include <semaphore>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <fstream>
#include <iostream>
#include "ActorManipulation.h"
#include <limits>

		
namespace Events
{
	using AlchemyEffect = Settings::AlchemyEffect;

	/// <summary>
	/// random number generator for processing probabilities
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	static std::mt19937 rand((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
	/// <summary>
	/// trims random numbers to 1 to 100 
	/// </summary>
	static std::uniform_int_distribution<signed> rand100(1, 100);	
	/// <summary>
	/// trims random numbers to 1 to 3
	/// </summary>
	static std::uniform_int_distribution<signed> rand3(1, 3);

#define Look(s) RE::TESForm::LookupByEditorID(s)

	static bool initialized = false;

	/// <summary>
	/// initializes importent variables, which need to be initialized every time a game is loaded
	/// </summary>
	void InitializeCompatibilityObjects()
	{
		// now that the game was loaded we can try to initialize all our variables we conuldn't before
		{
			// load our stuff like necessary forms
			// get VendorItemPotion keyword, if we don't find this, potion detection will be nearly impossible
			Settings::VendorItemPotion = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEC);
			if (Settings::VendorItemPotion == nullptr) {
				logger::info("[INIT] Couldn't find VendorItemPotion Keyword in game.");
			}

			// if we are in com mode, try to find the needed items. If we cannot find them, deactivate comp mode
			if (Settings::_CompatibilityPotionAnimatedFx) {
				RE::TESForm* tmp = RE::TESForm::LookupByEditorID(std::string_view{ Settings::Compatibility::PAF_NPCDrinkingCoolDownEffect_name });
				if (tmp)
					Settings::Compatibility::PAF_NPCDrinkingCoolDownEffect = tmp->As<RE::EffectSetting>();
				tmp = RE::TESForm::LookupByEditorID(std::string_view{ Settings::Compatibility::PAF_NPCDrinkingCoolDownSpell_name });
				if (tmp)
					Settings::Compatibility::PAF_NPCDrinkingCoolDownSpell = tmp->As<RE::SpellItem>();
				if (!(Settings::Compatibility::PAF_NPCDrinkingCoolDownEffect && Settings::Compatibility::PAF_NPCDrinkingCoolDownSpell)) {
					Settings::_CompatibilityPotionAnimatedFx = false;
					Settings::_CompatibilityPotionAnimatedFX_UseAnimations = false;
					logger::info("[INIT] Some Forms from PotionAnimatedfx.esp seem to be missing. Forcefully deactivated compatibility mode");
				}
			}
		}
		// if compatibility mode for PotionnAnimatedFx is activated to use the animations, send events
		// with required variables to the papyrus scripts
		if (Settings::_CompatibilityPotionAnimatedFX_UseAnimations && !initialized) {
			auto evs = SKSE::GetModCallbackEventSource();

			LOG_1("{}[LoadGameEvent] Setting variables for compatibility with PotionAnimatedfx.esp");
			SKSE::ModCallbackEvent* ev = nullptr;
			// send mod events to fill ALL the missing variables
			// since there are multiple plugins with the same name
			// and the same editor ids, but with different form
			// ids and we may only query FormIDs in papyrus

			LOG_4("{}[LoadGameEvent] Set 1");
			// PAF_DrinkSFX
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_DrinkSFX");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_DrinkSFX" })->As<RE::TESGlobal>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 2");
			// PAF_NPCDrinkingSlowVersion
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingSlowVersion");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingSlowVersion" })->As<RE::TESGlobal>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 3");
			// PAF_NPCFleeChance
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCFleeChance");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCFleeChance" })->As<RE::TESGlobal>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 6");
			// PAF_NPCDrinkingCoolDownSpell
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingCoolDownSpell");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingCoolDownSpell" })->As<RE::SpellItem>();
				evs->SendEvent(ev);
			}
			/*LOG_4("{}[LoadGameEvent] Set 5");
			// PAF_CustomDrinkPotionSound
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_CustomDrinkPotionSound");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_CustomDrinkPotionSound" })->As<RE::TESSound>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 4");
			// PAF_OriginalDrinkPotionSound
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_OriginalDrinkPotionSound");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_OriginalDrinkPotionSound" })->As<RE::TESSound>();
				evs->SendEvent(ev);
			}*/
			LOG_4("{}[LoadGameEvent] Set 7");
			// PAF_NPCDrinkingInterruptDetectAbility
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingInterruptDetectAbility");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingInterruptDetectAbility" })->As<RE::SpellItem>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 8");
			// PAF_NPCDrinkFleeSpell
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkFleeSpell");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkFleeSpell" })->As<RE::SpellItem>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 9");
			// PAF_NPCDrinkingInterruptSpell
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingInterruptSpell");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingInterruptSpell" })->As<RE::SpellItem>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 10");
			// PAF_NPCDrinkingCoolDownEffect
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingCoolDownEffect");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingCoolDownEffect" })->As<RE::EffectSetting>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 11");
			// PAF_NPCDrinkingInterruptDetectEffect
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingInterruptDetectEffect");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingInterruptDetectEffect" })->As<RE::EffectSetting>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 12");
			// PAF_NPCDrinkFleeEffect
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkFleeEffect");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkFleeEffect" })->As<RE::EffectSetting>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 13");
			// PAF_NPCDrinkingInterruptEffect
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_NPCDrinkingInterruptEffect");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_NPCDrinkingInterruptEffect" })->As<RE::EffectSetting>();
				evs->SendEvent(ev);
			}
			LOG_4("{}[LoadGameEvent] Set 14");
			// PAF_MCMQuest
			{
				ev = new SKSE::ModCallbackEvent();
				ev->eventName = RE::BSFixedString("NDP_Comp_PAF_MCMQuest");
				ev->strArg = RE::BSFixedString("");
				ev->numArg = 0.0f;
				ev->sender = Look(std::string_view{ "PAF_MCMQuest" })->As<RE::TESQuest>();
				evs->SendEvent(ev);
			}

			// send control event to enable compatibility mode and check the delivered objects
			ev = new SKSE::ModCallbackEvent();
			ev->eventName = RE::BSFixedString("NPCsDrinkPotionCompPAFX");
			ev->strArg = RE::BSFixedString("");
			ev->numArg = 1.0f;
			ev->sender = nullptr;
			evs->SendEvent(ev);
			LOG_4("{}[LoadGameEvent] Sent controlevent");
			initialized = true;
		}
	}


    /// <summary>
    /// Searches the inventory of an Actor for an appropiate potion and applies it.
	/// This is iterated until the desired actor values are above the threshold
	/// DEPRECATED
    /// </summary>
    /// <param name="_actor">The actor reference to process</param>
    /// <param name="magicka">Wether magicka should be restored</param>
    /// <param name="stamina">Wether stamina should be restored</param>
    /// <param name="health">Wether health should be restored</param>
    void ActorUsePotion(RE::Actor* _actor, bool magicka, bool stamina, bool health)
    {
		auto begin = std::chrono::steady_clock::now();
		// if there is nothing to do then just return
		if (!magicka && !stamina && !health)
			return;
        // get inventory contents from the reference
        //std::map<TESBoundObject*, std::pair<Count, std::unique_ptr<InventoryEntryData>>>;
        auto itemmap = _actor->GetInventory();
		// get starting point
		auto iter = itemmap.begin(); 
		// get endpoint
		auto end = itemmap.end(); 
		float mag = _actor->GetActorValue(RE::ActorValue::kMagicka);
		float stam = _actor->GetActorValue(RE::ActorValue::kStamina);
		float hea = _actor->GetActorValue(RE::ActorValue::kHealth);
		RE::AlchemyItem* item = nullptr;
		RE::EffectSetting* sett = nullptr;
		int idrunk = 0;
		// if there are items in the inventory then do something
		if (iter != end) {
			LOG_2("{}[UsePotion] trying to find potion");
			// searches for potions to use
			while (iter != end && (magicka || stamina || health) && idrunk < Settings::_maxPotionsPerCycle) {
				//logger::info("idrunk: {}", std::to_string(idrunk));
				item = iter->first->As<RE::AlchemyItem>();
				LOG_3("{}[UsePotion] checking item");
				// get item and check wether it is a potion
				if (item && !(item->IsPoison()) && !(item->IsFood())) {
					LOG_3("{}[UsePotion] found medicine");
					// object is alchemyitem and is a potion
					// get the actovalue associated with the potion
					if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
						iter++;
						continue;
					}
					RE::ActorValue avh = RE::ActorValue::kAlchemy;
					RE::ActorValue avm = RE::ActorValue::kAlchemy;
					RE::ActorValue avs = RE::ActorValue::kAlchemy;
					float magh = 0;
					float magm = 0;
					float mags = 0;
					// we will not abort the loop, since the number of effects on one item is normally very
					// limited, so we don't have much iterations
					if (item->effects.size() > 0) {
						for (uint32_t i = 0; i < item->effects.size(); i++) {
							LOG_4("{}[UsePotion] found array effect");
							sett = item->effects[i]->baseEffect;
							// if the primary AV affected by the effect matches one of the following
							// set the appropiate variables, so we effectively track the restoration of
							// all relevant stats, for items restoring more than one AV at a time.
							if (sett && sett->data.primaryAV == RE::ActorValue::kMagicka) {
								LOG_4("{}[UsePotion] found matching magicka effect");
								avm = sett->data.primaryAV;
								magm = item->effects[i]->effectItem.magnitude;
							} else if (sett && sett->data.primaryAV == RE::ActorValue::kStamina) {
								LOG_4("{}[UsePotion] found matching stamina effect");
								avs = sett->data.primaryAV;
								mags = item->effects[i]->effectItem.magnitude;
							} else if (sett && sett->data.primaryAV == RE::ActorValue::kHealth) {
								LOG_4("{}[UsePotion] found matching health effect");
								avh = sett->data.primaryAV;
								magh = item->effects[i]->effectItem.magnitude;
							}
						}
					} else {
						RE::MagicItem::SkillUsageData err;
						item->GetSkillUsageData(err);
						switch (item->avEffectSetting->data.primaryAV) {
						case RE::ActorValue::kHealth:
							avh = RE::ActorValue::kHealth;
							magh = err.magnitude;
							break;
						case RE::ActorValue::kMagicka:
							avm = RE::ActorValue::kMagicka;
							magm = err.magnitude;
							break;
						case RE::ActorValue::kStamina:
							avs = RE::ActorValue::kStamina;
							mags = err.magnitude;
							break;
						}
					}
					LOG_3("{}[UsePotion] Drink Potion prepare");
					RE::ExtraDataList* extra = new RE::ExtraDataList();
					extra->SetOwner(_actor);
					bool drunk = false;
					// based on what kind of potion we have, we will use it
					if (avh == RE::ActorValue::kHealth) {
						if (health) {
							LOG_3("{}[UsePotion] Drink Potion");
							if(Settings::_CompatibilityMode || Settings::_CompatibilityPotionAnimation)
							{
								LOG_3("{}[UsePotion] Compatibility Mode")
								SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
								ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
								ev->strArg = RE::BSFixedString("");
								ev->numArg = 0.0f;
								ev->sender = _actor;
								SKSE::GetModCallbackEventSource()->SendEvent(ev);
								ev = new SKSE::ModCallbackEvent();
								ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
								ev->strArg = RE::BSFixedString("");
								ev->numArg = 0.0f;
								ev->sender = item;
								SKSE::GetModCallbackEventSource()->SendEvent(ev);
							}
							else
							{
								RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, item, extra);
							}
							drunk = true;
							//logger::info("drinking");
							LOG1_4("{}[UsePotion] value health {}", std::to_string(magh));
						}
					}
					if (avm == RE::ActorValue::kMagicka) {
						if (magicka) {
							LOG_3("{}[UsePotion] Drink Potion");
							if(Settings::_CompatibilityMode || Settings::_CompatibilityPotionAnimation)
							{
								LOG_3("{}[UsePotion] Compatibility Mode")
								SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
								ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
								ev->strArg = RE::BSFixedString("");
								ev->numArg = 0.0f;
								ev->sender = _actor;
								SKSE::GetModCallbackEventSource()->SendEvent(ev);
								ev = new SKSE::ModCallbackEvent();
								ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
								ev->strArg = RE::BSFixedString("");
								ev->numArg = 0.0f;
								ev->sender = item;
								SKSE::GetModCallbackEventSource()->SendEvent(ev);
							}
							else
							{
								RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, item, extra);
							}
							drunk = true;
							//logger::info("drinking");
							LOG1_4("{}[UsePotion] value magicka {}", std::to_string(magm));
						}
					}
					if (avs == RE::ActorValue::kStamina) {
						if (stamina) {
							LOG_3("{}[UsePotion] Drink Potion");
							if (Settings::_CompatibilityMode || Settings::_CompatibilityPotionAnimation) {
								LOG_3("{}[UsePotion] Compatibility Mode")
								SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
								ev->eventName = RE::BSFixedString("NPCsDrinkPotionActorInfo");
								ev->strArg = RE::BSFixedString("");
								ev->numArg = 0.0f;
								ev->sender = _actor;
								SKSE::GetModCallbackEventSource()->SendEvent(ev);
								ev = new SKSE::ModCallbackEvent();
								ev->eventName = RE::BSFixedString("NPCsDrinkPotionEvent");
								ev->strArg = RE::BSFixedString("");
								ev->numArg = 0.0f;
								ev->sender = item;
								SKSE::GetModCallbackEventSource()->SendEvent(ev);
							} else {
								RE::ActorEquipManager::GetSingleton()->EquipObject(_actor, item, extra);
							}
							drunk = true;
							//logger::info("drinking");
							LOG1_4("{}[UsePotion] value stamina {}", std::to_string(mags));
						}
					}
					if (drunk) {
						//logger::info("increasing drunk state");
						mag += magm;
						stam += mags;
						hea += magh;
						idrunk++;
					}
				

					/// TODO build in mechanic to track how much ressuorces have been restored, since AV calculations
					/// take too long.


					// check wether the use of the potin pushed our desired actor values over the threshold
					// if so disable the appropiate ones, and continue.
					if (magicka && ACM::GetAVPercentageFromValue(_actor, RE::ActorValue::kMagicka, mag) > Settings::_magickaThresholdUpper) {
						magicka = false;
						LOG_3("{}[UsePotion] disabled magicka");
					}
					if (health && ACM::GetAVPercentageFromValue(_actor, RE::ActorValue::kHealth, hea) > Settings::_healthThresholdUpper) {
						health = false;
						LOG_3("{}[UsePotion] disabled health");
					}
					if (stamina && ACM::GetAVPercentageFromValue(_actor, RE::ActorValue::kStamina, stam) > Settings::_staminaThresholdUpper) {
						stamina = false;
						LOG_3("{}[UsePotion] disabled stamina");
					}
				}
				// advance to next item if not end of function
				iter++;
			}
		}
		PROF1_2("{}[PROF] [ActorUsePotion] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
    }   

	/// <summary>
	/// extracts all poisons from a list of AlchemyItems
	/// </summary>
	/// <param name="items"></param>
	/// <returns></returns>
	std::list<RE::AlchemyItem*> GetPoisons(std::list<RE::AlchemyItem*>& items)
	{
		std::list<RE::AlchemyItem*> ret{};
		for (auto item : items)
			if (item->IsPoison())
				ret.insert(ret.begin(), item);
		return ret;
	}
	/// <summary>
	/// extracts all food items from a list of alchemyitems
	/// </summary>
	/// <param name="items"></param>
	/// <returns></returns>
	std::list<RE::AlchemyItem*> GetFood(std::list<RE::AlchemyItem*>& items)
	{
		std::list<RE::AlchemyItem*> ret{};
		for (auto item : items)
			if (item->IsFood())
				ret.insert(ret.begin(), item);
		return ret;
	}
	/// <summary>
	/// extracts all potions from a list of alchemyitems
	/// </summary>
	/// <param name="items"></param>
	/// <returns></returns>
	std::list<RE::AlchemyItem*> GetPotions(std::list<RE::AlchemyItem*>& items)
	{
		std::list<RE::AlchemyItem*> ret{};
		for (auto item : items)
			if ((item->IsMedicine() || item->HasKeyword(Settings::VendorItemPotion)))
				ret.insert(ret.begin(), item);
		return ret;
	}

	/// <summary>
	/// This function returns all alchemy items contained in the death item lists of the actor
	/// </summary>
	std::list<RE::AlchemyItem*> FindDeathAlchemyItems(RE::Actor * _actor)
	{
		LOG_2("{}[FINDALCHEMYITEMS] begin");
		// create return list
		std::list<RE::AlchemyItem*> ret{};
		// get first leveled list
		if (_actor->GetActorBase() == nullptr)
			return ret;
		RE::TESLeveledList* deathlist = _actor->GetActorBase()->deathItem;
		if (deathlist == nullptr)
			return ret;
		// fix pointer to entries of first list
		auto entries = &(deathlist->entries);
		std::int8_t chancenone = deathlist->chanceNone;
		std::list<RE::TESLeveledList*> lists{};

		// tmp vars
		RE::TESLeveledList* ls = nullptr;
		RE::AlchemyItem* al = nullptr;
		int iter = 0;
		// while entries pointer is valid search the leveled list for matching items
		while (entries != nullptr && iter < 30) {
			LOG_4("{}[FINDALCHEMYITEMS] iter");
			if (rand100(rand) > chancenone)
				for (int i = 0; i < entries->size(); i++) {
					al = (*entries)[i].form->As<RE::AlchemyItem>();
					ls = (*entries)[i].form->As<RE::TESLeveledList>();
					LOG_4("{}[FINDALCHEMYITEMS] checking item");
					if (al) {
						ret.insert(ret.begin(), al);
						LOG_4("{}[FINDALCHEMYITEMS] found alchemy item");
					}
					else if (ls)
						lists.insert(lists.begin(), ls);
				}
			if (lists.size() > 0) {
				entries = &(lists.front()->entries);
				chancenone = lists.front()->chanceNone;
				lists.pop_front();
			} else {
				entries = nullptr;
				chancenone = 0;
			}
			iter++;
		}
		LOG_2("{}[FINDALCHEMYITEMS] end");
		return ret;
	}

    /// <summary>
    /// Processes TESHitEvents
    /// </summary>
    /// <param name=""></param>
    /// <param name=""></param>
    /// <returns></returns>
    EventResult EventHandler::ProcessEvent(const RE::TESHitEvent* /*a_event*/, RE::BSTEventSource<RE::TESHitEvent>*)
    {
		// currently unused 
		// MARK TO REMOVE
		return EventResult::kContinue;
    }

	// Actor, health cooldown, magicka cooldown, stamina cooldown, other cooldown, reg cooldown
	static std::vector<std::tuple<RE::Actor*, int, int, int, int, int>> aclist{};
	// semaphore used to sync access to actor handling, to prevent list changes while operations are done
	static std::binary_semaphore sem(1);
	// map actorid -> GameDay (reset)
	// used to determine which actors may be given potions, and which recently received some
	static std::map<uint64_t, float> actorresetmap{};

    /// <summary>
    /// handles TESCombatEvent
	/// registers the actor for tracking and handles giving them potions, poisons and food, beforehand.
	/// also makes then eat food before the fight.
    /// </summary>
    /// <param name="a_event">event parameters like the actor we need to handle</param>
    /// <param name=""></param>
    /// <returns></returns>
    EventResult EventHandler::ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
    {
		auto actor = a_event->actor->As<RE::Actor>();
		if (actor && !actor->IsDead() && actor != RE::PlayerCharacter::GetSingleton()) {
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				LOG_1("{}[TesCombatEvent] Trying to register new actor for potion tracking");

				//test section
				//LOG_1("{}find alchemy items");
				//auto ls = FindDeathAlchemyItems(actor);
				//std::ofstream out(R"(Data\SKSE\testfile.ini)", std::ofstream::app);
				//for (RE::AlchemyItem* it : ls) {
				//	out << it->formID << "\t\t" << it->GetName() << "\n";
				//}

				/*RE::ExtraDataList* extra = new RE::ExtraDataList();
				if (Settings::_potionsWeak_main.size() > 0) {
					actor->AddObjectToContainer(std::get<1>(Settings::_potionsWeak_main.front()), extra, 100, nullptr);
				}*/
				/*if (ls.size() == 0)
					out << "no items found\n";
				out << "\n\n";
				out.close();*/
				//auto poisons = GetPoisons(ls);
				
				sem.acquire();
				InitializeCompatibilityObjects();
				auto it = aclist.begin();
				auto end = aclist.end();
				bool cont = false;
				while (it != end) {
					if (std::get<0>(*it) == actor) {
						cont = true;
						break;
					}
					it++;
				}
				if (!cont)
					aclist.insert(aclist.begin(), { actor, 0, 0, 0, 0, 0 });
				sem.release();
				LOG_1("{}[TesCombatEvent] finished registering NPC");
			} else {
				LOG_1("{}[TesCombatEvent] Unregister NPC from potion tracking")
				sem.acquire();
				auto it = aclist.begin();
				auto end = aclist.end();
				while (it != end) {
					if (std::get<0>(*it) == actor) {
						aclist.erase(it);
						break;
					}
					it++;
				}
				sem.release();
				LOG_1("{}[TesCombatEvent] Unregistered NPC");
			}
		}

		return EventResult::kContinue;
    }

	/// <summary>
	/// if set to true stops the CheckActors thread on its next iteration
	/// </summary>
	static bool stopactorhandler = false;
	static bool actorhandlerrunning = false;
	/// <summary>
	/// thread running the CheckActors function
	/// </summary>
	static std::thread* actorhandler = nullptr;

#define UseHealth(t, dur, tolerance) \
	if (Settings::_featHealthRestoration && dur < tolerance && ACM::GetAVPercentage(std::get<0>(t), RE::ActorValue::kHealth) < Settings::_healthThresholdLower) \
		dur = ACM::ActorUsePotion(std::get<0>(aclist[i]), AlchemyEffect::kHealth);
#define UseMagicka(t, dur, tolerance) \
	if (Settings::_featMagickaRestoration && dur < tolerance && ACM::GetAVPercentage(std::get<0>(t), RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower) \
		dur = ACM::ActorUsePotion(std::get<0>(aclist[i]), AlchemyEffect::kMagicka);
#define UseStamina(t, dur, tolerance) \
	if (Settings::_featStaminaRestoration && dur < tolerance && ACM::GetAVPercentage(std::get<0>(t), RE::ActorValue::kStamina) < Settings::_staminaThresholdLower) \
		dur = ACM::ActorUsePotion(std::get<0>(aclist[i]), AlchemyEffect::kMagicka);



	/// <summary>
	/// Main routine that periodically checks the actors status, and applies items
	/// </summary>
	void CheckActors()
	{
		actorhandlerrunning = true;
		/// static section
		RE::UI* ui = RE::UI::GetSingleton();
		// profile
		auto begin = std::chrono::steady_clock::now();
		// tolerance for potion drinking, to diminish effects of computation times
		// on cycle time
		int tolerance = Settings::_cycletime / 5;

		/// player vars
		int durhp = 0; // duration health player
		int durmp = 0; // duration magicka player
		int dursp = 0; // duration stamina player
		// to get this to 0 you would need to play nearly 600 hours
		int durotherp = INT_MAX; // duration of buff potions for the player
		int durregp = INT_MAX; // duration of reg potions for the player

		/// temp section
		int durh = 0; // duration health
		int durm = 0; // duration magicka
		int durs = 0; // duration stamina
		int durother = 0; // duration other
		int durreg = 0; // duration of regeneration
		uint64_t alch = 0;
		uint64_t alch2 = 0;
		uint64_t alch3 = 0;
		bool player = false; // wether player was inserted into list
		
		// main loop, if the thread should be stopped, exit the loop
		while (!stopactorhandler) {
			// if we are in a paused menu (SoulsRE unpauses menus, which is supported by this)
			// do not compute, since nobody can actually take potions.
			if (!ui->GameIsPaused()) {
				// reset player var.
				player = false;
				// get starttime of iteration
				begin = std::chrono::steady_clock::now();

				sem.acquire();
				// checking if player should be handled
				if (Settings::_playerRestorationEnabled && RE::PlayerCharacter::GetSingleton()->IsInCombat()) {
					// inject player into the list and remove him later
					aclist.insert(aclist.end(), { RE::PlayerCharacter::GetSingleton(), durhp, durmp, dursp, durotherp, durregp });
					LOG_3("{}[CheckActors] Adding player to the list");
					player = true;
				}
				LOG1_1("{}[CheckActors] Handling all registered Actors {}", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count()));
				// handle all registered actors
				// the list does not change while doing this
				for (int i = 0; i < aclist.size(); i++) {
					LOG4_1("{}[CheckActors] [Actor] {}", Utility::GetHex(std::get<0>(aclist[i])->GetFormID()));
					// if actor is valid and not dead
					if (std::get<0>(aclist[i]) && !(std::get<0>(aclist[i]))->IsDead()) {
						// get current duration
						durh = std::get<1>(aclist[i]) - Settings::_cycletime;
						durm = std::get<2>(aclist[i]) - Settings::_cycletime;
						durs = std::get<3>(aclist[i]) - Settings::_cycletime;
						durother = std::get<4>(aclist[i]) - Settings::_cycletime;
						durreg = std::get<5>(aclist[i]) - Settings::_cycletime;
						// get combined effect for magicka, health, and stamina
						if (Settings::_featHealthRestoration && durh < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kHealth) < Settings::_healthThresholdLower)
							alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
						else
							alch = 0;
						if (Settings::_featMagickaRestoration && durm < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower)
							alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
						else
							alch2 = 0;
						if (Settings::_featStaminaRestoration && durs < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kStamina) < Settings::_staminaThresholdLower)
							alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
						else
							alch3 = 0;
						// construct combined effect
						alch |= alch2 | alch3;
						LOG4_4("{}[CheckActors] check for alchemyeffect {} with current dur health {} dur mag {} dur stam {} ", alch, durh, durm, durs);
						// use potions
						// do the first round
						if (alch != 0) {
							auto tup = ACM::ActorUsePotion(std::get<0>(aclist[i]), alch);
							LOG1_2("{}[CheckActors] found potion has Alchemy Effect {}", static_cast<uint64_t>(std::get<0>(tup)));
							switch (std::get<1>(tup)) {
							case AlchemyEffect::kHealth:
								durh = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;  // convert to milliseconds
								LOG2_4("{}[CheckActors] use health pot with duration {} and magnitude {}", durh, std::get<0>(tup));
								break;
							case AlchemyEffect::kMagicka:
								durm = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
								LOG2_4("{}[CheckActors] use magicka pot with duration {} and magnitude {}", durh, std::get<0>(tup));
								break;
							case AlchemyEffect::kStamina:
								durs = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
								LOG2_4("{}[CheckActors] use stamina pot with duration {} and magnitude {}", durh, std::get<0>(tup));
								break;
							}
							// do the rest of the rounds
							for (int c = 1; c < Settings::_maxPotionsPerCycle; c++) {
								// get combined effect for magicka, health, and stamina
								if (Settings::_featHealthRestoration && durh < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kHealth) < Settings::_healthThresholdLower)
									alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
								else
									alch = 0;
								if (Settings::_featMagickaRestoration && durm < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower)
									alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
								else
									alch2 = 0;
								if (Settings::_featStaminaRestoration && durs < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kStamina) < Settings::_staminaThresholdLower)
									alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
								else
									alch3 = 0;
								// construct combined effect
								alch |= alch2 | alch3;
								if (alch != 0) {
									tup = ACM::ActorUsePotion(std::get<0>(aclist[i]), std::get<2>(tup));
									switch (std::get<1>(tup)) {
									case AlchemyEffect::kHealth:
										durh = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
										break;
									case AlchemyEffect::kMagicka:
										durm = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
										break;
									case AlchemyEffect::kStamina:
										durs = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
										break;
									}
								} else if (durother < tolerance)
									goto DirJ;
								else
									break;
							}
						} else if (durother < tolerance) {
						DirJ:
							;
							// call function to use appropiate potion here

							// we only want to use at max one other potion at a time.
						}
						// update entry in vector
						aclist[i] = { std::get<0>(aclist[i]), durh, durm, durs, durother, durreg };
					} else {
						// actor dead or invalid
						// dont remove it, since we would need an iterator for that ... which we don't have
						// the list doesn't persist between game starts, so it doesn't hurt leaving it
						// and an actor is removed from combat once they die anyway, so this case shouldn't happen
					}
				}

				// if we inserted the player, remove them and get their applied values
				if (player) {
					LOG_3("{}[CheckActors] Removing Player from the list.");
					auto tup = aclist.back();
					aclist.pop_back();
					durhp = std::get<1>(tup);
					durmp = std::get<2>(tup);
					dursp = std::get<3>(tup);
				}
				// release lock.
				sem.release();
				// write execution time of iteration
				PROF1_1("{}[PROF] [CheckActors] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("{}[CheckActors] check {} actors", std::to_string(aclist.size()));
			}
			std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(Settings::_cycletime));
		}
		stopactorhandler = false;
		actorhandlerrunning = false;
	}

	/// <summary>
	/// EventHandler for TESLoadGameEvent. Loads main thread
	/// </summary>
	/// <param name="">unused</param>
	/// <param name="">unused</param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESLoadGameEvent*, RE::BSTEventSource<RE::TESLoadGameEvent>*)
	{
		// if we canceled the main thread, reset that
		stopactorhandler = false;
		// set initialized to false, since we did load or reload the game
		initialized = false;
		if (actorhandlerrunning == false) {
			if (actorhandler != nullptr) {
				// if the thread is there, then destroy and delete it
				// if it is joinable and not running it has already finished, but needs to be joined before
				// it can be destroyed savely
				if (actorhandler->joinable())
					actorhandler->join();
				actorhandler->~thread();
				delete actorhandler;
				actorhandler = nullptr;
			}
			actorhandler = new std::thread(CheckActors);
			LOG_1("{}[LoadGameEvent] Started CheckActors");
		}
		return EventResult::kContinue;
	}

    /// <summary>
    /// returns singleton to the EventHandler
    /// </summary>
    /// <returns></returns>
    EventHandler* EventHandler::GetSingleton()
    {
        static EventHandler singleton;
        return std::addressof(singleton);
    }

    /// <summary>
    /// Registers us for all Events we want to receive
    /// </summary>
    void EventHandler::Register()
    {
		auto scriptEventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
        scriptEventSourceHolder->GetEventSource<RE::TESHitEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESHitEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESCombatEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESCombatEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESLoadGameEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESCombatEvent).name());
	}

	/// <summary>
	/// Registers all EventHandlers, if we would have multiple
	/// </summary>
	void RegisterAllEventHandlers()
	{
		EventHandler::Register();
		LOG_1("{}Registered all event handlers"sv);
	}

	/// <summary>
	/// sets the main threads to stop on the next iteration
	/// </summary>
	void DisableThreads()
	{
		stopactorhandler = true;
	}
}
