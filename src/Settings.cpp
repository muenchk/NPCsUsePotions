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
	// set to false, to avoid other funcions running stuff on our variables
	Settings::Distribution::initialised = false;

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
	std::vector<std::tuple<std::vector<std::string>*, std::string, std::string>> copyrules;

	// extract the rules from all files
	for (std::string file : files) {
		try {
			std::ifstream infile(file);
			if (infile.is_open()) {
				std::string line;
				while (std::getline(infile, line)) {
					std::string tmp(line);
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

									std::pair<int, Settings::Distribution::Rule*> tmptuple = { rule->rulePriority, rule };

									// assign rules to search parameters
									LOGE2_2("[Settings] [LoadDistrRules] rule {} contains {} associated objects", rule->ruleName, objects.size());
									for (int i = 0; i < objects.size(); i++) {
										switch (std::get<0>(objects[i])) {
										case Settings::Distribution::AssocType::kFaction:
										case Settings::Distribution::AssocType::kCombatStyle:
										case Settings::Distribution::AssocType::kClass:
										case Settings::Distribution::AssocType::kRace:
										case Settings::Distribution::AssocType::kKeyword:
											if (auto item = Settings::Distribution::_assocMap.find(std::get<1>(objects[i])); item != Settings::Distribution::_assocMap.end()) {
												if (std::get<1>(item->second)->rulePriority < rule->rulePriority)
													Settings::Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
											} else {
												Settings::Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
											}
											break;
										case Settings::Distribution::AssocType::kNPC:
										case Settings::Distribution::AssocType::kActor:
											if (auto item = Settings::Distribution::_npcMap.find(std::get<1>(objects[i])); item != Settings::Distribution::_npcMap.end()) {
												if (item->second->rulePriority < rule->rulePriority)
													Settings::Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule );
											} else {
												Settings::Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
											}
											break;
										}
									}
									// add rule to the list of rules and we are finished! probably.
									Settings::Distribution::_rules.push_back(rule);
									if (rule->ruleName == DefaultRuleName && (Settings::Distribution::defaultRule == nullptr ||
																				 rule->rulePriority > Settings::Distribution::defaultRule->rulePriority))
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
											std::get<0>(items[i]) == Settings::Distribution::AssocType::kNPC ||
											std::get<0>(items[i]) == Settings::Distribution::AssocType::kFaction ||
											std::get<0>(items[i]) == Settings::Distribution::AssocType::kKeyword ||
											std::get<0>(items[i]) == Settings::Distribution::AssocType::kRace) {
											Distribution::_bosses.insert(std::get<1>(items[i]));
											LOGE1_2("[Settings] [LoadDistrRules] declared {} as boss.", Utility::GetHex(std::get<1>(items[i])));
										}
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
										switch (std::get<0>(items[i])) {
										case Settings::Distribution::AssocType::kActor:
										case Settings::Distribution::AssocType::kNPC:
											Distribution::_excludedNPCs.insert(std::get<1>(items[i]));
											break;
										case Settings::Distribution::AssocType::kFaction:
										case Settings::Distribution::AssocType::kKeyword:
										case Settings::Distribution::AssocType::kRace:
											Distribution::_excludedAssoc.insert(std::get<1>(items[i]));
											break;
										case Settings::Distribution::AssocType::kItem:
											Distribution::_excludedItems.insert(std::get<1>(items[i]));
											break;
										}
										if (EnableLog) {
											if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kActor ||
												std::get<0>(items[i]) == Settings::Distribution::AssocType::kNPC) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded actor {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kFaction) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded faction {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kKeyword) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded keyword {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kItem) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kRace) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded race {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
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
										switch (std::get<0>(items[i])) {
										case Settings::Distribution::AssocType::kFaction:
										case Settings::Distribution::AssocType::kKeyword:
										case Settings::Distribution::AssocType::kRace:
											Distribution::_baselineExclusions.insert(std::get<1>(items[i]));
											break;
										}

										if (EnableLog) {
											if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kFaction) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded faction {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kKeyword) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded keyword {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) == Settings::Distribution::AssocType::kRace) {
												LOGE1_2("[Settings] [LoadDistrRules] excluded race {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 6:
								{
									copyrules.push_back({ splits, file, tmp });
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

	if (copyrules.size() > 0) {
		for (auto cpy : copyrules) {
			auto splits = std::get<0>(cpy);
			if (splits->size() != 5) {
				logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 5. file: {}, rule:\"{}\", fields: {}", std::get<1>(cpy), std::get<2>(cpy), splits->size());
				continue;
			}
			std::string name = (splits)->at(2);
			std::string newname = (splits)->at(3);
			Settings::Distribution::Rule* rule = Distribution::FindRule(name);
			Settings::Distribution::Rule* newrule = rule->Clone();
			newrule->ruleName = newname;
			int prio = INT_MIN;
			try {
				prio = std::stoi(splits->at(4));
			} catch (std::out_of_range&) {
				logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
				prio = INT_MIN;
			} catch (std::invalid_argument&) {
				logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
				prio = INT_MIN;
			}
			if (prio != INT_MIN)
				newrule->rulePriority = prio;
			Settings::Distribution::_rules.push_back(newrule);
			if (newname == DefaultRuleName && (Settings::Distribution::defaultRule == nullptr ||
												  newrule->rulePriority > Settings::Distribution::defaultRule->rulePriority))
				Settings::Distribution::defaultRule = newrule;
			delete splits;
			LOGE1_2("[Settings] [LoadDistrRules] rule {} successfully coinialised.", newrule->ruleName);
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
					Settings::Distribution::Rule* rule = Distribution::FindRule(name);
					if (rule == nullptr) {
						logger::warn("[Settings] [LoadDistrRules] rule not found. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
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
							logger::warn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
							continue;
						} catch (std::invalid_argument&) {
							logger::warn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
							continue;
						}
					}


					// parse the associated objects
					bool error = false;
					std::vector<std::tuple<Settings::Distribution::AssocType, RE::FormID>> objects = Utility::ParseAssocObjects((std::get<0>(a)->at(3)), error, std::get<1>(a), std::get<2>(a));

					std::pair<int, Settings::Distribution::Rule*> tmptuple = { prio, rule };
					// assign rules to search parameters
					bool attach = false; // loop intern
					int oldprio = INT_MIN;
					for (int i = 0; i < objects.size(); i++) {
						switch (std::get<0>(objects[i])) {
						case Settings::Distribution::AssocType::kFaction:
						case Settings::Distribution::AssocType::kKeyword:
						case Settings::Distribution::AssocType::kRace:
						case Settings::Distribution::AssocType::kClass:
						case Settings::Distribution::AssocType::kCombatStyle:
							if (auto item = Settings::Distribution::_assocMap.find(std::get<1>(objects[i])); item != Settings::Distribution::_assocMap.end()) {
								if ((oldprio = std::get<1>(item->second)->rulePriority) < rule->rulePriority) {
									Settings::Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
									attach = false;
								}
							} else {
								Settings::Distribution::_assocMap.insert_or_assign(std::get<1>(objects[i]), tmptuple);
								attach = true;
							}
							break;
						case Settings::Distribution::AssocType::kNPC:
						case Settings::Distribution::AssocType::kActor:
							if (auto item = Settings::Distribution::_npcMap.find(std::get<1>(objects[i])); item != Settings::Distribution::_npcMap.end()) {
								if ((oldprio = item->second->rulePriority) < rule->rulePriority) {
									Settings::Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
									attach = false;
								}
							} else {
								Settings::Distribution::_npcMap.insert_or_assign(std::get<1>(objects[i]), rule);
								attach = true;
							}
							break;
						}
						if (EnableLog) {
							switch (std::get<0>(objects[i])) {
							case Settings::Distribution::AssocType::kFaction:
								if (attach) {
									LOGE3_2("[Settings] [LoadDistrRules] attached Faction {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else 
									LOGE5_2("[Settings] [LoadDistrRules] updated Faction {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Settings::Distribution::AssocType::kKeyword:
								if (attach) {
									LOGE3_2("[Settings] [LoadDistrRules] attached Keyword {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGE5_2("[Settings] [LoadDistrRules] updated Keyword {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Settings::Distribution::AssocType::kRace:
								if (attach) {
									LOGE3_2("[Settings] [LoadDistrRules] attached Race {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGE5_2("[Settings] [LoadDistrRules] updated Race {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Settings::Distribution::AssocType::kClass:
								if (attach) {
									LOGE3_2("[Settings] [LoadDistrRules] attached Class {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGE5_2("[Settings] [LoadDistrRules] updated Class {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Settings::Distribution::AssocType::kCombatStyle:
								if (attach) {
									LOGE3_2("[Settings] [LoadDistrRules] attached CombatStyle {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGE5_2("[Settings] [LoadDistrRules] updated CombatStyle {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Settings::Distribution::AssocType::kNPC:
							case Settings::Distribution::AssocType::kActor:
								if (attach) {
									LOGE3_2("[Settings] [LoadDistrRules] attached Actor {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGE5_2("[Settings] [LoadDistrRules] updated Actor {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							}
						}

					}
				}
			} else {
				// rule invalid
				logger::warn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 4 or 5. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
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
	Settings::CurrentHirelingFaction = RE::TESForm::LookupByID<RE::TESFaction>(0xbd738);
	if (Settings::CurrentHirelingFaction == nullptr) {
		logger::info("[INIT] Couldn't find CurrentHirelingFaction Faction in game.");
	}

	// hard exclude everyone that may become a follower
	//Settings::Distribution::_excludedAssoc.insert(0x0005C84E);

	// exclude currenhireling faction
	//Settings::Distribution::_excludedAssoc.insert(0xbd738);

	Settings::AlchemySkillBoosts = RE::TESForm::LookupByID<RE::BGSPerk>(0xA725C);
	if (Settings::AlchemySkillBoosts == nullptr)
		logger::info("[INIT] Couldn't find AlchemySkillBoosts Perk in game.");
	Settings::PerkSkillBoosts = RE::TESForm::LookupByID<RE::BGSPerk>(0xCF788);
	if (Settings::PerkSkillBoosts == nullptr)
		logger::info("[INIT] Couldn't find PerkSkillBoosts Perk in game.");




	// handle standard exclusions
	RE::TESForm* tmp = nullptr;

	/// EXCLUDE ITEMS
	
	// MQ201Drink (don't give quest items out)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00036D53, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// Unknown Potion with unknown effect (in-game type)
	if ((tmp = Utility::GetTESForm(datahandler, 0x0005661F, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// Kordirs skooma: its probably kordirs
	if ((tmp = Utility::GetTESForm(datahandler, 0x00057A7B, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// Stallion's potion: its probably stallions
	if ((tmp = Utility::GetTESForm(datahandler, 0x0005566A, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// DB03Poison (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00058CFB, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// DA16TorporPotion (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00005F6DF, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// IVDGhostPotion (special item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000663E1, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// DummyPotion
	if ((tmp = Utility::GetTESForm(datahandler, 0x0006A07E, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// TG00FalmerBlood - Falmer Blood Elixier
	// DA14Water - Holy Water
	// TGTQ02BalmoraBlue (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000DC172, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// CW01BWraithPoison (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x000E2D3D, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// Blades Potion: probably esberns
	if ((tmp = Utility::GetTESForm(datahandler, 0x000E6DF5, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// MS14WineAltoA: probably jessica's 
	if ((tmp = Utility::GetTESForm(datahandler, 0x000F257E, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x00102019, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201A, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201B, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201C, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201D, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201E, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// DA03FoodMammothMeat (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010211A, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// Mq101JuniperMead (quest item)
	if ((tmp = Utility::GetTESForm(datahandler, 0x00107A8A, "", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());

	// DLC1FoodSoulHusk
	if ((tmp = Utility::GetTESForm(datahandler, 0x014DC4, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());
	// DLC1FoodSoulHuskExtract
	if ((tmp = Utility::GetTESForm(datahandler, 0x015A1E, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedItems.insert(tmp->GetFormID());


	/// EXCLUDE SUMMONS

	// DA14Summoned
	if ((tmp = Utility::GetTESForm(datahandler, 0x0001F3AA, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlame
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C0, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrost
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C1, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStorm
	if ((tmp = Utility::GetTESForm(datahandler, 0x000204C2, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlamePotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E940, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E943, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0004E944, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// EncSummonFamiliar
	if ((tmp = Utility::GetTESForm(datahandler, 0x000640B5, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// MGArnielSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0006A152, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonPhantom
	if ((tmp = Utility::GetTESForm(datahandler, 0x00072310, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedSkeleton01Missile
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007503C, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlameThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87D, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87E, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0007E87F, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonFireStorm
	if ((tmp = Utility::GetTESForm(datahandler, 0x000877EB, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonFelldir
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923F9, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonGormlaith
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923FA, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonHakon
	if ((tmp = Utility::GetTESForm(datahandler, 0x000923FB, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// MGRDremoraSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00099F2F, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonFlamingThrall
	if ((tmp = Utility::GetTESForm(datahandler, 0x0009CE28, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedCreature
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CC5A2, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFlameThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECC, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECD, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormThrallPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CDECE, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// HowlSummonWolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x000CF79E, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunFortSnowhawkSummonedSkeleton01
	if ((tmp = Utility::GetTESForm(datahandler, 0x000D8D95, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// dunSummonedSkeleton01Melee1HShield
	if ((tmp = Utility::GetTESForm(datahandler, 0x000F90BC, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonEncDremoraLord
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010DDEE, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DA14DremoraSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010E38B, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostNPC
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE43, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormNPC
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE45, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachFrostNPCPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE46, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// SummonAtronachStormNPCPotent
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010EE47, "", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnWrathmanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B4, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnMistmanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B7, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1SoulCairnBonemanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x0045B9, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1HowlSummonIceWolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x008A6C, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1HowlSummonWerewolf
	if ((tmp = Utility::GetTESForm(datahandler, 0x008A6D, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC01SoulCairnHorseSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00BDD0, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1VQ05BonemanSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x00BFF0, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon1
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16A, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon2
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16B, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC1EncUndeadSummon3
	if ((tmp = Utility::GetTESForm(datahandler, 0x01A16C, "Dawnguard.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshGuardian
	if ((tmp = Utility::GetTESForm(datahandler, 0x0177B6, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshSpawn01
	if ((tmp = Utility::GetTESForm(datahandler, 0x01CDF8, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonAshGuardianNeloth
	if ((tmp = Utility::GetTESForm(datahandler, 0x01DBDC, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonTrollFrost
	if ((tmp = Utility::GetTESForm(datahandler, 0x01DFA1, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonSeeker
	if ((tmp = Utility::GetTESForm(datahandler, 0x01EEC9, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2dunKarstaagSummon
	if ((tmp = Utility::GetTESForm(datahandler, 0x024811, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonSeekerHigh
	if ((tmp = Utility::GetTESForm(datahandler, 0x030CDE, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2SummonWerebear
	if ((tmp = Utility::GetTESForm(datahandler, 0x0322B3, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
	// DLC2dunKarstaagIceWraithSummoned
	if ((tmp = Utility::GetTESForm(datahandler, 0x034B5A, "Dragonborn.esm", "")) != nullptr)
		Settings::Distribution::_excludedNPCs.insert(tmp->GetFormID());
		
	// EXCLUDED FACTIONS


	// template:
	//if ((tmp = Utility::GetTESForm(datahandler, 0, "", "")) != nullptr)
	//	Settings::Distribution::_excludedItems.insert(tmp->GetFormID());

	if (Settings::_ApplySkillBoostPerks)
		Settings::ApplySkillBoostPerks();

	Settings::Distribution::initialised = true;

	if (Settings::EnableLog) {
		logger::info("[Settings] [LoadDistrRules] Number of Rules: {}", Distribution::rules()->size());
		logger::info("[Settings] [LoadDistrRules] Number of NPCs: {}", Distribution::npcMap()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of NPCs: {}", Distribution::npcMap()->bucket_count());
		logger::info("[Settings] [LoadDistrRules] Number of Associations: {}", Distribution::assocMap()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of Associations: {}", Distribution::assocMap()->bucket_count());
		logger::info("[Settings] [LoadDistrRules] Number of Bosses: {}", Distribution::bosses()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of Bosses: {}", Distribution::bosses()->bucket_count());
		logger::info("[Settings] [LoadDistrRules] Number of Excluded NPCs: {}", Distribution::excludedNPCs()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of Excluded NPCs: {}", Distribution::excludedNPCs()->bucket_count());
		logger::info("[Settings] [LoadDistrRules] Number of Excluded Associations: {}", Distribution::excludedAssoc()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of Excluded Associations: {}", Distribution::excludedAssoc()->bucket_count());
		logger::info("[Settings] [LoadDistrRules] Number of Excluded Items: {}", Distribution::excludedItems()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of Excluded Items: {}", Distribution::excludedItems()->bucket_count());
		logger::info("[Settings] [LoadDistrRules] Number of Baseline Exclusions: {}", Distribution::baselineExclusions()->size());
		logger::info("[Settings] [LoadDistrRules] Buckets of Baseline Exclusions: {}", Distribution::baselineExclusions()->bucket_count());
		/*for (int i = 0; i < Settings::Distribution::_rules.size(); i++) {
			logger::info("rule {} pointer {}", i, Utility::GetHex((uintptr_t)Settings::Distribution::_rules[i]));
		}
		auto iter = Settings::Distribution::_assocMap.begin();
		while (iter != Settings::Distribution::_assocMap.end()) {
			logger::info("assoc\t{}\trule\t{}", Utility::GetHex(iter->first), Utility::GetHex((uintptr_t)(std::get<1>(iter->second))));
			iter++;
		}*/
	}
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
		;  //logger::info("template invalid");
	return false;
}

Settings::Distribution::NPCTPLTInfo Settings::Distribution::ExtractTemplateInfo(RE::TESLevCharacter* lvl)
{
	if (lvl == nullptr)
		return Settings::Distribution::NPCTPLTInfo{};
	// just try to grab the first entry of the leveled list, since they should all share
	// factions 'n stuff
	if (lvl->entries.size() > 0) {
		RE::TESForm* entry = lvl->entries[0].form;
		RE::TESNPC* tplt = entry->As<RE::TESNPC>();
		RE::TESLevCharacter* lev = entry->As<RE::TESLevCharacter>();
		if (tplt)
			return ExtractTemplateInfo(tplt);
		else if (lev)
			return ExtractTemplateInfo(lev);
		else
			;  //logger::info("template invalid");
	}
	return Settings::Distribution::NPCTPLTInfo{};
}

Settings::Distribution::NPCTPLTInfo Settings::Distribution::ExtractTemplateInfo(RE::TESNPC* npc)
{
	Settings::Distribution::NPCTPLTInfo info;
	if (npc == nullptr)
		return info; 
	if (npc->baseTemplateForm == nullptr) {
		// we are at the base, so do the main work
		info.tpltrace = npc->GetRace();
		info.tpltstyle = npc->combatStyle;
		info.tpltclass = npc->npcClass;
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				info.tpltkeywords.push_back(npc->keywords[i]);
		}
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				info.tpltfactions.push_back(npc->factions[i].faction);
		}
		return info;
	}
	RE::TESNPC* tplt = npc->baseTemplateForm->As<RE::TESNPC>();
	RE::TESLevCharacter* lev = npc->baseTemplateForm->As<RE::TESLevCharacter>();
	Settings::Distribution::NPCTPLTInfo tpltinfo;
	if (tplt) {
		// get info about template and then integrate into our local information according to what we use
		tpltinfo = ExtractTemplateInfo(tplt);
	} else if (lev) {
		tpltinfo = ExtractTemplateInfo(lev);
	} else {
		//logger::info("template invalid");
	}

	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kFactions) {
		info.tpltfactions = tpltinfo.tpltfactions;
	} else {
		for (uint32_t i = 0; i < npc->factions.size(); i++) {
			if (npc->factions[i].faction)
				info.tpltfactions.push_back(npc->factions[i].faction);
		}
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kKeywords) {
		info.tpltkeywords = tpltinfo.tpltkeywords;
	} else {
		for (uint32_t i = 0; i < npc->numKeywords; i++) {
			if (npc->keywords[i])
				info.tpltkeywords.push_back(npc->keywords[i]);
		}
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kTraits) {
		// race
		info.tpltrace = tpltinfo.tpltrace;
	} else {
		info.tpltrace = npc->GetRace();
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kStats) {
		// class
		info.tpltclass = tpltinfo.tpltclass;
	} else {
		info.tpltclass = npc->npcClass;
	}
	if (npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kAIData) {
		// combatstyle
		info.tpltstyle = tpltinfo.tpltstyle;
	} else {
		info.tpltstyle = npc->combatStyle;
	}
	return info;
}

void Settings::CheckActorsForRules()
{
	logger::info("[CheckActorsForRules] checking...");
	std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_NPCs_without_Rule.csv");
	std::ofstream outpris("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_NPCs_without_Rule_Prisoners.txt");
	out << "PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
	//PluginBase;

	auto datahandler = RE::TESDataHandler::GetSingleton();
	std::string_view name = std::string_view{ "" };
	bool lightplugin = false;
	const RE::TESFile* file = nullptr;

	auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
	auto end = hashtable->end();
	auto iter = hashtable->begin();
	std::set<RE::FormID> visited;
	RE::Actor* act = nullptr;
	RE::TESNPC* npc = nullptr;
	Settings::ActorStrength acs;
	Settings::ItemStrength is;
	//auto arr = datahandler->GetFormArray<RE::TESNPC>();
	//auto coun = 0;
	while (iter != hashtable->end()) {
		//while (arr.size() > (unsigned int)coun) {
		try {
			if ((*iter).second) {
				lightplugin = false;
				//npc = arr[coun];
				//act = nullptr;
				act = (*iter).second->As<RE::Actor>();
				npc = (*iter).second->As<RE::TESNPC>();
				logger::warn("[CheckActorsForRules] act {}\t\t npc {}", act ? Utility::GetHex(act->GetFormID()) : "", npc ? Utility::GetHex(npc->GetFormID()) : "");
				if (npc && npc->GetFormID() != 0x07 && (npc->GetFormID() >> 24) != 0xFF) {
					if (!visited.contains(npc->GetFormID())) {
						visited.insert(npc->GetFormID());
						//logger::info("check 1");
						{
							//logger::info("iter 5 {}", Utility::GetHex(npc->GetFormID()));
							name = std::string_view{ "" };
							if ((npc->GetFormID() >> 24) != 0xFE) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(npc->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									//logger::info("invalid plugin");
									continue;
								}
								name = file->GetFilename();
							}
							//logger::info("iter 5.1");
							if (name.empty()) {
								//name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() << 8)) >> 20))->GetFilename();
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									//logger::info("invalid plugin");
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							//logger::info("iter 5.2");
							//logger::info("[CheckActorsForRules] {} named {} from {}", Utility::GetHex(npc->GetFormID()), npc->GetName(), name);
							//logger::info("iter 5.3");
						}
						// check wether there is a rule that applies
						if (Settings::Distribution::ExcludedNPC(npc)) {
							iter++;
							//coun++;
							continue;  // the npc is covered by an exclusion
						}
						//logger::info("check 2");
						// get rule
						Settings::Distribution::NPCTPLTInfo npcinfo = Settings::Distribution::ExtractTemplateInfo(npc);
						Settings::Distribution::Rule* rl = Settings::Distribution::CalcRule(npc, acs, is, &npcinfo);
						//logger::info("check 3");
						//logger::warn("[CheckActorsForRules] got rule");

						//Utility::ToLower(std::string(npc->GetFormEditorID())).find("lvl") == std::string::npos
						if (rl && rl->ruleName == DefaultRuleName && !IsLeveledChar(npc)) {
							// lookup plugin of the actor red
							//logger::info("check 4");
							name = std::string_view{ "" };

							if ((npc->GetFormID() >> 24) != 0xFE) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(npc->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
							}
							//logger::info("check 4.1");
							if (name.empty()) {
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							//logger::info("check 4.2");
							if (lightplugin && (npc->GetFormID() & 0x00000FFF) < 0x800) {
								iter++;
								//coun++;
								continue;
							}
							if (name.empty() == false)
								out << name << ";";
							else
								out << ";";
							//logger::info("check 5");
							// we found an actor that does not have a rule, so print that to the output
							out << npc->GetName() << ";"
								<< "0x" << Utility::GetHex(npc->GetFormID()) << ";"
								<< ";";
							//logger::info("check 6");
							//logger::warn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							if (npc->GetRace())
								out << npc->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(npc->GetRace()->GetFormID()) << ";";
							else
								out << ";;";

							//logger::info("check 7");

							for (uint32_t i = 0; i < npc->factions.size(); i++) {
								out << ";"
									<< "0x" << Utility::GetHex(npc->factions[i].faction->GetFormID());
							}
							//logger::info("check 8");
							//logger::warn("[CheckActorsForRules] end");
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
								else if (lightplugin) {
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
						/* name = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetActorBase()->GetFormID() >> 24))->GetFilename();
					if (name.empty())
						name = datahandler->LookupLoadedLightModByIndex((uint16_t)((act->GetActorBase()->GetFormID() << 8)) >> 20)->GetFilename();
					if (name.empty() == false)
						out << name << ";";
					else
						out << ";";*/

						// we didn't consider the current actors base so far
						visited.insert(act->GetFormID());

						{
							//logger::info("iter 5 {}", Utility::GetHex(act->GetFormID()));
							name = std::string_view{ "" };
							if ((act->GetFormID() >> 24) != 0xFE) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									logger::info("invalid plugin");
									continue;
								}
								name = file->GetFilename();
							}
							//logger::info("iter 5.1");
							if (name.empty()) {
								//name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() << 8)) >> 20))->GetFilename();
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									logger::info("invalid plugin");
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							//logger::info("iter 5.2");
							logger::info("[CheckCellForActors] {} named {} from {}", Utility::GetHex(act->GetFormID()), act->GetName(), name);
							//logger::info("iter 5.3");
						}

						// check wether there is a rule that applies
						if (Settings::Distribution::ExcludedNPC(act)) {
							iter++;
							//coun++;
							continue;  // the npc is covered by an exclusion
						}
						// get rule
						Settings::Distribution::Rule* rl = Settings::Distribution::CalcRule(act);
						//logger::info("check 23");
						//logger::warn("[CheckActorsForRules] got rule");
						if (rl && rl->ruleName == DefaultRuleName) {
							// lookup plugin of the actor red
							//logger::info("check 24");
							name = std::string_view{ "" };
							if ((act->GetFormID() >> 24) != 0xFE && (act->GetFormID() >> 24) != 0xFF) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
							}
							if (name.empty()) {
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							if (lightplugin && (act->GetFormID() & 0x00000FFF) < 0x800) {
								iter++;
								//coun++;
								continue;
							}
							if (name.empty() == false)
								out << name << ";";
							else
								out << ";";
							//logger::info("check 25");
							// we found an actor that does not have a rule, so print that to the output
							out << act->GetName() << ";"
								<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
								<< "0x" << Utility::GetHex(act->GetFormID()) << ";";
							//logger::warn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							//logger::info("check 26");
							if (act->GetRace())
								out << act->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
							else
								out << ";;";
							//logger::info("check 27");
							//logger::warn("[CheckActorsForRules] Race: {} {}", act->GetRace()->GetFormEditorID(), Utility::GetHex(act->GetRace()->GetFormID()));
							if (act->GetSaveParentCell())
								out << act->GetSaveParentCell()->GetName();

							//logger::info("check 28");
							//logger::warn("[CheckActorsForRules] factions");
							act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
								if (a_faction)
									out << ";"
										<< "0x" << Utility::GetHex(a_faction->GetFormID());
								//logger::warn("[CheckActorsForRules] Faction: {}", Utility::GetHex(a_faction->GetFormID()));
								return false;
							});
							//logger::info("check 29");
							//logger::warn("[CheckActorsForRules] end");
							out << "\n";
							out.flush();
						}
					}
				}
			}
		} catch (...) {
			//logger::warn("catch");
			out << ";";
		}
		try {
			iter++;
			//coun++;
		} catch (...) {
			//logger::warn("catch finished");
			break;
		}
	}
	logger::info("[CheckActorsForRules] finished checking...");
}

static std::binary_semaphore lockcells(1);

void Settings::CheckCellForActors(RE::FormID cellid)
{
	lockcells.acquire();
	logger::info("[CheckCellForActors] checking cell {}...", Utility::GetHex(cellid));
	std::ofstream out("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv", std::ofstream::app);
	//out << "RuleApplied;PluginRef;ActorName;ActorBaseID;ReferenceID;RaceEditorID;RaceID;Cell;Factions\n";
	//PluginBase;

	auto datahandler = RE::TESDataHandler::GetSingleton();
	std::string_view name = std::string_view{ "" };
	bool lightplugin = false;

	std::set<RE::FormID> visited;
	RE::Actor* act = nullptr;

	RE::TESForm* tmp = RE::TESForm::LookupByID(cellid);
	RE::TESObjectCELL* cell = nullptr;
	const RE::TESFile* file = nullptr;
	if (tmp)
		cell = tmp->As<RE::TESObjectCELL>();
	if (cell) {
		auto hashtable = cell->references;
		auto iter = hashtable.begin();
		while (iter != hashtable.end()) {
			try {
				if ((*iter).get()) {
					lightplugin = false;
					//npc = arr[coun];
					//act = nullptr;
					//logger::info("iter 1");
					act = (*iter)->As<RE::Actor>();
					//logger::info("iter 2");
					if (act && !act->IsDeleted() && act->GetFormID() != 0x14) {
						//logger::info("iter 3");
						if (!visited.contains(act->GetFormID())) {
							//logger::info("iter 4");
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
							bool excluded = false;
							// check wether there is a rule that applies
							if (Settings::EnableLog) {
								//logger::info("iter 5 {}", Utility::GetHex(act->GetFormID()));
								name = std::string_view{ "" };
								if ((act->GetFormID() >> 24) != 0xFE) {
									file = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24));
									if (file == nullptr) {
										iter++;
										logger::info("invalid plugin");
										continue;
									}
									name = file->GetFilename();
								}
								//logger::info("iter 5.1");
								if (name.empty()) {
									//name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() << 8)) >> 20))->GetFilename();
									file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() & 0x00FFF000)) >> 12));
									if (file == nullptr) {
										iter++;
										logger::info("invalid plugin");
										continue;
									}
									name = file->GetFilename();
									lightplugin = true;
								}
								//logger::info("iter 5.2");
								logger::info("[CheckCellForActors] {} named {} from {}", Utility::GetHex(act->GetFormID()), act->GetName(), name);
								//logger::info("iter 5.3");
							}
							//logger::info("iter 6");
							if (Settings::Distribution::ExcludedNPC(act)) {
								excluded = true;
								LOG_1("{}[CheckCellForActors] excluded");
							}
							//logger::info("iter 7");
							// get rule
							Settings::Distribution::Rule* rl = Settings::Distribution::CalcRule(act);
							//logger::info("iter 8");
							//logger::info("check 23");
							//logger::warn("[CheckActorsForRules] got rule");
							// lookup plugin of the actor red
							//logger::info("check 24");
							out << cell->GetFormEditorID() << ";";
							if (excluded)
								out << "Excluded"
									<< ";";
							else
								out << rl->ruleName << ";";
							name = std::string_view{ "" };
							if ((act->GetFormID() >> 24) != 0xFE && (act->GetFormID() >> 24) != 0xFF) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
							}
							if (name.empty()) {
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							if (lightplugin && (act->GetFormID() & 0x00000FFF) < 0x800) {
								iter++;
								continue;
							}
							if (name.empty() == false)
								out << name << ";";
							else
								out << ";";
							//logger::info("check 25");
							// we found an actor that does not have a rule, so print that to the output
							out << act->GetName() << ";"
								<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
								<< "0x" << Utility::GetHex(act->GetFormID()) << ";";
							//logger::warn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							//logger::info("check 26");
							if (act->GetRace())
								out << act->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
							else
								out << ";;";
							//logger::info("check 27");
							//logger::warn("[CheckActorsForRules] Race: {} {}", act->GetRace()->GetFormEditorID(), Utility::GetHex(act->GetRace()->GetFormID()));
							if (act->GetSaveParentCell())
								out << act->GetSaveParentCell()->GetName();

							//logger::info("check 28");
							//logger::warn("[CheckActorsForRules] factions");
							act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
								if (a_faction)
									out << ";"
										<< "0x" << Utility::GetHex(a_faction->GetFormID());
								//logger::warn("[CheckActorsForRules] Faction: {}", Utility::GetHex(a_faction->GetFormID()));
								return false;
							});
							//logger::info("check 29");
							//logger::warn("[CheckActorsForRules] end");
							out << "\n";
							out.flush();
						}
					}
					//logger::info("end iter");
				}
			} catch (...) {
				//logger::warn("catch");
				out << ";";
			}
			try {
				iter++;
				//coun++;
			} catch (...) {
				//logger::warn("catch finished");
				break;
			}
		}
	}
	lockcells.release();
}

void Settings::ApplySkillBoostPerks()
{
	auto datahandler = RE::TESDataHandler::GetSingleton();
	auto npcs = datahandler->GetFormArray<RE::TESNPC>();
	for(auto& npc : npcs) {
		if (npc && npc->GetFormID() != 0x7 ){//&& !((npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kSpells))) {
			npc->AddPerk(Settings::AlchemySkillBoosts, 1);
			npc->AddPerk(Settings::PerkSkillBoosts, 1);
		}
	}
	/*
	if (actor->HasPerk(Settings::AlchemySkillBoosts) == false) {
		actor->AddPerk(Settings::AlchemySkillBoosts, 1);
		logger::info("Adding Perk AlchemySkillBoosts");
	}
	if (actor->HasPerk(Settings::PerkSkillBoosts) == false) {
		actor->AddPerk(Settings::PerkSkillBoosts, 1);
		logger::info("Adding Perk PerkSkillBoosts");
	}*/

}

#pragma endregion
