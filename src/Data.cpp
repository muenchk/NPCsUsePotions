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

std::shared_ptr<ActorInfo> Data::CreateActorInfo(RE::Actor* actor)
{
	std::shared_ptr<ActorInfo> acinfo = std::make_shared<ActorInfo>(actor);
	if (acinfo->IsValid()) {
		validActors.insert(acinfo->GetFormID());
		actorinfoMap.insert_or_assign(acinfo->GetFormID(), acinfo);
		Distribution::CalcRule(acinfo);
	}
	LOG1_4("{}[Data] [CreateActorInfo] {}", Utility::PrintForm(acinfo));
	return acinfo;
}

std::shared_ptr<ActorInfo> Data::CreateActorInfoNew()
{
	LOG_4("{}[Data] [CreateActorInfoNew]");
	return std::make_shared<ActorInfo>();
}

std::shared_ptr<ActorInfo> Data::CreateActorInfoEmpty()
{
	std::shared_ptr<ActorInfo> empty = std::make_shared<ActorInfo>(true); // blocks resetting this instance
	empty->SetInvalid();
	empty->SetDeleted();
	LOG_4("{}[Data] [CreateActorInfoEmpty]");
	return empty;
}

void Data::RegisterActorInfo(std::shared_ptr<ActorInfo> acinfo)
{
	if (acinfo->IsValid()) {
		validActors.insert(acinfo->GetFormID());
		actorinfoMap.insert_or_assign(acinfo->GetFormID(), acinfo);
		Distribution::CalcRule(acinfo);
	}
}

void Data::DeleteActorInfo(RE::FormID formid)
{
	validActors.erase(formid);
	actorinfoMap.erase(formid);
}

std::shared_ptr<ActorInfo> Data::FindActor(RE::Actor* actor)
{
	if (Utility::ValidateActor(actor) == false)
		return CreateActorInfoEmpty();  // worst case, should not be necessary here
	lockdata.acquire();
	// check whether the actor was deleted before
	if (deletedActors.contains(actor->GetFormID())) {
		// create dummy ActorInfo
		std::shared_ptr<ActorInfo> acinfo = CreateActorInfoEmpty();
		lockdata.release();
		return acinfo;
	}
	// if there already is an valid object for the actor return it
	if (validActors.contains(actor->GetFormID())) {
		// find the object
		auto itr = actorinfoMap.find(actor->GetFormID());
		if (itr != actorinfoMap.end()) {
			// found it, check it for validity and deleted status
			if (itr->second->IsValid() && !itr->second->GetDeleted()) {
				lockdata.release();
				return itr->second;
			}
			// else go to next point
		}
	}
	// not found or not valid
	// create new object. This will override existing objects for a formid as long as they are invalid or deleted
	// as checked above
	std::shared_ptr<ActorInfo> acinfo = CreateActorInfo(actor);
	lockdata.release();
	return acinfo;
}

std::shared_ptr<ActorInfo> Data::FindActorExisting(RE::Actor* actor)
{
	if (Utility::ValidateActor(actor) == false)
		return CreateActorInfoEmpty();  // worst case, should not be necessary here
	lockdata.acquire();
	// check whether the actor was deleted before
	if (deletedActors.contains(actor->GetFormID())) {
		// create dummy ActorInfo
		std::shared_ptr<ActorInfo> acinfo = CreateActorInfoEmpty();
		lockdata.release();
		return acinfo;
	}
	// if there already is an valid object for the actor return it
	if (validActors.contains(actor->GetFormID())) {
		// find the object
		auto itr = actorinfoMap.find(actor->GetFormID());
		if (itr != actorinfoMap.end()) {
			// found it, check it for validity and deleted status
			if (itr->second->IsValid() && !itr->second->GetDeleted()) {
				lockdata.release();
				return itr->second;
			}
			// else go to next point
		}
	}
	// if there is no valid actorinfo object present, return an empty one and do not create a new one
	std::shared_ptr<ActorInfo> acinfo = CreateActorInfoEmpty();
	lockdata.release();
	return acinfo;
}

std::shared_ptr<ActorInfo> Data::FindActor(RE::FormID actorid)
{
	RE::Actor* actor = RE::TESForm::LookupByID<RE::Actor>(actorid);
	if (actor)
		return FindActor(actor);
	else {
		// create dummy ActorInfo
		std::shared_ptr<ActorInfo> acinfo = CreateActorInfoEmpty();
		return acinfo;
	}
}

