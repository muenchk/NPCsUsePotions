#pragma once
//#ifndef KPMEvents
//#define KPMEvents

namespace Events
{
    using EventResult = RE::BSEventNotifyControl;

    class EventHandler :
		public RE::BSTEventSink<RE::TESHitEvent>,
		public RE::BSTEventSink<RE::TESCombatEvent>,
		public RE::BSTEventSink<RE::TESLoadGameEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>
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
}




//#endif
