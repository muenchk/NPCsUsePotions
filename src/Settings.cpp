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
#include "ActorManipulation.h"

using ActorStrength = Settings::ActorStrength;
using AlchemyEffect = Settings::AlchemyEffect;
using ItemStrength = Settings::ItemStrength;
using ItemType = Settings::ItemType;

static std::mt19937 randi((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
/// <summary>
/// trims random numbers to 1 to RR
/// </summary>
static std::uniform_int_distribution<signed> randRR(1, RandomRange);
static std::uniform_int_distribution<signed> rand100(1, 100);

#pragma region Settings

void Settings::LoadDistrConfig()
{
	std::vector<std::string> files;
	auto constexpr folder = R"(Data\SKSE\Plugins\)";
	for (const auto& entry : std::filesystem::directory_iterator(folder)) {
		if (entry.exists() && !entry.path().empty() && entry.path().extension() == ".ini") {
			if (auto path = entry.path().string(); path.rfind("NUP_DIST") != std::string::npos) {
				files.push_back(path);
				logger::info("[SETTINGS] [LoadDistrRules] found Distribution configuration file: {}", entry.path().filename().string());
			}
		}
	}
	if (files.empty()) {
		logger::info("[SETTINGS] [LoadDistrRules] No Distribution files were found");
	}
	// init datahandler
	auto datahandler = RE::TESDataHandler::GetSingleton();

	// vector of splits, filename and line
	std::vector<std::tuple<std::vector<std::string>*, std::string, std::string>> attachments;

	// extract the rules from all files
	for (std::string file : files) {
		try {
			std::ifstream infile(file);
			if (infile.is_open()) {
				std::string line;
				std::string tmp;
				while (std::getline(infile, line)) {
					tmp = line;
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
						line.erase(0, pos+1);
						pos = line.find("|");
					}
					if (line.length() != 0)
						splits->push_back(line);
					int splitindex = 0;
					// check wether we actually have a rule
					if (splits->size() < 3) {  // why 3? Cause first two fields are RuleVersion and RuleType and we don't accept empty rules.
						logger::warn("[Settings] [LoadDistrRules] Not a rule. file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// check what rule version we have
					int ruleVersion = -1;
					try {
						ruleVersion = std::stoi(splits->at(splitindex));
						splitindex++;
					} catch (std::out_of_range&) {
						logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"RuleVersion\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					} catch (std::invalid_argument&) {
						logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RuleVersion\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// check what kind of rule we have
					int ruleType = -1;
					try {
						ruleType = std::stoi(splits->at(splitindex));
						splitindex++;
					} catch (std::out_of_range&) {
						logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"RuleType\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					} catch (std::invalid_argument&) {
						logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RuleType\". file: {}, rule:\"{}\"", file, tmp);
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
									if (splits->size() != 25) {
										logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 25. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the rulename, so we just set it
									Distribution::Rule* rule = new Distribution::Rule();
									rule->ruleVersion = ruleVersion;
									rule->ruleType = ruleType;
									rule->ruleName = splits->at(splitindex);
									LOGE1_2("[Settings] [LoadDistrRules] loading rule: {}", rule->ruleName);
									splitindex++;
									// now come the rule priority
									rule->rulePriority = -1;
									try {
										rule->rulePriority = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes maxPotions
									rule->maxPotions = -1;
									try {
										rule->maxPotions = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion1Chance
									rule->potion1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion1Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Potion1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion2Chance
									rule->potion2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion2Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Potion2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion3Chance
									rule->potion3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion3Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Potion3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PotionAddChance
									rule->potionAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potionAdditionalChance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"PotionAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PotionsTierAdjust
									rule->potionTierAdjust = -1;
									try {
										rule->potionTierAdjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify1Chance
									rule->fortify1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify1Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Fortify1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify2Chance
									rule->fortify2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify2Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Fortify2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes maxPoisons
									rule->maxPoisons = -1;
									try {
										rule->maxPoisons = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison1Chance
									rule->poison1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison1Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Poison1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison2Chance
									rule->poison2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison2Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Poison2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison3Chance
									rule->poison3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison3Chance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"Poison3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PoisonAddChance
									rule->poisonAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poisonAdditionalChance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"PoisonAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PoisonsTierAdjust
									rule->poisonTierAdjust = -1;
									try {
										rule->poisonTierAdjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									// now comes FoodChance
									rule->foodChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->foodChance.size() == 0) {
										logger::warn("[Settings] [LoadDistrRules] fiels \"FoodChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									// get strings for the properties
									rule->assocObjects = splits->at(splitindex);
									splitindex++;
									rule->potionProperties = splits->at(splitindex);
									splitindex++;
									rule->fortifyproperties = splits->at(splitindex);
									splitindex++;
									rule->poisonProperties = splits->at(splitindex);
									splitindex++;
									rule->foodProperties = splits->at(splitindex);
									splitindex++;


									if (splits->at(splitindex) == "1")
										rule->allowMixed = true;
									else
										rule->allowMixed = false;
									splitindex++;

									bool error = false;

									// parse the associated objects
									std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> objects = Utility::ParseAssocObjects(rule->assocObjects, error, file, line);
									
									// parse the item properties
									std::vector<std::tuple<uint64_t, float>> potioneffects = Utility::ParseAlchemyEffects(rule->potionProperties, error);
									rule->potionDistr = Utility::GetDistribution(potioneffects, RandomRange);
									LOGE2_2("[Settings] [LoadDistrRules] rule {} contains {} potion effects", rule->ruleName, rule->potionDistr.size());
									rule->validPotions = Utility::SumAlchemyEffects(rule->potionDistr);
									std::vector<std::tuple<uint64_t, float>> poisoneffects = Utility::ParseAlchemyEffects(rule->poisonProperties, error);
									rule->poisonDistr = Utility::GetDistribution(poisoneffects, RandomRange);
									LOGE2_2("[Settings] [LoadDistrRules] rule {} contains {} poison effects", rule->ruleName, rule->poisonDistr.size());
									rule->validPoisons = Utility::SumAlchemyEffects(rule->poisonDistr);
									std::vector<std::tuple<uint64_t, float>> fortifyeffects = Utility::ParseAlchemyEffects(rule->fortifyproperties, error);
									rule->fortifyDistr = Utility::GetDistribution(fortifyeffects, RandomRange);
									LOGE2_2("[Settings] [LoadDistrRules] rule {} contains {} fortify potion effects", rule->ruleName, rule->fortifyDistr.size());
									rule->validFortifyPotions = Utility::SumAlchemyEffects(rule->fortifyDistr);
									std::vector<std::tuple<uint64_t, float>> foodeffects = Utility::ParseAlchemyEffects(rule->foodProperties, error);
									rule->foodDistr = Utility::GetDistribution(foodeffects, RandomRange);
									LOGE2_2("[Settings] [LoadDistrRules] rule {} contains {} food effects", rule->ruleName, rule->foodDistr.size());
									rule->validFood = Utility::SumAlchemyEffects(rule->foodDistr);

									// assign rules to search parameters
									LOGE2_2("[Settings] [LoadDistrRules] rule {} contains {} associated objects", rule->ruleName, objects.size());
									for (int i = 0; i < objects.size(); i++) {
										switch (std::get<0>(objects[i])) {
										case Settings::Distribution::AssocType::kFaction:
											if (auto item = Settings::Distribution::factionMap.find(std::get<1>(objects[i])); item != Settings::Distribution::factionMap.end()) {
												if (item->second->rulePriority < rule->rulePriority)
													Settings::Distribution::factionMap.insert_or_assign(std::get<1>(objects[i]), rule);
											} else {
												Settings::Distribution::factionMap.insert_or_assign(std::get<1>(objects[i]), rule);
											}
											break;
										case Settings::Distribution::AssocType::kKeyword:
											if (auto item = Settings::Distribution::keywordMap.find(std::get<1>(objects[i])); item != Settings::Distribution::keywordMap.end()) {
												if (item->second->rulePriority < rule->rulePriority)
													Settings::Distribution::keywordMap.insert_or_assign(std::get<1>(objects[i]), rule);
											} else {
												Settings::Distribution::keywordMap.insert_or_assign(std::get<1>(objects[i]), rule);
											}
											break;
										case Settings::Distribution::AssocType::kNPC:
										case Settings::Distribution::AssocType::kActor:
											if (auto item = Settings::Distribution::npcMap.find(std::get<1>(objects[i])); item != Settings::Distribution::npcMap.end()) {
												if (item->second->rulePriority < rule->rulePriority)
													Settings::Distribution::npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
											} else {
												Settings::Distribution::npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
											}
											break;
										case Settings::Distribution::AssocType::kRace:
											if (auto item = Settings::Distribution::raceMap.find(std::get<1>(objects[i])); item != Settings::Distribution::raceMap.end()) {
												if (item->second->rulePriority < rule->rulePriority)
													Settings::Distribution::raceMap.insert_or_assign(std::get<1>(objects[i]), rule);
											} else {
												Settings::Distribution::raceMap.insert_or_assign(std::get<1>(objects[i]), rule);
											}
											break;
										}
									}
									// add rule to the list of rules and we are finished! probably.
									Settings::Distribution::rules.push_back(rule);
									if (rule->ruleName == DefaultRuleName)
										Settings::Distribution::defaultRule = rule;
									delete splits;
									LOGE1_2("[Settings] [LoadDistrRules] rule {} successfully loaded.", rule->ruleName);
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
										logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kActor ||
											std::get<0>(items[i]) == Settings::Distribution::AssocType::kNPC)
											Distribution::bosses.insert(std::get<1>(items[i]));
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 4: // exclude object
								{
									if (splits->size() != 3) {
										logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kActor ||
											std::get<0>(items[i]) == Settings::Distribution::AssocType::kNPC) {
											Distribution::excludedNPCs.insert(std::get<1>(items[i]));
											LOGE1_2("[Settings] [LoadDistrRules] excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
										}
										else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kFaction) {
											RE::TESFaction* temp = Utility::GetTESForm(datahandler, std::get<1>(items[i]), "", "")->As<RE::TESFaction>();
											if (temp) {
												Distribution::excludedFactions.insert(temp);
												LOGE1_2("[Settings] [LoadDistrRules] excluded faction {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
										} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kKeyword) {
											RE::BGSKeyword* temp = Utility::GetTESForm(datahandler, std::get<1>(items[i]), "", "")->As<RE::BGSKeyword>();
											if (temp) {
												Distribution::excludedKeywords.insert(temp);
												LOGE1_2("[Settings] [LoadDistrRules] excluded keyword {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
										} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kItem) {
											Distribution::excludedItems.insert(std::get<1>(items[i]));
											LOGE1_2("[Settings] [LoadDistrRules] excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
										} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kRace) {
											Distribution::excludedRaces.insert(std::get<1>(items[i]));
											LOGE1_2("[Settings] [LoadDistrRules] excluded race {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 5: // exclude baseline
								{
									if (splits->size() != 3) {
										logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kFaction) {
											RE::TESFaction* temp = Utility::GetTESForm(datahandler, std::get<1>(items[i]), "", "")->As<RE::TESFaction>();
											if (temp) {
												Distribution::excludedFactions.insert(temp);
												LOGE1_2("[Settings] [LoadDistrRules] excluded faction {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
										} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kKeyword) {
											RE::BGSKeyword* temp = Utility::GetTESForm(datahandler, std::get<1>(items[i]), "", "")->As<RE::BGSKeyword>();
											if (temp) {
												Distribution::excludedKeywords.insert(temp);
												LOGE1_2("[Settings] [LoadDistrRules] excluded keyword {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
										} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kItem) {
											Distribution::excludedItems.insert(std::get<1>(items[i]));
											LOGE1_2("[Settings] [LoadDistrRules] excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
										} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kRace) {
											Distribution::excludedRaces.insert(std::get<1>(items[i]));
											LOGE1_2("[Settings] [LoadDistrRules] excluded race {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							default:
								logger::warn("[Settings] [LoadDistrRules] Rule type does not exist. file: {}, rule:\"{}\"", file, tmp);
								delete splits;
								break;
							}
							break;
						}
					default:
						logger::warn("[Settings] [LoadDistrRules] Rule version does not exist. file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						break;
					}
				}

			} else {
				logger::warn("[Settings] [LoadDistrRules] file {} couldn't be read successfully", file);
			}

		} catch (std::exception&) {
			logger::warn("[Settings] [LoadDistrRules] file {} couldn't be read successfully due to an error", file);
		}
	}

	// create default rule if there is none
	if (Settings::Distribution::defaultRule == nullptr) {
		Settings::Distribution::defaultRule = new Settings::Distribution::Rule(1 /*version*/, 1 /*type*/, DefaultRuleName, INT_MIN + 1 /*rulePriority*/, true /*allowMixed*/, 5 /*maxPotions*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*potion1Chance*/,
			std::vector<int>{ 20, 30, 40, 50, 60 } /*potion2Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*potion3Chance*/, std::vector<int>{ 0, 10, 20, 30, 40 } /*potionAddChance*/,
			std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify1Chance*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify2Chance*/, 0 /*potionTierAdjust*/, 5 /*maxPoisons*/,
			std::vector<int>{ 30, 35, 40, 45, 50 } /*poison1Chance*/, std::vector<int>{ 20, 25, 30, 35, 40 } /*poison2Chance*/, std::vector<int>{ 10, 15, 20, 25, 30 } /*poison3Chance*/,
			std::vector<int>{ 0, 5, 10, 15, 20 } /*poisonAddChance*/, 0 /*poisonTierAdjust*/, std::vector<int>{ 70, 80, 90, 100, 100 } /*foodChance*/,
			Settings::Distribution::GetVector(RandomRange, Settings::AlchemyEffect::kAnyPotion) /*potionDistr*/,
			Settings::Distribution::GetVector(RandomRange, Settings::AlchemyEffect::kAnyPoison) /*poisonDistr*/,
			Settings::Distribution::GetVector(RandomRange, Settings::AlchemyEffect::kAnyFortify) /*fortifyDistr*/,
			Settings::Distribution::GetVector(RandomRange, Settings::AlchemyEffect::kAnyFood) /*foodDistr*/,
			static_cast<uint64_t>(Settings::AlchemyEffect::kAnyPotion) /*validPotions*/,
			static_cast<uint64_t>(Settings::AlchemyEffect::kAnyPoison) /*validPoisons*/,
			static_cast<uint64_t>(Settings::AlchemyEffect::kAnyFortify) /*validFortifyPotions*/,
			static_cast<uint64_t>(Settings::AlchemyEffect::kAnyFood) /*validFood*/);
	}

	// and now for the attachement rules.
	// 
	// vector of splits, filename and line
	//std::vector<std::tuple<std::vector<std::string>*, std::string, std::string>> attachments;
	if (attachments.size() > 0) {
		std::string name;
		for (auto a : attachments) {
			// first two splits are version and type which are already confirmed, so just process the last two.
			// 3rd split is the name of the rule, which the objects in the 4th split are attached to
			if (std::get<0>(a)->size() == 4) {
				{
					// valid rule
					name = (std::get<0>(a))->at(2);
					// parse the associated objects
					bool error = false;
					std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> objects = Utility::ParseAssocObjects((std::get<0>(a)->at(3)), error, std::get<1>(a), std::get<2>(a));
					Settings::Distribution::Rule* rule = Distribution::FindRule(name);
					if (rule == nullptr)
						continue; // rule doesn't exist, evaluate next attachment
					// assign rules to search parameters
					for (int i = 0; i < objects.size(); i++) {
						switch (std::get<0>(objects[i])) {
						case Settings::Distribution::AssocType::kFaction:
							if (auto item = Settings::Distribution::factionMap.find(std::get<1>(objects[i])); item != Settings::Distribution::factionMap.end()) {
								if (item->second->rulePriority < rule->rulePriority) {
									Settings::Distribution::factionMap.insert_or_assign(std::get<1>(objects[i]), rule);
									LOGE2_2("[Settings] [LoadDistrRules] updated Faction {} to rule {}.", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
								}
							} else {
								Settings::Distribution::factionMap.insert_or_assign(std::get<1>(objects[i]), rule);
								LOGE2_2("[Settings] [LoadDistrRules] attached Faction {} to rule {}.", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
							}
							break;
						case Settings::Distribution::AssocType::kKeyword:
							if (auto item = Settings::Distribution::keywordMap.find(std::get<1>(objects[i])); item != Settings::Distribution::keywordMap.end()) {
								if (item->second->rulePriority < rule->rulePriority) {
									Settings::Distribution::keywordMap.insert_or_assign(std::get<1>(objects[i]), rule);
									LOGE2_2("[Settings] [LoadDistrRules] updated Keyword {} to rule {}.", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
								}
							} else {
								Settings::Distribution::keywordMap.insert_or_assign(std::get<1>(objects[i]), rule);
								LOGE2_2("[Settings] [LoadDistrRules] attached Keyword {} to rule {}.", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
							}
							break;
						case Settings::Distribution::AssocType::kNPC:
						case Settings::Distribution::AssocType::kActor:
							if (auto item = Settings::Distribution::npcMap.find(std::get<1>(objects[i])); item != Settings::Distribution::npcMap.end()) {
								if (item->second->rulePriority < rule->rulePriority) {
									Settings::Distribution::npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
									LOGE2_2("[LoadDistrRules] updated Actor {} to rule {}.", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
								}
							} else {
								Settings::Distribution::npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
								LOGE2_2("[Settings] [LoadDistrRules] attached Actor {} to rule {}.", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
							}
							break;
						case Settings::Distribution::AssocType::kRace:
							if (auto item = Settings::Distribution::raceMap.find(std::get<1>(objects[i])); item != Settings::Distribution::raceMap.end()) {
								if (item->second->rulePriority < rule->rulePriority) {
									Settings::Distribution::raceMap.insert_or_assign(std::get<1>(objects[i]), rule);
									LOGE2_2("[Settings] [LoadDistrRules] updated Race {} to rule {}.",Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
								}
							} else {
								Settings::Distribution::raceMap.insert_or_assign(std::get<1>(objects[i]), rule);
								LOGE2_2("[Settings] [LoadDistrRules] attached Race {} to rule {}.",Utility::GetHex(std::get<1>(objects[i])), rule->ruleName);
							}
							break;
						}
					}
				}
			} else {
				// rule invalid
				logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 4. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
				// delet splits since we don't need it anymore
				delete std::get<0>(a);
			}
		}
	}


	// load our stuff like necessary forms
	// get VendorItemPotion keyword, if we don't find this, potion detection will be nearly impossible
	Settings::VendorItemPotion = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEC);
	if (Settings::VendorItemPotion == nullptr) {
		logger::info("[INIT] Couldn't find VendorItemPotion Keyword in game.");
	}
	Settings::VendorItemPoison = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDED);
	if (Settings::VendorItemPoison == nullptr) {
		logger::info("[INIT] Couldn't find VendorItemPoison Keyword in game.");
	}
	Settings::VendorItemFood = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEA);
	if (Settings::VendorItemFood == nullptr) {
		logger::info("[INIT] Couldn't find VendorItemFood Keyword in game.");
	}
	Settings::VendorItemFoodRaw = RE::TESForm::LookupByID<RE::BGSKeyword>(0x000A0E56);
	if (Settings::VendorItemFoodRaw == nullptr) {
		logger::info("[INIT] Couldn't find VendorItemFoodRaw Keyword in game.");
	}
	Settings::CurrentFollowerFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0005C84E);
	if (Settings::CurrentFollowerFaction == nullptr) {
		logger::info("[INIT] Couldn't find CurrentFollowerFaction Faction in game.");
	}


	// handle standard exclusions
	RE::TESForm* tmp = nullptr;

	/// EXCLUDE ITEMS
	
	// MQ201Drink (don't give quest items out)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00036D53, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// Unknown Potion with unknown effect (in-game type)
	if ((tmp = Utility::GetTESForm(datahandler, 0x0005661F, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// Kordirs skooma: its probably kordirs
	if ((tmp = Utility::GetTESForm(datahandler, 0x00057A7B, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// Stallion's potion: its probably stallions
	if ((tmp = Utility::GetTESForm(datahandler, 0x0005566A, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// DB03Poison (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00058CFB, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// DA16TorporPotion (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00005F6DF, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// IVDGhostPotion (special item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000663E1, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// DummyPotion
	if ((tmp = Utility::GetTESForm(datahandler, 0x0006A07E, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// TG00FalmerBlood - Falmer Blood Elixier
	// DA14Water - Holy Water
	// TGTQ02BalmoraBlue (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000DC172, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// CW01BWraithPoison (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000E2D3D, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// Blades Potion: probably esberns
	if ((tmp = Utility::GetTESForm(datahandler, 0x000E6DF5, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// MS14WineAltoA: probably jessica's 
	if ((tmp = Utility::GetTESForm(datahandler, 0x000F257E, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x00102019, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201A, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201B, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201C, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201D, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201E, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// DA03FoodMammothMeat (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010211A, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// Mq101JuniperMead (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00107A8A, "", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());

	// DLC1FoodSoulHusk
	if ((tmp = Utility::GetTESForm(datahandler, 0x014DC4, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());
	// DLC1FoodSoulHuskExtract
	if ((tmp = Utility::GetTESForm(datahandler, 0x015A1E, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedItems.insert(tmp->GetFormID());


	/// EXCLUDE SUMMONS

	// DA14Summoned
	if ((tmp = Utility::GetTESForm(datahandler, 0x0001F3AA, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlame
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C0, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrost
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C1, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStorm
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C2, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlamePotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E940, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E943, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E944, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// EncSummonFamiliar
	if ((tmp = Utility::GetTESForm(datahandler, 0x000640B5, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// MGArnielSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0006A152, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonPhantom
	if ((tmp = Utility::GetTESForm(datahandler, 0x00072310, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedSkeleton01Missile
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007503C, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlameThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87D, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87E, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87F, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonFireStorm
	if ((tmp = Utility::GetTESForm(datahandler, 0x000877EB, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonFelldir
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923F9, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonGormlaith
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923FA, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonHakon
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923FB, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// MGRDremoraSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00099F2F, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonFlamingThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0009CE28, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedCreature
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CC5A2, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlameThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECC, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECD, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECE, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// HowlSummonWolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CF79E, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// dunFortSnowhawkSummonedSkeleton01
	if ((tmp = Utility::GetTESForm(datahandler, 0x000D8D95, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedSkeleton01Melee1HShield
	if ((tmp = Utility::GetTESForm(datahandler, 0x000F90BC, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonEncDremoraLord
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010DDEE, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DA14DremoraSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010E38B, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostNPC
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE43, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormNPC
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE45, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostNPCPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE46, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormNPCPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE47, "", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnWrathmanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B4, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnMistmanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B7, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnBonemanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B9, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1HowlSummonIceWolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x008A6C, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1HowlSummonWerewolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x008A6D, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC01SoulCairnHorseSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00BDD0, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1VQ05BonemanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00BFF0, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon1
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16A, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon2
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16B, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon3
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16C, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshGuardian
	if ((tmp = Utility::GetTESForm(datahandler, 0x0177B6, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshSpawn01
	if ((tmp = Utility::GetTESForm(datahandler, 0x01CDF8, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshGuardianNeloth
	if ((tmp = Utility::GetTESForm(datahandler, 0x01DBDC, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonTrollFrost
	if ((tmp = Utility::GetTESForm(datahandler, 0x01DFA1, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonSeeker
	if ((tmp = Utility::GetTESForm(datahandler, 0x01EEC9, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2dunKarstaagSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x024811, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonSeekerHigh
	if ((tmp = Utility::GetTESForm(datahandler, 0x030CDE, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonWerebear
	if ((tmp = Utility::GetTESForm(datahandler, 0x0322B3, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
	// DLC2dunKarstaagIceWraithSummoned
	if ((tmp = Utility::GetTESForm(datahandler, 0x034B5A, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::excludedNPCs.insert(tmp->GetFormID());
		



	// template:
	//if ((tmp = Utility::GetTESForm(datahandler, 0, "", "")) != nullptr)
	//	Settings::Distribution::excludedItems.insert(tmp->GetFormID());
}

void Settings::CheckActorsForRules()
{
	logger::info("[CheckActorsForRules] checking...");
	std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_NPCs_without_Rule.csv");
	out << "PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
	//PluginBase;

	auto datahandler = RE::TESDataHandler::GetSingleton();
	std::string_view name = std::string_view{ "" };

	auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
	auto end = hashtable->end();
	auto iter = hashtable->begin();
	std::set<RE::FormID> visited;
	RE::Actor* act = nullptr;
	while (iter != hashtable->end()) {
		try {
			if ((*iter).second) {
				act = (*iter).second->As<RE::Actor>();
				if (act) {
					if (!visited.contains(act->GetFormID())) {
						// lookup pluing of the actor base
						/* name = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetActorBase()->GetFormID() >> 24))->GetFilename();
					if (name.empty())
						name = datahandler->LookupLoadedLightModByIndex((uint16_t)((act->GetActorBase()->GetFormID() << 8)) >> 20)->GetFilename();
					if (name.empty() == false)
						out << name << ";";
					else
						out << ";";*/

						// we didn't consider the current actors base so far
						visited.insert(act->GetFormID());

						// check wether there is a rule that applies
						if (Utility::ExcludedNPC(act))
							continue;  // the npc is covered by an exclusion
						// get rule
						Settings::Distribution::Rule* rl = Settings::Distribution::CalcRule(act);
						//logger::warn("[CheckActorsForRules] got rule");
						if (rl->ruleName == DefaultRuleName) {
							// lookup plugin of the actor red
							name = std::string_view{ "" };
							if ((act->GetFormID() >> 24) != 0xFE)
								name = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24))->GetFilename();
							if (name.empty())
								name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() << 8)) >> 20))->GetFilename();
							if (name.empty() == false)
								out << name << ";";
							else
								out << ";";
							// we found an actor that does not have a rule, so print that to the output
							out << act->GetName() << ";"
								<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
								<< "0x" << Utility::GetHex(act->GetFormID()) << ";";
							//logger::warn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							if (act->GetRace())
								out << act->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
							else
								out << ";;";
							//logger::warn("[CheckActorsForRules] Race: {} {}", act->GetRace()->GetFormEditorID(), Utility::GetHex(act->GetRace()->GetFormID()));
							if (act->GetSaveParentCell())
								out << act->GetSaveParentCell()->GetName();

							//logger::warn("[CheckActorsForRules] factions");
							act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
								if (a_faction)
									out << ";"
										<< "0x" << Utility::GetHex(a_faction->GetFormID());
								//logger::warn("[CheckActorsForRules] Faction: {}", Utility::GetHex(a_faction->GetFormID()));
								return false;
							});
							//logger::warn("[CheckActorsForRules] end");
							out << "\n";
							out.flush();
						}
					}
				}
			}
		}
		catch (std::exception&) {
			//logger::warn("catch");
			out << ";";
		}
		try {
			iter++;
		}
		catch (std::exception&) {
			//logger::warn("catch finished");
			break;
		}
	}
	logger::info("[CheckActorsForRules] finished checking...");
}

#pragma endregion

#pragma region Rule

// private

Settings::AlchemyEffect Settings::Distribution::Rule::GetRandomEffect(Settings::ItemType type)
{
	int random = randRR(randi);
	switch (type) {
	case Settings::ItemType::kPotion:
		for (int i = 0; i < this->potionDistr.size(); i++) {
			if (random <= std::get<0>(this->potionDistr[i])) {
				return std::get<1>(this->potionDistr[i]);
			}
		}
		break;
	case Settings::ItemType::kPoison:
		for (int i = 0; i < this->poisonDistr.size(); i++) {
			if (random <= std::get<0>(this->poisonDistr[i])) {
				return std::get<1>(this->poisonDistr[i]);
			}
		}
		break;
	case Settings::ItemType::kFortifyPotion:
		for (int i = 0; i < this->fortifyDistr.size(); i++) {
			if (random <= std::get<0>(this->fortifyDistr[i])) {
				return std::get<1>(this->fortifyDistr[i]);
			}
		}
		break;
	case Settings::ItemType::kFood:
		for (int i = 0; i < this->foodDistr.size(); i++) {
			if (random <= std::get<0>(this->foodDistr[i])) {
				return std::get<1>(this->foodDistr[i]);
			}
		}
		break;
	}
	return Settings::AlchemyEffect::kNone;
}

RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion1(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potion1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion2(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potion2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion3(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potion3Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotionAdditional(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= potionAdditionalChance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPotion(int str)
{
	str += potionTierAdjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto eff = GetRandomEffect(ItemType::kPotion);
	if (eff == AlchemyEffect::kBlood) {
		items = Settings::GetMatchingItems(_potionsBlood, static_cast<uint64_t>(eff));
	} else {
	RetryPotion:
		switch (str) {
		case 1:  // weak
			items = Settings::GetMatchingItems(Settings::_potionsWeak_main, static_cast<uint64_t>(eff));
			break;
		case 2:  // standard
		items = Settings::GetMatchingItems(Settings::_potionsStandard_main, static_cast<uint64_t>(eff));
		if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
		}
		break;
	case 3:  // potent
		items = Settings::GetMatchingItems(Settings::_potionsPotent_main, static_cast<uint64_t>(eff));
		if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
		}
		break;
	case 4:  // insane
		items = Settings::GetMatchingItems(Settings::_potionsInsane_main, static_cast<uint64_t>(eff));
		if (items.size() == 0) {
				str -= 1;
				goto RetryPotion;
		}
		break;
		}
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison1(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poison1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison2(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poison2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison3(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poison3Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoisonAdditional(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= poisonAdditionalChance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomPoison(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomPoison(int str)
{
	str += poisonTierAdjust;
	if (str < 1)
		str = 1;
	if (str > 4)
		str = 4;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
RetryPoison:
	switch (str) {
	case 1:  // weak
		items = Settings::GetMatchingItems(Settings::_poisonsWeak, static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		break;
	case 2:  // standard
		items = Settings::GetMatchingItems(Settings::_poisonsStandard, static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryPoison;
		}
		break;
	case 3:  // potent
		items = Settings::GetMatchingItems(Settings::_poisonsPotent, static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryPoison;
		}
		break;
	case 4:  // insane
		items = Settings::GetMatchingItems(Settings::_poisonsInsane, static_cast<uint64_t>(GetRandomEffect(ItemType::kPoison)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryPoison;
		}
		break;
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion1(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= fortify1Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomFortifyPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion2(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	int str = static_cast<int>(strength);
	if (rand100(randi) <= fortify2Chance[astr]) {
		// we rolled successfully
	} else {
		return nullptr;
	}
	return GetRandomFortifyPotion(str);
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFortifyPotion(int str)
{
	// get matching items
	std::vector<RE::AlchemyItem*> items;
RetryFortify:
	switch (str) {
	case 1:  // weak
		items = Settings::GetMatchingItems(Settings::_potionsWeak_rest, static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		break;
	case 2:  // standard
		items = Settings::GetMatchingItems(Settings::_potionsStandard_rest, static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryFortify;
		}
		break;
	case 3:  // potent
		items = Settings::GetMatchingItems(Settings::_potionsPotent_rest, static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryFortify;
		}
		break;
	case 4:  // insane
		items = Settings::GetMatchingItems(Settings::_potionsInsane_rest, static_cast<uint64_t>(GetRandomEffect(ItemType::kFortifyPotion)));
		if (items.size() == 0) {
			str -= 1;
			goto RetryFortify;
		}
		break;
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}
RE::AlchemyItem* Settings::Distribution::Rule::GetRandomFood_intern(Settings::ItemStrength, Settings::ActorStrength acstrength)
{
	// if the rule is invalid (empty) return
	if (valid == false)
		return nullptr;
	int astr = static_cast<int>(acstrength);
	if (rand100(randi) <= foodChance[astr]) {
		// we rolled successfully
	} else
		return nullptr;
	// get matching items
	std::vector<RE::AlchemyItem*> items;
	auto reff = GetRandomEffect(ItemType::kFood);
	switch (reff) {
	case AlchemyEffect::kMagicka:
	case AlchemyEffect::kMagickaRate:
	case AlchemyEffect::kMagickaRateMult:
		items = Settings::GetMatchingItems(Settings::_foodmagicka, static_cast<uint64_t>(reff));
		break;
	case AlchemyEffect::kHealRate:
	case AlchemyEffect::kHealRateMult:
	case AlchemyEffect::kHealth :
		items = Settings::GetMatchingItems(Settings::_foodhealth, static_cast<uint64_t>(reff));
		break;
	case AlchemyEffect::kStamina:
	case AlchemyEffect::kStaminaRate:
	case AlchemyEffect::kStaminaRateMult:
		items = Settings::GetMatchingItems(Settings::_foodstamina, static_cast<uint64_t>(reff));
		break;
	}
	// return random item
	std::uniform_int_distribution<signed> r(0, (int)(items.size()) - 1);
	if (items.size() > 0)
		return items[r(randi)];
	return nullptr;
}

// public

std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomPotions(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomPotion1(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPotion2(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPotion3(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	while (ret.size() <= this->maxPotions && (tmp = GetRandomPotionAdditional(strength, acstrength)) != nullptr) {
		ret.push_back(tmp);
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomPoisons(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomPoison1(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPoison2(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomPoison3(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	while (ret.size() <= this->maxPoisons && (tmp = GetRandomPoisonAdditional(strength, acstrength)) != nullptr) {
		ret.push_back(tmp);
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomFortifyPotions(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	std::vector<RE::AlchemyItem*> ret;
	auto tmp = GetRandomFortifyPotion1(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	tmp = GetRandomFortifyPotion2(strength, acstrength);
	if (tmp)
		ret.push_back(tmp);
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::Rule::GetRandomFood(Settings::ItemStrength strength, Settings::ActorStrength acstrength)
{
	return std::vector<RE::AlchemyItem*>{ GetRandomFood_intern(strength, acstrength) };
}

#pragma endregion

#pragma region Distribution

void Settings::Distribution::CalcStrength(RE::Actor* actor, ActorStrength& acs, ItemStrength& is)
{
	if (_GameDifficultyScaling) {
		// 0 novice, 1 apprentice, 2 adept, 3 expert, 4 master, 5 legendary
		auto diff = RE::PlayerCharacter::GetSingleton()->difficulty;
		if (diff == 0 || diff == 1) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
		} else if (diff == 2 || diff == 3) {
			acs = Settings::ActorStrength::Normal;
			is = Settings::ItemStrength::kStandard;
		} else if (diff == 4) {
			acs = Settings::ActorStrength::Powerful;
			is = Settings::ItemStrength::kPotent;
		} else { // diff == 5
			acs = Settings::ActorStrength::Insane;
			is = Settings::ItemStrength::kInsane;
		}
		// get boss override
		if (Distribution::bosses.contains(actor->GetFormID()) || Distribution::bosses.contains(actor->GetActorBase()->GetFormID())) {
			acs = ActorStrength::Boss;
			LOG1_3("{}[GetDistrPotions] Found boss actor {}", std::to_string(actor->GetFormID()));
		}

	} else {
		// get level dependencies
		short lvl = actor->GetLevel();
		if (lvl <= _LevelEasy) {
			acs = Settings::ActorStrength::Weak;
			is = Settings::ItemStrength::kWeak;
			// weak actor
		} else if (lvl <= _LevelNormal) {
			acs = ActorStrength::Normal;
			is = ItemStrength::kStandard;
			// normal actor
		} else if (lvl <= _LevelDifficult) {
			acs = ActorStrength::Powerful;
			is = ItemStrength::kPotent;
			// difficult actor
		} else if (lvl <= _LevelInsane) {
			acs = ActorStrength::Insane;
			is = ItemStrength::kInsane;
			// insane actor
		} else {
			acs = ActorStrength::Boss;
			is = ItemStrength::kInsane;
			// boss actor
		}
		// get boss override
		if (Distribution::bosses.contains(actor->GetFormID()) || Distribution::bosses.contains(actor->GetActorBase()->GetFormID())) {
			acs = ActorStrength::Boss;
			LOG1_3("{}[GetDistrPotions] Found boss actor {}", std::to_string(actor->GetFormID()));
		}
	}
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrItems(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	CalcStrength(actor, acs, is);
	Rule* rule = CalcRule(actor);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		auto ritems = rule->GetRandomPotions(is, acs);
		LOG_4("{}[GetDistrItems] matching potions");
		auto items = ACM::GetMatchingPotions(actor, rule->validPotions);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] potions size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
	}
	if (Settings::_featDistributePoisons) {
		auto ritems = rule->GetRandomPoisons(is, acs);
		LOG_4("{}[GetDistrItems] matching poisons");
		auto items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] poisons size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		auto ritems = rule->GetRandomFortifyPotions(is, acs);
		LOG_4("{}[GetDistrItems] matching fortify");
		auto items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] fortify size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
	}
	if (Settings::_featDistributeFood) {
		auto ritems = rule->GetRandomFood(is, acs);
		LOG_4("{}[GetDistrItems] matching food");
		auto items = ACM::GetMatchingFood(actor, rule->validFood, false);
		int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
		LOG1_4("{}[GetDistrItems] diff: {}", diff);
		// if the number of found items is less then the number of items to add
		// then add the difference in numbers
		if (diff > 0) {
			LOG_4("{}[GetDistrItems] diff greater 0");
			ritems.resize(diff);
			LOG1_4("{}[GetDistrItems] food size: {}", std::to_string(ritems.size()));
			ret.insert(ret.end(), ritems.begin(), ritems.end());
		}
	}
	if (ret.size() > 0 && ret.back() == nullptr) {
		LOG_4("{}[GetDistrItems] remove last item");
		ret.pop_back();
	}
	return ret;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrPotions(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	CalcStrength(actor, acs, is);
	Rule* rule = CalcRule(actor);
	auto ritems = rule->GetRandomPotions(is, acs);
	auto items = ACM::GetMatchingPotions(actor, rule->validPotions);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrPoisons(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	CalcStrength(actor, acs, is);
	Rule* rule = CalcRule(actor);
	auto ritems = rule->GetRandomPoisons(is, acs);
	auto items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrFortifyPotions(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	CalcStrength(actor, acs, is);
	Rule* rule = CalcRule(actor);
	auto ritems = rule->GetRandomFortifyPotions(is, acs);
	auto items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}
std::vector<RE::AlchemyItem*> Settings::Distribution::GetDistrFood(RE::Actor* actor)
{
	Settings::ActorStrength acs = Settings::ActorStrength::Normal;
	Settings::ItemStrength is = Settings::ItemStrength::kStandard;
	CalcStrength(actor, acs, is);
	Rule* rule = CalcRule(actor);
	auto ritems = rule->GetRandomFood(is, acs);
	auto items = ACM::GetMatchingFood(actor, rule->validFood, false);
	int64_t diff = (int64_t)(ritems.size()) - (int64_t)(items.size());
	// if number of items to add is lesser equal the number of already present items
	// return an empty list
	if (diff <= 0)
		return std::vector<RE::AlchemyItem*>{};
	ritems.resize(diff);
	return ritems;
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetMatchingInventoryItemsUnique(RE::Actor* actor)
{
	Rule* rule = CalcRule(actor);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validPotions);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingFood(actor, rule->validFood, false);
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0 && ret.back() == nullptr)
		ret.pop_back();
	return ret;
}

std::vector<RE::AlchemyItem*> Settings::Distribution::GetMatchingInventoryItems(RE::Actor* actor)
{
	//logger::info("GetMatchingInventoryItems enter");
	Rule* rule = CalcRule(actor);
	std::vector<RE::AlchemyItem*> ret;
	if (Settings::_featDistributePotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validPotions);
		//logger::info("GetMatchingInventoryItems| potions {} | found: {}", Utility::GetHex(rule->validPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributePoisons) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPoisons(actor, rule->validPoisons);
		//logger::info("GetMatchingInventoryItems| poisons {} | found: {}", Utility::GetHex(rule->validPoisons), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFortifyPotions) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingPotions(actor, rule->validFortifyPotions);
		//logger::info("GetMatchingInventoryItems| fortify {} | found: {}", Utility::GetHex(rule->validFortifyPotions), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (Settings::_featDistributeFood) {
		std::list<std::tuple<float, int, RE::AlchemyItem*, Settings::AlchemyEffect>> items = ACM::GetMatchingFood(actor, rule->validFood, false);
		//logger::info("GetMatchingInventoryItems| food {} | found: {}", Utility::GetHex(rule->validFood), items.size());
		for (auto i : items) {
			ret.insert(ret.end(), std::get<2>(i));
		}
	}
	if (ret.size() != 0) {
		if (ret.back() == nullptr)
			ret.pop_back();
		auto map = actor->GetInventoryCounts();
		size_t currsize = ret.size();
		for (int i = 0; i < currsize; i++) {
			if (auto it = map.find(ret[i]); it != map.end()) {
				if (it->second > 1)
					for (int c = 1; c < it->second; c++)
						ret.push_back(ret[i]);
			}
		}
	}
	return ret;
}

Settings::Distribution::Rule* Settings::Distribution::CalcRule(RE::Actor* actor)
{
	bool baseexcluded = false;

	std::vector<Rule*> rls;
	// find rule in npc map
	// npc rules always have the highest priority
	auto it = npcMap.find(actor->GetFormID());
	if (it != npcMap.end()) // found the right rule!
		return it->second; // this can be null if the specific npc is excluded
	// now also perform a check on the actor base
	it = npcMap.find(actor->GetActorBase()->GetFormID());
	if (it != npcMap.end())  // found the right rule!
		return it->second;   // this can be null if the specific npc is excluded
	// now that we didnt't find something so far, check the rest
	// this time all the priorities are the same
	auto base = actor->GetActorBase();
	it = raceMap.find(base->GetRace()->GetFormID());
	if (it != raceMap.end())
		rls.push_back(it->second);
	else {
		baseexcluded |= baselineExclusions.contains(base->GetRace()->GetFormID());
	}
	// handle keywords
	for (unsigned int i = 0; i < base->GetNumKeywords(); i++) {
		auto opt = base->GetKeywordAt(i);
		if (opt.has_value()) {
			it = keywordMap.find((*opt)->GetFormID());
			if (it != keywordMap.end())
				rls.push_back(it->second);
			else {
				baseexcluded |= baselineExclusions.contains((*opt)->GetFormID());
			}
		}
	}
	// handle factions
	actor->VisitFactions([&rls, &baseexcluded](RE::TESFaction* a_faction, std::int8_t) {
		//logger::info("[CalcRule] faction visited: {}", a_faction->GetName());
		auto ite = factionMap.find(a_faction->GetFormID());
		if (ite != factionMap.end())
			rls.push_back(ite->second);
		else {
			baseexcluded |= baselineExclusions.contains(a_faction->GetFormID());
		}
		return false;
	});
	// now get the rule with the highest priority
	int prio = INT_MIN;
	int index = -1;
	for (int i = 0; i < rls.size(); i++) {
		if (rls[i]->rulePriority > prio) {
			prio = rls[i]->rulePriority;
			index = i;
		}
	}
	if (index > -1) {
		// return rule
		LOG1_1("{}[CalcRule] rule found: {}", rls[index]->ruleName);
		return rls[index];
	} else {
		// there are no rules!!!
		if (baseexcluded)
			return Settings::Distribution::emptyRule;
		LOG2_1("{}[CalcRule] default rule found: {}, {}", Settings::Distribution::defaultRule->ruleName, rls.size());
		return Settings::Distribution::defaultRule;
	}
}

#pragma endregion
