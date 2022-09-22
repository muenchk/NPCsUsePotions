#include "Data.h"
#include "Logging.h"




Data* Data::GetSingleton()
{
	static Data singleton;
	return std::addressof(singleton);
}

ActorInfo* Data::FindActor(RE::Actor* actor)
{
	ActorInfo* acinfo = nullptr;
	auto itr = actorinfoMap.find(actor->GetFormID());
	if (itr == actorinfoMap.end()) {
		acinfo = new ActorInfo(actor, 0, 0, 0, 0, 0);
		actorinfoMap.insert_or_assign(actor->GetFormID(), acinfo);
	} else if (itr->second == nullptr || itr->second->actor == nullptr || itr->second->actor->GetFormID() == 0 || itr->second->actor->GetFormID() != actor->GetFormID()) {
		// either delete acinfo, deleted actor, actor fid 0 or acinfo belongs to wrong actor
		actorinfoMap.erase(actor->GetFormID());
		acinfo = new ActorInfo(actor, 0, 0, 0, 0, 0);
		actorinfoMap.insert_or_assign(actor->GetFormID(), acinfo);
	} else {
		acinfo = itr->second;
		if (acinfo->citems == nullptr)
			acinfo->citems = new ActorInfo::CustomItems();
	}
	return acinfo;
}

std::unordered_map<uint32_t, ActorInfo*>* Data::ActorInfoMap()
{
	return &actorinfoMap;
}

void Data::ResetActorInfoMap()
{
	auto itr = actorinfoMap.begin();
	while (itr != actorinfoMap.end()) {
		if (itr->second)
			itr->second->_boss = false;
		itr->second->citems->Reset();
		itr++;
	}
}
