
namespace Events
{

	/// DEPRECATED

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
							LOG1_4("{}[UsePotion] value health {}", std::to_string(magh));
						}
					}
					if (avm == RE::ActorValue::kMagicka) {
						if (magicka) {
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

}
