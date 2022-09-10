#pragma once

#include <unordered_map>

#include "ActorInfo.h"

class Data
{
private:
	/// <summary>
	/// map that contains information about any npc that has entered combat during runtime
	/// </summary>
	std::unordered_map<uint32_t, ActorInfo*> actorinfoMap;

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
	/// Returns the map with all available actorinformation
	/// </summary>
	/// <returns></returns>
	std::unordered_map<uint32_t, ActorInfo*>* ActorInfoMap();
	/// <summary>
	/// Resets information about actors
	/// </summary>
	void ResetActorInfoMap();
}; 
