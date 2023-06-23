#pragma once

#include <semaphore>
#include <set>
#include <functional>

#include "Compatibility.h"
#include "Data.h"


/// <summary>
/// standard namespacee hash function for RE::ActorHandles
/// </summary>
template <>
struct std::hash<RE::ActorHandle>
{
	std::size_t operator()(RE::ActorHandle const& handle) const noexcept
	{
		return std::hash<unsigned long long>{}((uintptr_t)handle.get().get());
	}
};

/// <summary>
/// standard namespace equality comparator for RE::ActorHandles
/// </summary>
template <>
struct std::equal_to<RE::ActorHandle>
{
	std::size_t operator()(RE::ActorHandle const& lhs, RE::ActorHandle const& rhs) const
	{
		return lhs.get().get() == rhs.get().get();
	}
};

namespace Events
{
	class Main
	{
	public:
		static inline Data* data = nullptr;
		static inline Compatibility* comp = nullptr;

	private:
		/// <summary>
		/// determines whether events and functions are run
		/// </summary>
		static inline bool initialized = false;

		//--------------------Lists---------------------------

		/// <summary>
		/// holds all active actors
		/// </summary>
		static inline std::set<std::weak_ptr<ActorInfo>, std::owner_less<std::weak_ptr<ActorInfo>>> acset{};
		/// <summary>
		/// semaphore used to sync access to actor handling, to prevent list changes while operations are done
		/// </summary>
		static inline std::binary_semaphore sem{1};
		/// <summary>
		/// since the TESDeathEvent seems to be able to fire more than once for an actor we need to track the deaths
		/// </summary>
		static inline std::unordered_set<RE::ActorHandle> deads;
		/// <summary>
		/// list of npcs that are currently in combat
		/// </summary>
		static inline std::forward_list<std::shared_ptr<ActorInfo>> combatants;

		//-------------------Handler-------------------------

		/// <summary>
		/// if set to true stops the CheckActors thread on its next iteration
		/// </summary>
		static inline bool stopactorhandler = false;
		static inline bool skipactorhandler = false;
		/// <summary>
		/// [true] if the actorhandler is running, [false] if the thread died
		/// </summary>
		static inline bool actorhandlerrunning = false;
		/// <summary>
		/// [true] if the actorhandler is in an active iteration, [false] if it is sleeping
		/// </summary>
		static inline bool actorhandlerworking = false;
		/// <summary>
		/// thread running the CheckActors function
		/// </summary>
		static inline std::thread* actorhandler = nullptr;

		//--------------------Main---------------------------
		
		/// <summary>
		/// Tolerance for current cooldowns, due to variability in cycle length, due to processing time
		/// </summary>
		static inline int tolerance = 0;
		/// <summary>
		/// Number of actors currently in combat
		/// </summary>
		static inline int actorsincombat = 0;
		/// <summary>
		/// Number of actors hostile to player
		/// </summary>
		static inline int hostileactors = 0;

		//-------------------Processing----------------------

		/// <summary>
		/// signals whether the player has died
		/// </summary>
		static inline bool playerdied = false;
		/// <summary>
		/// enables all active functions
		/// </summary>
		static inline bool enableProcessing = false;

		//-------------------Support-------------------------

		/// <summary>
		/// Calculates the cooldowns of an actor for a specific effect
		/// </summary>
		static void CalcActorCooldowns(std::shared_ptr<ActorInfo> acinfo, AlchemicEffect effect, int dur);

		/// <summary>
		/// Calculates poison effects based on [combatdata], [target], and [tcombatdata]
		/// </summary>
		/// <param name="combatdata">combatdata of the actor using poison</param>
		/// <param name="target">target</param>
		/// <param name="tcombatdata">combatdata of the target</param>
		/// <returns>valid poison effects</returns>
		static AlchemicEffect CalcPoisonEffects(uint32_t combatdata, RE::Actor* target, uint32_t tcombatdata);

		/// <summary>
		/// Calculates all fortify effects that an actor is equitable for, based on their and their targets combat data
		/// </summary>
		/// <param name="acinfo">actoringo object</param>
		/// <param name="combatdata">combatdata of [acinfo]</param>
		/// <param name="tcombatdata">combatdata of target</param>
		/// <returns></returns>
		static AlchemicEffect CalcFortifyEffects(std::shared_ptr<ActorInfo> acinfo, uint32_t combatdata, uint32_t tcombatdata = 0);

		/// <summary>
		/// Calculates all regeneration effects that an actor is equitable for, based on their combat data
		/// </summary>
		/// <param name="combatdata">combatdata of the actor</param>
		/// <returns>valid regeneration effects</returns>
		static AlchemicEffect CalcRegenEffects(uint32_t combatdata);
		/// <summary>
		/// Calculates all regeneration effects that an actor is equitable for, based on their combat data
		/// </summary>
		/// <param name="acinfo"></param>
		/// <param name="combatdata"></param>
		/// <returns></returns>
		static AlchemicEffect CalcRegenEffects(std::shared_ptr<ActorInfo> acinfo, uint32_t combatdata);

