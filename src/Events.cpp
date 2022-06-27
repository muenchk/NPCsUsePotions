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
#include <filesystem>
#include <deque>

		
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

	/// <summary>
	/// determines whether events and functions are run
	/// </summary>
	static bool initialized = false;

	/// <summary>
	/// initializes importent variables, which need to be initialized every time a game is loaded
	/// </summary>
	void InitializeCompatibilityObjects()
	{
		// now that the game was loaded we can try to initialize all our variables we conuldn't before
		if (!initialized) {
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
			// if compatibility mode for PotionnAnimatedFx is activated to use the animations, send events
			// with required variables to the papyrus scripts
			if (Settings::_CompatibilityPotionAnimatedFX_UseAnimations) {
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
			}
			initialized = true;
		}
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
	// static std::vector<std::tuple<RE::Actor*, int, int, int, int, int>> aclist{};

	/// <summary>
	/// list that holds currently handled actors
	/// </summary>
	static std::vector<Events::ActorInfo*> aclist{};
	/// <summary>
	/// semaphore used to sync access to actor handling, to prevent list changes while operations are done
	/// </summary>
	static std::binary_semaphore sem(1);
	// map actorid -> GameDay (reset)
	/// <summary>
	/// used to determine which actors may be given potions, and which recently received some
	/// </summary>
	static std::map<uint64_t, float> actorresetmap{};
	/// <summary>
	/// semaphore used to limit access to the reset map, to avoid problems
	/// </summary>
	static std::binary_semaphore sem_actorreset(1);

	/// <summary>
	/// since the TESDeathEvent seems to be able to fire more than once for an actor we need to track the deaths
	/// </summary>
	static std::unordered_set<RE::FormID> deads;

	/// <summary>
	/// EventHandler for TESDeathEvent
	/// removed unused potions and poisons from actor, to avoid economy instability
	/// only registered if itemremoval is activated in the settings
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		InitializeCompatibilityObjects();
		auto actor = a_event->actorDying->As<RE::Actor>();
		if (actor && actor != RE::PlayerCharacter::GetSingleton()) {
			// as with potion distribution, exlude excluded actors and potential followers
			if (!Settings::Distribution::ExcludedNPC(actor) && deads.contains(actor->GetFormID()) == false) {
				// create and insert new event
				deads.insert(actor->GetFormID());
				sem_actorreset.acquire();
				LOG1_1("{}[HandleEvents] [TESDeathEvent] Removing items from actor {}", std::to_string(actor->GetFormID()));
				auto items = Settings::Distribution::GetMatchingInventoryItems(actor);
				LOG1_1("{}[HandleEvents] [TESDeathEvent] found {} items", items.size());
				if (items.size() > 0) {
					// remove items that are too much
					while (items.size() > Settings::_MaxItemsLeft) {
						RE::ExtraDataList* extra = new RE::ExtraDataList();
						extra->SetOwner(actor);
						actor->RemoveItem(items.back(), 1 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
						LOG1_1("{}[HandleEvents] [TESDeathEvent] Removed item {}", items.back()->GetName());
						items.pop_back();
					}
					if (actor->IsDead()) {
						sem_actorreset.release();
						return EventResult::kContinue;
					}
					//logger::info("[TESDeathEvent] 3");
					// remove the rest of the items per chance
					if (Settings::_ChanceToRemoveItem < 100) {
						for (int i = (int)items.size() - 1; i >= 0; i--) {
							if (rand100(rand) <= Settings::_ChanceToRemoveItem) {
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->RemoveItem(items[i], 100 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, extra, nullptr);
								LOG1_1("{}[HandleEvents] [TESDeathEvent] Removed item {}", items[i]->GetName());
							} else {
								LOG1_1("{}[HandleEvents] [TESDeathEvent] Did not remove item {}", items[i]->GetName());
							}
						}
					}
				}
				sem_actorreset.release();
			}
		}

		return EventResult::kContinue;
	}

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
		InitializeCompatibilityObjects();
			auto actor = a_event->actor->As<RE::Actor>();
		if (actor && !actor->IsDead() && actor != RE::PlayerCharacter::GetSingleton() && actor->IsChild() == false) {
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				LOG_1("{}[HandleEvents] [TesCombatEnterEvent] Trying to register new actor for potion tracking");

				// check wether this charackter maybe a follower
				sem_actorreset.acquire();
				auto iterac = actorresetmap.find(actor->GetFormID());
				if (iterac == actorresetmap.end() || RE::Calendar::GetSingleton()->GetDaysPassed() - iterac->second > 1) {
					actorresetmap.erase(actor->GetFormID());
					if (!Settings::Distribution::ExcludedNPC(actor)) {
						// if we have characters that should not get items, the function
						// just won't return anything, but we have to check for standard factions like CurrentFollowerFaction
						auto items = Settings::Distribution::GetDistrItems(actor);
						if (actor->IsDead()) {
							sem_actorreset.release();
							return EventResult::kContinue;
						}
						if (items.size() > 0) {
							for (int i = 0; i < items.size(); i++) {
								if (items[i] == nullptr) {
									//logger::info("[TESCombatEvent] Item: null");
									continue;
								}
								std::string name = items[i]->GetName();
								std::string id = Utility::GetHex(items[i]->GetFormID());
								//logger::info("[TESCombatEvent] Item: {} {}", id, name);
								RE::ExtraDataList* extra = new RE::ExtraDataList();
								extra->SetOwner(actor);
								actor->AddObjectToContainer(items[i], extra, 1, nullptr);
							}
							actorresetmap.insert_or_assign((uint64_t)(actor->GetFormID()), RE::Calendar::GetSingleton()->GetDaysPassed());
						}
					}
				}
				sem_actorreset.release();

				if (actor->IsDead())
					return EventResult::kContinue;

				if (Settings::_featUseFood) {
					// use food at the beginning of the fight to simulate the npc having eaten
					ACM::ActorUseAnyFood(actor, false);
				}

				sem.acquire();
				auto it = aclist.begin();
				auto end = aclist.end();
				bool cont = false;
				while (it != end) {
					if ((*it)->actor == actor) {
						cont = true;
						break;
					}
					it++;
				}
				if (!cont)
					aclist.insert(aclist.begin(), new Events::ActorInfo(actor, 0, 0, 0, 0, 0));
				sem.release();
				LOG_1("{}[HandleEvents] [TesCombatEnterEvent] finished registering NPC");
			} else {
				LOG_1("{}[HandleEvents] [TesCombatLeaveEvent] Unregister NPC from potion tracking")
				sem.acquire();
				auto it = aclist.begin();
				auto end = aclist.end();
				while (it != end) {
					if ((*it)->actor == actor) {
						//logger::info("CombatEvent deleting list entry");
						delete (*it);
						aclist.erase(it);
						break;
					}
					it++;
				}
				sem.release();
				LOG_1("{}[HandleEvents] [TesCombatLeaveEvent] Unregistered NPC");
			}
		}

		return EventResult::kContinue;
    }

	/// <summary>
	/// if set to true stops the CheckActors thread on its next iteration
	/// </summary>
	static bool stopactorhandler = false;
	/// <summary>
	/// [true] if the actorhandler is running, [false] if the thread died
	/// </summary>
	static bool actorhandlerrunning = false;
	/// <summary>
	/// thread running the CheckActors function
	/// </summary>
	static std::thread* actorhandler = nullptr;




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
		/* int durhp = 0;  // duration health player
		int durmp = 0; // duration magicka player
		int dursp = 0; // duration stamina player
		// to get this to 0 you would need to play nearly 600 hours
		int durotherp = 0;  //INT_MAX; // duration of buff potions for the player
		int durregp = 0;    //INT_MAX; // duration of reg potions for the player*/

		Events::ActorInfo * playerinfo = new Events::ActorInfo(RE::PlayerCharacter::GetSingleton(), 0, 0, 0, 0, 0);

		/// temp section
		uint64_t alch = 0;
		uint64_t alch2 = 0;
		uint64_t alch3 = 0;
		bool player = false; // wether player was inserted into list

		auto datahandler = RE::TESDataHandler::GetSingleton();
		const RE::TESFile* file = nullptr;
		std::string_view name = std::string_view{ "" };
		
		// main loop, if the thread should be stopped, exit the loop
		while (!stopactorhandler) {
			// current actor
			Events::ActorInfo* curr;
			// if we are in a paused menu (SoulsRE unpauses menus, which is supported by this)
			// do not compute, since nobody can actually take potions.
			if (!ui->GameIsPaused() && initialized) {
				// reset player var.
				player = false;
				// get starttime of iteration
				begin = std::chrono::steady_clock::now();

				sem.acquire();
				// checking if player should be handled
				if ((Settings::_playerRestorationEnabled || Settings::_playerUseFortifyPotions || Settings::_playerUsePoisons) && RE::PlayerCharacter::GetSingleton()->IsInCombat()) {
					// inject player into the list and remove him later
					aclist.insert(aclist.end(), playerinfo);
					LOG_3("{}[CheckActors] Adding player to the list");
					player = true;
				}
				LOG1_1("{}[CheckActors] Handling all registered Actors {}", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count()));
				// handle all registered actors
				// the list does not change while doing this
				for (int i = 0; i < aclist.size(); i++) {
					curr = aclist[i];
					if (curr == nullptr || curr->actor == nullptr) {
						continue;
					}
					if (Settings::EnableLog) {
						name = std::string_view{ "" };
						if ((curr->actor->GetFormID() >> 24) != 0xFE) {
							file = datahandler->LookupLoadedModByIndex((uint8_t)(curr->actor->GetFormID() >> 24));
							if (file) {
								name = file->GetFilename();
							}
						}
						if (name.empty()) {
							file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((curr->actor->GetFormID() & 0x00FFF000)) >> 12));
							if (file) {
								name = file->GetFilename();
							}
						}
						LOG3_1("{}[CheckActors] [Actor] {} named {} from {}", Utility::GetHex((curr->actor)->GetFormID()), curr->actor->GetName(), name);
					}
					// if actor is valid and not dead
					if (curr->actor && !(curr->actor->IsDead()) && curr->actor->GetActorValue(RE::ActorValue::kHealth) > 0) {
						// handle potions
						// get current duration
						curr->durHealth -= Settings::_cycletime;
						curr->durMagicka -= Settings::_cycletime;
						curr->durStamina -= Settings::_cycletime;
						curr->durFortify -= Settings::_cycletime;
						curr->durRegeneration -= Settings::_cycletime;
						LOG5_1("{}[CheckActors] [Actor] cooldown: {} {} {} {} {}", curr->durHealth, curr->durMagicka, curr->durStamina, curr->durFortify, curr->durRegeneration);

						// potions used this cycle
						int counter = 0;

						if (!curr->actor->IsPlayerRef() || Settings::_playerRestorationEnabled) {
							// get combined effect for magicka, health, and stamina
							if (Settings::_featHealthRestoration && curr->durHealth < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kHealth) < Settings::_healthThreshold)
								alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
							else
								alch = 0;
							if (Settings::_featMagickaRestoration && curr->durMagicka < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kMagicka) < Settings::_magickaThreshold)
								alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
							else
								alch2 = 0;
							if (Settings::_featStaminaRestoration && curr->durStamina < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kStamina) < Settings::_staminaThreshold)
								alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
							else
								alch3 = 0;
							// construct combined effect
							alch |= alch2 | alch3;
							LOG4_4("{}[CheckActors] check for alchemyeffect {} with current dur health {} dur mag {} dur stam {} ", alch, curr->durHealth, curr->durMagicka, curr->durStamina);
							// use potions
							// do the first round
							if (alch != 0 && (Settings::_UsePotionChance == 100 || rand100(rand) < Settings::_UsePotionChance)) {
								auto tup = ACM::ActorUsePotion(curr->actor, alch, Settings::_CompatibilityPotionAnimation);
								LOG1_2("{}[CheckActors] found potion has Alchemy Effect {}", static_cast<uint64_t>(std::get<1>(tup)));
								switch (std::get<1>(tup)) {
								case AlchemyEffect::kHealth:
									curr->durHealth = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;  // convert to milliseconds
									counter++;
									LOG2_4("{}[CheckActors] use health pot with duration {} and magnitude {}", curr->durHealth, std::get<0>(tup));
									break;
								case AlchemyEffect::kMagicka:
									curr->durMagicka = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
									counter++;
									LOG2_4("{}[CheckActors] use magicka pot with duration {} and magnitude {}", curr->durMagicka, std::get<0>(tup));
									break;
								case AlchemyEffect::kStamina:
									curr->durStamina = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
									counter++;
									LOG2_4("{}[CheckActors] use stamina pot with duration {} and magnitude {}", curr->durStamina, std::get<0>(tup));
									break;
								}
								// do the rest of the rounds
								for (int c = 1; c < Settings::_maxPotionsPerCycle; c++) {
									// get combined effect for magicka, health, and stamina
									if (Settings::_featHealthRestoration && curr->durHealth < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kHealth) < Settings::_healthThreshold)
										alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
									else
										alch = 0;
									if (Settings::_featMagickaRestoration && curr->durMagicka < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kMagicka) < Settings::_magickaThreshold)
										alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
									else
										alch2 = 0;
									if (Settings::_featStaminaRestoration && curr->durStamina < tolerance && ACM::GetAVPercentage(curr->actor, RE::ActorValue::kStamina) < Settings::_staminaThreshold)
										alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
									else
										alch3 = 0;
									// construct combined effect
									alch |= alch2 | alch3;
									if (alch != 0) {
										tup = ACM::ActorUsePotion(curr->actor, std::get<2>(tup),  Settings::_CompatibilityPotionAnimation);
										switch (std::get<1>(tup)) {
										case AlchemyEffect::kHealth:
											curr->durHealth = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
											counter++;
											break;
										case AlchemyEffect::kMagicka:
											curr->durMagicka = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
											counter++;
											break;
										case AlchemyEffect::kStamina:
											curr->durStamina = std::get<0>(tup) * 1000 > Settings::_MaxDuration ? Settings::_MaxDuration : std::get<0>(tup) * 1000;
											counter++;
											break;
										}
									} else
										break;
								}
							}
						}

						// get combatdata of current actor
						uint32_t combatdata = Utility::GetCombatData(curr->actor);
						uint32_t tcombatdata = 0;
						// retrieve target of current actor if present
						RE::ActorHandle handle = curr->actor->currentCombatTarget;
						
						if (Settings::_featUsePoisons && (Settings::_UsePoisonChance == 100 || rand100(rand) < Settings::_UsePoisonChance) && (!curr->actor->IsPlayerRef() || Settings::_playerUsePoisons)) {
							// handle poisons
							if (curr->actor->IsInFaction(Settings::CurrentFollowerFaction) || curr->actor->IsPlayerRef()) {
								if (combatdata != 0 && (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
									(combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0 && Utility::CanApplyPoison(curr->actor)) {
									// handle followers
									// they only use poisons if there are many npcs in the fight, or if the enemies they are targetting
									// have a high enough level, like starting at PlayerLevel*0.8 or so
									if (Settings::_EnemyNumberThreshold < aclist.size() || (handle && handle.get() && handle.get().get() && handle.get().get()->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::_EnemyLevelScalePlayerLevel)) {
										// time to use some potions
										uint64_t effects = 0;
										// kResistMagic, kResistFire, kResistFrost, kResistMagic should only be used if the follower is a spellblade
										if (combatdata & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) ||
											combatdata & (static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword))) {
											effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistMagic);
											if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
												if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire))
													effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistFire);
												if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost))
													effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistFrost);
												if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock))
													effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistShock);
											}
										}
										// incorporate enemy specific data
										if (handle && handle.get() && handle.get().get()) {
											// we can make the usage dependent on the target
											RE::Actor* target = handle.get().get();
											tcombatdata = Utility::GetCombatData(target);
											// determine main actor value of enemy. That is the one we want to target ideally
											float tmag = target->GetBaseActorValue(RE::ActorValue::kMagicka);
											float smag = target->GetBaseActorValue(RE::ActorValue::kStamina);
											//float shea = target->GetBaseActorValue(RE::ActorValue::kHealth);

											if (tmag > smag) {
												// the enemy is probably a mage
												// appropiate potions are:
												// kParalysis, kHealth, kMagicka, kMagickaRate, kHealRate, kSpeedMult, kDamageResist, kPoisonResist, kFrenzy, kFear
												effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kHealth) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kMagicka) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRate) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRateMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRate) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRateMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kDamageResist) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kPoisonResist) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kParalysis) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kFrenzy) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kFear);
											} else {
												// the enemy is probably a meele charackter
												// kParalysis, kHealth, kStamina, kHealRate, kStaminaRate, kDamageResist, kPoisonResist, kFrenzy, kFear, kSpeedMult, kWeaponSpeedMult, kAttackDamageMult
												effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kHealth) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kStamina) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRate) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRateMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRate) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRateMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kDamageResist) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kPoisonResist) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kFrenzy) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kFear) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kParalysis) |
												           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult);
											}
										} else {
											// we dont have a target so just use any poison
											effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kHealth) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kMagicka) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRate) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRateMult) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRate) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRateMult) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kDamageResist) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kPoisonResist) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kFrenzy) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kFear) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kParalysis) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kStamina) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRate) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRateMult) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
											           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult);
										}
										LOG1_4("{}[CheckActors] check for poison with effect {}", effects);
										auto tup = ACM::ActorUsePoison(curr->actor, effects);
										//if (std::get<1>(tup) != Settings::AlchemyEffect::kNone)
										//	logger::info("Used poison on actor:\t{}", curr->actor->GetName());
									}
								}
								if (combatdata == 0)
									LOG2_2("{}[CheckActors] couldn't determine combatdata for npc {} {}", curr->actor->GetName(), Utility::GetHex(curr->actor->GetFormID()));
								// else Mage of Hand to Hand which cannot use poisons

							} else if ((combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) == 0 &&
									   (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::HandToHand)) == 0) {
								// we dont handle a follower, so just let the enemy use any poison they have
								uint64_t effects = static_cast<uint64_t>(Settings::AlchemyEffect::kAnyPoison);
								LOG1_4("{}[CheckActors] check for poison with effect {}", Utility::GetHex(effects));
								ACM::ActorUsePoison(curr->actor, effects);
							}
							// else Mage or Hand to Hand which cannot use poisons
						}

						if (Settings::_featUseFortifyPotions && counter < Settings::_maxPotionsPerCycle && (!(curr->actor->IsPlayerRef()) || Settings::_playerUseFortifyPotions)) {
							//logger::info("fortify potions stuff");

							if (curr->actor->IsInFaction(Settings::CurrentFollowerFaction) || curr->actor->IsPlayerRef() && !(Settings::_EnemyNumberThreshold < aclist.size() || (handle && handle.get() && handle.get().get() && handle.get().get()->GetLevel() >= RE::PlayerCharacter::GetSingleton()->GetLevel() * Settings::_EnemyLevelScalePlayerLevel))) {
								goto SkipFortify;
							}
							// handle fortify potions
							if ((Settings::_UseFortifyPotionChance == 100 || rand100(rand) < Settings::_UseFortifyPotionChance)) {
								// general stuff
								uint64_t effects = 0;

								// determine valid regeneration effects
								if (curr->durRegeneration < tolerance) {
									effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kHealRate) |
									           static_cast<uint64_t>(Settings::AlchemyEffect::kHealRateMult);

									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRate) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRateMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRate) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRateMult);
									}
									if (combatdata &
										(static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword) |
											static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield) |
											static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded) |
											static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged) |
											static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield)
											)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRate) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kStaminaRateMult);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRate) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kMagickaRateMult);
									}
								}
								// determine valid fortify effects
								if (curr->durFortify < tolerance) {
									effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kDamageResist) |
									           static_cast<uint64_t>(Settings::AlchemyEffect::kResistMagic) |
									           static_cast<uint64_t>(Settings::AlchemyEffect::kPoisonResist) |
									           static_cast<uint64_t>(Settings::AlchemyEffect::kResistDisease) |
									           static_cast<uint64_t>(Settings::AlchemyEffect::kReflectDamage);

									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Spellsword)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kOneHanded) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kMeleeDamage) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Staffsword)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kOneHanded) |
											       static_cast<uint64_t>(Settings::AlchemyEffect::kMeleeDamage) |
											       static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
											       static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
											       static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult) |
											       static_cast<uint64_t>(Settings::AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::OneHandedShield)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kOneHanded) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kBlock) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::TwoHanded)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kTwoHanded) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kBlock) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kMeleeDamage) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Ranged)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kArchery) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kBowSpeed) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualWield)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kOneHanded) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kWeaponSpeedMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kAttackDamageMult) |
										           static_cast<uint64_t>(Settings::AlchemyEffect::kCriticalChance);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::DualStaff)) {
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::Mage)) {
									}
									// magic related stuff
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicAlteration)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kAlteration);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicConjuration)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kConjuration);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDestruction)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kDestruction);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicIllusion)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kIllusion);
									}
									if (combatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicRestoration)) {
										effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kRestoration);
									}

									if (tcombatdata != 0) {
										if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFire)) {
											effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistFire);
										}
										if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageFrost)) {
											effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistFrost);
										}
										if (tcombatdata & static_cast<uint32_t>(Utility::CurrentCombatStyle::MagicDamageShock)) {
											effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kResistShock);
										}
									}
								}

								// light and heavy armor
								uint32_t armordata = Utility::GetArmorData(curr->actor);
								if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::LightArmor))
									effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kLightArmor);
								if (armordata & static_cast<uint32_t>(Utility::CurrentArmor::HeavyArmor))
									effects |= static_cast<uint64_t>(Settings::AlchemyEffect::kHeavyArmor);

								// std::tuple<int, Settings::AlchemyEffect, std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>>>
								//logger::info("take fortify with effects: {}", Utility::GetHex(effects));
								LOG1_4("{}[CheckActors] check for fortify potion with effect {}", Utility::GetHex(effects));
								auto tup = ACM::ActorUsePotion(curr->actor, effects, Settings::_CompatibilityPotionAnimationFortify);
								switch (std::get<1>(tup)) {
								case Settings::AlchemyEffect::kHealRate:
								case Settings::AlchemyEffect::kMagickaRate:
								case Settings::AlchemyEffect::kStaminaRate:
								case Settings::AlchemyEffect::kHealRateMult:
								case Settings::AlchemyEffect::kMagickaRateMult:
								case Settings::AlchemyEffect::kStaminaRateMult:
									curr->durRegeneration = std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000;
									LOG1_4("{}[CheckActors] used regeneration potion with tracked duration {}", curr->durRegeneration);
									break;
								default:
									curr->durFortify = std::get<0>(tup) * 1000 > Settings::_MaxFortifyDuration ? Settings::_MaxFortifyDuration : std::get<0>(tup) * 1000;
									LOG1_4("{}[CheckActors] used fortify av potion with tracked duration {}", curr->durFortify);
									break;
								}
							}
						}
						SkipFortify:;

						// end all values are up to date
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
					aclist.pop_back();
					// we do not need to retrieve values, since wthey are written to the reference directly
				}
				// write execution time of iteration
				PROF1_1("{}[PROF] [CheckActors] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("{}[CheckActors] checked {} actors", std::to_string(aclist.size()));
				// release lock.
				sem.release();
			}
			std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(Settings::_cycletime));
		}
		stopactorhandler = false;
		actorhandlerrunning = false;
	}

	/// <summary>
	/// thread which executes varying test functions
	/// </summary>
	std::thread* testhandler = nullptr;

	/// <summary>
	/// Teleports the player to every cell in the game and calculates the distribution rules for all actors
	/// </summary>
	void TestHandler() {
		std::this_thread::sleep_for(10s);
		RE::UI* ui = RE::UI::GetSingleton();
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrder.csv";
		std::string pathid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrderID.csv";
		std::string pathfail = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellFail.csv";
		std::string pathfailid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellFailID.csv";
		std::unordered_set<uint32_t> done;
		std::unordered_set<std::string> excluded;
		std::unordered_set<uint32_t> excludedid;
		if (std::filesystem::exists(path)) {
			std::ifstream inp(path);
			std::ifstream inpid(pathid);
			std::ifstream dones("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv");
			std::string lineinp;
			std::string lastdone;
			std::string tmp;
			std::string id;
			uint32_t formid = 0;
			while (std::getline(dones, tmp))
				lastdone = tmp;
			size_t pos = lastdone.find(';');
			if (pos != std::string::npos) {
				lastdone = lastdone.substr(0, pos);
			}
			bool flag = false;
			while (std::getline(inp, lineinp) && std::getline(inpid, id)) {
				if (flag == true) {
					break;
				}
				if (lastdone == lineinp && lineinp != "Wilderness")
					flag = true;
				try {
					formid = static_cast<uint32_t>(std::stol(id, nullptr, 16));
					done.insert(formid);
				} catch (std::exception&) {
					//logger::info("tryna fail");
				}
			}
			excluded.insert(lineinp);
			if (std::filesystem::exists(pathfail)) {
				std::ifstream fail(pathfail);
				std::ifstream failid(pathfailid);
				while (std::getline(fail, tmp)) {
					excluded.insert(tmp);
				}
				while (std::getline(failid, tmp)) {
					try {
						formid = static_cast<uint32_t>(std::stol(tmp, nullptr, 16));
						excludedid.insert(formid);
					} catch (std::exception&) {
						//logger::info("tryna fail fail");
					}
				}
				fail.close();
				failid.close();
			}
			inp.close();
			dones.close();
			std::ofstream failout(pathfail);
			std::ofstream failoutid(pathfailid);
			auto it = excluded.begin();
			while (it != excluded.end()) {
				failout << *it << "\n";
				it++;
			}
			auto itr = excludedid.begin();
			while (itr != excludedid.end()) {
				failoutid << Utility::GetHex(*itr) << "\n";
				itr++;
			}
		}
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::ofstream outid = std::ofstream(pathid, std::ofstream::out);
		
		RE::TESObjectCELL * cell = nullptr;
		std::vector<RE::TESObjectCELL*> cs;
		const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
		{
			const RE::BSReadLockGuard locker{ lock };
			auto iter = hashtable->begin();
			while (iter != hashtable->end()) {
				if ((*iter).second) {
					cell = ((*iter).second)->As<RE::TESObjectCELL>();
					if (cell) {
						cs.push_back(cell);
						out << cell->GetFormEditorID() << "\n";
						outid << Utility::GetHex(cell->GetFormID()) << "\n";
					}
				}
				iter++;
			}
		}
		//auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
		//auto iter = hashtable->begin();

		//logger::info("tryna start");
		//while (iter != hashtable->end()) {
		//	if ((*iter).second) {
		//		cell = ((*iter).second)->As<RE::TESObjectCELL>();
		//		if (cell) {
		//			out << cell->GetFormEditorID() << "\n";
		//			outid << Utility::GetHex(cell->GetFormID()) << "\n";
		//		}
		//	}
		//	iter++;
		//}
		out.close();
		outid.close();
		auto console = RE::ConsoleLog::GetSingleton();
		console->Print("Start Test");
		logger::info("Start Test");
		//iter = hashtable->begin();
		//while (iter != hashtable->end()) {
		//	if ((*iter).second) {
		//		cell = ((*iter).second)->As<RE::TESObjectCELL>();
		for (size_t i = 0; i < cs.size(); i++) {
			cell = cs[(int)i];
				if (cell) {
					if (excludedid.contains(cell->GetFormID()) || done.contains(cell->GetFormID()) || std::string(cell->GetFormEditorID()) == "Wilderness") {
		//				iter++;
						continue;
					}
					while (ui->GameIsPaused()) {
						std::this_thread::sleep_for(100ms);
					}
					if (cell->references.size() > 0) {
						char buff[70] = "Moving to cell:\t";
						strcat_s(buff, 70, cell->GetFormEditorID());
						console->Print(buff);
						logger::info("Moving to cell:\t{}", cell->GetFormEditorID());
						RE::PlayerCharacter::GetSingleton()->MoveTo((*(cell->references.begin())).get());
					}
					std::this_thread::sleep_for(7s);
				}
		//	}
		//	iter++;
		}
		console->Print("Finished Test");
		logger::info("Finished Test");
		//logger::info("tryna end");
	}

	/// <summary>
	/// Calculates the distribution rules for all actors in all cells in the game
	/// </summary>
	void TestAllCells()
	{
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrder.csv";
		std::string pathid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrderID.csv";
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::ofstream outid = std::ofstream(pathid, std::ofstream::out);
		//logger::info("tryna start");
		auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
		auto iter = hashtable->begin();
		RE::TESObjectCELL* cell = nullptr;
		while (iter != hashtable->end()) {
			if ((*iter).second) {
				cell = ((*iter).second)->As<RE::TESObjectCELL>();
				if (cell) {
					out << cell->GetFormEditorID() << "\n";
					outid << Utility::GetHex(cell->GetFormID()) << "\n";
				}
			}
			iter++;
		}

		std::this_thread::sleep_for(10s);
		RE::UI* ui = RE::UI::GetSingleton();
		iter = hashtable->begin();
		while (iter != hashtable->end()) {
			if ((*iter).second) {
				cell = ((*iter).second)->As<RE::TESObjectCELL>();
				if (cell) {
					while (ui->GameIsPaused()) {
						std::this_thread::sleep_for(100ms);
					}
					Settings::CheckCellForActors(cell->GetFormID());
				}
			}
			iter++;
		}
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

		// delete the current actorresetmap, since we don't know wether its still valid across savegame load
		sem_actorreset.acquire();
		actorresetmap.clear();
		sem_actorreset.release();
		// reset the list of actors that died
		deads.clear();

		if (Settings::_Test) {
			if (testhandler == nullptr) {
				testhandler = new std::thread(TestAllCells);
				LOG_1("{}[LoadGameEvent] Started TestHandler");
			}
		}

		return EventResult::kContinue;
	}


	std::unordered_set<RE::FormID> cells;
	/// <summary>
		/// EventHandler to fix not playing potion, poison, food sound
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*) {
		//logger::info("[CELLEVENT]");
		if (cells.contains(a_event->cellID) == false) {
			cells.insert(a_event->cellID);
			Settings::CheckCellForActors(a_event->cellID);
		}
		return EventResult::kContinue;
	}

	
	/// <summary>
	/// EventHandler for Debug purposes. It calculates the distribution rules for all npcs in the cell
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>*)
	{
			if (a_event->actor.get()) {
			if (a_event->actor->IsPlayerRef()) {
				auto audiomanager = RE::BSAudioManager::GetSingleton();

				RE::AlchemyItem* obj = RE::TESForm::LookupByID<RE::AlchemyItem>(a_event->baseObject);
				if (obj) {
					if ((obj->IsFood() || obj->HasKeyword(Settings::VendorItemFood)) && Settings::FixedFoodEat) {
						RE::BSSoundHandle handle;
						if (obj->data.consumptionSound)
							audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
						else
							audiomanager->BuildSoundDataFromDescriptor(handle, Settings::FoodEat->soundDescriptor);
						handle.SetObjectToFollow(a_event->actor->Get3D());
						handle.SetVolume(1.0);
						handle.Play();
					} else if ((obj->IsPoison() || obj->HasKeyword(Settings::VendorItemPoison)) && Settings::FixedPoisonUse) {
						RE::BSSoundHandle handle;
						if (obj->data.consumptionSound)
							audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
						else
							audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
						handle.SetObjectToFollow(a_event->actor->Get3D());
						handle.SetVolume(1.0);
						handle.Play();
					} else if ((obj->IsMedicine() || obj->HasKeyword(Settings::VendorItemPotion)) && Settings::FixedPotionUse) {
						RE::BSSoundHandle handle;
						if (obj->data.consumptionSound)
							audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
						else
							audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PotionUse->soundDescriptor);
						handle.SetObjectToFollow(a_event->actor->Get3D());
						handle.SetVolume(1.0);
						handle.Play();
					}
				}
			}
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
		LOG1_1("{}Registered {}", typeid(RE::TESLoadGameEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESEquipEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESEquipEvent).name());
		if (Settings::_featRemoveItemsOnDeath) {
			scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(EventHandler::GetSingleton());
			LOG1_1("{}Registered {}", typeid(RE::TESDeathEvent).name());
		}
		if (Settings::_CalculateCellRules) {
			RE::PlayerCharacter::GetSingleton()->GetEventSource<RE::BGSActorCellEvent>()->AddEventSink(EventHandler::GetSingleton());
			LOG1_1("{}Registered {}", typeid(RE::BGSActorCellEvent).name());
		}
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
		//killEventHandler = true;
	}
}
