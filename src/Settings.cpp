#include "Settings.h"
#include "Utility.h"
#include <fstream>
#include <iostream>
#include <type_traits>
#include <utility>
#include <string_view>
#include <chrono>
#include <set>
#include <time.h>
#include <random>
#include <tuple>
#include <vector>
#include <unordered_map>
#include "ActorManipulation.h"
#include "Distribution.h"
#include "AlchemyEffect.h"
#include "Data.h"
#include "Compatibility.h"

using Comp = Compatibility;
using ActorStrength = ActorStrength;
using ItemStrength = ItemStrength;
using ItemType = Settings::ItemType;

static std::mt19937 randi((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
/// <summary>
/// trims random numbers to 1 to RR
/// </summary>
static std::uniform_int_distribution<signed> randRR(1, RandomRange);
static std::uniform_int_distribution<signed> rand100(1, 100);

#pragma region Settings

void Settings::Interfaces::RequestAPIs()
{
	loginfo("");
	// get tmp api
	if (!tdm_api) {
		loginfo("Trying to get True Directional Movement API");
		tdm_api = reinterpret_cast<TDM_API::IVTDM1*>(TDM_API::RequestPluginAPI(TDM_API::InterfaceVersion::V1));
		if (tdm_api) {
			loginfo("Acquired True Directional Movement API");
		} else {
			loginfo("Failed to get True Directional Movement API");
		}
	}
}

void Settings::InitGameStuff()
{
	loginfo("Load Game Stuff");
	RE::TESDataHandler* datahandler = RE::TESDataHandler::GetSingleton();
	const RE::TESFile* file = nullptr;
	uint32_t index = 0;
	for (int i = 0; i <= 254; i++) {
		file = datahandler->LookupLoadedModByIndex((uint8_t)i);
		if (file) {
			pluginnames[i] = std::string(file->GetFilename());
			index = (uint32_t)i << 24;
			pluginNameMap.insert_or_assign(pluginnames[i], index);
			pluginIndexMap.insert_or_assign(index, pluginnames[i]);
		} else
			pluginnames[i] = "";
	}
	// 0xFF... is reserved for objects created by the game during runtime 
	pluginnames[255] = "runtime";

#ifndef BUILD_SKYRIMVR
	// skyrim vr does not support esl plugins
	for (int i = 0; i <= 4095; i++) {
		file = datahandler->LookupLoadedLightModByIndex((uint16_t)i);
		if (file) {
			pluginnames[256 + i] = std::string(file->GetFilename());
			index = 0xFE000000 | ((uint32_t)i << 12);
			pluginNameMap.insert_or_assign(pluginnames[256 + i], index);
			pluginIndexMap.insert_or_assign(index, pluginnames[256 + i]);
		} else
			pluginnames[256 + i] = "";
	}
#endif
	for (auto& [name, id] : pluginNameMap)
		loginfo("ID: {:3}, Name:{}", Utility::GetHex(id), name);

	Settings::Equip_LeftHand = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x13F43);
	Settings::Equip_RightHand = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x13F42);
	Settings::Equip_EitherHand = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x13F44);
	Settings::Equip_BothHands = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x13F45);
	Settings::Equip_Shield = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x141E8);
	Settings::Equip_Voice = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x25BEE);
	Settings::Equip_Potion = RE::TESForm::LookupByID<RE::BGSEquipSlot>(0x35698);

	loginfo("Finished");
}

std::set<RE::FormID> Settings::CalcRacesWithoutPotionSlot()
{
	LOG_1("");
	std::set<RE::FormID> ret;
	auto races = RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESRace>();
	LOG_1("found {} races.", races.size());
	for (RE::TESRace* race : races) {
		bool potionenabled = false;
		for (auto slot : race->equipSlots) {
			if (slot->GetFormID() == 0x35698)
				potionenabled = true;
		}
		if (potionenabled == false) {
			ret.insert(race->GetFormID());
			LOG_1("{} does not have potion slot and has been excluded.", Utility::PrintForm(race));
		}
	}
	LOG_1("end");
	return ret;
}

