#include <semaphore>
#include <algorithm>
#include <iterator>

#include "Data.h"
#include "Logging.h"
#include "Utility.h"
#include "Statistics.h"
#include "ActorManipulation.h"

void Data::Init()
{
	datahandler = RE::TESDataHandler::GetSingleton();
}

Data* Data::GetSingleton()
{
	static Data singleton;
	return std::addressof(singleton);
}

std::binary_semaphore lockdata{ 1 };

ActorInfo* Data::FindActor(RE::Actor* actor)
{
	if (Utility::ValidateActor(actor) == false)
		return nullptr; // worst case, should not be necessary here
	ActorInfo* acinfo = nullptr;
	lockdata.acquire();
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
	lockdata.release();
	return acinfo;
}

ActorInfo* Data::FindActor(RE::FormID actorid)
{
	RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(actorid);
	if (actor)
		return FindActor(actor);
	else
		return nullptr;
}

void Data::DeleteActor(RE::FormID actorid)
{
	ActorInfo* acinfo = nullptr;
	lockdata.acquire();
	auto itr = actorinfoMap.find(actorid);
	if (itr != actorinfoMap.end()) {
		acinfo = itr->second;
		actorinfoMap.erase(actorid);
		delete acinfo;
		acinfo = nullptr;
	}
	lockdata.release();
}

void Data::ResetActorInfoMap()
{
	lockdata.acquire();
	auto itr = actorinfoMap.begin();
	while (itr != actorinfoMap.end()) {
		if (itr->second)
			itr->second->_boss = false;
		itr->second->citems->Reset();
		itr++;
	}
	lockdata.release();
}

