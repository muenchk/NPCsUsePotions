#include <semaphore>
#include <algorithm>
#include <iterator>

#include "Data.h"
#include "Logging.h"
#include "Utility.h"
#include "Statistics.h"
#include "ActorManipulation.h"
#include "BufferOperations.h"

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
// stores temp actor info until tey are deleted on reset
std::vector<ActorInfo*> emptyActorInfos;

ActorInfo* Data::FindActor(RE::Actor* actor)
{
	if (Utility::ValidateActor(actor) == false)
		return nullptr; // worst case, should not be necessary here
	ActorInfo* acinfo = nullptr;
	lockdata.acquire();
	// check whether the actor was deleted before
	if (deletedActors.contains(actor->GetFormID())) {
		// create dummy ActorInfo
		acinfo = new ActorInfo();
		emptyActorInfos.push_back(acinfo);
		return acinfo;
	}
	auto itr = actorinfoMap.find(actor->GetFormID());
	if (itr == actorinfoMap.end()) {
		acinfo = new ActorInfo(actor, 0, 0, 0, 0, 0);
		actorinfoMap.insert_or_assign(actor->GetFormID(), acinfo);
	} else if (itr->second == nullptr || itr->second->IsValid() && (itr->second->actor == nullptr || itr->second->actor->GetFormID() == 0 || itr->second->actor->GetFormID() != actor->GetFormID())) {
		// either delete acinfo, deleted actor, actor fid 0 or acinfo belongs to wrong actor
		actorinfoMap.erase(actor->GetFormID());
		acinfo = new ActorInfo(actor, 0, 0, 0, 0, 0);
		actorinfoMap.insert_or_assign(actor->GetFormID(), acinfo);
	} else if (itr->second->IsValid() == false) {
		acinfo = itr->second;
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
	else {
		// create dummy ActorInfo
		ActorInfo* acinfo = new ActorInfo();
		emptyActorInfos.push_back(acinfo);
		return acinfo;
	}
}

void Data::DeleteActor(RE::FormID actorid)
{
	// if we delete the object itself, we may have problems when someone tries to access the deleted object
	// so just flag it as invalid and move it to the list of empty actor refs
	ActorInfo* acinfo = nullptr;
	lockdata.acquire();
	auto itr = actorinfoMap.find(actorid);
	if (itr != actorinfoMap.end()) {
		acinfo = itr->second;
		actorinfoMap.erase(actorid);
		acinfo->SetInvalid();
		emptyActorInfos.push_back(acinfo);
		acinfo = nullptr;
		// save deleted actors, so we do not create new actorinfos for these
		deletedActors.insert(actorid);
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

long Data::SaveDeletedActors(SKSE::SerializationInterface* a_intfc)
{
	lockdata.acquire();
	LOG_1("{}[Data] [SaveDeletedActors] Writing Deleted Actors");
	LOG1_1("{}[Data] [SaveDeletedActors] {} actors to write", deletedActors.size());

	long size = 0;
	long successfulwritten = 0;

	for (auto& actorid : deletedActors) {
		uint32_t formid = Utility::Mods::GetIndexLessFormID(actorid);
		std::string pluginname = Utility::Mods::GetPluginNameFromID(actorid);
		if (a_intfc->OpenRecord('DAID', 0)) {
			// get entry length
			int length = 4 + Buffer::CalcStringLength(pluginname);
			// save written bytes number
			size += length;
			// create buffer
			unsigned char* buffer = new unsigned char[length + 1];
			if (buffer == nullptr) {
				logwarn("[DataStorage] [WriteData] failed to write Deleted Actor record: buffer null");
				continue;
			}
			// fill buffer
			int offset = 0;
			Buffer::Write(actorid, buffer, offset);
			Buffer::Write(pluginname, buffer, offset);
			// write record
			a_intfc->WriteRecordData(buffer, length);
			delete[] buffer;
			successfulwritten++;
		}
	}
	LOG_1("{}[Data] [SaveDeletedActors] Writing Deleted Actors finished.");

	return size;
}

long Data::ReadDeletedActors(SKSE::SerializationInterface* a_intfc, uint32_t length)
{
	long size = 0;
	// get map lock
	lockdata.acquire();

	LOG_1("{}[Data] [ReadDeletedActors] Reading Deleted Actor...");
	unsigned char* buffer = new unsigned char[length];
	a_intfc->ReadRecordData(buffer, length);
	if (length >= 12) {
		int offset = 0;
		RE::TESForm* form = RE::TESDataHandler::GetSingleton()->LookupForm(Buffer::ReadUInt32(buffer, offset), Buffer::ReadString(buffer, offset));
		if (form)
			deletedActors.insert(form->GetFormID());
	}
	delete[] buffer;
	// release lock
	lockdata.release();

	return size;
}

long Data::SaveActorInfoMap(SKSE::SerializationInterface* a_intfc)
{
	lockdata.acquire();
	LOG_1("{}[Data] [SaveActorInfoMap] Writing ActorInfo");
	LOG1_1("{}[Data] [SaveActorInfoMap] {} records to write", actorinfoMap.size());
	
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
		LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if begin", i);
		if (acvec[i] != nullptr) {// && acvec[i]->actor != nullptr && acvec[i]->actor->IsDeleted() == false && acvec[i]->actor->GetFormID() != 0 && acvec[i]->actor->IsDead() == false) {
			if (acvec[i]->IsValid()) {
				LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if valid", i);
				LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if not nullptr", i);
				if (acvec[i]->actor != nullptr) {
					LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not null", i);
					if ((acvec[i]->actor->formFlags & RE::TESForm::RecordFlags::kDeleted) == 0) {
						LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not deleted", i);
						if (acvec[i]->actor->GetFormID() != 0) {
							LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if id not 0", i);
							if (acvec[i]->actor->boolBits & RE::Actor::BOOL_BITS::kDead) {
								LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor dead", i);
							} else {
								LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not dead", i);
								LOG2_3("{}[Data] [SaveActorInfoMap] Writing {}, number {}", acvec[i]->name, i);
								if (a_intfc->OpenRecord('ACIF', ActorInfo::GetVersion())) {
									LOG_3("{}[Data] [SaveActorInfoMap] \tget data size");
									// get entry length
									int length = acvec[i]->GetDataSize();
									if (length == 0) {
										logwarn("[Data] [WriteData] failed to write ActorInfo record: record length 0");
										continue;
									}
									// save written bytes nu´mber
									size += length;
									LOG_3("{}[Data] [SaveActorInfoMap] \tcreate buffer");
									// create buffer
									unsigned char* buffer = new unsigned char[length + 1];
									if (buffer == nullptr) {
										logwarn("[DataStorage] [WriteData] failed to write ActorInfo record: buffer null");
										continue;
									}
									LOG_3("{}[Data] [SaveActorInfoMap] \twrite data to buffer");
									// fill buffer
									if (acvec[i]->WriteData(buffer, 0) == false) {
										logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: Writing of ActorInfo failed");
										delete[] buffer;
										continue;
									}
									LOG_3("{}[Data] [SaveActorInfoMap] \twrite record");
									// write record
									a_intfc->WriteRecordData(buffer, length);
									LOG_3("{}[Data] [SaveActorInfoMap] \tDelete buffer");
									delete[] buffer;
									successfulwritten++;
								} else if (acvec[i] == nullptr) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: ActorInfo invalidated");
								} else if (acvec[i]->actor == nullptr) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: actor invalidated");
								} else if (acvec[i]->actor->GetFormID() == 0) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: formid invalid");
								} else if (acvec[i]->actor->IsDead() == true) {
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
				loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo invalid", i);
			
		} else
			loginfo("[Data] [SaveActorInfoMap] {} Writing ActorInfo if nullptr", i);
	}

	lockdata.release();
	Statistics::Storage_ActorsSavedLast = successfulwritten;
	return size;
}

long Data::ReadActorInfoMap(SKSE::SerializationInterface * a_intfc, uint32_t length, int& accounter, int& acdcounter, int& acfcounter)
{
	long size = 0;

	// get map lock
	lockdata.acquire();

	LOG_1("{}[Data] [ReadActorInfoMap] Reading ActorInfo...");
	unsigned char* buffer = new unsigned char[length];
	a_intfc->ReadRecordData(buffer, length);
	ActorInfo* acinfo = new ActorInfo();
	if (acinfo->ReadData(buffer, 0, length) == false) {
		acfcounter++;
		logwarn("[Data] [ReadActorInfoMap] Couldn't read ActorInfo");
	} else if (acinfo->IsValid() == false) {
		acdcounter++;
		logwarn("[Data] [ReadActorInfoMap] actor invalid {}", acinfo->name);
	} else if ((acinfo->actor->formFlags & RE::TESForm::RecordFlags::kDeleted) || acinfo->actor->IsDead()) {
		acdcounter++;
		logwarn("[Data] [ReadActorInfoMap] actor dead or deleted {}", acinfo->name);
	} else {
		accounter++;
		actorinfoMap.insert_or_assign(acinfo->actor->GetFormID(), acinfo);
		LOG1_3("{}[Data] [ReadActorInfoMap] read ActorInfo. actor: {}", Utility::PrintForm(acinfo->actor));
	}
	delete[] buffer;
	// release lock
	lockdata.release();

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
	// delete temporary ActorInfos
	for (int i = (int)emptyActorInfos.size() - 1; i >= 0; i--) {
		delete emptyActorInfos[i];
	}
	emptyActorInfos.clear();
	deletedActors.clear();
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