		/// <summary>
		/// Processes the item distribution for an actor
		/// </summary>
		/// <param name="acinfo"></param>
		static void ProcessDistribution(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Removes all distributable alchemy items from all actors in the game on loading a game
		/// </summary>
		static void RemoveItemsOnStartup();

		/// <summary>
		/// Calculates the internal duration of a potion with duration [dur]
		/// </summary>
		/// <param name="dur"></param>
		/// <returns></returns>
		static int CalcPotionDuration(int dur);

		/// <summary>
		/// Calculates the internal duration of a fortify potion with duration [dur]
		/// </summary>
		/// <param name="dur"></param>
		/// <returns></returns>
		static int CalcFortifyDuration(int dur);

		/// <summary>
		/// Calculates the internal duration of a regeneration potion with duration [dur]
		/// </summary>
		/// <param name="dur"></param>
		/// <returns></returns>
		static int CalcRegenerationDuration(int dur);

		/// <summary>
		/// Calculates the internal duration of a food with duration [dur]
		/// </summary>
		/// <param name="dur"></param>
		/// <returns></returns>
		static float CalcFoodDuration(int dur);

		//----------------------Main-----------------------

		/// <summary>
		/// Decreases all of the actors active cooldowns
		/// </summary>
		static void DecreaseActorCooldown(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Handles actor potion usage
		/// </summary>
		/// <param name="acinfo"></param>
		static void HandleActorPotions(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Handles actor fortify potion usage
		/// </summary>
		/// <param name="acinfo"></param>
		static void HandleActorFortifyPotions(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Handles actor poison usage
		/// </summary>
		/// <param name="acinfo"></param>
		static void HandleActorPoisons(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Handles actor food usage
		/// </summary>
		/// <param name="acinfo"></param>
		static void HandleActorFood(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Handles actor out-of-combat potion usage
		/// </summary>
		static void HandleActorOOCPotions(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Refreshes important runtime data of an ActorInfo, including combatdata and status
		/// </summary>
		/// <param name="acinfo"></param>
		static void HandleActorRuntimeData(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Main routine that periodically checks the actors status, and applies items
		/// </summary>
		static void CheckActors();

		//---------------------Friends---------------------

	public:

		//-------------------------Registering----------------------

		/// <summary>
		/// Registers an NPC for handling
		/// </summary>
		/// <param name="actor"></param>
		static void RegisterNPC(RE::Actor* actor);

		/// <summary>
		/// Unregisters an NPC form handling
		/// </summary>
		/// <param name="actor"></param>
		static void UnregisterNPC(RE::Actor* actor);

		/// <summary>
		/// Unregisters an NPC from handling
		/// </summary>
		/// <param name="acinfo"></param>
		static void UnregisterNPC(std::shared_ptr<ActorInfo> acinfo);

		/// <summary>
		/// Unregisters an NPC from handling
		/// </summary>
		/// <param name="acinfo"></param>
		static void UnregisterNPC(RE::FormID formid);

		//----------------------Processing--------------------------

		/// <summary>
		/// Returns whether processing of actors is allowed
		/// </summary>
		/// <returns></returns>
		static bool CanProcess() { return enableProcessing; }

		/// <summary>
		/// Temporarily locks processing for all functions
			static  /// </summary>
		bool LockProcessing()
		{
			bool val = Main::enableProcessing;
			Main::enableProcessing = false;
			return val;
		}

		/// <summary>
		/// Unlocks processing for all functions
		/// </summary>
		static void UnlockProcessing()
		{
			enableProcessing = true;
		}

		//----------------------Support----------------------------

		/// <summary>
		/// Returns whether the actor is dead, or the TESDeathEvent has already been fired for the npc
		/// </summary>
		/// <param name="actor"></param>
		/// <returns></returns>
		static bool IsDead(RE::Actor* actor);

		/// <summary>
		/// Returns whether the TESDeathEvent has already been fired for the npc
		/// </summary>
		/// <param name="actor"></param>
		/// <returns></returns>
		static bool IsDeadEventFired(RE::Actor* actor);

		/// <summary>
		/// Sets that the TESDeathEvent has been fired for the npc
		/// </summary>
		/// <param name="actor"></param>
		/// <returns></returns>
		static void SetDead(RE::ActorHandle actor);

		/// <summary>
		/// initializes important variables, which need to be initialized every time a game is loaded
		/// </summary>
		static void InitializeCompatibilityObjects();

		/// <summary>
		/// Returns whether the player character has died
		/// </summary>
		/// <returns></returns>
		static bool IsPlayerDead() { return playerdied; }
		/// <summary>
		/// Sets the live status of the player character
		/// </summary>
		/// <param name="died"></param>
		static void PlayerDied(bool died) { playerdied = died; }

		//-------------------GameFunctions-------------------------

		/// <summary>
		/// Callback on loading a save game, initializes actor processing
		/// </summary>
		/// <param name=""></param>
		static void LoadGameCallback(SKSE::SerializationInterface* a_intfc);

		/// <summary>
		/// Callback on reverting the game. Disables processing and stops all handlers
		/// </summary>
		/// <param name=""></param>
		static void RevertGameCallback(SKSE::SerializationInterface* a_intfc);
		/// <summary>
		/// Callback on saving the game
		/// </summary>
		/// <param name=""></param>
		static void SaveGameCallback(SKSE::SerializationInterface* a_intfc);

		/// <summary>
		/// Saves the list of already dead actors
		/// </summary>
		/// <param name="a_intfc"></param>
		static long SaveDeadActors(SKSE::SerializationInterface* a_intfc);

		/// <summary>
		/// Read a dead actor record
		/// </summary>
		/// <param name="a_intfc"></param>
		/// <param name="length"></param>
		/// <returns></returns>
		static long ReadDeadActors(SKSE::SerializationInterface* a_intfc, uint32_t length);

	};




    using EventResult = RE::BSEventNotifyControl;

    class EventHandler :
		public RE::BSTEventSink<RE::TESHitEvent>,
		public RE::BSTEventSink<RE::TESCombatEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<RE::BGSActorCellEvent>,
		public RE::BSTEventSink<RE::TESCellAttachDetachEvent>,
		public RE::BSTEventSink<RE::TESEquipEvent>,
		public RE::BSTEventSink<RE::TESFormDeleteEvent>,
		public RE::BSTEventSink<RE::TESContainerChangedEvent>
	{
	public:
		/// <summary>
		/// returns singleton to the EventHandler
		/// </summary>
		/// <returns></returns>
		static EventHandler* GetSingleton();
		/// <summary>
		/// Registers us for all Events we want to receive
		/// </summary>
		static void Register();

		/// <summary>
		/// Processes TESHitEvents
		/// </summary>
		/// <param name=""></param>
		/// <param name=""></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override;
		/// <summary>
		/// handles TESCombatEvent
		/// registers the actor for tracking and handles giving them potions, poisons and food, beforehand.
		/// also makes then eat food before the fight.
		/// </summary>
		/// <param name="a_event">event parameters like the actor we need to handle</param>
		/// <param name=""></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource) override;
		/// <summary>
		/// EventHandler for TESDeathEvent
		/// removed unused potions and poisons from actor, to avoid economy instability
		/// only registered if itemremoval is activated in the settings
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) override;
		/// <summary>
		/// EventHandler for Debug purposes. It calculates the distribution rules for all npcs in the cell
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::BGSActorCellEvent* a_event, RE::BSTEventSource<RE::BGSActorCellEvent>* a_eventSource) override;
		/// <summary>
		/// EventHandler for Actors being attached / detached
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESCellAttachDetachEvent* a_event, RE::BSTEventSource<RE::TESCellAttachDetachEvent>* a_eventSource) override;
		/// <summary>
		/// EventHandler for Debug purposes. It calculates the distribution rules for all npcs in the cell
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;
		/// <summary>
		/// EventHandler for catching deleted forms / actors
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESFormDeleteEvent* a_event, RE::BSTEventSource<RE::TESFormDeleteEvent>* a_eventSource) override;
		/// <summary>
		/// EventHandler for catching remove item events
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESContainerChangedEvent* a_event, RE::BSTEventSource<RE::TESContainerChangedEvent>* a_eventSource) override;


		/// <summary>
		/// Handles an item being removed from a container
		/// </summary>
		/// <param name="container">The container the item was removed from</param>
		/// <param name="baseObj">The base object that has been removed</param>
		/// <param name="count">The number of items that have been removed</param>
		/// <param name="destinationContainer">The container the items have been moved to</param>
		/// <param name="a_event">The event information</param>
		void OnItemRemoved(RE::TESObjectREFR* container, RE::TESBoundObject* baseObj, int count, RE::TESObjectREFR* destinationContainer, const RE::TESContainerChangedEvent* a_event);

		/// <summary>
		/// Handles an item being added to a container
		/// </summary>
		/// <param name="container">The container the item is added to</param>
		/// <param name="baseObj">The base object that has been added</param>
		/// <param name="count">The number of items added</param>
		/// <param name="sourceContainer">The container the item was in before</param>
		/// <param name="a_event">The event information</param>
		void OnItemAdded(RE::TESObjectREFR* container, RE::TESBoundObject* baseObj, int count, RE::TESObjectREFR* sourceContainer, const RE::TESContainerChangedEvent* a_event);

	private:
		EventHandler() = default;
		EventHandler(const EventHandler&) = delete;
		EventHandler(EventHandler&&) = delete;
		virtual ~EventHandler() = default;

		EventHandler& operator=(const EventHandler&) = delete;
		EventHandler& operator=(EventHandler&&) = delete;
	};

	/// <summary>
	/// Registers all EventHandlers, if we would have multiple
	/// </summary>
	void RegisterAllEventHandlers();
}