long Data::SaveActorInfoMap(SKSE::SerializationInterface* a_intfc)
{
	lockdata.acquire();
	loginfo("[Data] [SaveActorInfoMap] Writing ActorInfo");
	loginfo("[Data] [SaveActorInfoMap] {} records to write", actorinfoMap.size());
	
	long size = 0;
	long successfulwritten = 0;

	// transform second values of map into a vector and operate on the vector instead
	std::vector<ActorInfo*> acvec;
	// write map data to vector
	std::transform(
		actorinfoMap.begin(),
		actorinfoMap.end(),
		std::back_inserter(acvec),
		[](auto& kv) { return kv.second; });
	// iterate over the vector entries
	for (int i = 0; i < acvec.size(); i++) {
		loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if begin", i);
		if (acvec[i] != nullptr) {// && acvec[i]->actor != nullptr && acvec[i]->actor->IsDeleted() == false && acvec[i]->actor->GetFormID() != 0 && acvec[i]->actor->IsDead() == false) {
			loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if not nullptr", i);
			if (acvec[i]->actor != nullptr) {
				loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not null", i);
				if (acvec[i]->actor->IsDeleted() == false) {
					loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not deleted", i);
					if (acvec[i]->actor->GetFormID() != 0) {
						loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if id not 0", i);
						if (acvec[i]->actor->boolBits & RE::Actor::BOOL_BITS::kDead) {
							loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor dead", i);
						} else {
							loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not dead", i);
							loginfo("[Data] [SaveActorInfoMap] Writing {}, number {}", acvec[i]->name, i);
							if (a_intfc->OpenRecord('ACIF', ActorInfo::GetVersion())) {
								loginfo("[Data] [SaveActorInfoMap] \tget data size");
								// get entry length
								int length = acvec[i]->GetDataSize();
								if (length == 0) {
									logwarn("[Data] [WriteData] failed to write ActorInfo record: record length 0");
									continue;
								}
								// save written bytes nu´mber
								size += length;
								loginfo("[Data] [SaveActorInfoMap] \tcreate buffer");
								// create buffer
								unsigned char* buffer = new unsigned char[length + 1];
								if (buffer == nullptr) {
									logwarn("[DataStorage] [WriteData] failed to write ActorInfo record: buffer null");
									continue;
								}
								loginfo("[Data] [SaveActorInfoMap] \twrite data to buffer");
								// fill buffer
								if (acvec[i]->WriteData(buffer, 0) == false) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: Writing of ActorInfo failed");
									delete[] buffer;
									continue;
								}
								loginfo("[Data] [SaveActorInfoMap] \twrite record");
								// write record
								a_intfc->WriteRecordData(buffer, length);
								loginfo("[Data] [SaveActorInfoMap] \tDelete buffer");
								delete[] buffer;
								successfulwritten++;
							} else if (acvec[i] == nullptr) {
								logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: ActorInfo invalidated");
							} else if (acvec[i]->actor == nullptr) {
								logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: actor invalidated");
							} else if (acvec[i]->actor->GetFormID() == 0) {
								logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: formid invalid");
							} else if ((acvec[i]->actor->boolBits & RE::Actor::BOOL_BITS::kDead)) {
								logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: actor died");
							} else {
								logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: unknown reason");
							}
						}
					} else
						loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if id 0", i);
				} else
					loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor deleted", i);
			} else
				loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor null", i);

			
		} else
			loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if nullptr", i);
	}

	/*
	// the following seems to ccause ctds, since it is semantically correct try antother approach.
	auto itr = actorinfoMap.begin();
	while (itr != actorinfoMap.end()) {
		loginfo("[Data] [SaveActorInfoMap] begin iteration");
		if (itr->second && itr->second->actor && itr->second->actor->GetFormID() != 0 && itr->second->actor->IsDead() == false) {
			loginfo("[Data] [SaveActorInfoMap] Writing {}", itr->second->actor->GetName());
			// open skse record
			if (a_intfc->OpenRecord('ACIF', 0)) {
				loginfo("[Data] [SaveActorInfoMap] \tget data size");
				// get entry length
				int length = itr->second->GetDataSize();
				if (length == 0) {
					logwarn("[Data] [WriteData] failed to write ActorInfo record: record length 0");
					itr++;
					continue;
				}
				loginfo("[Data] [SaveActorInfoMap] \tcreate buffer");
				// create buffer
				unsigned char* buffer = new unsigned char[length];
				if (buffer == nullptr) {
					logwarn("[DataStorage] [WriteData] failed to write ActorInfo record: buffer null");
					itr++;
					continue;
				}
				loginfo("[Data] [SaveActorInfoMap] \twrite data to buffer");
				// fill buffer
				if (itr->second->WriteData(buffer, 0) == false) {
					logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: Writing of ActorInfo failed");
					itr++;
					continue;
				}
				loginfo("[Data] [SaveActorInfoMap] \twrite record");
				// write record
				a_intfc->WriteRecordData(buffer, length);
				loginfo("[Data] [SaveActorInfoMap] \tDelete buffer");
				delete[] buffer;
			} else if (itr == actorinfoMap.end()) {
				logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: iterator invalidated");
			} else if (itr->second == nullptr) {
				logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: ActorInfo invalidated");
			} else if (itr->second->actor == nullptr) {
				logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: actor invalidated");
			} else if (itr->second->actor->GetFormID() == 0) {
				logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: formid invalid");
			} else if (itr->second->actor->IsDead() == true) {
				logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: actor died");
			} else {
				logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record for {}", Utility::GetHex(itr->second->actor->GetFormID()));
			}
		}
		itr++;
	}*/
	lockdata.release();
	Statistics::Storage_ActorsSavedLast = successfulwritten;
	return size;
}

long Data::ReadActorInfoMap(SKSE::SerializationInterface* a_intfc)
{
	uint32_t type = 0;
	uint32_t version = 0;
	uint32_t length = 0;

	int accounter = 0;
	int acfcounter = 0;
	int acdcounter = 0;

	long size = 0;

	// get map lock
	lockdata.acquire();

	loginfo("[Data] [ReadActorInfoMap] Reading ActorInfoMap...");
	while (a_intfc->GetNextRecordInfo(type, version, length)) {
		loginfo("[Data] [ReadActorInfoMap] found record with type {} and length {}", type, length);
		size += length;
		switch (type) {
		case 'ACIF':  // ActorInfo
			unsigned char* buffer = new unsigned char[length];
			a_intfc->ReadRecordData(buffer, length);
			ActorInfo* acinfo = new ActorInfo();
			if (acinfo->ReadData(buffer, 0, length) == false) {
				acfcounter++;
				logwarn("[Data] [ReadActorInfoMap] Couldn't read ActorInfo");
			} else if (acinfo->actor->IsDeleted() || (acinfo->actor->boolBits & RE::Actor::BOOL_BITS::kDead)) {
				acdcounter++;
				logwarn("[Data] [ReadActorInfoMap] actor dead or deleted {}", acinfo->name);
			} else {
				accounter++;
				actorinfoMap.insert_or_assign(acinfo->actor->GetFormID(), acinfo);
				loginfo("[Data] [ReadActorInfoMap] read ActorInfo. actor: {}", Utility::PrintForm(acinfo->actor));
			}
			delete[] buffer;
			break;
		}
	}
	// release lock
	lockdata.release();

	loginfo("[Data] [ReadActorInfoMap] Read {} ActorInfos", accounter);
	loginfo("[Data] [ReadActorInfoMap] Read {} dead or deleted ActorInfos", acdcounter);
	loginfo("[Data] [ReadActorInfoMap] Failed to read {} ActorInfos", acfcounter);
	Statistics::Storage_ActorsReadLast = accounter;
	return size;
}

