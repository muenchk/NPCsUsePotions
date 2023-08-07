#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "ActorInfo.h"
#include "AlchemyEffect.h"

class Data
{
private:
	/// <summary>
	/// map that contains information about any npc that has entered combat during runtime
	/// </summary>
	std::unordered_map<uint32_t, std::shared_ptr<ActorInfo>> actorinfoMap;
	/// <summary>
	/// set that contains the ids of all deleted actors, so we do not accidentally create new actor infos for invalid actors
	/// </summary>
	std::unordered_set<RE::FormID> deletedActors;
	std::unordered_set<RE::FormID> validActors;

	/// <summary>
	/// map that maps potionids to potion properties (effect, duration, magnitude, detrimental, dosage)
	/// </summary>
	std::unordered_map<uint32_t, std::tuple<AlchemicEffect, int, float, bool, int>> alchitemEffectMap;

	/// <summary>
	/// map that contains game objects that are used in custom object conditions, for fast access
	/// </summary>
	std::unordered_map<uint32_t, RE::TESForm*> customItemFormMap;

	/// <summary>
	/// datahandler
	/// </summary>
	RE::TESDataHandler* datahandler = nullptr;

	/// <summary>
	/// Creates a new shared pointer to an ActorInfo and inserts it into the map and valid actors
	/// </summary>
	std::shared_ptr<ActorInfo> CreateActorInfo(RE::Actor* actor);
	/// <summary>
	/// Creates a new shared pointer to an ActorInfo, without initializing it
	/// </summary>
	std::shared_ptr<ActorInfo> CreateActorInfoNew();
	/// <summary>
	/// Returns a shared pointer to an invalid ActorInfo
	/// </summary>
	std::shared_ptr<ActorInfo> CreateActorInfoEmpty();
	/// <summary>
	/// Inserts a shared pointer to an ActorInfo into the map and valid actors
	/// </summary>
	/// <param name="actor"></param>
	/// <returns></returns>
	void RegisterActorInfo(std::shared_ptr<ActorInfo> acinfo);
	/// <summary>
	/// Deletes an ActorInfo from the map and valid actors
	/// </summary>
	/// <param name="actorid"></param>
	/// <returns></returns>
	void DeleteActorInfo(RE::FormID formid);

public:
	/// <summary>
	/// Initializes data.
	/// </summary>
	void Init();
	/// <summary>
	/// returns a pointer to a static Data object
	/// </summary>
	/// <returns></returns>
	static Data* GetSingleton();
	/// <summary>
	/// Returns an actorinfo object with information about [actor]
	/// </summary>
	/// <param name="actor">the actor to find</param>
	/// <returns></returns>
	std::shared_ptr<ActorInfo> FindActor(RE::Actor* actor);
	/// <summary>
	/// Returns an actorinfo object with information about [actor]
	/// Does not create new information objects
	/// </summary>
	/// <param name="actor">the actor to find</param>
	/// <returns></returns>
	std::shared_ptr<ActorInfo> FindActorExisting(RE::Actor* actor);
	/// <summary>
	/// Returns an actorinfo object with information about [actor]
	/// Does not create new information objects
	/// </summary>
	/// <param name="actor">the actor to find</param>
	/// <returns></returns>
	std::shared_ptr<ActorInfo> FindActorExisting(RE::FormID actor);
	/// <summary>
	/// Returns and actorinfo object with information about [actorid]. THIS MAY RETURN NULLPTR.
	/// </summary>
	/// <param name="actorid"></param>
	/// <returns></returns>
	std::shared_ptr<ActorInfo> FindActor(RE::FormID actorid);
	/// <summary>
	/// Updates an actorinfo object and returns whether it is still valid or not
	/// </summary>
	bool UpdateActorInfo(std::shared_ptr<ActorInfo> acinfo);
	/// <summary>
	/// Removes an actor from the data
	/// </summary>
	/// <param name="actor"></param>
	void DeleteActor(RE::FormID actorid);
	/// <summary>
	/// Cleans invalid or deleted actorinfos
	/// </summary>
	void CleanActorInfos();
	/// <summary>
	/// Resets information about actors
	/// </summary>
	void ResetActorInfoMap();
	/// <summary>
	/// Deletes all objects in the actorinfomap
	/// </summary>
	void DeleteActorInfoMap();
	/// <summary>
	/// saves ActorInfoMap in skse co-save
	/// </summary>
	/// <param name="a_intfc"></param>
	/// <return>Returns number of bytes written</return>
	long SaveActorInfoMap(SKSE::SerializationInterface* a_intfc);
	/// <summary>
	/// reads ActorInfoMap from skse co-save
	/// </summary>
	/// <param name="a_intfc"></param>
	/// <return>Returns number of bytes read</return>
	long ReadActorInfoMap(SKSE::SerializationInterface* a_intfc, uint32_t length, int& accounter, int& acdcounter, int& acfcounter);
	/// <summary>
	/// Saves the list of deleted actors
	/// </summary>
	long SaveDeletedActors(SKSE::SerializationInterface* a_intfc);
	/// <summary>
	/// Reads the list of deleted actors
	/// </summary>
	long ReadDeletedActors(SKSE::SerializationInterface* a_intfc, uint32_t length);

	/// <summary>
	/// Saves the alchemic effects of an AlchemyItem for later use
	/// </summary>
	/// <param name="id">id of the AlchemyItem</param>
	/// <param name="effects">the alchemic effects of the item</param>
	void SetAlchItemEffects(uint32_t id, AlchemicEffect effects, int duration, float magnitude, bool detrimental, int dosage);
	/// <summary>
	/// Retrieves the alchemic effects of an AlchemyItem
	/// </summary>
	/// <param name="id">the id of the AlchemyItem</param>
	/// <returns>whether the item was found, and the saved effects [found, effect, duration, magnitude, detrimental, dosage]</returns>
	std::tuple<bool, AlchemicEffect, int, float, bool, int> GetAlchItemEffects(uint32_t id);
	/// <summary>
	/// Resets all saved AlchemyItem effects
	/// </summary>
	void ResetAlchItemEffects();

	/// <summary>
	/// Returns the TESForm associated with the formid from an internal buffer
	/// </summary>
	/// <param name="formid"></param>
	/// <param name="pluginname"></param>
	/// <returns></returns>
	RE::TESForm* FindForm(uint32_t formid, std::string pluginname);
	/// <summary>
	/// Returns the MagicEffect associated with the formid from an internal buffer
	/// </summary>
	/// <param name="formid"></param>
	/// <param name="pluginname"></param>
	/// <returns></returns>
	RE::EffectSetting* FindMagicEffect(uint32_t formid, std::string pluginname);
	/// <summary>
	/// Returns the Perk associated with the formid from an internal buffer
	/// </summary>
	/// <param name="formid"></param>
	/// <param name="pluginname"></param>
	/// <returns></returns>
	RE::BGSPerk* FindPerk(uint32_t formid, std::string pluginname);
	/// <summary>
	/// Returns the AlchemyItem associated with the formid from an internal buffer
	/// </summary>
	/// <param name="formid"></param>
	/// <param name="pluginname"></param>
	/// <returns></returns>
	RE::AlchemyItem* FindAlchemyItem(uint32_t formid, std::string pluginname);
	/// <summary>
	/// Removes an actor from the data
	/// </summary>
	/// <param name="actor"></param>
	void DeleteFormCustom(RE::FormID actorid);

	/// <summary>
	/// Returns the dosage of the given poison that is to be applied
	/// </summary>
	/// <param name="poison"></param>
	/// <returns></returns>
	int GetPoisonDosage(RE::AlchemyItem* poison);
}; 
