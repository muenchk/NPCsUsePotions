#pragma once
//#ifndef KPMEvents
//#define KPMEvents

#include<ActorInfo.h>

namespace Events
{
    using EventResult = RE::BSEventNotifyControl;

    class EventHandler :
		public RE::BSTEventSink<RE::TESHitEvent>,
		public RE::BSTEventSink<RE::TESCombatEvent>,
		public RE::BSTEventSink<RE::TESLoadGameEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<RE::BGSActorCellEvent>,
		public RE::BSTEventSink<RE::TESEquipEvent>
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
		/// EventHandler for TESLoadGameEvent. Loads main thread
		/// </summary>
		/// <param name="">unused</param>
		/// <param name="">unused</param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESLoadGameEvent* a_event, RE::BSTEventSource<RE::TESLoadGameEvent>* a_eventSource) override;
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
		/// EventHandler for Debug purposes. It calculates the distribution rules for all npcs in the cell
		/// </summary>
		/// <param name="a_event"></param>
		/// <param name="a_eventSource"></param>
		/// <returns></returns>
		virtual EventResult ProcessEvent(const RE::TESEquipEvent* a_event, RE::BSTEventSource<RE::TESEquipEvent>* a_eventSource) override;

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
	/// <summary>
	/// sets the main threads to stop on the next iteration
	/// </summary>
	void DisableThreads();

	/// <summary>
	/// Resets information about actors
	/// </summary>
	void ResetActorInfoMap();

	/// <summary>
	/// Retrieves Information about an actor from an internal database
	/// </summary>
	ActorInfo* FindActor(RE::Actor* actor);

	/// <summary>
	/// temporarily disables processing
	/// </summary>
	bool LockProcessing();

	/// <summary>
	/// unlocks temporary lock on processing
	/// </summary>
	void UnlockProcessing();

	/// <summary>
	/// The type of event
	/// </summary>
	enum EventType
	{
		None = 0,
		TESDeathEvent = 1 << 0,
		TESCombatEnterEvent = 1 << 1,
		TESCombatLeaveEvent = 1 << 2
	};
	/// <summary>
	/// stores data about an event to process
	/// </summary>
	class EventData
	{
	public:
		RE::Actor* actor;
		EventType evn;
	};
}




//#endif