bool Data::UpdateActorInfo(std::shared_ptr<ActorInfo> acinfo)
{
	acinfo->Update();
	if (!acinfo->IsValid() || acinfo->GetDeleted()) {
		validActors.erase(acinfo->GetFormID());
		DeleteActorInfo(acinfo->GetFormID());
		return false;
	}
	return true;
}

void Data::DeleteActor(RE::FormID actorid)
{
	// if we delete the object itself, we may have problems when someone tries to access the deleted object
	// so just flag it as invalid and move it to the list of empty actor refs
	lockdata.acquire();
	auto itr = actorinfoMap.find(actorid);
	if (itr != actorinfoMap.end()) {
		std::shared_ptr<ActorInfo> acinfo = itr->second;
		acinfo->SetInvalid();
		acinfo->SetDeleted();
		// save deleted actors, so we do not create new actorinfos for these
		deletedActors.insert(actorid);
		DeleteActorInfo(actorid);
	}
	lockdata.release();
}

void Data::ResetActorInfoMap()
{
	lockdata.acquire();
	auto itr = actorinfoMap.begin();
	while (itr != actorinfoMap.end()) {
		itr->second->citems.Reset();
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
	lockdata.release();

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
		uint32_t formid = Buffer::ReadUInt32(buffer, offset);
		std::string pluginname = Buffer::ReadString(buffer, offset);
		RE::TESForm* form = RE::TESDataHandler::GetSingleton()->LookupForm(formid, pluginname);
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
	std::vector<std::weak_ptr<ActorInfo>> acvec;
	// write map data to vector
	std::transform(
		actorinfoMap.begin(),
		actorinfoMap.end(),
		std::back_inserter(acvec),
		[](auto& kv) { return kv.second; });
	// iterate over the vector entries
	for (int i = 0; i < acvec.size(); i++) {
		LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if begin", i);
		if (std::shared_ptr<ActorInfo> acinfo = acvec[i].lock()) {
			if (acinfo->IsValid()) {
				LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if valid", i);
				if (acinfo->GetActor() != nullptr) {
					LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not null", i);
					if ((acinfo->GetFormFlags() & RE::TESForm::RecordFlags::kDeleted) == 0) {
						LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not deleted", i);
						if (acinfo->GetActor()->GetFormID() != 0) {
							LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if id not 0", i);
							if (acinfo->IsDead()) {
								LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor dead", i);
							} else {
								LOG1_3("{}[Data] [SaveActorInfoMap] {} Writing ActorInfo if actor not dead", i);
								LOG2_3("{}[Data] [SaveActorInfoMap] Writing {}, number {}", acinfo->GetName(), i);
								if (a_intfc->OpenRecord('ACIF', ActorInfo::GetVersion())) {
									LOG_3("{}[Data] [SaveActorInfoMap] \tget data size");
									// get entry length
									int length = acinfo->GetDataSize();
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
									if (acinfo->WriteData(buffer, 0) == false) {
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
								} else if (acinfo == nullptr) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: ActorInfo invalidated");
								} else if (acinfo->GetActor() == nullptr) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: actor invalidated");
								} else if (acinfo->GetActor()->GetFormID() == 0) {
									logwarn("[Data] [SaveActorInfoMap] failed to write ActorInfo record: formid invalid");
								} else if (acinfo->IsDead() == true) {
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
	std::shared_ptr<ActorInfo> acinfo = CreateActorInfoNew();
	if (acinfo->ReadData(buffer, 0, length) == false) {
		acfcounter++;
		logwarn("[Data] [ReadActorInfoMap] Couldn't read ActorInfo");
	} else if (acinfo->IsValid() == false) {
		acdcounter++;
		logwarn("[Data] [ReadActorInfoMap] actor invalid {}", acinfo->GetName());
	} else if ((acinfo->GetFormFlags() & RE::TESForm::RecordFlags::kDeleted) || acinfo->IsDead()) {
		acdcounter++;
		logwarn("[Data] [ReadActorInfoMap] actor dead or deleted {}", acinfo->GetName());
	} else {
		accounter++;
		RegisterActorInfo(acinfo);
		LOG1_3("{}[Data] [ReadActorInfoMap] read ActorInfo. actor: {}", Utility::PrintForm(acinfo));
	}
	delete[] buffer;
	// release lock
	lockdata.release();

	return size;
}

void Data::DeleteActorInfoMap()
{
	lockdata.acquire();
	validActors.clear();
	actorinfoMap.clear();
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