void Settings::LoadDistrConfig()
{
	// set to false, to avoid other funcions running stuff on our variables
	Distribution::initialised = false;

	// disable generic logging, if load logging is disabled
	if (Logging::EnableLoadLog == false)
		Logging::EnableGenericLogging = false;

	// reset custom items
	Distribution::ResetCustomItems();
	// reset loaded rules
	Distribution::ResetRules();

	std::vector<std::string> files;
	auto constexpr folder = R"(Data\SKSE\Plugins\)";
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		if (entry.exists() && !entry.path().empty() && entry.path().extension() == ".ini") {
			if (auto path = entry.path().string(); path.rfind("NUP_DIST") != std::string::npos) {
				files.push_back(path);
			}
		}
	}
	if (files.empty()) {
		loginfo("No Distribution files were found");
	}
	// init datahandler
	auto datahandler = RE::TESDataHandler::GetSingleton();

	// change order of files handled, so that files that include "default" are loaded first, so other rules may override them
	int defind = 0;
	for (int k = 0; k < files.size(); k++)
	{
		if (Utility::ToLower(files[k]).find("default") != std::string::npos)
		{
			std::string tmp = files[defind];
			files[defind] = files[k];
			files[k] = tmp;
			defind++;
		}
	}
	for (int k = 0; k < files.size(); k++)
	{
		loginfo("found Distribution configuration file: {}", files[k]);
	}

	// vector of splits, filename and line
	std::vector<std::tuple<std::vector<std::string>*, std::string, std::string>> attachments;
	std::vector<std::tuple<std::vector<std::string>*, std::string, std::string>> copyrules;

	const int chancearraysize = 5;
	char buffer[1024];

	// extract the rules from all files
	for (size_t i = 0; i < files.size(); i++) {
		std::string file = files[i];
		loginfo("Evaluating file: {}", file);
		//try 
		{
			std::ifstream infile(file);
			if (infile.is_open()) {
				std::string line;
				while (std::getline(infile, line)) {
					//std::string tmp = line.substr(0, line.length());
					line.copy(buffer, line.length() > 1023 ? 1023 : line.length(), 0);
					line.length() > 1023 ? buffer[1023] = 0 : buffer[line.length()] = 0;
					std::string tmp = std::string(buffer);
					// we read another line
					// check if its empty or with a comment
					if (line.empty())
						continue;
					// remove leading spaces and tabs
					while (line.length() > 0 && (line[0] == ' ' || line[0] == '\t')) {
						line = line.substr(1, line.length() - 1);
					}
					// check again
					if (line.length() == 0 || line[0] == ';')
						continue;
					// now begin the actual processing
					std::vector<std::string>* splits = new std::vector<std::string>();
					// split the string into parts
					size_t pos = line.find('|');
					while (pos != std::string::npos) {
						splits->push_back(line.substr(0, pos));
						line.erase(0, pos + 1);
						pos = line.find("|");
					}
					if (line.length() != 0)
						splits->push_back(line);
					int splitindex = 0;
					// check wether we actually have a rule
					if (splits->size() < 3) {  // why 3? Cause first two fields are RuleVersion and RuleType and we don't accept empty rules.
						logwarn("Not a rule. file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// check what rule version we have
					int ruleVersion = -1;
					try {
						ruleVersion = std::stoi(splits->at(splitindex));
						splitindex++;
					} catch (std::out_of_range&) {
						logwarn("out-of-range expection in field \"RuleVersion\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					} catch (std::invalid_argument&) {
						logwarn("invalid-argument expection in field \"RuleVersion\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// check what kind of rule we have
					int ruleType = -1;
					try {
						ruleType = std::stoi(splits->at(splitindex));
						splitindex++;
					} catch (std::out_of_range&) {
						logwarn("out-of-range expection in field \"RuleType\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					} catch (std::invalid_argument&) {
						logwarn("invalid-argument expection in field \"RuleType\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// now we can actually make differences for the different rule version and types
					switch (ruleVersion) {
					case 1:
						{
							switch (ruleType) {
							case 1:  // distribution rule
								{
									logcritical("Rule is no longer supported. file: {}, rule:\"{}\"", file, tmp);
									delete splits;
									continue;
								}
								break;
							case 2:  // distribution attachement
								{
									attachments.push_back({ splits, file, tmp });
								}
								// dont delete splits since we need it later
								break;
							case 3:  // declare boss
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										if (std::get<0>(items[i]) & Distribution::AssocType::kActor ||
											std::get<0>(items[i]) & Distribution::AssocType::kNPC ||
											std::get<0>(items[i]) & Distribution::AssocType::kFaction ||
											std::get<0>(items[i]) & Distribution::AssocType::kKeyword ||
											std::get<0>(items[i]) & Distribution::AssocType::kRace) {
											Distribution::_bosses.insert(std::get<1>(items[i]));
											LOGL_2("declared {} as boss.", Utility::GetHex(std::get<1>(items[i])));
										} else {
											LOGL_2("{} has the wrong FormType to be declared as a boss.", Utility::GetHex(std::get<1>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 4:  // exclude object
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kActor:
										case Distribution::AssocType::kNPC:
											Distribution::_excludedNPCs.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
										case Distribution::AssocType::kEffectSetting:
											Distribution::_excludedAssoc.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kItem:
											Distribution::_excludedItems.insert(std::get<1>(items[i]));
											break;
										}
										if (Logging::EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kActor ||
												std::get<0>(items[i]) & Distribution::AssocType::kNPC) {
												LOGL_2("excluded actor {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kFaction) {
												LOGL_2("excluded faction {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kKeyword) {
												LOGL_2("excluded keyword {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kItem) {
												LOGL_2("excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kRace) {
												LOGL_2("excluded race {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kEffectSetting) {
												LOGL_2("excluded magic effect {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
											else
											{
												LOGL_2("{} has the wrong FormType to be excluded from distribution. file: {}, rule:\"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											}
											EXCL("Exclusion:                {}", Utility::PrintForm(std::get<2>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 5:  // exclude baseline
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
											Distribution::_baselineExclusions.insert(std::get<1>(items[i]));
											break;
										}

										if (Logging::EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kFaction) {
												LOGL_2("excluded faction {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kKeyword) {
												LOGL_2("excluded keyword {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kRace) {
												LOGL_2("excluded race {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
											else
											{
												LOGL_2("{} has the wrong FormType for exclusion from base line distribution. file: {}, rule:\"{}\"", Utility::GetHex(std::get<1>(items[i])),file, tmp);
											}
											EXCL("Exclusion Baseline:       {}", Utility::PrintForm(std::get<2>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 6: // copy rule
								{
									copyrules.push_back({ splits, file, tmp });
								}
								break;
							case 7: // whitelist rule
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID,RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kItem:
											Distribution::_whitelistItems.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kNPC:
										case Distribution::AssocType::kActor:
										case Distribution::AssocType::kClass:
										case Distribution::AssocType::kCombatStyle:
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
											Distribution::_whitelistNPCs.insert(std::get<1>(items[i]));
											if (Logging::EnableLoadLog) {
												LOGL_2("whitelisted object {}.", Utility::GetHex(std::get<1>(items[i])));
											}
											break;
										default:
											LOGL_2("{} has the wrong FormType for whitelisting. file: {}, rule:\"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											break;
										}
										if (Logging::EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kItem) {
												LOGL_2("whitelisted item {}.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kRace) {
											}
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 8: // custom object distribution
								{
									if (splits->size() != 5) {
										logwarn("rule has wrong number of fields, expected 5. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string id = splits->at(splitindex);
									splitindex++;
									Distribution::CustomItemStorage* citems = new Distribution::CustomItemStorage();
									citems->id = id;
									// parse associated obj
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									bool errorcustom = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> assocobj = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									
									// parse items associated
									assoc = splits->at(splitindex);
									splitindex++;
									error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> associtm = Utility::ParseCustomObjects(assoc, errorcustom, file, tmp);
									RE::TESForm* tmpf = nullptr;
									RE::TESBoundObject* tmpb = nullptr;
									RE::AlchemyItem* alch = nullptr;
									// parse items first: if there are no items we don't need to do anything further
									for (int i = 0; i < associtm.size(); i++) {
										tmpf = nullptr;
										tmpb = nullptr;
										switch (std::get<0>(associtm[i])) {
										case Distribution::AssocType::kItem:
											{
												tmpf = RE::TESForm::LookupByID(std::get<1>(associtm[i]));
												if (tmpf) {
													tmpb = tmpf->As<RE::TESBoundObject>();
													alch = tmpf->As<RE::AlchemyItem>();
												}
												LOGL_3("Flag converted: {}", static_cast<uint64_t>(std::get<3>(associtm[i])));
												if (tmpb) {
													if (std::get<5>(associtm[i]))
														Distribution::_excludedItems.insert(std::get<2>(associtm[i]));
													switch (std::get<3>(associtm[i])) {
													case CustomItemFlag::Object:
														{
															LOGL_3("Path 1");
															CustomItem* cit = new CustomItem();
															cit->chance = std::get<4>(associtm[i]);
															cit->conditionsall = std::get<6>(associtm[i]);
															cit->conditionsany = std::get<7>(associtm[i]);
															cit->giveonce = std::get<8>(associtm[i]);
															cit->num = std::get<2>(associtm[i]);
															cit->object = tmpb;
															citems->items.push_back(cit);
														}
														break;
													case CustomItemFlag::DeathObject:
														{
															LOGL_3("Path 2");

															CustomItem* cit = new CustomItem();
															cit->chance = std::get<4>(associtm[i]);
															cit->conditionsall = std::get<6>(associtm[i]);
															cit->conditionsany = std::get<7>(associtm[i]);
															cit->giveonce = std::get<8>(associtm[i]);
															cit->num = std::get<2>(associtm[i]);
															cit->object = tmpb;
															citems->death.push_back(cit);
														}
														break;
													case CustomItemFlag::Food:
														LOGL_3("Path 3");
														if (alch) {
															CustomItemAlch* cit = new CustomItemAlch();
															cit->chance = std::get<4>(associtm[i]);
															cit->conditionsall = std::get<6>(associtm[i]);
															cit->conditionsany = std::get<7>(associtm[i]);
															cit->giveonce = std::get<8>(associtm[i]);
															cit->num = std::get<2>(associtm[i]);
															cit->object = alch;
															citems->food.push_back(cit);
														}
														break;
													case CustomItemFlag::Fortify:
														LOGL_3("Path 4");
														if (alch) {
															CustomItemAlch* cit = new CustomItemAlch();
															cit->chance = std::get<4>(associtm[i]);
															cit->conditionsall = std::get<6>(associtm[i]);
															cit->conditionsany = std::get<7>(associtm[i]);
															cit->giveonce = std::get<8>(associtm[i]);
															cit->num = std::get<2>(associtm[i]);
															cit->object = alch;
															citems->fortify.push_back(cit);
														}
														break;
													case CustomItemFlag::Poison:
														LOGL_3("Path 5");
														if (alch) {
															CustomItemAlch* cit = new CustomItemAlch();
															cit->chance = std::get<4>(associtm[i]);
															cit->conditionsall = std::get<6>(associtm[i]);
															cit->conditionsany = std::get<7>(associtm[i]);
															cit->giveonce = std::get<8>(associtm[i]);
															cit->num = std::get<2>(associtm[i]);
															cit->object = alch;
															citems->poisons.push_back(cit);
														}
														break;
													case CustomItemFlag::Potion:
														LOGL_3("Path 6");
														if (alch) {
															CustomItemAlch* cit = new CustomItemAlch();
															cit->chance = std::get<4>(associtm[i]);
															cit->conditionsall = std::get<6>(associtm[i]);
															cit->conditionsany = std::get<7>(associtm[i]);
															cit->giveonce = std::get<8>(associtm[i]);
															cit->num = std::get<2>(associtm[i]);
															cit->object = alch;
															citems->potions.push_back(cit);
														}
														break;
													}
												} else {
													LOGL_2("custom rule for item {} cannot be applied, due to the item not being an TESBoundObject.", Utility::GetHex(std::get<1>(associtm[i])));
												}
											}
											break;
										}
									}
									if (citems->items.size() == 0 && 
										citems->death.size() == 0 &&
										citems->food.size() == 0 &&
										citems->fortify.size() == 0 &&
										citems->poisons.size() == 0 &&
										citems->potions.size() == 0)
									{
										logwarn("rule does not contain any items. file: {}, rule:\"{}\"", file, tmp);
										delete citems;
										continue;
									}

									int cx = 0;
									// now parse associations
									for (int i = 0; i < assocobj.size(); i++) {
										switch (std::get<0>(assocobj[i])) {
										case Distribution::AssocType::kActor:
										case Distribution::AssocType::kNPC:
										case Distribution::AssocType::kClass:
										case Distribution::AssocType::kCombatStyle:
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
											citems->assocobjects.insert(std::get<1>(assocobj[i]));
											auto iter = Distribution::_customItems.find(std::get<1>(assocobj[i]));
											if (iter != Distribution::_customItems.end())
											{
												auto vec = iter->second;
												vec.push_back(citems);
												Distribution::_customItems.insert_or_assign(std::get<1>(assocobj[i]), vec);
												cx++;
											} else {
												std::vector<Distribution::CustomItemStorage*> vec = { citems };
												Distribution::_customItems.insert_or_assign(std::get<1>(assocobj[i]), vec);
												cx++;
											}
										}
										if (Logging::EnableLoadLog) {
											if (std::get<0>(assocobj[i]) & Distribution::AssocType::kKeyword) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kRace) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kFaction) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kCombatStyle) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kClass) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kActor || 
													   std::get<0>(assocobj[i]) & Distribution::AssocType::kNPC) {
											}
										}
										LOGL_2("attached custom rule to specific objects");
									}
									if (cx == 0 && total == 0) {
										auto iter = Distribution::_customItems.find(0x0);
										if (iter != Distribution::_customItems.end()) {
											std::vector<Distribution::CustomItemStorage*> vec = iter->second;
											vec.push_back(citems);
											Distribution::_customItems.insert_or_assign(0x0, vec);
											cx++;
										} else {
											std::vector<Distribution::CustomItemStorage*> vec = { citems };
											Distribution::_customItems.insert_or_assign(0x0, vec);
											cx++;
										}
										LOGL_2("attached custom rule to everything");
									}

									// since we are done delete splits
									delete splits;
								}
								break;
							case 9:  // exclude plugin
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string plugin = splits->at(splitindex);
									splitindex++;
									uint32_t index = Utility::Mods::GetPluginIndex(plugin);

									if (index != 0x1) {
										// index is a normal mod
										Distribution::_excludedPlugins.insert(index);
										LOGL_2("Rule 9 excluded plugin {} with index {}", plugin, Utility::GetHex(index));
										EXCL("Exclusion Plugin:         {}", plugin);
									} else {
										LOGL_2("Rule 9 cannot exclude plugin {}. It is either not loaded or not present", plugin);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 10: // set item strength
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string strength = splits->at(splitindex);
									splitindex++;
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									ItemStrength str = ItemStrength::kWeak;
									// arse item strength
									try {
										str = static_cast<ItemStrength>(std::stoi(strength));
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"ItemStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"ItemStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									catch (std::exception&) {
										logwarn("generic expection in field \"ItemStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kItem:
											Distribution::_itemStrengthMap.insert_or_assign(std::get<1>(items[i]), str);
											LOGL_2("set item strength {}.", Utility::GetHex(std::get<1>(items[i])));
											break;
										default:
											LOGL_2("rule 10 is not applicable to object {}. file: {}, rule:\"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											break;
										}
									}

									// since we are done delete splits
									delete splits;
								}
								break;
							case 11: // adjust actor strength
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string strength = splits->at(splitindex);
									splitindex++;
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									int str = 0;
									// arse item strength
									try {
										str = std::stoi(strength);
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"RelativeActorStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"RelativeActorStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									} catch (std::exception&) {
										logwarn("generic expection in field \"RelativeActorStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									if (str != 0) {
										for (int i = 0; i < items.size(); i++) {
											switch (std::get<0>(items[i])) {
											case Distribution::AssocType::kActor:
											case Distribution::AssocType::kNPC:
											case Distribution::AssocType::kFaction:
											case Distribution::AssocType::kKeyword:
											case Distribution::AssocType::kRace:
												Distribution::_actorStrengthMap.insert_or_assign(std::get<1>(items[i]), str);
												LOGL_2("set relative actor strength {}.", Utility::GetHex(std::get<1>(items[i])));
												break;
											default:
												LOGL_2("rule 11 is not applicable to object {}. file: {}, rule:\"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
												break;
											}
										}
									}

									// since we are done delete splits
									delete splits;
								}
								break;
							case 12: // whitelist plugin
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string pluginname = splits->at(splitindex);
									splitindex++;
									bool error = false;
									auto forms = Utility::Mods::GetFormsInPlugin<RE::AlchemyItem>(pluginname);
									for (int i = 0; i < forms.size(); i++) {
										Distribution::_whitelistItems.insert(forms[i]->GetFormID());
										LOGL_2("whitelisted item. id: {}, name: {}, plugin: {}.", Utility::GetHex(forms[i]->GetFormID()), forms[i]->GetName(), pluginname);
									}
									if (forms.size()) {
										LOGL_2("Plugin {} has no forms to whitelist.", pluginname);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 13: // follower detection
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++)
									{
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kFaction:
											Distribution::_followerFactions.insert(std::get<1>(items[i]));
											LOGL_2("Whitelisted follower faction {}", std::get<1>(items[i]));
											break;
										default:
											LOGL_2("{} has the wrong FormType to be whitelisted as follower faction. file: {}, rule: \"{}\"", std::get<1>(items[i]), file, tmp);
										}
									}
									delete splits;
								}
								break;
							case 14: // poison dosage item based
								{
									// version, type, enforce, assocobjects (items), dosage or setting 
									if (splits->size() != 5) {
										logwarn("rule has wrong number of fields, expected 5. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string senforce = splits->at(splitindex);
									splitindex++;
									bool enforce = false;
									try {
										if (std::stol(senforce) == 1)
											enforce = true;
									}
									catch (std::exception&) {
										enforce = false;
									}

									std::string assoc = splits->at(splitindex);
									splitindex++;
									std::string sdosage = splits->at(splitindex);
									splitindex++;
									bool setting = false;
									int dosage = 0;
									if (sdosage == "setting") {
										setting = true;
										dosage = 1;
									} else {
										try {
											dosage = std::stoi(sdosage);
										}
										catch (std::exception&) {
											logwarn("expection in field \"Dosage\". file: {}, rule:\"{}\"", file, tmp);
											delete splits;
											continue;
										}
									}
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										if (std::get<0>(items[i]) == Distribution::AssocType::kItem) {
											Distribution::_dosageItemMap.insert_or_assign(std::get<1>(items[i]), std::tuple<bool, bool, int>{ enforce, setting, dosage });
											LOGL_2("Set dosage for item: {}.", std::get<1>(items[i]));
										}
										else
										{
											LOGL_2("{} has the wrong FormType to set a dosage. file: {}, rule: \"{}\"", std::get<1>(items[i]), file, tmp);
										}
									}
									delete splits;
								}
								break;
							case 15: // poison dosage effect based
								{
									// version, type, enforce, effect, dosage or setting
									if (splits->size() != 5) {
										logwarn("rule has wrong number of fields, expected 5. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string senforce = splits->at(splitindex);
									splitindex++;
									bool enforce = false;
									try {
										if (std::stol(senforce) == 1)
											enforce = true;
									} catch (std::exception&) {
										enforce = false;
									}

									std::string seffects = splits->at(splitindex);
									splitindex++;
									std::string sdosage = splits->at(splitindex);
									splitindex++;
									bool setting = false;
									int dosage = 0;
									if (sdosage == "setting") {
										setting = true;
										dosage = 1;
									} else {
										try {
											dosage = std::stoi(sdosage);
										} catch (std::exception&) {
											logwarn("expection in field \"Dosage\". file: {}, rule:\"{}\"", file, tmp);
											delete splits;
											continue;
										}
									}
									bool error = false;
									std::vector<std::tuple<AlchemicEffect, float, int>> effects = Utility::ParseAlchemyEffects(seffects, error);
									for (int i = 0; i < effects.size(); i++) {
										AlchemicEffect effect = std::get<0>(effects[i]);
										if (effect != AlchemicEffect::kNone) {
											Distribution::_dosageEffectMap.insert_or_assign(effect, std::tuple<bool, bool, int>{ enforce, setting, dosage });
											LOGL_2("Set dosage for effect: {}.", effect.string());
										}
										else
										{
											LOGL_2("Effect {} is empty. file: {}, rule: \"{}\"", effect.string(), file, tmp);
										}
									}
									delete splits;
								}
								break;
							case 16:  // exclude effect
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string effect = splits->at(splitindex);
									splitindex++;
									AlchemicEffect e = 0;
									try {
										e = effect;
									} catch (std::exception&) {
										logwarn("expection in field \"Effect\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									if (e != AlchemicEffect::kNone) {
										Distribution::_excludedEffects.insert(e);
										EXCL("Exclusion Effect:         {}", e.string());
										LOGL_2("Exluded Effect:	{}.", e.string());
									} else {
										LOGL_2("Effect {} is empty. file: {}, rule: \"{}\"", e.string(), file, tmp);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 17:  // exclude plugin NPCs
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string plugin = splits->at(splitindex);
									splitindex++;
									uint32_t plugindex = Utility::Mods::GetPluginIndex(plugin);
									if (plugindex != 0x1) {
										// valid plugin index
										Distribution::_excludedPlugins_NPCs.insert(plugindex);
										LOGL_2("Rule 17 excluded plugin {}.", plugin);
										EXCL("Exclusion Plugin NPCs:    {}", plugin);
									} else {
										LOGL_2("Rule 17 cannot exclude plugin {}. It is either not loaded or not present", plugin);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 18:  // whitelist plugin NPCs
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string plugin = splits->at(splitindex);
									splitindex++;
									uint32_t plugindex = Utility::Mods::GetPluginIndex(plugin);
									if (plugindex != 0x1) {
										// valid plugin index
										Distribution::_whitelistNPCsPlugin.insert(plugindex);
										LOGL_2("Rule 18 whitelisted plugin {}.", plugin);
									} else {
										LOGL_2("Rule 18 cannot whitelist plugin {}. It is either not loaded or not present", plugin);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 19:  // mark item as alcoholic
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kItem:
											Distribution::_alcohol.insert(std::get<1>(items[i]));
											LOGL_2("marked {} as alcoholic", Utility::GetHex(std::get<1>(items[i])));
											break;
										default:
											LOGL_2("{} has the wrong FormType to be marked alcoholic. file: {}, rule: \"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											break;
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 20: // define alchemy effect
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									std::string effect = splits->at(splitindex);
									splitindex++;
									AlchemicEffect e = 0;
									try {
										e = effect;
									} catch (std::exception&) {
										logwarn("expection in field \"Effect\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									if (e != AlchemicEffect::kNone) {
										bool error = false;
										int total = 0;
										auto items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
										for (int i = 0; i < items.size(); i++) {
											switch (std::get<0>(items[i])) {
											case Distribution::AssocType::kEffectSetting:
												Distribution::_magicEffectAlchMap.insert_or_assign(std::get<1>(items[i]), e);
												LOGL_2("fixed {} to effect {}", Utility::GetHex(std::get<1>(items[i])), Utility::ToString(e));
												break;
											default:
												LOGL_2("cannot fix {} to effect {}: Wrong FormType. file: {}, rule: \"{}\"", Utility::GetHex(std::get<1>(items[i])), Utility::ToString(e), file, tmp);
												break;
											}
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 21:  // disallow player usage of items
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kEffectSetting:
										case Distribution::AssocType::kItem:
											Distribution::_excludedItemsPlayer.insert(std::get<1>(items[i]));
											LOGL_2("excluded {} for the player", Utility::GetHex(std::get<1>(items[i])));
											break;
										default:
											LOGL_2("cannot exclude {} for the player: Wrong FormType. file: {}, rule: \"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											break;
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 22: // exclude item from distribution only
								{
									if (splits->size() != 3) {
										logwarn("rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kItem:
											Distribution::_excludedDistrItems.insert(std::get<1>(items[i]));
											LOGL_2("excluded {} fro distribution only", Utility::GetHex(std::get<1>(items[i])));
											break;
										default:
											LOGL_2("cannot exclude {} from distribution: Wrong FormType. file: {}, rule: \"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											break;
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 30: // exclude if plugin is loaded
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string plugin = splits->at(splitindex);
									splitindex++;
									if (RE::TESDataHandler::GetSingleton()->LookupLoadedModByName(plugin) == nullptr && RE::TESDataHandler::GetSingleton()->LookupLoadedLightModByName(plugin) == nullptr) {
										logwarn("File is not loaded: {}. file: {}, rule:\"{}\"", plugin, file, tmp);
										delete splits;
										continue;
									}

									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									int total = 0;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> items = Utility::ParseAssocObjects(assoc, error, file, tmp, total);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kActor:
										case Distribution::AssocType::kNPC:
											Distribution::_excludedNPCs.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
										case Distribution::AssocType::kEffectSetting:
											Distribution::_excludedAssoc.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kItem:
											Distribution::_excludedItems.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kCombatStyle:
										case Distribution::AssocType::kClass:
											break;
										}
										if (Logging::EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kActor ||
												std::get<0>(items[i]) & Distribution::AssocType::kNPC) {
												LOGL_2("excluded actor {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kFaction) {
												LOGL_2("excluded faction {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kKeyword) {
												LOGL_2("excluded keyword {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kItem) {
												LOGL_2("excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kRace) {
												LOGL_2("excluded race {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kEffectSetting) {
												LOGL_2("excluded magic effect {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else {
												LOGL_2("{} has the wrong FormType to be excluded from distribution. file: {}, rule:\"{}\"", Utility::GetHex(std::get<1>(items[i])), file, tmp);
											}
											EXCL("Exclusion:                {}", Utility::PrintForm(std::get<2>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 31:  // excludeeffect if plugin is loaded
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string plugin = splits->at(splitindex);
									splitindex++;
									if (RE::TESDataHandler::GetSingleton()->LookupLoadedModByName(plugin) == nullptr && RE::TESDataHandler::GetSingleton()->LookupLoadedLightModByName(plugin) == nullptr)
									{
										logwarn("File is not loaded: {}. file: {}, rule:\"{}\"", plugin, file, tmp);
										delete splits;
										continue;
									}

									std::string effect = splits->at(splitindex);
									splitindex++;
									AlchemicEffect e = 0;
									try {
										e = effect;
									} catch (std::exception&) {
										logwarn("expection in field \"Effect\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									if (e != AlchemicEffect::kNone) {
										Distribution::_excludedEffects.insert(e);
										EXCL("Exclusion Effect:         {}", e.string());
										LOGL_2("Exluded Effect:	{}.", e.string());
									} else {
										LOGL_2("Effect {} is empty. file: {}, rule: \"{}\"", e.string(), file, tmp);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							default:
								logwarn("Rule type does not exist. file: {}, rule:\"{}\"", file, tmp);
								delete splits;
								break;
							}
						}
						break;
					case 2:
						{
						switch (ruleType) {
							case 1:  // distribution rule
								{
									if (splits->size() != 33) {
										logwarn("rule has wrong number of fields, expected 33. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the rulename, so we just set it
									Distribution::Rule* rule = nullptr;
									bool existing = false;
									for (auto rl : Distribution::_rules) {
										//loginfo("Compare {} || {} || {} || {}", Utility::ToLower(rl->ruleName), Utility::ToLower(splits->at(splitindex)), rl->ruleName.length(), splits->at(splitindex).length())
										if (Utility::ToLower(rl->ruleName).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && rl->ruleName.length() == splits->at(splitindex).length()) {
											loginfo("Overriding rule: {}. file: {}", rl->ruleName, file);
											rule = rl;
											existing = true;
											break;
										}
									}
									if (existing == false)
										rule = new Distribution::Rule();
									rule->ruleVersion = ruleVersion;
									rule->ruleType = ruleType;
									rule->ruleName = splits->at(splitindex);
									LOGL_2("loading rule: {}", rule->ruleName);
									splitindex++;
									// now come the rule priority
									rule->rulePriority = -1;
									try {
										rule->rulePriority = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes maxPotions
									int maxPotions = -1;
									try {
										maxPotions = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion1Chance
									std::vector<int> potion1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (potion1Chance.size() != chancearraysize) {
										logwarn("field \"Potion1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion2Chance
									std::vector<int> potion2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (potion2Chance.size() != chancearraysize) {
										logwarn("field \"Potion2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion3Chance
									std::vector<int> potion3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (potion3Chance.size() != chancearraysize) {
										logwarn("field \"Potion3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion4Chance
									std::vector<int> potion4Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (potion4Chance.size() != chancearraysize) {
										logwarn("field \"Potion4Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PotionAddChance
									std::vector<int> potionAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (potionAdditionalChance.size() != chancearraysize) {
										logwarn("field \"PotionAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PotionsTierAdjust
									int potionTierAdjust = -1;
									try {
										potionTierAdjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes maxFortify
									int maxFortify = -1;
									try {
										maxFortify = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"MaxFortify\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"MaxFortify\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify1Chance
									std::vector<int> fortify1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (fortify1Chance.size() != chancearraysize) {
										logwarn("field \"Fortify1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify2Chance
									std::vector<int> fortify2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (fortify2Chance.size() != chancearraysize) {
										logwarn("field \"Fortify2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify3Chance
									std::vector<int> fortify3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (fortify3Chance.size() != chancearraysize) {
										logwarn("field \"Fortify3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify4Chance
									std::vector<int> fortify4Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (fortify4Chance.size() != chancearraysize) {
										logwarn("field \"Fortify4Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes FortifyAddChance
									std::vector<int> fortifyAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (fortifyAdditionalChance.size() != chancearraysize) {
										logwarn("field \"FortifyAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes FortifyTierAdjust
									int fortifyTierAdjust = -1;
									try {
										fortifyTierAdjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"FortifyTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"FortifyTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes maxPoisons
									int maxPoisons = -1;
									try {
										maxPoisons = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison1Chance
									std::vector<int> poison1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (poison1Chance.size() != chancearraysize) {
										logwarn("field \"Poison1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison2Chance
									std::vector<int> poison2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (poison2Chance.size() != chancearraysize) {
										logwarn("field \"Poison2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison3Chance
									std::vector<int> poison3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (poison3Chance.size() != chancearraysize) {
										logwarn("field \"Poison3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison4Chance
									std::vector<int> poison4Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (poison4Chance.size() != chancearraysize) {
										logwarn("field \"Poison4Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PoisonAddChance
									std::vector<int> poisonAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (poisonAdditionalChance.size() != chancearraysize) {
										logwarn("field \"PoisonAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PoisonsTierAdjust
									int poisonTierAdjust = -1;
									try {
										poisonTierAdjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									// now comes FoodChance
									std::vector<int> foodChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (foodChance.size() != chancearraysize) {
										logwarn("field \"FoodChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									// get strings for the properties
									std::string assocObjects = splits->at(splitindex);
									splitindex++;
									std::string potionProperties = splits->at(splitindex);
									splitindex++;
									std::string fortifyproperties = splits->at(splitindex);
									splitindex++;
									std::string poisonProperties = splits->at(splitindex);
									splitindex++;
									std::string foodProperties = splits->at(splitindex);
									splitindex++;

									if (splits->at(splitindex) == "1")
										rule->allowMixed = true;
									else
										rule->allowMixed = false;
									splitindex++;
									if (splits->at(splitindex) == "1")
										rule->styleScaling = true;
									else
										rule->styleScaling = false;
									splitindex++;

									bool error = false;
									int total = 0;

									// parse the associated objects
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> objects = Utility::ParseAssocObjects(assocObjects, error, file, tmp, total);

									// parse the item properties
									rule->potions = new Distribution::EffCategoryPreset();
									rule->potions->name = rule->ruleName + "_potions_catpreset";
									Distribution::_internEffectCategoryPresets.insert_or_assign(rule->potions->name, rule->potions);
									rule->poisons = new Distribution::EffCategoryPreset();
									rule->poisons->name = rule->ruleName + "_poisons_catpreset";
									Distribution::_internEffectCategoryPresets.insert_or_assign(rule->poisons->name, rule->poisons);
									rule->fortify = new Distribution::EffCategoryPreset();
									rule->fortify->name = rule->ruleName + "_fortify_catpreset";
									Distribution::_internEffectCategoryPresets.insert_or_assign(rule->fortify->name, rule->fortify);
									rule->food = new Distribution::EffCategoryPreset();
									rule->food->name = rule->ruleName + "_food_catpreset";
									Distribution::_internEffectCategoryPresets.insert_or_assign(rule->food->name, rule->food);
									
									rule->potionEffects = new Distribution::EffectPreset();
									rule->potionEffects->name = rule->ruleName + "_potion_effectspreset";
									Distribution::_internEffectPresets.insert_or_assign(rule->potionEffects->name, rule->potionEffects);
									rule->poisonEffects = new Distribution::EffectPreset();
									rule->poisonEffects->name = rule->ruleName + "_poison_effectspreset";
									Distribution::_internEffectPresets.insert_or_assign(rule->poisonEffects->name, rule->poisonEffects);
									rule->fortifyEffects = new Distribution::EffectPreset();
									rule->fortifyEffects->name = rule->ruleName + "_fortify_effectspreset";
									Distribution::_internEffectPresets.insert_or_assign(rule->fortifyEffects->name, rule->fortifyEffects);
									rule->foodEffects = new Distribution::EffectPreset();
									rule->foodEffects->name = rule->ruleName + "_food_effectspreset";
									Distribution::_internEffectPresets.insert_or_assign(rule->foodEffects->name, rule->foodEffects);


									auto meanfalloff = [](std::vector<int> vec1, std::vector<int> vec2, std::vector<int> vec3, std::vector<int> vec4, int num) {
										float mean = 0;
										int mac = 0;
										if (vec1[num] != 0) {
											mean += (float)vec2[num] / (float)vec1[num];
											mac++;
										}
										if (vec2[num] != 0) {
											mean += (float)vec3[num] / (float)vec2[num];
											mac++;
										}
										if (vec3[num] != 0) {
											mean += (float)vec4[num] / (float)vec3[num];
											mac++;
										}
										if (mac != 0)
											mean = mean / (float)mac;
										return mean;
									};
									auto meanfallofflin = [](std::vector<int> vec1, std::vector<int> vec2, std::vector<int> vec3, std::vector<int> vec4, int num) {
										float mean = 0;
										mean += vec1[num] - vec2[num];
										mean += vec2[num] - vec3[num];
										mean += vec3[num] - vec4[num];
										mean = mean / 3;
										loginfo("mean: {}", mean);
										return mean;
									};
									auto addcats = [](Distribution::EffCategoryPreset* preset) {
										for (int i = 0; i < 5; i++)
										{
											preset->cats[i] = new Distribution::EffCategory();
											preset->cats[i]->name = preset->name + "_" + std::to_string(i);
											Distribution::_internEffectCategories.insert_or_assign(preset->cats[i]->name, preset->cats[i]);
										}
									};
									auto setcat = [](Distribution::EffCategoryPreset* preset, int num, int base, float falloff, int max, Distribution::FalloffFunction fall) {
										preset->cats[num]->baseChance = base;
										preset->cats[num]->falloff = falloff;
										preset->cats[num]->max = max;
										preset->cats[num]->falloffFunc = fall;
									};
									addcats(rule->potions);
									addcats(rule->poisons);
									addcats(rule->fortify);
									addcats(rule->food);
									rule->potions->tieradjust = potionTierAdjust;
									rule->poisons->tieradjust = poisonTierAdjust;
									rule->fortify->tieradjust = fortifyTierAdjust;
									rule->food->tieradjust = 0;

									setcat(rule->potions, 0, potion1Chance[0], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 0), maxPotions, Distribution::FalloffFunction::Exponential);
									setcat(rule->potions, 1, potion1Chance[1], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 1), maxPotions, Distribution::FalloffFunction::Exponential);
									setcat(rule->potions, 2, potion1Chance[2], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 2), maxPotions, Distribution::FalloffFunction::Exponential);
									setcat(rule->potions, 3, potion1Chance[3], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 3), maxPotions, Distribution::FalloffFunction::Exponential);
									setcat(rule->potions, 4, potion1Chance[4], meanfalloff(potion1Chance, potion2Chance, potion3Chance, potion4Chance, 4), maxPotions, Distribution::FalloffFunction::Exponential);

									setcat(rule->poisons, 0, poison1Chance[0], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 0), maxPoisons, Distribution::FalloffFunction::Exponential);
									setcat(rule->poisons, 1, poison1Chance[1], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 1), maxPoisons, Distribution::FalloffFunction::Exponential);
									setcat(rule->poisons, 2, poison1Chance[2], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 2), maxPoisons, Distribution::FalloffFunction::Exponential);
									setcat(rule->poisons, 3, poison1Chance[3], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 3), maxPoisons, Distribution::FalloffFunction::Exponential);
									setcat(rule->poisons, 4, poison1Chance[4], meanfalloff(poison1Chance, poison2Chance, poison3Chance, poison4Chance, 4), maxPoisons, Distribution::FalloffFunction::Exponential);

									setcat(rule->fortify, 0, fortify1Chance[0], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 0), maxFortify, Distribution::FalloffFunction::Exponential);
									setcat(rule->fortify, 1, fortify1Chance[1], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 1), maxFortify, Distribution::FalloffFunction::Exponential);
									setcat(rule->fortify, 2, fortify1Chance[2], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 2), maxFortify, Distribution::FalloffFunction::Exponential);
									setcat(rule->fortify, 3, fortify1Chance[3], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 3), maxFortify, Distribution::FalloffFunction::Exponential);
									setcat(rule->fortify, 4, fortify1Chance[4], meanfalloff(fortify1Chance, fortify2Chance, fortify3Chance, fortify4Chance, 4), maxFortify, Distribution::FalloffFunction::Exponential);

									setcat(rule->food, 0, foodChance[0], 0, 1, Distribution::FalloffFunction::Exponential);
									setcat(rule->food, 1, foodChance[1], 0, 1, Distribution::FalloffFunction::Exponential);
									setcat(rule->food, 2, foodChance[2], 0, 1, Distribution::FalloffFunction::Exponential);
									setcat(rule->food, 3, foodChance[3], 0, 1, Distribution::FalloffFunction::Exponential);
									setcat(rule->food, 4, foodChance[4], 0, 1, Distribution::FalloffFunction::Exponential);

									
									std::vector<std::tuple<AlchemicEffect, float, int>> potioneffects = Utility::ParseAlchemyEffects(potionProperties, error);

									auto potionDistr = Utility::GetDistribution(potioneffects, RandomRange);
									LOGL_4("PotionDistr:\t{}", Utility::PrintDistribution(potionDistr));
									auto potionDistrChance = Utility::GetDistribution(potioneffects, RandomRange, true);
									auto potionEffectMap = Utility::UnifyEffectMap(potioneffects);
									for (auto [alch, weight] : potionEffectMap) {
										Distribution::Effect eff;
										eff.effect = alch;
										eff.max = 0;
										eff.current = 0;
										eff.weight = 1000 * weight;
										rule->potionEffects->effects.insert_or_assign(alch, eff);
									}
									rule->potionEffects->standardDistr = Distribution::GetEffectDistribution(rule->potionEffects->effects);
									rule->potionEffects->validEffects = Distribution::SumAlchemyEffects(rule->potionEffects->standardDistr);
									LOGL_4("PotionEffMap:\t{}", Utility::PrintEffectMap(potionEffectMap));
									LOGL_2("rule {} contains {} potion effects", rule->ruleName, potionDistr.size());
									std::vector<std::tuple<AlchemicEffect, float, int>> poisoneffects = Utility::ParseAlchemyEffects(poisonProperties, error);
									auto poisonDistr = Utility::GetDistribution(poisoneffects, RandomRange);
									LOGL_4("PoisonDistr:\t{}", Utility::PrintDistribution(poisonDistr));
									auto poisonDistrChance = Utility::GetDistribution(poisoneffects, RandomRange, true);
									auto poisonEffectMap = Utility::UnifyEffectMap(poisoneffects);
									for (auto [alch, weight] : poisonEffectMap) {
										Distribution::Effect eff;
										eff.effect = alch;
										eff.max = 0;
										eff.current = 0;
										eff.weight = 1000 * weight;
										rule->poisonEffects->effects.insert_or_assign(alch, eff);
									}
									rule->poisonEffects->standardDistr = Distribution::GetEffectDistribution(rule->poisonEffects->effects);
									rule->poisonEffects->validEffects = Distribution::SumAlchemyEffects(rule->poisonEffects->standardDistr);
									LOGL_4("PoisonEffMap:\t{}", Utility::PrintEffectMap(poisonEffectMap));
									LOGL_2("rule {} contains {} poison effects", rule->ruleName, poisonDistr.size());
									std::vector<std::tuple<AlchemicEffect, float, int>> fortifyeffects = Utility::ParseAlchemyEffects(fortifyproperties, error);
									auto fortifyDistr = Utility::GetDistribution(fortifyeffects, RandomRange);
									LOGL_4("FortifyDistr:\t{}", Utility::PrintDistribution(fortifyDistr));
									auto fortifyDistrChance = Utility::GetDistribution(fortifyeffects, RandomRange, true);
									auto fortifyEffectMap = Utility::UnifyEffectMap(fortifyeffects);
									for (auto [alch, weight] : fortifyEffectMap) {
										Distribution::Effect eff;
										eff.effect = alch;
										eff.max = 0;
										eff.current = 0;
										eff.weight = 1000 * weight;
										rule->fortifyEffects->effects.insert_or_assign(alch, eff);
									}
									rule->fortifyEffects->standardDistr = Distribution::GetEffectDistribution(rule->fortifyEffects->effects);
									rule->fortifyEffects->validEffects = Distribution::SumAlchemyEffects(rule->fortifyEffects->standardDistr);
									LOGL_4("FortifyEffMap:\t{}", Utility::PrintEffectMap(fortifyEffectMap));
									LOGL_2("rule {} contains {} fortify potion effects", rule->ruleName, fortifyDistr.size());
									std::vector<std::tuple<AlchemicEffect, float, int>> foodeffects = Utility::ParseAlchemyEffects(foodProperties, error);
									auto foodDistr = Utility::GetDistribution(foodeffects, RandomRange);
									LOGL_4("FoodDistr:\t{}", Utility::PrintDistribution(foodDistr));
									auto foodDistrChance = Utility::GetDistribution(foodeffects, RandomRange, true);
									auto foodEffectMap = Utility::UnifyEffectMap(foodeffects);
									for (auto [alch, weight] : foodEffectMap) {
										Distribution::Effect eff;
										eff.effect = alch;
										eff.max = 0;
										eff.current = 0;
										eff.weight = 1000 * weight;
										rule->foodEffects->effects.insert_or_assign(alch, eff);
									}
									rule->foodEffects->standardDistr = Distribution::GetEffectDistribution(rule->foodEffects->effects);
									rule->foodEffects->validEffects = Distribution::SumAlchemyEffects(rule->foodEffects->standardDistr);
									LOGL_4("FoodEffMap:\t{}", Utility::PrintEffectMap(foodEffectMap));
									LOGL_2("rule {} contains {} food effects", rule->ruleName, foodDistr.size());

									std::pair<int, Distribution::Rule*> tmptuple = { rule->rulePriority, rule };

									// assign rules to search parameters
									LOGL_2("rule {} contains {} associated objects", rule->ruleName, objects.size());
									for (int i = 0; i < objects.size(); i++) {
										switch (std::get<0>(objects[i])) {
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kCombatStyle:
										case Distribution::AssocType::kClass:
										case Distribution::AssocType::kRace:
										case Distribution::AssocType::kKeyword:
											if (auto item = Distribution::_assocMap.find(std::get<1>(objects[i])); item != Distribution::_assocMap.end()) {
												if (std::get<1>(item->second)->rulePriority < rule->rulePriority)
													Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
											} else {
												Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
											}
											break;
										case Distribution::AssocType::kNPC:
										case Distribution::AssocType::kActor:
											if (auto item = Distribution::_npcMap.find(std::get<1>(objects[i])); item != Distribution::_npcMap.end()) {
												if (item->second->rulePriority < rule->rulePriority)
													Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
											} else {
												Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
											}
											break;
										}
									}
									// add rule to the list of rules and we are finished! probably.
									/* if (Distribution::_rules.contains(rule)) {
										auto ritr = Distribution::_rules.find(rule);
										if (ritr != Distribution::_rules.end()) {
											// get old rule
											auto rtmp = *ritr;
											// erase old rule
											Distribution::_rules.erase(rule);
											// delete old rule
											delete rtmp;
											// insert new rule
											Distribution::_rules.insert(rule);
										} else {
											logcritical("Critical error in code, please report to author. file: {}, rule:\"{}\"", file, tmp);
											delete rule;
											delete splits;
											continue;
										}
									} else*/
										Distribution::_rules.insert(rule);

									if (rule->ruleName == DefaultRuleName && (Distribution::defaultRule == nullptr ||
																				 rule->rulePriority > Distribution::defaultRule->rulePriority))
										Distribution::defaultRule = rule;
									delete splits;
									LOGL_2("rule {} successfully loaded.", rule->ruleName);
								}
								break;
							default:
								logwarn("Rule type does not exist for ruleversion 2. file: {}, rule:\"{}\"", file, tmp);
								delete splits;
								break;
							}
						}
						break;
					case 3:
						{
							switch (ruleType) {
							case 1:  // rule
								{
									if (splits->size() != 14) {
										logwarn("rule has wrong number of fields, expected 14. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the rulename, so we just set it
									Distribution::Rule* rule = nullptr;
									bool existing = false;
									for (auto rl : Distribution::_rules) {
										//loginfo("Compare {} || {} || {} || {}", Utility::ToLower(rl->ruleName), Utility::ToLower(splits->at(splitindex)), rl->ruleName.length(), splits->at(splitindex).length()) if (Utility::ToLower(rl->ruleName).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && rl->ruleName.length() == splits->at(splitindex).length())
										if (Utility::ToLower(rl->ruleName).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && rl->ruleName.length() == splits->at(splitindex).length()) {
											loginfo("Overriding rule: {}. file: {}", rl->ruleName, file);
											rule = rl;
											existing = true;
											break;
										}
									}
									if (existing == false)
										rule = new Distribution::Rule();
									rule->ruleVersion = ruleVersion;
									rule->ruleType = ruleType;
									rule->ruleName = splits->at(splitindex);
									LOGL_2("loading rule: {}", rule->ruleName);
									splitindex++;
									// now come the rule priority
									rule->rulePriority = -1;
									try {
										rule->rulePriority = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// potion preset
									std::string potionpreset = splits->at(splitindex);
									splitindex++;
									// potion effect preset
									std::string potioneffectpreset = splits->at(splitindex);
									splitindex++;
									// poison preset
									std::string poisonpreset = splits->at(splitindex);
									splitindex++;
									// poison effect preset
									std::string poisoneffectpreset = splits->at(splitindex);
									splitindex++;
									// fortify preset
									std::string fortifypreset = splits->at(splitindex);
									splitindex++;
									// fortify effect preset
									std::string fortifyeffectpreset = splits->at(splitindex);
									splitindex++;
									// food preset
									std::string foodpreset = splits->at(splitindex);
									splitindex++;
									// food effect preset
									std::string foodeffectpreset = splits->at(splitindex);
									splitindex++;
									// allow mixed
									if (splits->at(splitindex) == "1")
										rule->allowMixed = true;
									else
										rule->allowMixed = false;
									splitindex++;
									// style scaling
									if (splits->at(splitindex) == "1")
										rule->styleScaling = true;
									else
										rule->styleScaling = false;
									splitindex++;

									// we got all relevant information, now find all presets we need
									try {
										auto potions = Distribution::_internEffectCategoryPresets.at(potionpreset);
										if (!potions)
											logwarn("Could not find preset {}", potionpreset);
										LOGL_3("found preset {}", potionpreset);
										auto potionEffects = Distribution::_internEffectPresets.at(potioneffectpreset);
										if (!potionEffects)
											logwarn("Could not find preset {}", potioneffectpreset);
										LOGL_3("found preset {}", potioneffectpreset);
										auto poisons = Distribution::_internEffectCategoryPresets.at(poisonpreset);
										if (!poisons)
											logwarn("Could not find preset {}", poisonpreset);
										LOGL_3("found preset {}", poisonpreset);
										auto poisonEffects = Distribution::_internEffectPresets.at(poisoneffectpreset);
										if (!poisonEffects)
											logwarn("Could not find preset {}", poisoneffectpreset);
										LOGL_3("found preset {}", poisoneffectpreset);
										auto fortify = Distribution::_internEffectCategoryPresets.at(fortifypreset);
										if (!fortify)
											logwarn("Could not find preset {}", fortifypreset);
										LOGL_3("found preset {}", fortifypreset);
										auto fortifyEffects = Distribution::_internEffectPresets.at(fortifyeffectpreset);
										if (!fortifyEffects)
											logwarn("Could not find preset {}", fortifyeffectpreset);
										LOGL_3("found preset {}", fortifyeffectpreset);
										auto food = Distribution::_internEffectCategoryPresets.at(foodpreset);
										if (!food)
											logwarn("Could not find preset {}", foodpreset);
										LOGL_3("found preset {}", foodpreset);
										auto foodEffects = Distribution::_internEffectPresets.at(foodeffectpreset);
										if (!foodEffects)
											logwarn("Could not find preset {}", foodeffectpreset);
										LOGL_3("found preset {}", foodeffectpreset);

										if (potions && potionEffects && poisons && poisonEffects && fortify && fortifyEffects && food && foodEffects) {
											// all components found, assemble rule
											rule->potions = potions;
											rule->potionEffects = potionEffects;
											rule->poisons = poisons;
											rule->poisonEffects = poisonEffects;
											rule->fortify = fortify;
											rule->fortifyEffects = fortifyEffects;
											rule->food = food;
											rule->foodEffects = foodEffects;
										} else {
											logwarn("At least one preset could not be found. file: {}, rule:\"{}\"", file, tmp);
											delete splits;
											delete rule;
											continue;
										}
									} catch (std::exception& e) {
										logwarn("At least one preset could not be found, error: {}. file: {}, rule:\"{}\"", e.what(), file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									Distribution::_rules.insert(rule);

									if (rule->ruleName == DefaultRuleName && (Distribution::defaultRule == nullptr ||
																				 rule->rulePriority > Distribution::defaultRule->rulePriority))
										Distribution::defaultRule = rule;
									delete splits;
									LOGL_2("rule {} successfully loaded.", rule->ruleName);

									
									loginfo("Distr Effect Map: {}", Utility::PrintEffectMap(rule->potionEffects->effects));
								}
								break;
							case 23:  // Effect Preset
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}

									// next entry is the effect preset name, so we just set it
									Distribution::EffectPreset* preset = nullptr;
									bool existing = false;
									for (auto [name, pres] : Distribution::_internEffectPresets) {
										if (Utility::ToLower(name).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && name.length() == splits->at(splitindex).length()) {
											loginfo("Overriding preset: {}", name);
											preset = pres;
											existing = true;
											break;
										}
									}
									if (existing == false)
										preset = new Distribution::EffectPreset();
									// name
									preset->name = splits->at(splitindex);
									splitindex++;
									// associated effects
									std::string effectProperties = splits->at(splitindex);
									splitindex++;

									Utility::ParseAlchemyEffects(effectProperties, preset);
									preset->standardDistr = Distribution::GetEffectDistribution(preset->effects);
									preset->validEffects = Distribution::SumAlchemyEffects(preset->standardDistr);
									Distribution::_internEffectPresets.insert_or_assign(preset->name, preset);

									delete splits;
									LOGL_2("EffectPreset {} successfully loaded.", preset->name);
								}
								break;
							case 24:  // effect preset attach
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									auto names = Utility::SplitString(splits->at(splitindex), ',', true);
									splitindex++;
									std::vector<Distribution::EffectPreset*> presets;
									for (std::string pname : names) {
										// next entry is the effect preset name, so we just set it
										Distribution::EffectPreset* preset = nullptr;
										bool existing = false;
										for (auto [name, pres] : Distribution::_internEffectPresets) {
											if (Utility::ToLower(name).find(Utility::ToLower(pname)) != std::string::npos && name.length() == pname.length()) {
												loginfo("Overriding preset: {}", name);
												preset = pres;
												existing = true;
												break;
											}
										}
										if (existing == false || preset == nullptr) {
											LOGL_2("EffectPreset {} cannot be found and not attachment be performed. file: {}, rule:\"{}\"", splits->at(splitindex-1), file, tmp);
										}
										else
										{
											presets.push_back(preset);
										}
									}

									// associated effects
									std::string effectProperties = splits->at(splitindex);
									splitindex++;
									std::string cumnames = "|";
									for (auto preset : presets)
									{
										Utility::ParseAlchemyEffects(effectProperties, preset);
										preset->standardDistr = Distribution::GetEffectDistribution(preset->effects);
										preset->validEffects = Distribution::SumAlchemyEffects(preset->standardDistr);
										cumnames += preset->name + "|";
									}

									delete splits;
									LOGL_2("Effect Attachment has been performed on presets {}", cumnames);
								}
								break;
							case 25:  // category
								{
									if (splits->size() != 8) {
										logwarn("rule has wrong number of fields, expected 8. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the category name, so we just set it
									Distribution::EffCategory* category = nullptr;
									bool existing = false;
									for (auto [name, cat] : Distribution::_internEffectCategories) {
										if (Utility::ToLower(name).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && name.length() == splits->at(splitindex).length()) {
											loginfo("Overriding category: {}", name);
											category = cat;
											existing = true;
											break;
										}
									}
									if (existing == false)
										category = new Distribution::EffCategory();
									// name
									category->name = splits->at(splitindex);
									splitindex++;
									// base chance
									category->baseChance = 0;
									try {
										category->baseChance  = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"BaseChance\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"BaseChance\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									}
									// falloff
									category->falloff = 0;
									try {
										category->falloff = std::stof(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"Falloff\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"Falloff\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									}
									// falloff delay
									category->falloffdelay = 0;
									try {
										category->falloffdelay = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"FallOffDelay\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"FallOffDelay\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									}
									// max
									category->max = 0;
									try {
										category->max = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"Max\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"Max\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									}
									// fallofffunc
									category->falloffFunc = Distribution::FalloffFunction::Exponential;
									try {
										category->falloffFunc = (Distribution::FalloffFunction)std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"FalloffFunction\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"FalloffFunction\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete category;
										continue;
									}
									// insert category
									Distribution::_internEffectCategories.insert_or_assign(category->name, category);
									delete splits;
									LOGL_2("category {} successfully loaded.", category->name);
								}
								break;
							case 26:  // category preset
								{
									if (splits->size() != 9) {
										logwarn("rule has wrong number of fields, expected 9. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the preset name, so we just set it
									Distribution::EffCategoryPreset* catpreset = nullptr;
									bool existing = false;
									for (auto [name, cat] : Distribution::_internEffectCategoryPresets) {
										if (Utility::ToLower(name).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && name.length() == splits->at(splitindex).length()) {
											loginfo("Overriding category: {}", name);
											catpreset = cat;
											existing = true;
											break;
										}
									}
									if (existing == false)
										catpreset = new Distribution::EffCategoryPreset();
									// name
									catpreset->name = splits->at(splitindex);
									splitindex++;
									// strength 1
									std::string strength1 = splits->at(splitindex);
									splitindex++;
									// strength 2
									std::string strength2 = splits->at(splitindex);
									splitindex++;
									// strength 3
									std::string strength3 = splits->at(splitindex);
									splitindex++;
									// strength 4
									std::string strength4 = splits->at(splitindex);
									splitindex++;
									// strength 5
									std::string strength5 = splits->at(splitindex);
									splitindex++;
									// tieradjust
									int tieradjust = 0;
									try {
										tieradjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("out-of-range expection in field \"TierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete catpreset;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("invalid-argument expection in field \"TierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete catpreset;
										continue;
									}
									// find categories
									try {
										auto str1 = Distribution::_internEffectCategories.at(strength1);
										auto str2 = Distribution::_internEffectCategories.at(strength2);
										auto str3 = Distribution::_internEffectCategories.at(strength3);
										auto str4 = Distribution::_internEffectCategories.at(strength4);
										auto str5 = Distribution::_internEffectCategories.at(strength5);
										if (str1 && str2 && str3 && str4 && str5) {
											catpreset->cats[0] = str1;
											catpreset->cats[1] = str2;
											catpreset->cats[2] = str3;
											catpreset->cats[3] = str4;
											catpreset->cats[4] = str5;
										} else {
											logwarn("One or more categories could not be found. file: {}, rule:\"{}\"", file, tmp);
											delete splits;
											delete catpreset;
											continue;
										}
									} catch (std::exception&) {
										logwarn("One or more categories could not be found. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete catpreset;
										continue;
									}
									// set stuff
									catpreset->tieradjust = tieradjust;
									// insert category preset
									Distribution::_internEffectCategoryPresets.insert_or_assign(catpreset->name, catpreset);
									delete splits;
									LOGL_2("category preset {} successfully loaded.", catpreset->name);
								}
								break;
							case 27:  // effect preset copy
								{
									if (splits->size() != 4) {
										logwarn("rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the new effect preset name, so we just set it
									Distribution::EffectPreset* oldpreset = nullptr;
									bool existing = false;
									for (auto [name, pres] : Distribution::_internEffectPresets) {
										if (Utility::ToLower(name).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && name.length() == splits->at(splitindex).length()) {
											loginfo("Copying preset: {}", name);
											oldpreset = pres;
											existing = true;
											break;
										}
									}
									if (existing == false)
									{
										delete splits;
										logwarn("Original EffectPreset {} could not be found. file: {}, rule:\"{}\"", splits->at(splitindex), file, tmp);
										continue;
									}
									splitindex++;
									// next entry is the new effect preset name, so we just set it
									Distribution::EffectPreset* preset = nullptr;
									existing = false;
									for (auto [name, pres] : Distribution::_internEffectPresets) {
										if (Utility::ToLower(name).find(Utility::ToLower(splits->at(splitindex))) != std::string::npos && name.length() == splits->at(splitindex).length()) {
											loginfo("Overriding preset: {}", name);
											preset = pres;
											existing = true;
											break;
										}
									}
									if (existing == false)
										preset = new Distribution::EffectPreset();
									// name
									preset->name = splits->at(splitindex);
									splitindex++;

									preset->effects = oldpreset->effects;
									preset->standardDistr = oldpreset->standardDistr;
									preset->validEffects = oldpreset->validEffects;

									Distribution::_internEffectPresets.insert_or_assign(preset->name, preset);

									delete splits;
									LOGL_2("EffectPreset {} successfully copied to {}.", oldpreset->name, preset->name);
								}
								break;
							}
						}
					break;
					default:
						logwarn("Rule version does not exist. file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						break;
					}
				}
			} else {
				logwarn("file {} couldn't be read successfully", file);
			}

		} //catch (std::exception& e) {
		//	logwarn("file {} couldn't be read successfully due to an error: {}", file, e.what());
		//}
	}

	// create default rule if there is none
	if (Distribution::defaultRule == nullptr) {
		Distribution::defaultRule = new Distribution::Rule(1 /*version*/, 1 /*type*/, DefaultRuleName, INT_MIN + 1 /*rulePriority*/, true /*allowMixed*/, true /*styleScaling*/, 5 /*maxPotions*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*potion1Chance*/,
			std::vector<int>{ 20, 30, 40, 50, 60 } /*potion2Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*potion3Chance*/, std::vector<int>{ 5, 15, 25, 35, 45 } /*potion4Chance*/, std::vector<int>{ 0, 10, 20, 30, 40 } /*potionAddChance*/, 0 /*potionTierAdjust*/,
			5 /*maxFortify*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify1Chance*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify2Chance*/, std::vector<int>{ 20, 30, 40, 50, 60 } /*fortify3Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*fortify4Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*fortifyAddChance*/, 0 /*fortifyTierAdjust*/,
			5 /*maxPoisons*/, std::vector<int>{ 30, 35, 40, 45, 50 } /*poison1Chance*/, std::vector<int>{ 20, 25, 30, 35, 40 } /*poison2Chance*/, std::vector<int>{ 10, 15, 20, 25, 30 } /*poison3Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*poison4Chance*/,
			std::vector<int>{ 0, 5, 10, 15, 20 } /*poisonAddChance*/, 0 /*poisonTierAdjust*/, 
			std::vector<int>{ 70, 80, 90, 100, 100 } /*foodChance*/,
			Distribution::GetVector(RandomRange, AlchemicEffect::kAnyPotion) /*potionDistr*/,
			Distribution::GetVector(RandomRange, AlchemicEffect::kAnyPoison) /*poisonDistr*/,
			Distribution::GetVector(RandomRange, AlchemicEffect::kAnyFortify) /*fortifyDistr*/,
			Distribution::GetVector(RandomRange, AlchemicEffect::kAnyFood) /*foodDistr*/,
			AlchemicEffect::kAnyPotion | AlchemicEffect::kCustom /*validPotions*/,
			AlchemicEffect::kAnyPoison | AlchemicEffect::kCustom /*validPoisons*/,
			AlchemicEffect::kAnyFortify | AlchemicEffect::kCustom /*validFortifyPotions*/,
			AlchemicEffect::kAnyFood | AlchemicEffect::kCustom /*validFood*/);
	}
	if (Distribution::defaultCustomRule == nullptr) {
		Distribution::defaultCustomRule = new Distribution::Rule(1 /*version*/, 1 /*type*/, DefaultRuleName, INT_MIN + 1 /*rulePriority*/, true /*allowMixed*/, true /*styleScaling*/, 5 /*maxPotions*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*potion1Chance*/,
			std::vector<int>{ 20, 30, 40, 50, 60 } /*potion2Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*potion3Chance*/, std::vector<int>{ 5, 15, 25, 35, 45 } /*potion4Chance*/, std::vector<int>{ 0, 10, 20, 30, 40 } /*potionAddChance*/, 0 /*potionTierAdjust*/,
			5 /*maxFortify*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify1Chance*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify2Chance*/, std::vector<int>{ 20, 30, 40, 50, 60 } /*fortify3Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*fortify4Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*fortifyAddChance*/, 0 /*fortifyTierAdjust*/,
			5 /*maxPoisons*/, std::vector<int>{ 30, 35, 40, 45, 50 } /*poison1Chance*/, std::vector<int>{ 20, 25, 30, 35, 40 } /*poison2Chance*/, std::vector<int>{ 10, 15, 20, 25, 30 } /*poison3Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*poison4Chance*/,
			std::vector<int>{ 0, 5, 10, 15, 20 } /*poisonAddChance*/, 0 /*poisonTierAdjust*/,
			std::vector<int>{ 70, 80, 90, 100, 100 } /*foodChance*/,
			std::vector<std::tuple<int, AlchemicEffect>>{} /*potionDistr*/,
			std::vector<std::tuple<int, AlchemicEffect>>{} /*poisonDistr*/,
			std::vector<std::tuple<int, AlchemicEffect>>{} /*fortifyDistr*/,
			std::vector<std::tuple<int, AlchemicEffect>>{} /*foodDistr*/,
			AlchemicEffect::kCustom /*validPotions*/,
			AlchemicEffect::kCustom /*validPoisons*/,
			AlchemicEffect::kCustom /*validFortifyPotions*/,
			AlchemicEffect::kCustom /*validFood*/);
	}

	if (copyrules.size() > 0) {
		for (auto cpy : copyrules) {
			auto splits = std::get<0>(cpy);
			if (splits->size() != 5) {
				logwarn("rule has wrong number of fields, expected 5. file: {}, rule:\"{}\", fields: {}", std::get<1>(cpy), std::get<2>(cpy), splits->size());
				continue;
			}
			std::string name = (splits)->at(2);
			std::string newname = (splits)->at(3);
			
			Distribution::Rule* rule = Distribution::FindRule(name);
			if (rule == nullptr) {
				logwarn("base rule couldn't be found file: {}, rule:\"{}\", fields: {}", std::get<1>(cpy), std::get<2>(cpy), splits->size());
				delete splits;
				continue;
			}
			Distribution::Rule* newrule = nullptr;
			bool existing = false;
			for (auto rl : Distribution::_rules)
				if (Utility::ToLower(rl->ruleName).find(Utility::ToLower(newname)) != std::string::npos && rl->ruleName.length() == newname.length()) {
					loginfo("Overriding rule: {}", rl->ruleName);
					newrule = rl;
					existing = true;
					break;
				}
			if (existing == false)
				newrule = rule->Clone();
			newrule->ruleName = newname;
			int prio = INT_MIN;
			try {
				prio = std::stoi(splits->at(4));
			} catch (std::out_of_range&) {
				logwarn("out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
				prio = INT_MIN;
			} catch (std::invalid_argument&) {
				logwarn("invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
				prio = INT_MIN;
			}
			if (prio != INT_MIN)
				newrule->rulePriority = prio;

			/* if (Distribution::_rules.contains(newrule)) {
				auto ritr = Distribution::_rules.find(newrule);
				if (ritr != Distribution::_rules.end()) {
					// get old newrule
					auto rtmp = *ritr;
					// erase old newrule
					Distribution::_rules.erase(newrule);
					// delete old newrule
					delete rtmp;
					// insert new newrule
					Distribution::_rules.insert(newrule);
				} else {
					logcritical("Critical error in code, please report to author. file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
					delete newrule;
					delete splits;
					continue;
				}
			} else*/
				Distribution::_rules.insert(newrule);

			if (newname == DefaultRuleName && (Distribution::defaultRule == nullptr ||
												  newrule->rulePriority > Distribution::defaultRule->rulePriority))
				Distribution::defaultRule = newrule;
			delete splits;
			LOGL_2("rule {} successfully coinitialised.", newrule->ruleName);
		}
	}

	// and now for the attachement rules.
	// 
	// vector of splits, filename and line
	//std::vector<std::tuple<std::vector<std::string>*, std::string, std::string>> attachments;
	if (attachments.size() > 0) {
		std::string name;
		for (auto a : attachments) {
			// first two splits are version and type which are already confirmed, so just process the last two.
			// 3rd split is the name of the rule, which the objects in the 4th or 5th split are attached to
			if (std::get<0>(a)->size() == 4 || std::get<0>(a)->size() == 5) {
				{
					// valid rule
					name = (std::get<0>(a))->at(2);
					Distribution::Rule* rule = Distribution::FindRule(name);
					if (rule == nullptr) {
						logwarn("rule not found. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
						continue;  // rule doesn't exist, evaluate next attachment
					}

					// get rule priority and index for assoc objects
					int prio = INT_MIN;
					int associdx = 3;
					// get priority if we have 5 fields, otherwise use rule priority
					if (std::get<0>(a)->size() == 4)
						prio = rule->rulePriority;
					else {
						try {
							prio = std::stoi((std::get<0>(a))->at(3));
							associdx = 4;
						} catch (std::out_of_range&) {
							logwarn("out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
							continue;
						} catch (std::invalid_argument&) {
							logwarn("invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
							continue;
						}
					}


					// parse the associated objects
					bool error = false;
					int total = 0;
					std::vector<std::tuple<Distribution::AssocType, RE::FormID, RE::TESForm*>> objects = Utility::ParseAssocObjects((std::get<0>(a)->at(3)), error, std::get<1>(a), std::get<2>(a), total);

					std::pair<int, Distribution::Rule*> tmptuple = { prio, rule };
					// assign rules to search parameters
					bool attach = false; // loop intern
					int oldprio = INT_MIN;
					for (int i = 0; i < objects.size(); i++) {
						switch (std::get<0>(objects[i])) {
						case Distribution::AssocType::kFaction:
						case Distribution::AssocType::kKeyword:
						case Distribution::AssocType::kRace:
						case Distribution::AssocType::kClass:
						case Distribution::AssocType::kCombatStyle:
							if (auto item = Distribution::_assocMap.find(std::get<1>(objects[i])); item != Distribution::_assocMap.end()) {
								if ((oldprio = std::get<1>(item->second)->rulePriority) < rule->rulePriority) {
									Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
									attach = false;
								}
							} else {
								Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
								attach = true;
							}
							break;
						case Distribution::AssocType::kNPC:
						case Distribution::AssocType::kActor:
							if (auto item = Distribution::_npcMap.find(std::get<1>(objects[i])); item != Distribution::_npcMap.end()) {
								if ((oldprio = item->second->rulePriority) < rule->rulePriority) {
									Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
									attach = false;
								}
							} else {
								Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
								attach = true;
							}
							break;
						}
						if (Logging::EnableLoadLog) {
							switch (std::get<0>(objects[i])) {
							case Distribution::AssocType::kFaction:
								if (attach) {
									LOGL_2("attached Faction {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else 
									LOGL_2("updated Faction {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kKeyword:
								if (attach) {
									LOGL_2("attached Keyword {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGL_2("updated Keyword {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kRace:
								if (attach) {
									LOGL_2("attached Race {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGL_2("updated Race {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kClass:
								if (attach) {
									LOGL_2("attached Class {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGL_2("updated Class {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kCombatStyle:
								if (attach) {
									LOGL_2("attached CombatStyle {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGL_2("updated CombatStyle {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kNPC:
							case Distribution::AssocType::kActor:
								if (attach) {
									LOGL_2("attached Actor {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGL_2("updated Actor {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							default:
								LOGL_2("{} has the wrong FormType for rule attachment. file: {}, rule: \"{}\"", Utility::GetHex(std::get<1>(objects[i])), std::get<1>(a), std::get<2>(a));
							}
						}

					}
				}
			} else {
				// rule invalid
				logwarn("rule has wrong number of fields, expected 4 or 5. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
				// delet splits since we don't need it anymore
				delete std::get<0>(a);
			}
		}
	}

	StaticExclusions(datahandler);

	// create backup of crucial lists
	std::copy(Distribution::_excludedItems.begin(), Distribution::_excludedItems.end(), std::inserter(Distribution::_excludedItemsBackup, Distribution::_excludedItemsBackup.begin()));


	// load our stuff like necessary forms
	// get VendorItemPotion keyword, if we don't find this, potion detection will be nearly impossible
	Settings::VendorItemPotion = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEC);
	if (Settings::VendorItemPotion == nullptr) {
		loginfo("[INIT] Couldn't find VendorItemPotion Keyword in game.");
	}
	Settings::VendorItemPoison = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDED);
	if (Settings::VendorItemPoison == nullptr) {
		loginfo("[INIT] Couldn't find VendorItemPoison Keyword in game.");
	}
	Settings::VendorItemFood = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEA);
	if (Settings::VendorItemFood == nullptr) {
		loginfo("[INIT] Couldn't find VendorItemFood Keyword in game.");
	}
	Settings::VendorItemFoodRaw = RE::TESForm::LookupByID<RE::BGSKeyword>(0x000A0E56);
	if (Settings::VendorItemFoodRaw == nullptr) {
		loginfo("[INIT] Couldn't find VendorItemFoodRaw Keyword in game.");
	}
	Settings::CurrentFollowerFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0005C84E);
	if (Settings::CurrentFollowerFaction == nullptr) {
		loginfo("[INIT] Couldn't find CurrentFollowerFaction Faction in game.");
	}
	Settings::CurrentHirelingFaction = RE::TESForm::LookupByID<RE::TESFaction>(0xbd738);
	if (Settings::CurrentHirelingFaction == nullptr) {
		loginfo("[INIT] Couldn't find CurrentHirelingFaction Faction in game.");
	}
	Settings::ActorTypeDwarven = RE::TESForm::LookupByID<RE::BGSKeyword>(0x1397A);
	if (Settings::ActorTypeDwarven == nullptr) {
		loginfo("[INIT] Couldn't find ActorTypeDwarven Keyword in game.");
	}
	Settings::ActorTypeCreature = RE::TESForm::LookupByID<RE::BGSKeyword>(0x13795);
	if (Settings::ActorTypeCreature == nullptr) {
		loginfo("[INIT] Couldn't find ActorTypeCreature Keyword in game.");
	}
	Settings::ActorTypeAnimal = RE::TESForm::LookupByID<RE::BGSKeyword>(0x13798);
	if (Settings::ActorTypeAnimal == nullptr) {
		loginfo("[INIT] Couldn't find ActorTypeAnimal Keyword in game.");
	}
	Settings::Vampire = RE::TESForm::LookupByID<RE::BGSKeyword>(0xA82BB);
	if (Settings::Vampire == nullptr) {
		loginfo("[INIT] Couldn't find Vampire Keyword in game.");
	}

	Settings::AlchemySkillBoosts = RE::TESForm::LookupByID<RE::BGSPerk>(0xA725C);
	if (Settings::AlchemySkillBoosts == nullptr)
		loginfo("[INIT] Couldn't find AlchemySkillBoosts Perk in game.");
	Settings::PerkSkillBoosts = RE::TESForm::LookupByID<RE::BGSPerk>(0xCF788);
	if (Settings::PerkSkillBoosts == nullptr)
		loginfo("[INIT] Couldn't find PerkSkillBoosts Perk in game.");

	Distribution::initialised = true;

	if (Settings::Fixes::_ApplySkillBoostPerks)
		Settings::ApplySkillBoostPerks();

	if (Logging::EnableLoadLog) {
		LOGL_2("Number of Rules: {}", Distribution::rules()->size());
		LOGL_2("Number of NPCs: {}", Distribution::npcMap()->size());
		LOGL_2("Buckets of NPCs: {}", Distribution::npcMap()->bucket_count());
		LOGL_2("Number of Associations: {}", Distribution::assocMap()->size());
		LOGL_2("Buckets of Associations: {}", Distribution::assocMap()->bucket_count());
		LOGL_2("Number of Bosses: {}", Distribution::bosses()->size());
		LOGL_2("Buckets of Bosses: {}", Distribution::bosses()->bucket_count());
		LOGL_2("Number of Excluded NPCs: {}", Distribution::excludedNPCs()->size());
		LOGL_2("Buckets of Excluded NPCs: {}", Distribution::excludedNPCs()->bucket_count());
		LOGL_2("Number of Excluded Associations: {}", Distribution::excludedAssoc()->size());
		LOGL_2("Buckets of Excluded Associations: {}", Distribution::excludedAssoc()->bucket_count());
		LOGL_2("Number of Excluded Items: {}", Distribution::excludedItems()->size());
		LOGL_2("Buckets of Excluded Items: {}", Distribution::excludedItems()->bucket_count());
		LOGL_2("Number of Baseline Exclusions: {}", Distribution::baselineExclusions()->size());
		LOGL_2("Buckets of Baseline Exclusions: {}", Distribution::baselineExclusions()->bucket_count());
		/*for (int i = 0; i < Distribution::_rules.size(); i++) {
			LOGL_2("rule {} pointer {}", i, Utility::GetHex((uintptr_t)Distribution::_rules[i]));
		}
		auto iter = Distribution::_assocMap.begin();
		while (iter != Distribution::_assocMap.end()) {
			LOGL_2("assoc\t{}\trule\t{}", Utility::GetHex(iter->first), Utility::GetHex((uintptr_t)(std::get<1>(iter->second))));
			iter++;
		}*/


		auto cuitr = Distribution::_customItems.begin();
		int x = 0;
		while (cuitr != Distribution::_customItems.end()) {
			x++;
			std::vector<Distribution::CustomItemStorage*> cust = cuitr->second;
			LOGL_1("{}: FormID: {}\tEntries: {}", std::to_string(x), std::to_string(cuitr->first), cust.size());
			for (int b = 0; b < cust.size(); b++) {
				
				for (int i = 0; i < cust[b]->items.size(); i++) {
					auto cit = cust[b]->items[i];
					LOGL_1("{}: Items: Name: {}\tChance: {}", std::to_string(x), cit->object->GetName(), std::to_string(cit->chance));
				}
				for (int i = 0; i < cust[b]->death.size(); i++) {
					auto cit = cust[b]->death[i];
					LOGL_1("{{}: Death: Name: {}\tChance: {}", std::to_string(x), cit->object->GetName(), std::to_string(cit->chance));
				}
				for (int i = 0; i < cust[b]->poisons.size(); i++) {
					auto cit = cust[b]->poisons[i];
					LOGL_1("{}: Poisons: Name: {}\tChance: {}", std::to_string(x), cit->object->GetName(), std::to_string(cit->chance));
				}
				for (int i = 0; i < cust[b]->potions.size(); i++) {
					auto cit = cust[b]->potions[i];
					LOGL_1("{}: Potions: Name: {}\tChance: {}", std::to_string(x), cit->object->GetName(), std::to_string(cit->chance));
				}
				for (int i = 0; i < cust[b]->fortify.size(); i++) {
					auto cit = cust[b]->fortify[i];
					LOGL_1("{}: Fortify: Name: {}\tChance: {}", std::to_string(x), cit->object->GetName(), std::to_string(cit->chance));
				}
			}
			cuitr++;
		}
	}

	// reactivate generic logging
	if (Logging::EnableLog == true)
		Logging::EnableGenericLogging = true;
	else
		Logging::EnableGenericLogging = false;
}

void Settings::StaticExclusions(RE::TESDataHandler* datahandler)
{
	/// EXCLUDE ITEMS

	// handle standard exclusions
	RE::TESForm* tmp = nullptr;

	// MQ201Drink (don't give quest items out)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00036D53, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// Unknown Potion with unknown effect (in-game type)
	if ((tmp = Utility::GetTESForm(datahandler, 0x0005661F, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// Kordirs skooma: its probably kordirs
	if ((tmp = Utility::GetTESForm(datahandler, 0x00057A7B, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// Stallion's potion: its probably stallions
	if ((tmp = Utility::GetTESForm(datahandler, 0x0005566A, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// DB03Poison (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00058CFB, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// DA16TorporPotion (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00005F6DF, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// IVDGhostPotion (special item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000663E1, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// DummyPotion
	if ((tmp = Utility::GetTESForm(datahandler, 0x0006A07E, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// TG00FalmerBlood - Falmer Blood Elixier
	// DA14Water - Holy Water
	// TGTQ02BalmoraBlue (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000DC172, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// CW01BWraithPoison (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000E2D3D, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// Blades Potion: probably esberns
	if ((tmp = Utility::GetTESForm(datahandler, 0x000E6DF5, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// MS14WineAltoA: probably jessica's
	if ((tmp = Utility::GetTESForm(datahandler, 0x000F257E, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x00102019, "", "")) != nullptr)
		Distribution::_excludedDistrItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201A, "", "")) != nullptr)
		Distribution::_excludedDistrItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201B, "", "")) != nullptr)
		Distribution::_excludedDistrItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201C, "", "")) != nullptr)
		Distribution::_excludedDistrItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201D, "", "")) != nullptr)
		Distribution::_excludedDistrItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201E, "", "")) != nullptr)
		Distribution::_excludedDistrItems.insert(tmp->GetFormID());
	// DA03FoodMammothMeat (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010211A, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// Mq101JuniperMead (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00107A8A, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());

	// DLC1FoodSoulHusk
	if ((tmp = Utility::GetTESForm(datahandler, 0x014DC4, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// DLC1FoodSoulHuskExtract
	if ((tmp = Utility::GetTESForm(datahandler, 0x015A1E, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());

	/// EXCLUDE SUMMONS

	// DA14Summoned
	if ((tmp = Utility::GetTESForm(datahandler, 0x0001F3AA, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlame
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C0, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrost
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C1, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStorm
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C2, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlamePotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E940, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E943, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E944, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// EncSummonFamiliar
	if ((tmp = Utility::GetTESForm(datahandler, 0x000640B5, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// MGArnielSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0006A152, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonPhantom
	if ((tmp = Utility::GetTESForm(datahandler, 0x00072310, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedSkeleton01Missile
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007503C, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlameThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87D, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87E, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87F, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonFireStorm
	if ((tmp = Utility::GetTESForm(datahandler, 0x000877EB, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonFelldir
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923F9, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonGormlaith
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923FA, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonHakon
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923FB, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// MGRDremoraSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00099F2F, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonFlamingThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0009CE28, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedCreature
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CC5A2, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlameThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECC, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECD, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECE, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// HowlSummonWolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CF79E, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunFortSnowhawkSummonedSkeleton01
	if ((tmp = Utility::GetTESForm(datahandler, 0x000D8D95, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedSkeleton01Melee1HShield
	if ((tmp = Utility::GetTESForm(datahandler, 0x000F90BC, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonEncDremoraLord
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010DDEE, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DA14DremoraSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010E38B, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostNPC
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE43, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormNPC
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE45, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostNPCPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE46, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormNPCPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE47, "", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnWrathmanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B4, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnMistmanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B7, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnBonemanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B9, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1HowlSummonIceWolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x008A6C, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1HowlSummonWerewolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x008A6D, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC01SoulCairnHorseSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00BDD0, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1VQ05BonemanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00BFF0, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon1
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16A, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon2
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16B, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon3
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16C, "Dawnguard.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshGuardian
	if ((tmp = Utility::GetTESForm(datahandler, 0x0177B6, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshSpawn01
	if ((tmp = Utility::GetTESForm(datahandler, 0x01CDF8, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshGuardianNeloth
	if ((tmp = Utility::GetTESForm(datahandler, 0x01DBDC, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonTrollFrost
	if ((tmp = Utility::GetTESForm(datahandler, 0x01DFA1, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonSeeker
	if ((tmp = Utility::GetTESForm(datahandler, 0x01EEC9, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2dunKarstaagSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x024811, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonSeekerHigh
	if ((tmp = Utility::GetTESForm(datahandler, 0x030CDE, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonWerebear
	if ((tmp = Utility::GetTESForm(datahandler, 0x0322B3, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2dunKarstaagIceWraithSummoned
	if ((tmp = Utility::GetTESForm(datahandler, 0x034B5A, "Dragonborn.esm", "")) != nullptr)
		Distribution::_excludedNPCs.insert(tmp->GetFormID());

	// EXCLUDED FACTIONS

	// template:
	//if ((tmp = Utility::GetTESForm(datahandler, 0, "", "")) != nullptr)
	//	Distribution::_excludedItems.insert(tmp->GetFormID());
}


static bool IsLeveledChar(RE::TESNPC* npc)
{
	if (npc->baseTemplateForm == nullptr)
		return true;
	RE::TESNPC* tplt = npc->baseTemplateForm->As<RE::TESNPC>();
	RE::TESLevCharacter* lvl = npc->baseTemplateForm->As<RE::TESLevCharacter>();
	if (tplt) {
		return IsLeveledChar(tplt);
	} else if (lvl)
		return false;
	else
		;  //loginfo("template invalid");
	return false;
}

void Settings::CheckActorsForRules()
{
	loginfo("checking...");
	std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_NPCs_without_Rule.csv");
	std::ofstream outpris("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_NPCs_without_Rule_Prisoners.txt");
	out << "PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";

	const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
	{
		const RE::BSReadLockGuard locker{ lock };
		if (hashtable) {
			std::set<RE::FormID> visited{};
			RE::Actor* act = nullptr;
			RE::TESNPC* npc = nullptr;
			ActorStrength acs;
			ItemStrength is;
			uint32_t index;
			std::string name;
			for (auto& [id, form] : *hashtable) {
				if (form) {
					act = form->As<RE::Actor>();
					npc = form->As<RE::TESNPC>();
					logwarn("act {}\t\t npc {}", act ? Utility::PrintForm(act) : "", npc ? Utility::PrintForm(npc) : "");
					if (npc && npc->GetFormID() != 0x07 && (npc->GetFormID() >> 24) != 0xFF) {
						if (!visited.contains(npc->GetFormID())) {
							visited.insert(npc->GetFormID());
							{
								index = Utility::ExtractTemplateInfo(npc).pluginID;
								if (index == 0x1) {
									continue;
								}
								name = Utility::Mods::GetPluginName(index);
							}
							// check wether there is a rule that applies
							if (Distribution::ExcludedNPC(npc)) {
								//coun++;
								continue;  // the npc is covered by an exclusion
							}
							// get rule
							Misc::NPCTPLTInfo npcinfo = Utility::ExtractTemplateInfo(npc);
							Distribution::Rule* rl = Distribution::CalcRule(npc, acs, is, &npcinfo);

							//Utility::ToLower(std::string(npc->GetFormEditorID())).find("lvl") == std::string::npos
							if (rl && rl->ruleName == DefaultRuleName && !IsLeveledChar(npc)) {
								if (name.empty() == false)
									out << name << ";";
								else
									out << ";";
								// we found an actor that does not have a rule, so print that to the output
								out << npc->GetName() << ";"
									<< "0x" << Utility::GetHex(npc->GetFormID()) << ";"
									<< ";";

								if (npc->GetRace())
									out << npc->GetRace()->GetFormEditorID() << ";"
										<< "0x" << Utility::GetHex(npc->GetRace()->GetFormID()) << ";";
								else
									out << ";;";

								for (uint32_t i = 0; i < npc->factions.size(); i++) {
									out << ";"
										<< "0x" << Utility::GetHex(npc->factions[i].faction->GetFormID());
								}
								out << "\n";
								out.flush();

								// prisoner check
								if (std::string(npc->GetName()).find("prisoner") != std::string::npos ||
									std::string(npc->GetName()).find("Prisoner") != std::string::npos ||
									std::string(npc->GetName()).find("Slave") != std::string::npos ||
									std::string(npc->GetName()).find("slave") != std::string::npos) {
									if (std::string(name).find("Skyrim.esm") != std::string::npos)
										outpris << "<"
												<< Utility::GetHex((npc->GetFormID() & 0x00FFFFFF))
												<< ","
												<< ">";
									else if ((index & 0x00FFF000) != 0) {  // light plugin
										outpris << "<"
												<< Utility::GetHex((npc->GetFormID() & 0x00000FFF))
												<< ","
												<< name
												<< ">";
									} else {
										outpris << "<"
												<< Utility::GetHex((npc->GetFormID() & 0x00FFFFFF))
												<< ","
												<< name
												<< ">";
									}
								}
							}
						}
					} else if (act && act->GetFormID() != 0x14 && (act->GetFormID() >> 24) != 0xFF) {
						if (!visited.contains(act->GetFormID())) {
							// lookup pluing of the actor base
							{
								index = Utility::ExtractTemplateInfo(act).pluginID;
								if (index == 0x1) {
									continue;
								}
								name = Utility::Mods::GetPluginName(index);
							}

							// we didn't consider the current actors base so far
							visited.insert(act->GetFormID());

							std::shared_ptr<ActorInfo> acinfo = std::make_shared<ActorInfo>(act);
							// get rule
							Distribution::Rule* rl = Distribution::CalcRule(acinfo);
							// check wether there is a rule that applies
							if (Distribution::ExcludedNPC(acinfo)) {
								//coun++;
								continue;  // the npc is covered by an exclusion
							}
							//logwarn("[CheckActorsForRules] got rule");
							if (rl && rl->ruleName == DefaultRuleName) {
								// lookup plugin of the actor red
								if (name.empty() == false)
									out << name << ";";
								else
									out << ";";
								// we found an actor that does not have a rule, so print that to the output
								out << act->GetName() << ";"
									<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
									<< "0x" << Utility::GetHex(act->GetFormID()) << ";";

								if (act->GetRace())
									out << act->GetRace()->GetFormEditorID() << ";"
										<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
								else
									out << ";;";

								if (act->GetSaveParentCell())
									out << act->GetSaveParentCell()->GetName();

								act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
									if (a_faction)
										out << ";"
											<< "0x" << Utility::GetHex(a_faction->GetFormID());
									return false;
								});
								out << "\n";
								out.flush();
							}
						}
					}
				}
			}
		}
	}
	loginfo("finished checking...");
}

static std::binary_semaphore lockcells(1);

void Settings::CheckCellForActors(RE::FormID cellid)
{
	lockcells.acquire();
	loginfo("checking cell {}...", Utility::GetHex(cellid));
	std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv", std::ofstream::app);
	//out << "RuleApplied;PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
	//PluginBase;


	std::set<RE::FormID> visited;
	RE::Actor* act = nullptr;

	RE::TESForm* tmp = RE::TESForm::LookupByID(cellid);
	RE::TESObjectCELL* cell = nullptr;
	uint32_t index;
	std::string name;
	if (tmp)
		cell = tmp->As<RE::TESObjectCELL>();
	if (cell) {
		for (auto& ptr : cell->GetRuntimeData().references) {
			try {
				if (ptr.get()) {
					act = ptr->As<RE::Actor>();
					if (Utility::ValidateActor(act) && act->GetFormID() != 0x14) {
						if (!visited.contains(act->GetFormID())) {
							// we didn't consider the current actors base so far
							visited.insert(act->GetFormID());
							{
								index = Utility::ExtractTemplateInfo(act).pluginID;
								if (index == 0x1) {
									continue;
								}
								name = Utility::Mods::GetPluginName(index);
							}
							bool excluded = false;
							// check wether there is a rule that applies
							if (Logging::EnableLog) {
								std::shared_ptr<ActorInfo> acinfo = std::make_shared<ActorInfo>(act);
								// get rule
								Distribution::Rule* rl = Distribution::CalcRule(acinfo);
								if (Distribution::ExcludedNPC(acinfo)) {
									excluded = true;
									LOG_1("excluded");
								}

								out << cell->GetFormEditorID() << ";";
								if (excluded)
									out << "Excluded"
										<< ";";
								else
									out << rl->ruleName << ";";

								if (name.empty() == false)
									out << name << ";";
								else
									out << ";";

								// we found an actor that does not have a rule, so print that to the output
								out << act->GetName() << ";"
									<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
									<< "0x" << Utility::GetHex(act->GetFormID()) << ";";

								if (act->GetRace())
									out << act->GetRace()->GetFormEditorID() << ";"
										<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
								else
									out << ";;";

								if (act->GetSaveParentCell())
									out << act->GetSaveParentCell()->GetName();

								act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
									if (a_faction)
										out << ";"
											<< "0x" << Utility::GetHex(a_faction->GetFormID());
									return false;
								});

								out << "\n";
								out.flush();
							}
						}
					}
				}
			} catch (...) {
				out << ";";
			}
		}
	}
	lockcells.release();
	loginfo("end");
}

void Settings::ApplySkillBoostPerks()
{
	auto races = CalcRacesWithoutPotionSlot();
	auto datahandler = RE::TESDataHandler::GetSingleton();
	auto npcs = datahandler->GetFormArray<RE::TESNPC>();
	for(auto& npc : npcs) {
		// make sure it isn't the player, isn't excluded, and the race isn't excluded from the perks
		if (npc && npc->GetFormID() != 0x7 && npc->GetRace() && !Distribution::ExcludedNPC(npc) && races.contains(npc->GetRace()->GetFormID()) == false) {
			// some creatures have cause CTDs or other problems, if they get the perks, so try to filter some of them out
			// if they are a creature and do not have any explicit rule, they will not get any perks
			// at the same time, their id will be blacklisted for the rest of the plugin, to avoid any handling and distribution problems
			if (Settings::Compatibility::_DisableCreaturesWithoutRules && (npc->GetRace()->HasKeyword(Settings::ActorTypeCreature) || npc->GetRace()->HasKeyword(ActorTypeAnimal))) {
				ActorStrength acs;
				ItemStrength is;
				auto tplt = Utility::ExtractTemplateInfo(npc);
				auto rule = Distribution::CalcRule(npc, acs, is, &tplt);
				if (rule->ruleName == Distribution::emptyRule->ruleName || rule->ruleName == Distribution::defaultRule->ruleName) {
					// blacklist the npc
					Distribution::_excludedNPCs.insert(npc->GetFormID());
					logwarn("[Settings] [AddPerks] Excluded creature {}", Utility::PrintForm(npc));
					// handle next npc
					continue;
				}
			}
			npc->AddPerk(Settings::AlchemySkillBoosts, 1);
			npc->AddPerk(Settings::PerkSkillBoosts, 1);
			LOGL_4("Added perks to npc {}", Utility::PrintForm(npc));
		}
	}

}

void Settings::UpdateSettings()
{
	LOG_2("Apply configuration changes");
	uint32_t flag = Settings::_updateSettings;
	Settings::_updateSettings = 0;
	if (flag & (uint32_t)UpdateFlag::kProhibitedEffects)
	{
		Distribution::_excludedItems.clear();
		std::copy(Distribution::_excludedItemsBackup.begin(), Distribution::_excludedItemsBackup.end(), std::inserter(Distribution::_excludedItems, Distribution::_excludedItems.begin()));
	}
	if ((flag & (uint32_t)UpdateFlag::kMagnitude) || (flag & (uint32_t)UpdateFlag::kProhibitedEffects))
	{
		ClassifyItems();
	}
	if (flag & (uint32_t)UpdateFlag::kCompatibility)
	{
		::Compatibility::GetSingleton()->Clear();
		::Compatibility::GetSingleton()->Load();
	}
}

void Settings::ClassifyItems()
{
	LOG_2("enter function");
	// resetting all items
	_itemsInit = false;

	_potionsWeak_main.clear();
	_potionsWeak_rest.clear();
	_potionsStandard_main.clear();
	_potionsStandard_rest.clear();
	_potionsPotent_main.clear();
	_potionsPotent_rest.clear();
	_potionsInsane_main.clear();
	_potionsInsane_rest.clear();
	_potionsBlood.clear();
	_poisonsWeak.clear();
	_poisonsStandard.clear();
	_poisonsPotent.clear();
	_poisonsInsane.clear();
	_foodall.clear();

	// set that holds the formids of all items that were excluded in earlier executions of this function
	// all items that have been excluded this way need to be removed from Distribution::_excludedItems since they
	// may depend on runtime values thaqt can be changed in the MCM
	static std::unordered_set<RE::FormID> priorexcluded;
	static std::unordered_set<RE::FormID> priorexcludedplayer;
	LOG_2("found {} prior excluded items and {} excluded for player only", priorexcluded.size(), priorexcludedplayer.size());
	// remove pror excluded items from global exclusion list
	for (auto id : priorexcluded)
	{
		Distribution::_excludedItems.erase(id);
	}
	for (auto id : priorexcludedplayer)
	{
		Distribution::_excludedItemsPlayer.erase(id);
	}
	priorexcluded.clear();
	priorexcludedplayer.clear();

	Data* data = Data::GetSingleton();
	data->ResetAlchItemEffects();
	data->ResetMagicItemPoisonResist();
	Comp* comp = Comp::GetSingleton();

	std::vector<std::tuple<std::string, std::string>> ingredienteffectmap;

	// start sorting items

	auto begin = std::chrono::steady_clock::now();
	const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
	{
		const RE::BSReadLockGuard locker{ lock };
		if (hashtable) {
			RE::AlchemyItem* item = nullptr;
			RE::IngredientItem* itemi = nullptr;
			for (auto& [id, form] : *hashtable) {
				if (form && form->IsMagicItem()) {
					item = form->As<RE::AlchemyItem>();
					if (item) {
						LOGL_4("Found AlchemyItem {}", Utility::PrintForm(item));
						// check for exclusion based on keywords
						if (item->HasKeyword(comp->NUP_ExcludeItem)) {
							EXCL("[Keyword Excl] Item:      {}", Utility::PrintForm<RE::AlchemyItem>(item));
							Distribution::_excludedItems.insert(item->GetFormID());
							continue;
						}

						// unnamed items cannot appear in anyones inventory normally so son't add them to our lists
						if (item->GetName() == nullptr || item->GetName() == (const char*)"" || strlen(item->GetName()) == 0 ||
							std::string(item->GetName()).find(std::string("Dummy")) != std::string::npos ||
							std::string(item->GetName()).find(std::string("dummy")) != std::string::npos) {
							EXCL("[Dummy Item] Item:        {}", Utility::PrintForm<RE::AlchemyItem>(item));
							Distribution::_excludedItems.insert(item->GetFormID());
							continue;
						}
						// check whether item is excluded, or whether it is not whitelisted when in whitelist mode
						// if it is excluded and whitelisted it is still excluded
						if (Distribution::excludedItems()->contains(item->GetFormID()) ||
							Settings::Whitelist::EnabledItems &&
								!Distribution::whitelistItems()->contains(item->GetFormID())) {
							EXCL("[Excluded Item] Item:     {}", Utility::PrintForm<RE::AlchemyItem>(item));
							continue;
						}
						// check whether the plugin is excluded
						if (Distribution::excludedPlugins()->contains(Utility::Mods::GetPluginIndex(item)) == true) {
							EXCL("[Excluded Plugin] Item:   {}", Utility::PrintForm<RE::AlchemyItem>(item));
							Distribution::_excludedItems.insert(item->GetFormID());
							continue;
						}
						// check whether the item is excluded from distrubtion only
						if (Distribution::excludedDistrItems()->contains(item->GetFormID())) {
							EXCL("Excluded Distr] Item:     {}", Utility::PrintForm<RE::AlchemyItem>(item));
							continue;
						}

						if (item->effects.size() > 0) {
							for (uint32_t i = 0; i < item->effects.size() && i < 4; i++) {
								auto sett = item->effects[i]->baseEffect;
								if (Distribution::excludedAssoc()->contains(sett->GetFormID()))
								{
									Distribution::_excludedItems.insert(item->GetFormID());
									EXCL("[Excluded] Item:   {}", Utility::PrintForm<RE::AlchemyItem>(item));
									continue;
								}
							}
						}

						auto clas = ClassifyItem(item);

						// there is a little bit of a problem for some items that have wrong flags and no keywords set. Try to detect them by sound and set the flags
						if (item->IsFood() == false && item->IsMedicine() == false && item->IsPoison() == false && item->HasKeyword(Settings::VendorItemFood) == false && item->HasKeyword(Settings::VendorItemFoodRaw) == false && item->HasKeyword(Settings::VendorItemPoison) == false && item->HasKeyword(Settings::VendorItemPotion) == false) {
							if (item->data.consumptionSound == Settings::FoodEat) {
								item->data.flags = RE::AlchemyItem::AlchemyFlag::kFoodItem | item->data.flags;
								//} else if (item->data.consumptionSound == Settings::PoisonUse) {
								//	item->data.flags = RE::AlchemyItem::AlchemyFlag::kPoison | item->data.flags;
							} else if (item->data.consumptionSound == Settings::PotionUse) {
								item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
							}
						}
						// set medicine flag for those who need it
						if (item->IsFood() == false && item->IsPoison() == false) {  //  && item->IsMedicine() == false
							item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
							if (Logging::EnableLoadLog && Logging::LogLevel >= 4) {
								//LOGL_1("Item: {}", Utility::PrintForm(item));
								if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kCostOverride)
									LOGL_1("\tFlag: CostOverride");
								if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kFoodItem)
									LOGL_1("\tFlag: FoodItem");
								if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kExtendDuration)
									LOGL_1("\tFlag: ExtendedDuration");
								if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kMedicine)
									LOGL_1("\tFlag: Medicine");
								if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kPoison)
									LOGL_1("\tFlag: Poison");
							}
							//LOGL_1("[AssignPotionFlag] {}", Utility::PrintForm(item));
						}
						// exclude item, if it has an alchemy effect that has been excluded
						bool excluded = false;
						AlchemicEffect effects = std::get<0>(clas);
						auto itr = Distribution::excludedEffects()->begin();
						while (itr != Distribution::excludedEffects()->end()) {
							if ((effects & *itr).IsValid()) {
								Distribution::_excludedItems.insert(item->GetFormID());
								priorexcluded.insert(item->GetFormID());
								excluded = true;
								EXCL("[Magic Effects] Item:     {}", Utility::PrintForm<RE::AlchemyItem>(item));
							}
							itr++;
						}
						if (excluded || Distribution::excludedItems()->contains(item->GetFormID())) {
							continue;
						}
						// check for AlchemicEffects exclusions based on user settings
						ItemType type = std::get<2>(clas);
						switch (type) {
						case ItemType::kPotion:
						case ItemType::kFortifyPotion:
							if ((effects & Potions::_prohibitedEffects).IsValid()) {
								// found effect that has been marked as excluded
								Distribution::_excludedItems.insert(item->GetFormID());
								priorexcluded.insert(item->GetFormID());
								excluded = true;
								EXCL("[Alchemic Effects] Item:  {}", Utility::PrintForm<RE::AlchemyItem>(item));
							}
							break;
						case ItemType::kPoison:
							if ((effects & Poisons::_prohibitedEffects).IsValid()) {
								// found effect that has been marked as excluded
								Distribution::_excludedItems.insert(item->GetFormID());
								priorexcluded.insert(item->GetFormID());
								excluded = true;
								EXCL("[Alchemic Effects] Item:  {}", Utility::PrintForm<RE::AlchemyItem>(item));
							}
							break;
						case ItemType::kFood:
							if ((effects & Food::_prohibitedEffects).IsValid()) {
								// found effect that has been marked as excluded
								Distribution::_excludedItems.insert(item->GetFormID());
								priorexcluded.insert(item->GetFormID());
								excluded = true;
								EXCL("[Alchemic Effects] Item:  {}", Utility::PrintForm<RE::AlchemyItem>(item));
							}
							break;
						}
						if (excluded)
							continue;

						// if the item has the ReflectDamage effect, with a strength of more than 50%, remove the item
						if ((std::get<0>(clas) & AlchemicEffect::kReflectDamage).IsValid()) {
							for (int i = 0; i < (int)item->effects.size(); i++) {
								if (item->effects[i]->baseEffect &&
									((ConvertToAlchemyEffectPrimary(item->effects[i]->baseEffect) == AlchemicEffect::kReflectDamage) ||
										(item->effects[i]->baseEffect->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (ConvertToAlchemyEffectSecondary(item->effects[i]->baseEffect) == AlchemicEffect::kReflectDamage)))) {
									if (item->effects[i]->effectItem.magnitude > 50) {
										Distribution::_excludedItems.insert(item->GetFormID());
										priorexcluded.insert(item->GetFormID());
										EXCL("[Reflect Damage] Item:    {}", Utility::PrintForm<RE::AlchemyItem>(item));
										continue;
									}
								}
							}
						}

						// check if item has known alcohol keywords and add it to list of alcohol
						if (comp->LoadedCACO() && item->HasKeyword(comp->CACO_VendorItemDrinkAlcohol) || comp->LoadedApothecary() && (item->HasKeyword(comp->Gour_FoodTypeAle) || item->HasKeyword(comp->Gour_FoodTypeDrugs) || item->HasKeyword(comp->Gour_FoodTypeWine))) {
							Distribution::_alcohol.insert(item->GetFormID());
						}

						// check for player excluded magiceffects
						for (int i = 0; i < (int)item->effects.size(); i++) {
							if (item->effects[i]->baseEffect && Distribution::excludedItemsPlayer()->contains(item->effects[i]->baseEffect->GetFormID())) {
								LOGL_1("Excluded {} for player due to effect", Utility::PrintForm(item));
								Distribution::_excludedItemsPlayer.insert(item->GetFormID());
								priorexcludedplayer.insert(item->GetFormID());
								EXCL("[Player Magic Eff] Item:  {}", Utility::PrintForm<RE::AlchemyItem>(item));
							}
						}

						// since the item is not to be excluded, save which alchemic effects are present
						_alchemyEffectsFound |= std::get<0>(clas);

						RE::Actor* player = RE::PlayerCharacter::GetSingleton();

						// if the value of the item is less than zero, we should not insert them into the distribution lists, since they are likely to be broken
						// or test/dummy items
						if (item->CalculateTotalGoldValue(player) > 0) {
							// determine the type of item
							if (std::get<2>(clas) == ItemType::kFood &&
								(Settings::Food::_AllowDetrimentalEffects || std::get<5>(clas) == false /*either we allow detrimental effects or there are none*/)) {
								_foodall.insert(_foodall.end(), { std::get<0>(clas), item });
								_foodEffectsFound |= std::get<0>(clas);
							} else if (std::get<2>(clas) == ItemType::kPoison &&
									   (Settings::Poisons::_AllowPositiveEffects || std::get<5>(clas) == false /*either we allow positive effects or there are none*/)) {
								switch (std::get<1>(clas)) {
								case ItemStrength::kWeak:
									_poisonsWeak.insert(_poisonsWeak.end(), { std::get<0>(clas), item });
									break;
								case ItemStrength::kStandard:
									_poisonsStandard.insert(_poisonsStandard.end(), { std::get<0>(clas), item });
									break;
								case ItemStrength::kPotent:
									_poisonsPotent.insert(_poisonsPotent.end(), { std::get<0>(clas), item });
									break;
								case ItemStrength::kInsane:
									_poisonsInsane.insert(_poisonsInsane.end(), { std::get<0>(clas), item });
									break;
								}
								_poisonEffectsFound |= std::get<0>(clas);
							} else if (std::get<2>(clas) == ItemType::kPotion &&
									   (Settings::Potions::_AllowDetrimentalEffects || std::get<5>(clas) == false /*either we allow detrimental effects or there are none*/)) {
								if ((std::get<0>(clas) & AlchemicEffect::kBlood) > 0)
									_potionsBlood.insert(_potionsBlood.end(), { std::get<0>(clas), item });
								else if ((std::get<0>(clas) & AlchemicEffect::kHealth) > 0 ||
										 (std::get<0>(clas) & AlchemicEffect::kMagicka) > 0 ||
										 (std::get<0>(clas) & AlchemicEffect::kStamina) > 0 ||
										 (std::get<0>(clas) & AlchemicEffect::kInvisibility) > 0) {
									switch (std::get<1>(clas)) {
									case ItemStrength::kWeak:
										_potionsWeak_main.insert(_potionsWeak_main.end(), { std::get<0>(clas), item });
										break;
									case ItemStrength::kStandard:
										_potionsStandard_main.insert(_potionsStandard_main.end(), { std::get<0>(clas), item });
										break;
									case ItemStrength::kPotent:
										_potionsPotent_main.insert(_potionsPotent_main.end(), { std::get<0>(clas), item });
										break;
									case ItemStrength::kInsane:
										_potionsInsane_main.insert(_potionsPotent_main.end(), { std::get<0>(clas), item });
										break;
									}
								} else if (std::get<0>(clas) != AlchemicEffect::kNone) {
									switch (std::get<1>(clas)) {
									case ItemStrength::kWeak:
										_potionsWeak_rest.insert(_potionsWeak_rest.end(), { std::get<0>(clas), item });
										break;
									case ItemStrength::kStandard:
										_potionsStandard_rest.insert(_potionsStandard_rest.end(), { std::get<0>(clas), item });
										break;
									case ItemStrength::kPotent:
										_potionsPotent_rest.insert(_potionsPotent_rest.end(), { std::get<0>(clas), item });
										break;
									case ItemStrength::kInsane:
										_potionsInsane_rest.insert(_potionsInsane_rest.end(), { std::get<0>(clas), item });
										break;
									}
								}
								_potionEffectsFound |= std::get<0>(clas);
							}
						} else {
							LOGL_1("Item {} has value 0 and will not be distributed", Utility::PrintForm(item));
						}
						int dosage = 0;
						if (item->IsPoison())
							dosage = Distribution::GetPoisonDosage(item, std::get<0>(clas));
						// add item into effect map
						data->SetAlchItemEffects(item->GetFormID(), std::get<0>(clas), std::get<3>(clas), std::get<4>(clas), std::get<5>(clas), dosage);
						LOGL_1("Saved effects for {} dur {} mag {} effect {}", Utility::PrintForm(item), std::get<3>(clas), std::get<4>(clas), std::get<0>(clas).string());
						data->SetMagicItemPoisonResist(item->GetFormID(), ACM::HasPoisonResistValue(item));
					}

					itemi = form->As<RE::IngredientItem>();
					if (itemi) {
						LOGL_4("Found IngredientItem {}", Utility::PrintForm(itemi));
						for (int i = 0; i < (int)itemi->effects.size(); i++) {
							auto sett = itemi->effects[i]->baseEffect;
							// just retrieve the effects, we will analyze them later
							if (sett) {
								ingredienteffectmap.push_back({ itemi->GetName(), Utility::ToString(ConvertToAlchemyEffectPrimary(sett)) });

								// the effects of ingredients may lead to valid potions being brewed, so we need to save that these effects actually exist in the game
								_alchemyEffectsFound |= ConvertToAlchemyEffectPrimary(sett);
							}
						}
					}
				}
			}
		}
	}
	PROF_1(TimeProfiling, "function execution time.");

	// add alcoholic items to player exclusion list
	if (Settings::Player::_DontDrinkAlcohol) {
		auto itr = Distribution::_alcohol.begin();
		while (itr != Distribution::_alcohol.end()) {
			Distribution::_excludedItemsPlayer.insert(*itr);
			priorexcludedplayer.insert(*itr);
			EXCL("[Player Alcoholic] Item:  {}", Utility::PrintForm<RE::AlchemyItem>(RE::TESForm::LookupByID<RE::AlchemyItem>(*itr)));
			itr++;
		}
	}

	// items initialised
	_itemsInit = true;

	LOGL_1("_potionsWeak_main {}", potionsWeak_main()->size());
	LOGL_1("_potionsWeak_rest {}", potionsWeak_rest()->size());
	LOGL_1("_potionsStandard_main {}", potionsStandard_main()->size());
	LOGL_1("_potionsStandard_rest {}", potionsStandard_rest()->size());
	LOGL_1("_potionsPotent_main {}", potionsPotent_main()->size());
	LOGL_1("_potionsPotent_rest {}", potionsPotent_rest()->size());
	LOGL_1("_potionsInsane_main {}", potionsInsane_main()->size());
	LOGL_1("_potionsInsane_rest {}", potionsInsane_rest()->size());
	LOGL_1("_potionsBlood {}", potionsBlood()->size());
	LOGL_1("_poisonsWeak {}", poisonsWeak()->size());
	LOGL_1("_poisonsStandard {}", poisonsStandard()->size());
	LOGL_1("_poisonsPotent {}", poisonsPotent()->size());
	LOGL_1("_poisonsInsane {}", poisonsInsane()->size());
	LOGL_1("_foodall {}", foodall()->size());

	if (Logging::EnableLoadLog && Logging::LogLevel >= 4) {
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\items.txt";
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::unordered_set<RE::FormID> visited;
		out << "potionsWeak_main\n";
		auto it = potionsWeak_main()->begin();
		while (it != potionsWeak_main()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "potionsStandard_main\n";
		it = potionsStandard_main()->begin();
		while (it != potionsStandard_main()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "potionsStandard_rest\n";
		it = potionsStandard_rest()->begin();
		while (it != potionsStandard_rest()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "potionsPotent_main\n";
		it = potionsPotent_main()->begin();
		while (it != potionsPotent_main()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "potionsInsane_main\n";
		it = potionsInsane_main()->begin();
		while (it != potionsInsane_main()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "potionsBlood\n";
		it = potionsBlood()->begin();
		while (it != potionsBlood()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "poisonsWeak\n";
		it = poisonsWeak()->begin();
		while (it != poisonsWeak()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "poisonsStandard\n";
		it = poisonsStandard()->begin();
		while (it != poisonsStandard()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "poisonsPotent\n";
		it = poisonsPotent()->begin();
		while (it != poisonsPotent()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "poisonsInsane\n";
		it = poisonsInsane()->begin();
		while (it != poisonsInsane()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "foodall\n";
		it = foodall()->begin();
		while (it != foodall()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}

		std::string pathing = "Data\\SKSE\\Plugins\\NPCsUsePotions\\ingredients.csv";
		std::ofstream outing = std::ofstream(pathing, std::ofstream::out);
		for (int i = 0; i < ingredienteffectmap.size(); i++) {
			outing << std::get<0>(ingredienteffectmap[i]) << ";" << std::get<1>(ingredienteffectmap[i]) << "\n";
		}
	}
	LOG_2("exit function");
}

std::tuple<AlchemicEffect, ItemStrength, ItemType, int, float, bool> Settings::ClassifyItem(RE::AlchemyItem* item)
{
	RE::EffectSetting* sett = nullptr;
	if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
		return { 0, ItemStrength::kStandard, ItemType::kFood, 0, 0.0f, false};
	}
	// we look at max 4 effects
	AlchemicEffect av[4]{
		0,
		0,
		0,
		0
	};
	float mag[]{
		0,
		0,
		0,
		0
	};
	int dur[]{
		0,
		0,
		0,
		0
	};
	bool detrimental = false;
	bool positive = false;
	// we will not abort the loop, since the number of effects on one item is normally very
	// limited, so we don't have much iterations
	AlchemicEffect tmp = 0;
	if (item->effects.size() > 0) {
		for (uint32_t i = 0; i < item->effects.size() && i < 4; i++) {
			sett = item->effects[i]->baseEffect;
			// just retrieve the effects, we will analyze them later
			if (sett) {
				mag[i] = item->effects[i]->effectItem.magnitude;
				dur[i] = item->effects[i]->effectItem.duration;

				// force area to zero, to avoid CTDs when using the item.
				item->effects[i]->effectItem.area = 0;

				detrimental |= sett->IsDetrimental();
				positive |= !sett->IsDetrimental();

				uint32_t formid = sett->GetFormID();
				if ((tmp = (ConvertToAlchemyEffectPrimary(sett))) > 0) {

					av[i] |= tmp;
				}
				if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = ConvertToAlchemyEffectSecondary(sett)) > 0) {

					av[i] |= tmp;
				}
				// we only need this for magnitude calculations, so its not used as cooldown
				if (dur[i] == 0)
					dur[i] = 1;
			}
		}
	} else {
		// emergency fallback // more or less unused
		RE::MagicItem::SkillUsageData err;
		item->GetSkillUsageData(err);
		detrimental |= item->avEffectSetting->IsDetrimental();
		positive |= !item->avEffectSetting->IsDetrimental();
		switch (item->avEffectSetting->data.primaryAV) {
		case RE::ActorValue::kHealth:
			av[0] = ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV);
			mag[0] = err.magnitude;
			dur[0] = 1;
			break;
		case RE::ActorValue::kMagicka:
			av[0] = ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV);
			mag[0] = err.magnitude;
			dur[0] = 1;
			break;
		case RE::ActorValue::kStamina:
			av[0] = ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV);
			mag[0] = err.magnitude;
			dur[0] = 1;
			break;
		}
	}
	// analyze the effect types
	AlchemicEffect alch = 0;
	ItemStrength str = ItemStrength::kWeak;
	float maxmag = 0;
	int maxdur = 0;
	for (int i = 0; i < 4; i++) {
		if (mag[i] == 0)
			mag[i] = 1;
		if (dur[i] == 0)
			dur[i] = 1;
		if (mag[i] * dur[i] > maxmag) {
			maxmag = mag[i] * dur[i];
			maxdur = dur[i];
		}
		alch |= av[i];
	}
	if (std::string(item->GetName()).find(std::string("Weak")) != std::string::npos)
		str = ItemStrength::kWeak;
	else if (std::string(item->GetName()).find(std::string("Standard")) != std::string::npos)
		str = ItemStrength::kStandard;
	else if (std::string(item->GetName()).find(std::string("Potent")) != std::string::npos)
		str = ItemStrength::kPotent;
	else if (maxmag == 0)
		str = ItemStrength::kStandard;
	else if (maxmag <= Distr::_MaxMagnitudeWeak)
		str = ItemStrength::kWeak;
	else if (maxmag <= Distr::_MaxMagnitudeStandard)
		str = ItemStrength::kStandard;
	else if (maxmag <= Distr::_MaxMagnitudePotent)
		str = ItemStrength::kPotent;
	else
		str = ItemStrength::kInsane;
	auto iter = Distribution::itemStrengthMap()->find(item->GetFormID());
	if (iter != Distribution::itemStrengthMap()->end()) {
		str = iter->second;
	}

	// if the potion is a blood potion it should only ever appear on vampires, no the
	// effects are overriden to AlchemyEffect::kBlood
	if (std::string(item->GetName()).find(std::string("Blood")) != std::string::npos &&
		std::string(item->GetName()).find(std::string("Potion")) != std::string::npos) {
		alch = AlchemicEffect::kBlood;
		// if we have a blood potion, make sure that it has the medicine flag
		if (item->IsMedicine() == false)
			item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;

	}

	ItemType type = ItemType::kPotion;
	if (item->IsFood() || item->HasKeyword(VendorItemFood) || item->HasKeyword(VendorItemFoodRaw))
		type = ItemType::kFood;
	else if (item->IsPoison() || item->HasKeyword(Settings::VendorItemPoison)) {
		type = ItemType::kPoison;
		return {
			alch,
			str,
			type,
			maxdur,
			maxmag,
			positive // return whether there is a positive effect on the poison
		};
	}

	return {
		alch,
		str,
		type,
		maxdur,
		maxmag,
		detrimental
	};
}

void Settings::CleanAlchemyEffects()
{
	std::vector<AlchemicEffect> effectsToRemovePotion;
	std::vector<AlchemicEffect> effectsToRemovePoison;
	std::vector<AlchemicEffect> effectsToRemoveFood;
	// iterate over existing alchemy effects
	for (uint64_t i = 0; i <= 127; i++) {
		AlchemicEffect eff = AlchemicEffect(0, 1) << i;
		LOG_5("[DDD]X {}", eff.string());
		// potion
		if ((_potionEffectsFound & (eff)).IsValid() && Distribution::excludedEffects()->contains(eff) == false) {
			// found existing effect, which is not excluded
		} else {
			// effect excluded or not present in any items
			// remove from all distribution rules
			effectsToRemovePotion.push_back(eff);
		}
		// poison
		if ((_poisonEffectsFound & (eff)).IsValid() && Distribution::excludedEffects()->contains(eff) == false) {
			// found existing effect, which is not excluded
		} else {
			// effect excluded or not present in any items
			// remove from all distribution rules
			effectsToRemovePoison.push_back(eff);
		}
		// food
		if ((_foodEffectsFound & (eff)).IsValid() && Distribution::excludedEffects()->contains(eff) == false) {
			// found existing effect, which is not excluded
		} else {
			// effect excluded or not present in any items
			// remove from all distribution rules
			effectsToRemoveFood.push_back(eff);
		}
	}

	// iterate over all rules
	auto itr = Distribution::rules()->begin();
	while (itr != Distribution::rules()->end()) {
		// potion
		for (int i = 0; i < effectsToRemovePotion.size(); i++) {
			(*itr)->RemoveAlchemyEffectPotion(effectsToRemovePotion[i]);
			(*itr)->RemoveAlchemyEffectFortifyPotion(effectsToRemovePotion[i]);
			LOGL_3("Removed AlchemyEffect {} from potions in rule {}. ID: {}", Utility::ToString(effectsToRemovePotion[i]), (*itr)->ruleName, effectsToRemovePotion[i].string());
		} // poison
		for (int i = 0; i < effectsToRemovePoison.size(); i++) {
			(*itr)->RemoveAlchemyEffectPoison(effectsToRemovePoison[i]);
			LOGL_3("Removed AlchemyEffect {} from poisons in rule {}. ID: {}", Utility::ToString(effectsToRemovePoison[i]), (*itr)->ruleName, effectsToRemovePoison[i].string());
		}
		// food
		for (int i = 0; i < effectsToRemoveFood.size(); i++) {
			(*itr)->RemoveAlchemyEffectFood(effectsToRemoveFood[i]);
			LOGL_3("Removed AlchemyEffect {} from food in rule {}. ID: {}", Utility::ToString(effectsToRemoveFood[i]), (*itr)->ruleName, effectsToRemoveFood[i].string());
		}
		itr++;
	}

	for (uint64_t i = 0; i <= effectsToRemovePotion.size(); i++) {
		LOG_5("[DDD]P {}", effectsToRemovePotion[i].string());
	}
	for (uint64_t i = 0; i <= effectsToRemovePoison.size(); i++) {
		LOG_5("[DDD]S {}", effectsToRemovePoison[i].string());
	}
	for (uint64_t i = 0; i <= effectsToRemoveFood.size(); i++) {
		LOG_5("[DDD]F {}", effectsToRemoveFood[i].string());
	}
}

#pragma endregion
