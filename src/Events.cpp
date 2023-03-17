#include <string.h>
#include <chrono>
#include <thread>
#include <forward_list>
#include <semaphore>
#include <stdlib.h>
#include <time.h>
#include <random>
#include <fstream>
#include <iostream>
#include <limits>
#include <filesystem>
#include <deque>

#include "Distribution.h"
#include "Events.h"
#include "Game.h"
#include "Settings.h"
#include "Statistics.h"
#include "Utility.h"
		
namespace Events
{
	using AlchemyEffect = AlchemyEffect;

	//-------------------Random-------------------------

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

#pragma region Data

#define EvalProcessing()   \
	if (!enableProcessing) \
		return;
#define GetProcessing() \
	enableProcessing
#define WaitProcessing() \
	while (!enableProcessing) \
		std::this_thread::sleep_for(100ms);
#define EvalProcessingEvent() \
	if (!Main::CanProcess())    \
		return EventResult::kContinue;

#define CheckDeadEvent                       \
	LOG1_1("{}[PlayerDead] {}", playerdied); \
	if (playerdied == true) {                \
		return EventResult::kContinue;       \
	}

#define ReEvalPlayerDeath                                         \
	if (RE::PlayerCharacter::GetSingleton()->IsDead() == false) { \
		playerdied = false;                                       \
	}

#define CheckDeadCheckHandlerLoop \
	if (playerdied) {             \
		break;                    \
	}                                              


#pragma endregion


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
		Statistics::Events_TESDeathEvent++;
		EvalProcessingEvent();
		auto begin = std::chrono::steady_clock::now();
		LOG_1("{}[Events] [TESDeathEvent]");
		Main::InitializeCompatibilityObjects();
		RE::Actor* actor = nullptr;
		if (a_event == nullptr || a_event->actorDying == nullptr) {
			LOG_4("{}[Events] [TESDeathEvent] Died due to invalid event");
			goto TESDeathEventEnd;
		}
		actor = a_event->actorDying->As<RE::Actor>();
		if (!Utility::ValidateActor(actor)) {
			LOG_4("{}[Events] [TESDeathEvent] Died due to actor validation fail");
			goto TESDeathEventEnd;
		}
		if (Utility::ValidateActor(actor)) {
			if (actor->IsPlayerRef()) {
				LOG_4("{}[Events] [TESDeathEvent] player died");
				Main::PlayerDied(true);
			} else {
				// if not already dead, do stuff
				if (Main::IsDeadEventFired(actor) == false) {
					EvalProcessingEvent();
					// invalidate actor
					std::shared_ptr<ActorInfo> acinfo = Main::data->FindActor(actor);
					bool excluded = Distribution::ExcludedNPC(acinfo);
					acinfo->SetInvalid();
					// all npcs must be unregistered, even if distribution oes not apply to them
					Main::UnregisterNPC(actor);
					// as with potion distribution, exlude excluded actors and potential followers
					if (!excluded) {
						// create and insert new event
						Main::SetDead(actor);
						if (Settings::Removal::_RemoveItemsOnDeath) {
							LOG1_1("{}[Events] [TESDeathEvent] Removing items from actor {}", std::to_string(actor->GetFormID()));
							auto items = Distribution::GetAllInventoryItems(acinfo);
							LOG1_1("{}[Events] [TESDeathEvent] found {} items", items.size());
							if (items.size() > 0) {
								// remove items that are too much
								while (items.size() > Settings::Removal::_MaxItemsLeft) {
									actor->RemoveItem(items.back(), 1 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
									LOG1_1("{}[Events] [TESDeathEvent] Removed item {}", Utility::PrintForm(items.back()));
									items.pop_back();
								}
								//loginfo("[Events] [TESDeathEvent] 3");
								// remove the rest of the items per chance
								if (Settings::Removal::_ChanceToRemoveItem < 100) {
									for (int i = (int)items.size() - 1; i >= 0; i--) {
										if (rand100(rand) <= Settings::Removal::_ChanceToRemoveItem) {
											actor->RemoveItem(items[i], 100 /*remove all there are*/, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
											LOG1_1("{}[Events] [TESDeathEvent] Removed item {}", Utility::PrintForm(items[i]));
										} else {
											LOG1_1("{}[Events] [TESDeathEvent] Did not remove item {}", Utility::PrintForm(items[i]));
										}
									}
								}
							}
						}

					} else {
						LOG1_4("{}[Events] [TESDeathEvent] actor {} is excluded or already dead", Utility::PrintForm(actor));
					}
					// distribute death items, independently of whether the npc is excluded
					auto ditems = acinfo->FilterCustomConditionsDistrItems(acinfo->citems.death);
					// item, chance, num, cond1, cond2
					for (int i = 0; i < ditems.size(); i++) {
						// calc chances
						if (rand100(rand) <= ditems[i]->chance) {
							// distr item
							actor->AddObjectToContainer(ditems[i]->object, nullptr, ditems[i]->num, nullptr);
						}
					}
					// delete actor from data
					Main::data->DeleteActor(actor->GetFormID());
					Main::comp->AnPois_RemoveActorPoison(actor->GetFormID());
					Main::comp->AnPoti_RemoveActorPotion(actor->GetFormID());
					Main::comp->AnPoti_RemoveActorPoison(actor->GetFormID());
				}
			}
		}
	TESDeathEventEnd:
		PROF1_1("{}[Events] [TESDeathEvent] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
		return EventResult::kContinue;
	}

	/// <summary>
	/// Processes TESHitEvents
	/// </summary>
	/// <param name=""></param>
	/// <param name=""></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		Statistics::Events_TESHitEvent++;
		EvalProcessingEvent();
		
		if (a_event && a_event->target.get()) {
			RE::Actor* actor = a_event->target.get()->As<RE::Actor>();
			if (actor) {
				// check whether the actor is queued for poison application
				if (Main::comp->LoadedAnimatedPoisons() && Main::comp->AnPois_FindActorPoison(actor->GetFormID()) != nullptr) {
					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPoisonsHitEvent");
					ev->strArg = RE::BSFixedString();
					ev->numArg = 0.0f;
					ev->sender = actor;
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
				}
				if (Main::comp->LoadedAnimatedPotions() && Main::comp->AnPoti_FindActorPotion(actor->GetFormID()) != nullptr) {
					SKSE::ModCallbackEvent* ev = new SKSE::ModCallbackEvent();
					ev->eventName = RE::BSFixedString("NPCsUsePotions_AnimatedPotionsHitEvent");
					ev->strArg = RE::BSFixedString();
					ev->numArg = 0.0f;
					ev->sender = actor;
					SKSE::GetModCallbackEventSource()->SendEvent(ev);
				}
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
		Statistics::Events_TESCombatEvent++;
		EvalProcessingEvent();
		//if (!Settings::_featDisableOutOfCombatProcessing)
		//	return EventResult::kContinue;
		auto begin = std::chrono::steady_clock::now();
		LOG_1("{}[Events] [TESCombatEvent]");
		Main::InitializeCompatibilityObjects();
		auto actor = a_event->actor->As<RE::Actor>();
		if (Utility::ValidateActor(actor) && !Main::IsDead(actor) && actor != RE::PlayerCharacter::GetSingleton() && actor->IsChild() == false) {
			// register / unregister
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				// register for tracking
				if (Distribution::ExcludedNPCFromHandling(actor) == false)
					Main::RegisterNPC(actor);
			} else {
				if (Settings::Usage::_DisableOutOfCombatProcessing)
					Main::UnregisterNPC(actor);
			}

			// save combat state of npc
			std::shared_ptr<ActorInfo> acinfo = Main::data->FindActor(actor);
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat)
				acinfo->combatstate = CombatState::InCombat;
			else if (a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching)
				acinfo->combatstate = CombatState::Searching;
			else if (a_event->newState == RE::ACTOR_COMBAT_STATE::kNone)
				acinfo->combatstate = CombatState::OutOfCombat;

		}
		PROF1_2("{}[Events] [TESCombatEvent] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
		return EventResult::kContinue;
	}

	/// <summary>
	/// EventHandler for Actors being attached / detached
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESCellAttachDetachEvent* a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>*)
	{
		Statistics::Events_TESCellAttachDetachEvent;
		EvalProcessingEvent();
		// return if feature disabled
		if (Settings::Usage::_DisableOutOfCombatProcessing)
			return EventResult::kContinue;
		Main::PlayerDied((bool)(RE::PlayerCharacter::GetSingleton()->boolBits & RE::Actor::BOOL_BITS::kDead));
		//auto begin = std::chrono::steady_clock::now();

		if (a_event && a_event->reference) {
			RE::Actor* actor = a_event->reference->As<RE::Actor>();
			if (Utility::ValidateActor(actor) && !Main::IsDead(actor) && !actor->IsPlayerRef()) {
				if (a_event->attached) {
					if (Distribution::ExcludedNPCFromHandling(actor) == false)
						Main::RegisterNPC(actor);
				} else {
					Main::UnregisterNPC(actor);
				}
			}
			//PROF1_2("{}[Events] [CellAttachDetachEvent] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
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
	EventResult EventHandler::ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>*)
	{
		Statistics::Events_BGSActorCellEvent++;
		EvalProcessingEvent();
		//LOG_1("{}[Events] [BGSActorCellEvent]");
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
		Statistics::Events_TESEquipEvent++;
		EvalProcessingEvent();
		if (a_event->actor.get()) {
			if (a_event->actor->IsPlayerRef()) {
				auto audiomanager = RE::BSAudioManager::GetSingleton();

				RE::AlchemyItem* obj = RE::TESForm::LookupByID<RE::AlchemyItem>(a_event->baseObject);
				if (obj) {
					if (obj->data.consumptionSound) {
						RE::BSSoundHandle handle;
						audiomanager->BuildSoundDataFromDescriptor(handle, obj->data.consumptionSound->soundDescriptor);
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
	/// Handles an item being removed from a container
	/// </summary>
	/// <param name="container">The container the item was removed from</param>
	/// <param name="baseObj">The base object that has been removed</param>
	/// <param name="count">The number of items that have been removed</param>
	/// <param name="destinationContainer">The container the items have been moved to</param>
	/// <param name="a_event">The event information</param>
	void EventHandler::OnItemRemoved(RE::TESObjectREFR* container, RE::TESBoundObject* baseObj, int /*count*/, RE::TESObjectREFR* /*destinationContainer*/, const RE::TESContainerChangedEvent* /*a_event*/)
	{
		LOG2_1("{}[Events] [OnItemRemovedEvent] {} removed from {}", Utility::PrintForm(baseObj), Utility::PrintForm(container));
		RE::Actor* actor = container->As<RE::Actor>();
		if (actor) {
			// handle event for an actor
			//std::shared_ptr<ActorInfo> acinfo = data->FindActor(actor);
			/* if (comp->LoadedAnimatedPoisons()) {
				// handle removed poison
				RE::AlchemyItem* alch = baseObj->As<RE::AlchemyItem>();
				if (alch && alch->IsPoison()) {
					LOG_1("{}[Events] [OnItemRemovedEvent] AnimatedPoison animation");

					//ACM::AnimatedPoison_ApplyPoison(acinfo, alch);

					//std::string AnimationEventString = "poisondamagehealth02";
					//acinfo->actor->NotifyAnimationGraph(AnimationEventString);
						
					//RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> point(nullptr);
					//a_vm->DispatchStaticCall("Debug", "SendAnimationEvent", SKSE::Impl::VMArg(actor, RE::BSFixedString("poisondamagehealth02")), point);
					//RE::MakeFunctionArguments(actor, RE::BSFixedString("poisondamagehealth02"));
				}
			}
			*/
		}
		
		// handle event for generic reference
		
		return;
	}

	/// <summary>
	/// Handles an item being added to a container
	/// </summary>
	/// <param name="container">The container the item is added to</param>
	/// <param name="baseObj">The base object that has been added</param>
	/// <param name="count">The number of items added</param>
	/// <param name="sourceContainer">The container the item was in before</param>
	/// <param name="a_event">The event information</param>
	void EventHandler::OnItemAdded(RE::TESObjectREFR* container, RE::TESBoundObject* baseObj, int /*count*/, RE::TESObjectREFR* /*sourceContainer*/, const RE::TESContainerChangedEvent* /*a_event*/)
	{
		LOG2_1("{}[Events] [OnItemAddedEvent] {} added to {}", Utility::PrintForm(baseObj), Utility::PrintForm(container));
		RE::Actor* actor = container->As<RE::Actor>();
		if (actor) {
			// handle event for an actor
			//std::shared_ptr<ActorInfo> acinfo = data->FindActor(actor);
			
		}
		
		// handle event for generic objects
		return;
	}

	EventResult EventHandler::ProcessEvent(const RE::TESContainerChangedEvent* a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>* /*a_eventSource*/)
	{
		// this event handles all object transfers between containers in the game
		// this can be deived into multiple base events: OnItemRemoved and OnItemAdded
		Statistics::Events_TESContainerChangedEvent++;
		EvalProcessingEvent();

		if (a_event && a_event->baseObj != 0 && a_event->itemCount != 0) {
			RE::TESObjectREFR* oldCont = RE::TESForm::LookupByID<RE::TESObjectREFR>(a_event->oldContainer);
			RE::TESObjectREFR* newCont = RE::TESForm::LookupByID<RE::TESObjectREFR>(a_event->newContainer);
			RE::TESBoundObject* baseObj = RE::TESForm::LookupByID<RE::TESBoundObject>(a_event->baseObj);
			if (baseObj && oldCont) {
				OnItemRemoved(oldCont, baseObj, a_event->itemCount, newCont, a_event);
			}
			if (baseObj && newCont) {
				OnItemAdded(newCont, baseObj, a_event->itemCount, oldCont, a_event);
			}
		}

		return EventResult::kContinue;
	}
	
	/// <summary>
	/// EventHandler for catching deleted forms / actors
	/// </summary>
	/// <param name="a_event"></param>
	/// <param name="a_eventSource"></param>
	/// <returns></returns>
	EventResult EventHandler::ProcessEvent(const RE::TESFormDeleteEvent* a_event, RE::BSTEventSource<RE::TESFormDeleteEvent>*)
	{
		// very important event. Allows to catch actors and other stuff that gets deleted, without dying, which could cause CTDs otherwise
		Statistics::Events_TESFormDeleteEvent++;
		if (a_event && a_event->formID != 0) {
			Main::data->DeleteActor(a_event->formID);
			Main::UnregisterNPC(a_event->formID);
			Main::data->DeleteFormCustom(a_event->formID);
			Main::comp->AnPois_DeleteActorPoison(a_event->formID);
			Main::comp->AnPoti_DeleteActorPotion(a_event->formID);
			Main::comp->AnPoti_DeleteActorPoison(a_event->formID);
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
		scriptEventSourceHolder->GetEventSource<RE::TESEquipEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESEquipEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESDeathEvent).name());
		if (Settings::Debug::_CalculateCellRules) {
			RE::PlayerCharacter::GetSingleton()->GetEventSource<RE::BGSActorCellEvent>()->AddEventSink(EventHandler::GetSingleton());
			LOG1_1("{}Registered {}", typeid(RE::BGSActorCellEvent).name());
		}
		scriptEventSourceHolder->GetEventSource<RE::TESCellAttachDetachEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESCellAttachDetachEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESFormDeleteEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESFormDeleteEvent).name())
		scriptEventSourceHolder->GetEventSource<RE::TESContainerChangedEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("{}Registered {}", typeid(RE::TESContainerChangedEvent).name())
		Game::SaveLoad::GetSingleton()->RegisterForLoadCallback(0xFF000001, Main::LoadGameCallback);
		LOG1_1("{}Registered {}", typeid(Main::LoadGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0xFF000002, Main::RevertGameCallback);
		LOG1_1("{}Registered {}", typeid(Main::RevertGameCallback).name());
		Game::SaveLoad::GetSingleton()->RegisterForSaveCallback(0xFF000003, Main::SaveGameCallback);
		LOG1_1("{}Registered {}", typeid(Main::SaveGameCallback).name());
		Main::data = Data::GetSingleton();
		Main::comp = Compatibility::GetSingleton();
	}

	/// <summary>
	/// Registers all EventHandlers, if we would have multiple
	/// </summary>
	void RegisterAllEventHandlers()
	{
		EventHandler::Register();
		LOG_1("{}Registered all event handlers"sv);
	}
}
