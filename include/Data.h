#pragma once

#include <unordered_map>

#include "ActorInfo.h"
#include "AlchemyEffect.h"

class Data
{
private:
	/// <summary>
	/// map that contains information about any npc that has entered combat during runtime
	/// </summary>
	std::unordered_map<uint32_t, ActorInfo*> actorinfoMap;

	/// <summary>
	/// map that maps potionids to potion properties (effect, duration, magnitude, detrimental)
	/// </summary>
	std::unordered_map<uint32_t, std::tuple<AlchemyEffectBase, int, float, bool>> alchitemEffectMap;

	/// <summary>
	/// map that contains game objects that are used in custom object conditions, for fast access
	/// </summary>
	std::unordered_map<uint32_t, RE::TESForm*> customItemFormMap;

public:
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
	ActorInfo* FindActor(RE::Actor* actor);
	/// <summary>
	/// Removes an actor from the data
	/// </summary>
	/// <param name="actor"></param>
	void DeleteActor(RE::FormID actorid);
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
	void SaveActorInfoMap(SKSE::SerializationInterface* a_intfc);
	/// <summary>
	/// reads ActorInfoMap from skse co-save
	/// </summary>
	/// <param name="a_intfc"></param>
	void ReadActorInfoMap(SKSE::SerializationInterface* a_intfc);

	/// <summary>
	/// Saves the alchemic effects of an AlchemyItem for later use
	/// </summary>
	/// <param name="id">id of the AlchemyItem</param>
	/// <param name="effects">the alchemic effects of the item</param>
	void SetAlchItemEffects(uint32_t id, AlchemyEffectBase effects, int duration, float magnitude, bool detrimental);
	/// <summary>
	/// Retrieves the alchemic effects of an AlchemyItem
	/// </summary>
	/// <param name="id">the id of the AlchemyItem</param>
	/// <returns>whether the item was found, and the saved effects</returns>
	std::tuple<bool, AlchemyEffectBase, int, float, bool> GetAlchItemEffects(uint32_t id);
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
	/// Removes an actor from the data
	/// </summary>
	/// <param name="actor"></param>
	void DeleteFormCustom(RE::FormID actorid);
}; 
