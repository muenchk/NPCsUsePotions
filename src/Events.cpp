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

		
namespace Events
{
	using AlchemyEffect = Settings::AlchemyEffect;

	static std::mt19937 rand((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
	static std::uniform_int_distribution<signed> rand100(1, 100);	
	static std::uniform_int_distribution<signed> rand3(1, 3);

    /// <summary>
    /// Searches the inventory of an Actor for an appropiate potion and applies it.
	/// This is iterated until the desired actor values are above the threshold
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
			LOG_2("[UsePotion] trying to find potion");
			// searches for potions to use
			while (iter != end && (magicka || stamina || health) && idrunk < Settings::_maxPotionsPerCycle) {
				//logger::info("idrunk: {}", std::to_string(idrunk));
				item = iter->first->As<RE::AlchemyItem>();
				LOG_3("[UsePotion] checking item");
				// get item and check wether it is a potion
				if (item && !(item->IsPoison()) && !(item->IsFood())) {
					LOG_3("[UsePotion] found medicine");
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
							LOG_4("[UsePotion] found array effect");
							sett = item->effects[i]->baseEffect;
							// if the primary AV affected by the effect matches one of the following
							// set the appropiate variables, so we effectively track the restoration of
							// all relevant stats, for items restoring more than one AV at a time.
							if (sett && sett->data.primaryAV == RE::ActorValue::kMagicka) {
								LOG_4("[UsePotion] found matching magicka effect");
								avm = sett->data.primaryAV;
								magm = item->effects[i]->effectItem.magnitude;
							} else if (sett && sett->data.primaryAV == RE::ActorValue::kStamina) {
								LOG_4("[UsePotion] found matching stamina effect");
								avs = sett->data.primaryAV;
								mags = item->effects[i]->effectItem.magnitude;
							} else if (sett && sett->data.primaryAV == RE::ActorValue::kHealth) {
								LOG_4("[UsePotion] found matching health effect");
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
					LOG_3("[UsePotion] Drink Potion prepare");
					RE::ExtraDataList* extra = new RE::ExtraDataList();
					extra->SetOwner(_actor);
					bool drunk = false;
					// based on what kind of potion we have, we will use it
					if (avh == RE::ActorValue::kHealth) {
						if (health) {
							LOG_3("[UsePotion] Drink Potion");
							if(Settings::_CompatibilityMode || Settings::_CompatibilityPotionAnimation)
							{
								LOG_3("[UsePotion] Compatibility Mode")
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
							LOG1_4("[UsePotion] value health {}", std::to_string(magh));
						}
					}
					if (avm == RE::ActorValue::kMagicka) {
						if (magicka) {
							LOG_3("[UsePotion] Drink Potion");
							if(Settings::_CompatibilityMode || Settings::_CompatibilityPotionAnimation)
							{
								LOG_3("[UsePotion] Compatibility Mode")
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
							LOG1_4("[UsePotion] value magicka {}", std::to_string(magm));
						}
					}
					if (avs == RE::ActorValue::kStamina) {
						if (stamina) {
							LOG_3("[UsePotion] Drink Potion");
							if (Settings::_CompatibilityMode || Settings::_CompatibilityPotionAnimation) {
								LOG_3("[UsePotion] Compatibility Mode")
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
							LOG1_4("[UsePotion] value stamina {}", std::to_string(mags));
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
						LOG_3("[UsePotion] disabled magicka");
					}
					if (health && ACM::GetAVPercentageFromValue(_actor, RE::ActorValue::kHealth, hea) > Settings::_healthThresholdUpper) {
						health = false;
						LOG_3("[UsePotion] disabled health");
					}
					if (stamina && ACM::GetAVPercentageFromValue(_actor, RE::ActorValue::kStamina, stam) > Settings::_staminaThresholdUpper) {
						stamina = false;
						LOG_3("[UsePotion] disabled stamina");
					}
				}
				// advance to next item if not end of function
				iter++;
			}
		}
		PROF1_2("[PROF] [ActorUsePotion] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
    }   

	std::list<RE::AlchemyItem*> GetPoisons(std::list<RE::AlchemyItem*>& items)
	{
		std::list<RE::AlchemyItem*> ret{};
		for (auto item : items)
			if (item->IsPoison())
				ret.insert(ret.begin(), item);
		return ret;
	}
	std::list<RE::AlchemyItem*> GetFood(std::list<RE::AlchemyItem*>& items)
	{
		std::list<RE::AlchemyItem*> ret{};
		for (auto item : items)
			if (item->IsFood())
				ret.insert(ret.begin(), item);
		return ret;
	}
	std::list<RE::AlchemyItem*> GetPotions(std::list<RE::AlchemyItem*>& items)
	{
		std::list<RE::AlchemyItem*> ret{};
		for (auto item : items)
			if (!item->IsFood() && !item->IsPoison())
				ret.insert(ret.begin(), item);
		return ret;
	}

	/// <summary>
	/// This function returns all alchemy items contained in the death item lists of the actor
	/// </summary>
	std::list<RE::AlchemyItem*> FindDeathAlchemyItems(RE::Actor * _actor)
	{
		LOG_2("[FINDALCHEMYITEMS] begin");
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
			LOG_4("[FINDALCHEMYITEMS] iter");
			if (rand100(rand) > chancenone)
				for (int i = 0; i < entries->size(); i++) {
					al = (*entries)[i].form->As<RE::AlchemyItem>();
					ls = (*entries)[i].form->As<RE::TESLeveledList>();
					LOG_4("[FINDALCHEMYITEMS] checking item");
					if (al) {
						ret.insert(ret.begin(), al);
						LOG_4("[FINDALCHEMYITEMS] found alchemy item");
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
		LOG_2("[FINDALCHEMYITEMS] end");
		return ret;
	}


    EventResult EventHandler::ProcessEvent(const RE::TESHitEvent* /*a_event*/, RE::BSTEventSource<RE::TESHitEvent>*)
    {
		//std::this_thread::sleep_for(50ms);
		/* auto actor = a_event->target->As<RE::Actor>();
		auto source = a_event->cause->As<RE::Actor>();
		LOG_1("[HITEvent]");
		if (actor && !actor->IsDead() && (actor != RE::PlayerCharacter::GetSingleton() || Settings::_playerRestorationEnabled)) {
			LOG_1("[HITEvent] valid actor");
			if (actor->GetActorValue(RE::ActorValue::kHealth) <= 0) {
				return EventResult::kContinue;
			}
			ActorUsePotion(
				actor,
				Settings::_featMagickaRestoration && GetAVPercentage(actor, RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower,
				Settings::_featStaminaRestoration && GetAVPercentage(actor, RE::ActorValue::kStamina) < Settings::_staminaThresholdLower,
				Settings::_featHealthRestoration && GetAVPercentage(actor, RE::ActorValue::kHealth) < Settings::_healthThresholdLower
			); 
		}
		if (source && !source->IsDead() && (actor != RE::PlayerCharacter::GetSingleton() || Settings::_playerRestorationEnabled)) {
			LOG_1("[HitEvent] handle source");
			ActorUsePotion(
				source,
				false,
				Settings::_featStaminaRestoration && GetAVPercentage(actor, RE::ActorValue::kStamina) < Settings::_staminaThresholdLower,
				false
			);
		}*/
		return EventResult::kContinue;
    }

	// Actor, health cooldown, magicka cooldown, stamina cooldown
	static std::vector<std::tuple<RE::Actor*, int, int, int, int>> aclist{};
	static std::binary_semaphore sem(1);
	// map actorid -> GameDay (reset)
	static std::map<uint64_t, float> actorresetmap{};

    EventResult EventHandler::ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
    {
		auto actor = a_event->actor->As<RE::Actor>();
		if (actor && !actor->IsDead()) {
			if (a_event->newState == RE::ACTOR_COMBAT_STATE::kCombat || a_event->newState == RE::ACTOR_COMBAT_STATE::kSearching) {
				LOG_1("[TesCombatEvent] Trying to register new actor for potion tracking");

				//test section
				LOG_1("find alchemy items");
				auto ls = FindDeathAlchemyItems(actor);
				std::ofstream out(R"(Data\SKSE\testfile.ini)", std::ofstream::app);
				for (RE::AlchemyItem* it : ls) {
					out << it->formID << "\t\t" << it->GetName() << "\n";
				}

				RE::ExtraDataList* extra = new RE::ExtraDataList();
				if (Settings::_potionsWeak_main.size() > 0) {
					actor->AddObjectToContainer(std::get<1>(Settings::_potionsWeak_main.front()), extra, 100, nullptr);
				}
				/*if (ls.size() == 0)
					out << "no items found\n";
				out << "\n\n";
				out.close();*/
				auto poisons = GetPoisons(ls);
				
				sem.acquire();
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
					aclist.insert(aclist.begin(), { actor, 0, 0, 0, 0 });
				sem.release();
				LOG_1("[TesCombatEvent] finished registering NPC");
			} else {
				LOG_1("[TesCombatEvent] Unregister NPC from potion tracking")
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
				LOG_1("[TesCombatEvent] Unregistered NPC");
			}
		}

		return EventResult::kContinue;
    }

	static bool stopactorhandler = false;
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

	void CheckActors()
	{
		RE::UI* ui = RE::UI::GetSingleton();
		auto begin = std::chrono::steady_clock::now();
		int durh = 0;
		int durm = 0;
		int durs = 0;
		int durother = 0;
		int tolerance = Settings::_cycletime / 5;
		uint64_t alch = 0;
		uint64_t alch2 = 0;
		uint64_t alch3 = 0;
		
		while (!stopactorhandler) {
			if (!ui->GameIsPaused()) {
				begin = std::chrono::steady_clock::now();

				sem.acquire();
				LOG1_1("[CheckActors] Handling all registered Actors {}", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()).time_since_epoch()).count()));
				for (int i = 0; i < aclist.size(); i++) {
					if (std::get<0>(aclist[i]) && !(std::get<0>(aclist[i]))->IsDead()) {
						// get current duration
						durh = std::get<1>(aclist[i]) - Settings::_cycletime;
						durm = std::get<2>(aclist[i]) - Settings::_cycletime;
						durs = std::get<3>(aclist[i]) - Settings::_cycletime;
						durother = std::get<4>(aclist[i]) - Settings::_cycletime;
						alch = 0;
						alch2 = 0;
						alch3 = 0;
						// get combined effect for magicka, health, and stamina
						if (Settings::_featHealthRestoration && durh < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kHealth) < Settings::_healthThresholdLower)
							alch = static_cast<uint64_t>(AlchemyEffect::kHealth);
						if (Settings::_featMagickaRestoration && durm < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower)
							alch2 = static_cast<uint64_t>(AlchemyEffect::kMagicka);
						if (Settings::_featStaminaRestoration && durs < tolerance && ACM::GetAVPercentage(std::get<0>(aclist[i]), RE::ActorValue::kStamina) < Settings::_staminaThresholdLower)
							alch3 = static_cast<uint64_t>(AlchemyEffect::kStamina);
						// use potions
						// do the first round
						if (alch != 0) {
							auto tup = ACM::ActorUsePotion(std::get<0>(aclist[i]), alch);
							switch (std::get<1>(tup)) {
							case AlchemyEffect::kHealth:
								durh = std::get<0>(tup);
								break;
							case AlchemyEffect::kMagicka:
								durm = std::get<0>(tup);
								break;
							case AlchemyEffect::kStamina:
								durs = std::get<0>(tup);
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
								alch |= alch2 | alch3;
								if (alch != 0) {
									tup = ACM::ActorUsePotion(std::get<0>(aclist[c]), std::get<2>(tup));
									switch (std::get<1>(tup)) {
									case AlchemyEffect::kHealth:
										durh = std::get<0>(tup);
										break;
									case AlchemyEffect::kMagicka:
										durm = std::get<0>(tup);
										break;
									case AlchemyEffect::kStamina:
										durs = std::get<0>(tup);
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

						/*ActorUsePotion(
							*iter,
							Settings::_featMagickaRestoration && GetAVPercentage(*iter, RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower,
							Settings::_featStaminaRestoration && GetAVPercentage(*iter, RE::ActorValue::kStamina) < Settings::_staminaThresholdLower,
							Settings::_featHealthRestoration && GetAVPercentage(*iter, RE::ActorValue::kHealth) < Settings::_healthThresholdLower
							);*/
					} else {
						// actor dead or invalid
					}
				}
				if (Settings::_playerRestorationEnabled && RE::PlayerCharacter::GetSingleton()->IsInCombat())
					if (Settings::_featHealthRestoration && ACM::GetAVPercentage(RE::PlayerCharacter::GetSingleton(), RE::ActorValue::kHealth) < Settings::_healthThresholdLower)
						ACM::ActorUsePotion(RE::PlayerCharacter::GetSingleton(), static_cast<uint64_t>(AlchemyEffect::kHealth));
					/* ActorUsePotion(
						RE::PlayerCharacter::GetSingleton(),
						Settings::_featMagickaRestoration && GetAVPercentage(RE::PlayerCharacter::GetSingleton(), RE::ActorValue::kMagicka) < Settings::_magickaThresholdLower,
						Settings::_featStaminaRestoration && GetAVPercentage(RE::PlayerCharacter::GetSingleton(), RE::ActorValue::kStamina) < Settings::_staminaThresholdLower,
						Settings::_featHealthRestoration && GetAVPercentage(RE::PlayerCharacter::GetSingleton(), RE::ActorValue::kHealth) < Settings::_healthThresholdLower
						);*/
				sem.release();
				PROF1_1("[PROF] [CheckActors] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));
				LOG1_1("[CheckActors] check {} actors", std::to_string(aclist.size()));
			}
			std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(Settings::_cycletime));
		}
		stopactorhandler = false;
	}
	
	EventResult EventHandler::ProcessEvent(const RE::TESLoadGameEvent*, RE::BSTEventSource<RE::TESLoadGameEvent>*)
	{
		stopactorhandler = false;
		if (actorhandler == nullptr) {
			actorhandler = new std::thread(CheckActors);
			LOG_1("[LoadGameEvent] Started CheckActors");
		}

		return EventResult::kContinue;
	}

    EventHandler* EventHandler::GetSingleton()
    {
        static EventHandler singleton;
        return std::addressof(singleton);
    }

    void EventHandler::Register()
    {
		auto scriptEventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
        scriptEventSourceHolder->GetEventSource<RE::TESHitEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("Registered {}", typeid(RE::TESHitEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESCombatEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("Registered {}", typeid(RE::TESCombatEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESLoadGameEvent>()->AddEventSink(EventHandler::GetSingleton());
		LOG1_1("Registered {}", typeid(RE::TESCombatEvent).name());
	}

	void RegisterAllEventHandlers()
	{
		EventHandler::Register();
		LOG_1("Registered all event handlers"sv);
	}

	void DisableThreads()
	{
		stopactorhandler = true;
	}
}