void Data::DeleteActorInfoMap()
{
	lockdata.acquire();
	auto itr = actorinfoMap.begin();
	while (itr != actorinfoMap.end()) {
		if (itr->second)
			try {
				delete itr->second;
			} catch (std::exception&) {}
		itr++;
	}
	actorinfoMap.clear();
	lockdata.release();
}

void Data::SetAlchItemEffects(uint32_t id, AlchemyEffectBase effects, int duration, float magnitude, bool detrimental, int dosage)
{
	std::tuple<AlchemyEffectBase, int, float, bool, int> t = { effects, duration, magnitude, detrimental, dosage };
	alchitemEffectMap.insert_or_assign(id, t);
}

std::tuple<bool, AlchemyEffectBase, int, float, bool, int> Data::GetAlchItemEffects(uint32_t id)
{
	auto itr = alchitemEffectMap.find(id);
	if (itr != alchitemEffectMap.end()) {
		auto [eff, dur, mag, detri, dosage] = itr->second;
		// found
		return { true, eff, dur, mag, detri, dosage };
	} else {
		// not found
		return { false, 0, 0, 0.0f, false, 0 };
	}
}

void Data::ResetAlchItemEffects()
{
	alchitemEffectMap.clear();
}

RE::TESForm* Data::FindForm(uint32_t formid, std::string pluginname)
{
	auto itr = customItemFormMap.find(formid);
	if (itr != customItemFormMap.end())
		return itr->second;
	RE::TESForm* form = Utility::GetTESForm(datahandler, formid, pluginname, "");
	if (form != nullptr) {
		customItemFormMap.insert_or_assign(formid, form);
		return form;
	}
	return nullptr;
}

RE::EffectSetting* Data::FindMagicEffect(uint32_t formid, std::string pluginname)
{
	auto itr = customItemFormMap.find(formid);
	if (itr != customItemFormMap.end())
		return itr->second->As<RE::EffectSetting>();
	RE::TESForm* form = Utility::GetTESForm(datahandler, formid, pluginname, "");
	if (form != nullptr) {
		customItemFormMap.insert_or_assign(formid, form);
		return form->As<RE::EffectSetting>();
	}
	return nullptr;
}

RE::BGSPerk* Data::FindPerk(uint32_t formid, std::string pluginname)
{
	auto itr = customItemFormMap.find(formid);
	if (itr != customItemFormMap.end())
		return itr->second->As<RE::BGSPerk>();
	RE::TESForm* form = Utility::GetTESForm(datahandler, formid, pluginname, "");
	if (form != nullptr) {
		customItemFormMap.insert_or_assign(formid, form);
		return form->As<RE::BGSPerk>();
	}
	return nullptr;
}

void Data::DeleteFormCustom(RE::FormID formid)
{
	ActorInfo* acinfo = nullptr;
	lockdata.acquire();
	auto itr = customItemFormMap.find(formid);
	if (itr != customItemFormMap.end()) {
		customItemFormMap.erase(formid);
	}
	lockdata.release();
}

int Data::GetPoisonDosage(RE::AlchemyItem* poison)
{
	{
		auto [mapf, eff, dur, mag, detr, dosage] = GetAlchItemEffects(poison->GetFormID());
		if (mapf) {
			// found it in database
			return dosage;
		}
	}
	// we didn't find it, so we need to calculate it
	ACM::HasAlchemyEffect(poison, 0xFFFFFFFFFFFFFFFF); // find any effect, results will be entered into database if valid
	{
		auto [mapf, eff, dur, mag, detr, dosage] = GetAlchItemEffects(poison->GetFormID());
		if (mapf) {
			// found it in database
			return dosage;
		}
	}
	return Settings::Poisons::_Dosage;
}
