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
#include "Distribution.h"
#include "AlchemyEffect.h"
#include "Data.h"

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

void Settings::LoadDistrConfig()
{
	// set to false, to avoid other funcions running stuff on our variables
	Distribution::initialised = false;

	// disable generic logging, if load logging is disabled
	if (Logging::EnableLoadLog == false)
		Logging::EnableGenericLogging = false;

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
		loginfo("[SETTINGS] [LoadDistrRules] No Distribution files were found");
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
		loginfo("[SETTINGS] [LoadDistrRules] found Distribution configuration file: {}", files[k]);
	}

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
					std::string tmp = line;
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
						logwarn("[Settings] [LoadDistrRules] Not a rule. file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// check what rule version we have
					int ruleVersion = -1;
					try {
						ruleVersion = std::stoi(splits->at(splitindex));
						splitindex++;
					} catch (std::out_of_range&) {
						logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RuleVersion\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					} catch (std::invalid_argument&) {
						logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RuleVersion\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					}
					// check what kind of rule we have
					int ruleType = -1;
					try {
						ruleType = std::stoi(splits->at(splitindex));
						splitindex++;
					} catch (std::out_of_range&) {
						logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RuleType\". file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						continue;
					} catch (std::invalid_argument&) {
						logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RuleType\". file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 25. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the rulename, so we just set it
									Distribution::Rule* rule = new Distribution::Rule();
									rule->ruleVersion = ruleVersion;
									rule->ruleType = ruleType;
									rule->ruleName = splits->at(splitindex);
									LOGLE1_2("[Settings] [LoadDistrRules] loading rule: {}", rule->ruleName);
									splitindex++;
									// now come the rule priority
									rule->rulePriority = -1;
									try {
										rule->rulePriority = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion1Chance
									rule->potion1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion1Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion2Chance
									rule->potion2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion2Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion3Chance
									rule->potion3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion3Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PotionAddChance
									rule->potionAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potionAdditionalChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"PotionAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify1Chance
									rule->fortify1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify1Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Fortify1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify2Chance
									rule->fortify2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify2Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Fortify2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison1Chance
									rule->poison1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison1Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison2Chance
									rule->poison2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison2Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison3Chance
									rule->poison3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison3Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PoisonAddChance
									rule->poisonAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poisonAdditionalChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"PoisonAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									// now comes FoodChance
									rule->foodChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->foodChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"FoodChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> objects = Utility::ParseAssocObjects(rule->assocObjects, error, file, tmp);

									// parse the item properties
									std::vector<std::tuple<uint64_t, float>> potioneffects = Utility::ParseAlchemyEffects(rule->potionProperties, error);
									rule->potionDistr = Utility::GetDistribution(potioneffects, RandomRange);
									rule->potionDistrChance = Utility::GetDistribution(potioneffects, RandomRange, true);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} potion effects", rule->ruleName, rule->potionDistr.size());
									rule->validPotions = Utility::SumAlchemyEffects(rule->potionDistr, true);
									std::vector<std::tuple<uint64_t, float>> poisoneffects = Utility::ParseAlchemyEffects(rule->poisonProperties, error);
									rule->poisonDistr = Utility::GetDistribution(poisoneffects, RandomRange);
									rule->poisonDistrChance = Utility::GetDistribution(poisoneffects, RandomRange, true);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} poison effects", rule->ruleName, rule->poisonDistr.size());
									rule->validPoisons = Utility::SumAlchemyEffects(rule->poisonDistr, true);
									std::vector<std::tuple<uint64_t, float>> fortifyeffects = Utility::ParseAlchemyEffects(rule->fortifyproperties, error);
									rule->fortifyDistr = Utility::GetDistribution(fortifyeffects, RandomRange);
									rule->fortifyDistrChance = Utility::GetDistribution(fortifyeffects, RandomRange, true);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} fortify potion effects", rule->ruleName, rule->fortifyDistr.size());
									rule->validFortifyPotions = Utility::SumAlchemyEffects(rule->fortifyDistr, true);
									std::vector<std::tuple<uint64_t, float>> foodeffects = Utility::ParseAlchemyEffects(rule->foodProperties, error);
									rule->foodDistr = Utility::GetDistribution(foodeffects, RandomRange);
									rule->foodDistrChance = Utility::GetDistribution(foodeffects, RandomRange, true);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} food effects", rule->ruleName, rule->foodDistr.size());
									rule->validFood = Utility::SumAlchemyEffects(rule->foodDistr, true);

									std::pair<int, Distribution::Rule*> tmptuple = { rule->rulePriority, rule };

									// assign rules to search parameters
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} associated objects", rule->ruleName, objects.size());
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
									Distribution::_rules.push_back(rule);
									if (rule->ruleName == DefaultRuleName && (Distribution::defaultRule == nullptr ||
																				 rule->rulePriority > Distribution::defaultRule->rulePriority))
										Distribution::defaultRule = rule;
									delete splits;
									LOGLE1_2("[Settings] [LoadDistrRules] rule {} successfully loaded.", rule->ruleName);
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
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										if (std::get<0>(items[i]) & Distribution::AssocType::kActor ||
											std::get<0>(items[i]) & Distribution::AssocType::kNPC ||
											std::get<0>(items[i]) & Distribution::AssocType::kFaction ||
											std::get<0>(items[i]) & Distribution::AssocType::kKeyword ||
											std::get<0>(items[i]) & Distribution::AssocType::kRace) {
											Distribution::_bosses.insert(std::get<1>(items[i]));
											LOGLE1_2("[Settings] [LoadDistrRules] declared {} as boss.", Utility::GetHex(std::get<1>(items[i])));
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 4:  // exclude object
								{
									if (splits->size() != 3) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kActor:
										case Distribution::AssocType::kNPC:
											Distribution::_excludedNPCs.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
											Distribution::_excludedAssoc.insert(std::get<1>(items[i]));
											break;
										case Distribution::AssocType::kItem:
											Distribution::_excludedItems.insert(std::get<1>(items[i]));
											break;
										}
										if (EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kActor ||
												std::get<0>(items[i]) & Distribution::AssocType::kNPC) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded actor {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kFaction) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded faction {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kKeyword) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded keyword {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kItem) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded item {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kRace) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded race {} from distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
										}
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 5:  // exclude baseline
								{
									if (splits->size() != 3) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kFaction:
										case Distribution::AssocType::kKeyword:
										case Distribution::AssocType::kRace:
											Distribution::_baselineExclusions.insert(std::get<1>(items[i]));
											break;
										}

										if (EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kFaction) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded faction {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kKeyword) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded keyword {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											} else if (std::get<0>(items[i]) & Distribution::AssocType::kRace) {
												LOGLE1_2("[Settings] [LoadDistrRules] excluded race {} from base line distribution.", Utility::GetHex(std::get<1>(items[i])));
											}
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
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kItem:
											Distribution::_whitelistItems.insert(std::get<1>(items[i]));
											break;
										}
										if (EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kItem) {
												LOGLE1_2("[Settings] [LoadDistrRules] whitelisted item {}.", Utility::GetHex(std::get<1>(items[i])));
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
									if (splits->size() != 4) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}

									Distribution::CustomItemStorage* citems = new Distribution::CustomItemStorage();
									// parse associated obj
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> assocobj = Utility::ParseAssocObjects(assoc, error, file, tmp);
									
									// parse items associated
									assoc = splits->at(splitindex);
									splitindex++;
									error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID, int32_t, CustomItemFlag, int8_t, bool, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, std::vector<std::tuple<uint64_t, uint32_t, std::string>>, bool>> associtm = Utility::ParseCustomObjects(assoc, error, file, tmp);
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
												LOGL1_3("{}[Settings] [LoadDstrRules] Flag converted: {}", static_cast<uint64_t>(std::get<3>(associtm[i])));
												if (tmpb) {
													if (std::get<5>(associtm[i]))
														Distribution::_excludedItems.insert(std::get<2>(associtm[i]));
													switch (std::get<3>(associtm[i])) {
													case CustomItemFlag::Object:
														{
															LOGL_3("{}[Settings] [LoadDstrRules] Path 1");
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
															LOGL_3("{}[Settings] [LoadDstrRules] Path 2");

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
														LOGL_3("{}[Settings] [LoadDstrRules] Path 3");
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
														LOGL_3("{}[Settings] [LoadDstrRules] Path 4");
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
														LOGL_3("{}[Settings] [LoadDstrRules] Path 5");
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
														LOGL_3("{}[Settings] [LoadDstrRules] Path 6");
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
													LOGLE1_2("[Settings] [LoadDistrRules] custom rule for item {} cannot be applied, due to the item not being an TESBoundObject.", Utility::GetHex(std::get<1>(associtm[i])));
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
										logwarn("[Settings] [LoadDistrRules] rule does not contain any items. file: {}, rule:\"{}\"", file, tmp);
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
											break;
										}
										if (EnableLoadLog) {
											if (std::get<0>(assocobj[i]) & Distribution::AssocType::kKeyword) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kRace) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kFaction) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kCombatStyle) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kClass) {
											} else if (std::get<0>(assocobj[i]) & Distribution::AssocType::kActor || 
													   std::get<0>(assocobj[i]) & Distribution::AssocType::kNPC) {
											}
										}
									}
									if (cx == 0) {
										auto iter = Distribution::_customItems.find(0x0);
										if (iter != Distribution::_customItems.end()) {
											auto vec = iter->second;
											vec.push_back(citems);
											Distribution::_customItems.insert_or_assign(0x0, vec);
											cx++;
										} else {
											std::vector<Distribution::CustomItemStorage*> vec = { citems };
											Distribution::_customItems.insert_or_assign(0x0, vec);
											cx++;
										}
									}

									// since we are done delete splits
									delete splits;
								}
								break;
							case 9:  // exclude plugin
								{
									if (splits->size() != 3) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string plugin = splits->at(splitindex);
									splitindex++;
									int8_t mainindex = datahandler->GetLoadedModIndex(plugin).value_or(-1);
									int16_t subindex = datahandler->GetLoadedLightModIndex(plugin).value_or(-1);
									if (mainindex > -1 && mainindex < 0xFE) {
										// index is a normal mod
										Distribution::_excludedPlugins.insert(mainindex);
										loginfo("[Settings] [LoadDistrRules] Rule 9 excluded plugin {} with index {}", plugin, mainindex);
									} else if (subindex > -1 && subindex <= 0xFFF) {
										// we have a valid light mod
										Distribution::_excludedPluginsLight.insert(subindex);
										loginfo("[Settings] [LoadDistrRules] Rule 9 excluded light plugin {} with subindex {}", plugin, subindex);
									} else {
										loginfo("[Settings] [LoadDistrRules] Rule 9 cannot exclude plugin {}. It is either not loaded or not present", plugin);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 10: // set item strength
								{
									if (splits->size() != 4) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string strength = splits->at(splitindex);
									splitindex++;
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									ItemStrength str = ItemStrength::kWeak;
									// arse item strength
									try {
										str = static_cast<ItemStrength>(std::stoi(strength));
									} catch (std::out_of_range&) {
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"ItemStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"ItemStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									catch (std::exception&) {
										logwarn("[Settings] [LoadDistrRules] generic expection in field \"ItemStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									}
									for (int i = 0; i < items.size(); i++) {
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kItem:
											Distribution::_itemStrengthMap.insert_or_assign(std::get<1>(items[i]), str);
											break;
										}
										if (EnableLoadLog) {
											if (std::get<0>(items[i]) & Distribution::AssocType::kItem) {
												LOGLE1_2("[Settings] [LoadDistrRules] set item strength {}.", Utility::GetHex(std::get<1>(items[i])));
											} else {
												logwarn("[Settings] [LoadDistrRules] rule 10 is not applicable to object {}.", Utility::GetHex(std::get<1>(items[i])));
											}
										}
									}

									// since we are done delete splits
									delete splits;
								}
								break;
							case 11: // adjust actor strength
								{
									if (splits->size() != 4) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 4. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string strength = splits->at(splitindex);
									splitindex++;
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									int str = 0;
									// arse item strength
									try {
										str = std::stoi(strength);
									} catch (std::out_of_range&) {
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RelativeActorStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RelativeActorStrength\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										continue;
									} catch (std::exception&) {
										logwarn("[Settings] [LoadDistrRules] generic expection in field \"RelativeActorStrength\". file: {}, rule:\"{}\"", file, tmp);
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
												break;
											}
											if (EnableLoadLog) {
												if (std::get<0>(items[i]) & Distribution::AssocType::kActor ||
													std::get<0>(items[i]) & Distribution::AssocType::kNPC ||
													std::get<0>(items[i]) & Distribution::AssocType::kFaction ||
													std::get<0>(items[i]) & Distribution::AssocType::kKeyword ||
													std::get<0>(items[i]) & Distribution::AssocType::kRace) {
													LOGLE1_2("[Settings] [LoadDistrRules] set relative actor strength {}.", Utility::GetHex(std::get<1>(items[i])));
												} else {
													logwarn("[Settings] [LoadDistrRules] rule 11 is not applicable to object {}.", Utility::GetHex(std::get<1>(items[i])));
												}
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
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string pluginname = splits->at(splitindex);
									splitindex++;
									bool error = false;
									auto forms = Utility::GetFormsInPlugin<RE::AlchemyItem>(pluginname);
									for (int i = 0; i < forms.size(); i++) {
										Distribution::_whitelistItems.insert(forms[i]->GetFormID());
										if (EnableLoadLog)
											LOGLE3_2("[Settings] [LoadDistrRules] whitelisted item. id: {}, name: {}, plugin: {}.", Utility::GetHex(forms[i]->GetFormID()), forms[i]->GetName(), pluginname);
									}
									// since we are done delete splits
									delete splits;
								}
								break;
							case 13: // follower detection
								{
									if (splits->size() != 3) {
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 3. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										continue;
									}
									std::string assoc = splits->at(splitindex);
									splitindex++;
									bool error = false;
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> items = Utility::ParseAssocObjects(assoc, error, file, tmp);
									for (int i = 0; i < items.size(); i++)
									{
										switch (std::get<0>(items[i])) {
										case Distribution::AssocType::kFaction:
											Distribution::_followerFactions.insert(std::get<1>(items[i]));
											break;
										}
									}
									delete splits;
								}
								break;
							default:
								logwarn("[Settings] [LoadDistrRules] Rule type does not exist. file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 33. file: {}, rule:\"{}\", fields: {}", file, tmp, splits->size());
										delete splits;
										continue;
									}
									// next entry is the rulename, so we just set it
									Distribution::Rule* rule = new Distribution::Rule();
									rule->ruleVersion = ruleVersion;
									rule->ruleType = ruleType;
									rule->ruleName = splits->at(splitindex);
									LOGLE1_2("[Settings] [LoadDistrRules] loading rule: {}", rule->ruleName);
									splitindex++;
									// now come the rule priority
									rule->rulePriority = -1;
									try {
										rule->rulePriority = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxPotions\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion1Chance
									rule->potion1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion1Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion2Chance
									rule->potion2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion2Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion3Chance
									rule->potion3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion3Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Potion4Chance
									rule->potion4Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potion4Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Potion4Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PotionAddChance
									rule->potionAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->potionAdditionalChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"PotionAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"PotionsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes maxFortify
									rule->maxFortify = -1;
									try {
										rule->maxFortify = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxFortify\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxFortify\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify1Chance
									rule->fortify1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify1Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Fortify1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify2Chance
									rule->fortify2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify2Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Fortify2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify3Chance
									rule->fortify3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify3Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Fortify3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Fortify4Chance
									rule->fortify4Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortify4Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Fortify4Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes FortifyAddChance
									rule->fortifyAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->fortifyAdditionalChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"FortifyAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes FortifyTierAdjust
									rule->fortifyTierAdjust = -1;
									try {
										rule->fortifyTierAdjust = std::stoi(splits->at(splitindex));
										splitindex++;
									} catch (std::out_of_range&) {
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"FortifyTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"FortifyTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"MaxPoisons\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison1Chance
									rule->poison1Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison1Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison1Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison2Chance
									rule->poison2Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison2Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison2Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison3Chance
									rule->poison3Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison3Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison3Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes Poison4Chance
									rule->poison4Chance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poison4Chance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"Poison4Chance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}
									// now comes PoisonAddChance
									rule->poisonAdditionalChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->poisonAdditionalChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"PoisonAddChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
										logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									} catch (std::invalid_argument&) {
										logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"PoisonsTierAdjust\". file: {}, rule:\"{}\"", file, tmp);
										delete splits;
										delete rule;
										continue;
									}

									// now comes FoodChance
									rule->foodChance = Utility::ParseIntArray(splits->at(splitindex));
									splitindex++;
									if (rule->foodChance.size() == 0) {
										logwarn("[Settings] [LoadDistrRules] fiels \"FoodChance\" couldn't be parsed. file: {}, rule:\"{}\"", file, tmp);
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
									if (splits->at(splitindex) == "1")
										rule->styleScaling = true;
									else
										rule->styleScaling = false;
									splitindex++;

									bool error = false;

									// parse the associated objects
									std::vector<std::tuple<Distribution::AssocType, RE::FormID>> objects = Utility::ParseAssocObjects(rule->assocObjects, error, file, tmp);

									// parse the item properties
									std::vector<std::tuple<uint64_t, float>> potioneffects = Utility::ParseAlchemyEffects(rule->potionProperties, error);
									rule->potionDistr = Utility::GetDistribution(potioneffects, RandomRange);
									rule->potionDistrChance = Utility::GetDistribution(potioneffects, RandomRange, true);
									rule->potionEffectMap = Utility::UnifyEffectMap(potioneffects);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} potion effects", rule->ruleName, rule->potionDistr.size());
									rule->validPotions = Utility::SumAlchemyEffects(rule->potionDistr, true);
									std::vector<std::tuple<uint64_t, float>> poisoneffects = Utility::ParseAlchemyEffects(rule->poisonProperties, error);
									rule->poisonDistr = Utility::GetDistribution(poisoneffects, RandomRange);
									rule->poisonDistrChance = Utility::GetDistribution(poisoneffects, RandomRange, true);
									rule->poisonEffectMap = Utility::UnifyEffectMap(poisoneffects);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} poison effects", rule->ruleName, rule->poisonDistr.size());
									rule->validPoisons = Utility::SumAlchemyEffects(rule->poisonDistr, true);
									std::vector<std::tuple<uint64_t, float>> fortifyeffects = Utility::ParseAlchemyEffects(rule->fortifyproperties, error);
									rule->fortifyDistr = Utility::GetDistribution(fortifyeffects, RandomRange);
									rule->fortifyDistrChance = Utility::GetDistribution(fortifyeffects, RandomRange, true);
									rule->fortifyEffectMap = Utility::UnifyEffectMap(fortifyeffects);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} fortify potion effects", rule->ruleName, rule->fortifyDistr.size());
									rule->validFortifyPotions = Utility::SumAlchemyEffects(rule->fortifyDistr, true);
									std::vector<std::tuple<uint64_t, float>> foodeffects = Utility::ParseAlchemyEffects(rule->foodProperties, error);
									rule->foodDistr = Utility::GetDistribution(foodeffects, RandomRange);
									rule->foodDistrChance = Utility::GetDistribution(foodeffects, RandomRange, true);
									rule->foodEffectMap = Utility::UnifyEffectMap(foodeffects);
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} food effects", rule->ruleName, rule->foodDistr.size());
									rule->validFood = Utility::SumAlchemyEffects(rule->foodDistr, true);

									std::pair<int, Distribution::Rule*> tmptuple = { rule->rulePriority, rule };

									// assign rules to search parameters
									LOGLE2_2("[Settings] [LoadDistrRules] rule {} contains {} associated objects", rule->ruleName, objects.size());
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
									Distribution::_rules.push_back(rule);
									if (rule->ruleName == DefaultRuleName && (Distribution::defaultRule == nullptr ||
																				 rule->rulePriority > Distribution::defaultRule->rulePriority))
										Distribution::defaultRule = rule;
									delete splits;
									LOGLE1_2("[Settings] [LoadDistrRules] rule {} successfully loaded.", rule->ruleName);
								}
								break;
							default:
								logwarn("[Settings] [LoadDistrRules] Rule type does not exist for ruleversion 2. file: {}, rule:\"{}\"", file, tmp);
								delete splits;
								break;
							}
						}
						break;
					default:
						logwarn("[Settings] [LoadDistrRules] Rule version does not exist. file: {}, rule:\"{}\"", file, tmp);
						delete splits;
						break;
					}
				}
			} else {
				logwarn("[Settings] [LoadDistrRules] file {} couldn't be read successfully", file);
			}

		} catch (std::exception&) {
			logwarn("[Settings] [LoadDistrRules] file {} couldn't be read successfully due to an error", file);
		}
	}

	// create default rule if there is none
	if (Distribution::defaultRule == nullptr) {
		Distribution::defaultRule = new Distribution::Rule(1 /*version*/, 1 /*type*/, DefaultRuleName, INT_MIN + 1 /*rulePriority*/, true /*allowMixed*/, true /*styleScaling*/, 5 /*maxPotions*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*potion1Chance*/,
			std::vector<int>{ 20, 30, 40, 50, 60 } /*potion2Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*potion3Chance*/, std::vector<int>{ 5, 15, 25, 35, 45 } /*potion4Chance*/, std::vector<int>{ 0, 10, 20, 30, 40 } /*potionAddChance*/, 0 /*potionTierAdjust*/,
			5 /*maxFortify*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify1Chance*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify2Chance*/, std::vector<int>{ 20, 30, 40, 50, 60 } /*fortify3Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*fortify4Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*fortifyAddChance*/, 0 /*fortifyTierAdjust*/,
			5 /*maxPoisons*/, std::vector<int>{ 30, 35, 40, 45, 50 } /*poison1Chance*/, std::vector<int>{ 20, 25, 30, 35, 40 } /*poison2Chance*/, std::vector<int>{ 10, 15, 20, 25, 30 } /*poison3Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*poison4Chance*/,
			std::vector<int>{ 0, 5, 10, 15, 20 } /*poisonAddChance*/, 0 /*poisonTierAdjust*/, 
			std::vector<int>{ 70, 80, 90, 100, 100 } /*foodChance*/,
			Distribution::GetVector(RandomRange, AlchemyEffect::kAnyPotion) /*potionDistr*/,
			Distribution::GetVector(RandomRange, AlchemyEffect::kAnyPoison) /*poisonDistr*/,
			Distribution::GetVector(RandomRange, AlchemyEffect::kAnyFortify) /*fortifyDistr*/,
			Distribution::GetVector(RandomRange, AlchemyEffect::kAnyFood) /*foodDistr*/,
			static_cast<uint64_t>(AlchemyEffect::kAnyPotion) | static_cast<uint64_t>(AlchemyEffect::kCustom) /*validPotions*/,
			static_cast<uint64_t>(AlchemyEffect::kAnyPoison) | static_cast<uint64_t>(AlchemyEffect::kCustom) /*validPoisons*/,
			static_cast<uint64_t>(AlchemyEffect::kAnyFortify) | static_cast<uint64_t>(AlchemyEffect::kCustom) /*validFortifyPotions*/,
			static_cast<uint64_t>(AlchemyEffect::kAnyFood) | static_cast<uint64_t>(AlchemyEffect::kCustom) /*validFood*/);
	}
	if (Distribution::defaultCustomRule == nullptr) {
		Distribution::defaultCustomRule = new Distribution::Rule(1 /*version*/, 1 /*type*/, DefaultRuleName, INT_MIN + 1 /*rulePriority*/, true /*allowMixed*/, true /*styleScaling*/, 5 /*maxPotions*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*potion1Chance*/,
			std::vector<int>{ 20, 30, 40, 50, 60 } /*potion2Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*potion3Chance*/, std::vector<int>{ 5, 15, 25, 35, 45 } /*potion4Chance*/, std::vector<int>{ 0, 10, 20, 30, 40 } /*potionAddChance*/, 0 /*potionTierAdjust*/,
			5 /*maxFortify*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify1Chance*/, std::vector<int>{ 30, 40, 50, 60, 70 } /*fortify2Chance*/, std::vector<int>{ 20, 30, 40, 50, 60 } /*fortify3Chance*/, std::vector<int>{ 10, 20, 30, 40, 50 } /*fortify4Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*fortifyAddChance*/, 0 /*fortifyTierAdjust*/,
			5 /*maxPoisons*/, std::vector<int>{ 30, 35, 40, 45, 50 } /*poison1Chance*/, std::vector<int>{ 20, 25, 30, 35, 40 } /*poison2Chance*/, std::vector<int>{ 10, 15, 20, 25, 30 } /*poison3Chance*/, std::vector<int>{ 5, 10, 15, 20, 25 } /*poison4Chance*/,
			std::vector<int>{ 0, 5, 10, 15, 20 } /*poisonAddChance*/, 0 /*poisonTierAdjust*/,
			std::vector<int>{ 70, 80, 90, 100, 100 } /*foodChance*/,
			std::vector<std::tuple<int, AlchemyEffect>>{} /*potionDistr*/,
			std::vector<std::tuple<int, AlchemyEffect>>{} /*poisonDistr*/,
			std::vector<std::tuple<int, AlchemyEffect>>{} /*fortifyDistr*/,
			std::vector<std::tuple<int, AlchemyEffect>>{} /*foodDistr*/,
			static_cast<uint64_t>(AlchemyEffect::kCustom) /*validPotions*/,
			static_cast<uint64_t>(AlchemyEffect::kCustom) /*validPoisons*/,
			static_cast<uint64_t>(AlchemyEffect::kCustom) /*validFortifyPotions*/,
			static_cast<uint64_t>(AlchemyEffect::kCustom) /*validFood*/);
	}

	if (copyrules.size() > 0) {
		for (auto cpy : copyrules) {
			auto splits = std::get<0>(cpy);
			if (splits->size() != 5) {
				logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 5. file: {}, rule:\"{}\", fields: {}", std::get<1>(cpy), std::get<2>(cpy), splits->size());
				continue;
			}
			std::string name = (splits)->at(2);
			std::string newname = (splits)->at(3);
			
			Distribution::Rule* rule = Distribution::FindRule(name);
			Distribution::Rule* newrule = rule->Clone();
			newrule->ruleName = newname;
			int prio = INT_MIN;
			try {
				prio = std::stoi(splits->at(4));
			} catch (std::out_of_range&) {
				logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
				prio = INT_MIN;
			} catch (std::invalid_argument&) {
				logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(cpy), std::get<2>(cpy));
				prio = INT_MIN;
			}
			if (prio != INT_MIN)
				newrule->rulePriority = prio;
			Distribution::_rules.push_back(newrule);
			if (newname == DefaultRuleName && (Distribution::defaultRule == nullptr ||
												  newrule->rulePriority > Distribution::defaultRule->rulePriority))
				Distribution::defaultRule = newrule;
			delete splits;
			LOGLE1_2("[Settings] [LoadDistrRules] rule {} successfully coinialised.", newrule->ruleName);
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
						logwarn("[Settings] [LoadDistrRules] rule not found. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
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
							logwarn("[Settings] [LoadDistrRules] out-of-range expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
							continue;
						} catch (std::invalid_argument&) {
							logwarn("[Settings] [LoadDistrRules] invalid-argument expection in field \"RulePrio\". file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
							continue;
						}
					}


					// parse the associated objects
					bool error = false;
					std::vector<std::tuple<Distribution::AssocType, RE::FormID>> objects = Utility::ParseAssocObjects((std::get<0>(a)->at(3)), error, std::get<1>(a), std::get<2>(a));

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
						if (EnableLoadLog) {
							switch (std::get<0>(objects[i])) {
							case Distribution::AssocType::kFaction:
								if (attach) {
									LOGLE3_2("[Settings] [LoadDistrRules] attached Faction {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else 
									LOGLE5_2("[Settings] [LoadDistrRules] updated Faction {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kKeyword:
								if (attach) {
									LOGLE3_2("[Settings] [LoadDistrRules] attached Keyword {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGLE5_2("[Settings] [LoadDistrRules] updated Keyword {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kRace:
								if (attach) {
									LOGLE3_2("[Settings] [LoadDistrRules] attached Race {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGLE5_2("[Settings] [LoadDistrRules] updated Race {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kClass:
								if (attach) {
									LOGLE3_2("[Settings] [LoadDistrRules] attached Class {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGLE5_2("[Settings] [LoadDistrRules] updated Class {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kCombatStyle:
								if (attach) {
									LOGLE3_2("[Settings] [LoadDistrRules] attached CombatStyle {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGLE5_2("[Settings] [LoadDistrRules] updated CombatStyle {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							case Distribution::AssocType::kNPC:
							case Distribution::AssocType::kActor:
								if (attach) {
									LOGLE3_2("[Settings] [LoadDistrRules] attached Actor {} to rule {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, std::get<1>(a));
								} else
									LOGLE5_2("[Settings] [LoadDistrRules] updated Actor {} to rule {} with new Priority {} overruling {}.\t\t\t{}", Utility::GetHex(std::get<1>(objects[i])), rule->ruleName, prio, oldprio, std::get<1>(a));
								break;
							}
						}

					}
				}
			} else {
				// rule invalid
				logwarn("[Settings] [LoadDistrRules] rule has wrong number of fields, expected 4 or 5. file: {}, rule:\"{}\"", std::get<1>(a), std::get<2>(a));
				// delet splits since we don't need it anymore
				delete std::get<0>(a);
			}
		}
	}


	// load our stuff like necessary forms
	// get VendorItemPotion keyword, if we don't find this, potion detection will be nearly impossible
	Settings::VendorItemPotion = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEC);
	if (Settings::VendorItemPotion == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find VendorItemPotion Keyword in game.");
	}
	Settings::VendorItemPoison = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDED);
	if (Settings::VendorItemPoison == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find VendorItemPoison Keyword in game.");
	}
	Settings::VendorItemFood = RE::TESForm::LookupByID<RE::BGSKeyword>(0x0008CDEA);
	if (Settings::VendorItemFood == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find VendorItemFood Keyword in game.");
	}
	Settings::VendorItemFoodRaw = RE::TESForm::LookupByID<RE::BGSKeyword>(0x000A0E56);
	if (Settings::VendorItemFoodRaw == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find VendorItemFoodRaw Keyword in game.");
	}
	Settings::CurrentFollowerFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x0005C84E);
	if (Settings::CurrentFollowerFaction == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find CurrentFollowerFaction Faction in game.");
	}
	Settings::CurrentHirelingFaction = RE::TESForm::LookupByID<RE::TESFaction>(0xbd738);
	if (Settings::CurrentHirelingFaction == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find CurrentHirelingFaction Faction in game.");
	}
	Settings::ActorTypeDwarven = RE::TESForm::LookupByID<RE::BGSKeyword>(0x1397A);
	if (Settings::ActorTypeDwarven == nullptr) {
		loginfo("[Settings] [INIT] Couldn't find ActorTypeDwarven Keyword in game.");
	}

	// hard exclude everyone that may become a follower
	//Distribution::_excludedAssoc.insert(0x0005C84E);

	// exclude currenhireling faction
	//Distribution::_excludedAssoc.insert(0xbd738);

	Settings::AlchemySkillBoosts = RE::TESForm::LookupByID<RE::BGSPerk>(0xA725C);
	if (Settings::AlchemySkillBoosts == nullptr)
		loginfo("[Settings] [INIT] Couldn't find AlchemySkillBoosts Perk in game.");
	Settings::PerkSkillBoosts = RE::TESForm::LookupByID<RE::BGSPerk>(0xCF788);
	if (Settings::PerkSkillBoosts == nullptr)
		loginfo("[Settings] [INIT] Couldn't find PerkSkillBoosts Perk in game.");





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
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201A, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201B, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201C, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201D, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
	// White Phial
	if ((tmp = Utility::GetTESForm(datahandler, 0x0010201E, "", "")) != nullptr)
		Distribution::_excludedItems.insert(tmp->GetFormID());
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

	if (Settings::_ApplySkillBoostPerks)
		Settings::ApplySkillBoostPerks();

	Distribution::initialised = true;

	if (Settings::EnableLoadLog) {
		loginfo("[Settings] [LoadDistrRules] Number of Rules: {}", Distribution::rules()->size());
		loginfo("[Settings] [LoadDistrRules] Number of NPCs: {}", Distribution::npcMap()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of NPCs: {}", Distribution::npcMap()->bucket_count());
		loginfo("[Settings] [LoadDistrRules] Number of Associations: {}", Distribution::assocMap()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of Associations: {}", Distribution::assocMap()->bucket_count());
		loginfo("[Settings] [LoadDistrRules] Number of Bosses: {}", Distribution::bosses()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of Bosses: {}", Distribution::bosses()->bucket_count());
		loginfo("[Settings] [LoadDistrRules] Number of Excluded NPCs: {}", Distribution::excludedNPCs()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of Excluded NPCs: {}", Distribution::excludedNPCs()->bucket_count());
		loginfo("[Settings] [LoadDistrRules] Number of Excluded Associations: {}", Distribution::excludedAssoc()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of Excluded Associations: {}", Distribution::excludedAssoc()->bucket_count());
		loginfo("[Settings] [LoadDistrRules] Number of Excluded Items: {}", Distribution::excludedItems()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of Excluded Items: {}", Distribution::excludedItems()->bucket_count());
		loginfo("[Settings] [LoadDistrRules] Number of Baseline Exclusions: {}", Distribution::baselineExclusions()->size());
		loginfo("[Settings] [LoadDistrRules] Buckets of Baseline Exclusions: {}", Distribution::baselineExclusions()->bucket_count());
		/*for (int i = 0; i < Distribution::_rules.size(); i++) {
			loginfo("rule {} pointer {}", i, Utility::GetHex((uintptr_t)Distribution::_rules[i]));
		}
		auto iter = Distribution::_assocMap.begin();
		while (iter != Distribution::_assocMap.end()) {
			loginfo("assoc\t{}\trule\t{}", Utility::GetHex(iter->first), Utility::GetHex((uintptr_t)(std::get<1>(iter->second))));
			iter++;
		}*/
	}

	// reactivate generic logging
	Logging::EnableGenericLogging = true;
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

Distribution::NPCTPLTInfo Distribution::ExtractTemplateInfo(RE::TESLevCharacter* lvl)
{
	if (lvl == nullptr)
		return Distribution::NPCTPLTInfo{};
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
			;  //loginfo("template invalid");
	}
	return Distribution::NPCTPLTInfo{};
}

Distribution::NPCTPLTInfo Distribution::ExtractTemplateInfo(RE::TESNPC* npc)
{
	Distribution::NPCTPLTInfo info;
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
	Distribution::NPCTPLTInfo tpltinfo;
	if (tplt) {
		// get info about template and then integrate into our local information according to what we use
		tpltinfo = ExtractTemplateInfo(tplt);
	} else if (lev) {
		tpltinfo = ExtractTemplateInfo(lev);
	} else {
		//loginfo("template invalid");
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
	loginfo("[Settings] [CheckActorsForRules] checking...");
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
	ActorStrength acs;
	ItemStrength is;
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
				logwarn("[Settings] [CheckActorsForRules] act {}\t\t npc {}", act ? Utility::GetHex(act->GetFormID()) : "", npc ? Utility::GetHex(npc->GetFormID()) : "");
				if (npc && npc->GetFormID() != 0x07 && (npc->GetFormID() >> 24) != 0xFF) {
					if (!visited.contains(npc->GetFormID())) {
						visited.insert(npc->GetFormID());
						//loginfo("check 1");
						{
							//loginfo("iter 5 {}", Utility::GetHex(npc->GetFormID()));
							name = std::string_view{ "" };
							if ((npc->GetFormID() >> 24) != 0xFE) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(npc->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									//loginfo("invalid plugin");
									continue;
								}
								name = file->GetFilename();
							}
							//loginfo("iter 5.1");
							if (name.empty()) {
								//name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() << 8)) >> 20))->GetFilename();
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									//loginfo("invalid plugin");
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							//loginfo("iter 5.2");
							//loginfo("[CheckActorsForRules] {} named {} from {}", Utility::GetHex(npc->GetFormID()), npc->GetName(), name);
							//loginfo("iter 5.3");
						}
						// check wether there is a rule that applies
						if (Distribution::ExcludedNPC(npc)) {
							iter++;
							//coun++;
							continue;  // the npc is covered by an exclusion
						}
						//loginfo("check 2");
						// get rule
						Distribution::NPCTPLTInfo npcinfo = Distribution::ExtractTemplateInfo(npc);
						Distribution::Rule* rl = Distribution::CalcRule(npc, acs, is, &npcinfo);
						//loginfo("check 3");
						//logwarn("[CheckActorsForRules] got rule");

						//Utility::ToLower(std::string(npc->GetFormEditorID())).find("lvl") == std::string::npos
						if (rl && rl->ruleName == DefaultRuleName && !IsLeveledChar(npc)) {
							// lookup plugin of the actor red
							//loginfo("check 4");
							name = std::string_view{ "" };

							if ((npc->GetFormID() >> 24) != 0xFE) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(npc->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
							}
							//loginfo("check 4.1");
							if (name.empty()) {
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							//loginfo("check 4.2");
							if (lightplugin && (npc->GetFormID() & 0x00000FFF) < 0x800) {
								iter++;
								//coun++;
								continue;
							}
							if (name.empty() == false)
								out << name << ";";
							else
								out << ";";
							//loginfo("check 5");
							// we found an actor that does not have a rule, so print that to the output
							out << npc->GetName() << ";"
								<< "0x" << Utility::GetHex(npc->GetFormID()) << ";"
								<< ";";
							//loginfo("check 6");
							//logwarn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							if (npc->GetRace())
								out << npc->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(npc->GetRace()->GetFormID()) << ";";
							else
								out << ";;";

							//loginfo("check 7");

							for (uint32_t i = 0; i < npc->factions.size(); i++) {
								out << ";"
									<< "0x" << Utility::GetHex(npc->factions[i].faction->GetFormID());
							}
							//loginfo("check 8");
							//logwarn("[CheckActorsForRules] end");
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
							//loginfo("iter 5 {}", Utility::GetHex(act->GetFormID()));
							name = std::string_view{ "" };
							if ((act->GetFormID() >> 24) != 0xFE) {
								file = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24));
								if (file == nullptr) {
									iter++;
									loginfo("invalid plugin");
									continue;
								}
								name = file->GetFilename();
							}
							//loginfo("iter 5.1");
							if (name.empty()) {
								//name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() << 8)) >> 20))->GetFilename();
								file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() & 0x00FFF000)) >> 12));
								if (file == nullptr) {
									iter++;
									loginfo("invalid plugin");
									continue;
								}
								name = file->GetFilename();
								lightplugin = true;
							}
							//loginfo("iter 5.2");
							loginfo("[Settings] [CheckCellForActors] {} named {} from {}", Utility::GetHex(act->GetFormID()), act->GetName(), name);
							//loginfo("iter 5.3");
						}

						// check wether there is a rule that applies
						if (Distribution::ExcludedNPC(act)) {
							iter++;
							//coun++;
							continue;  // the npc is covered by an exclusion
						}
						// get rule
						ActorInfo* acinfo = new ActorInfo(act, 0, 0, 0, 0, 0);
						Distribution::Rule* rl = Distribution::CalcRule(acinfo);
						delete acinfo;
						//loginfo("check 23");
						//logwarn("[CheckActorsForRules] got rule");
						if (rl && rl->ruleName == DefaultRuleName) {
							// lookup plugin of the actor red
							//loginfo("check 24");
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
							//loginfo("check 25");
							// we found an actor that does not have a rule, so print that to the output
							out << act->GetName() << ";"
								<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
								<< "0x" << Utility::GetHex(act->GetFormID()) << ";";
							//logwarn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							//loginfo("check 26");
							if (act->GetRace())
								out << act->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
							else
								out << ";;";
							//loginfo("check 27");
							//logwarn("[CheckActorsForRules] Race: {} {}", act->GetRace()->GetFormEditorID(), Utility::GetHex(act->GetRace()->GetFormID()));
							if (act->GetSaveParentCell())
								out << act->GetSaveParentCell()->GetName();

							//loginfo("check 28");
							//logwarn("[CheckActorsForRules] factions");
							act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
								if (a_faction)
									out << ";"
										<< "0x" << Utility::GetHex(a_faction->GetFormID());
								//logwarn("[CheckActorsForRules] Faction: {}", Utility::GetHex(a_faction->GetFormID()));
								return false;
							});
							//loginfo("check 29");
							//logwarn("[CheckActorsForRules] end");
							out << "\n";
							out.flush();
						}
					}
				}
			}
		} catch (...) {
			//logwarn("catch");
			out << ";";
		}
		try {
			iter++;
			//coun++;
		} catch (...) {
			//logwarn("catch finished");
			break;
		}
	}
	loginfo("[Settings] [CheckActorsForRules] finished checking...");
}

static std::binary_semaphore lockcells(1);

void Settings::CheckCellForActors(RE::FormID cellid)
{
	lockcells.acquire();
	loginfo("[Settings] [CheckCellForActors] checking cell {}...", Utility::GetHex(cellid));
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
					//loginfo("iter 1");
					act = (*iter)->As<RE::Actor>();
					//loginfo("iter 2");
					if (Utility::ValidateActor(act) && act->GetFormID() != 0x14) {
						//loginfo("iter 3");
						if (!visited.contains(act->GetFormID())) {
							//loginfo("iter 4");
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
								//loginfo("iter 5 {}", Utility::GetHex(act->GetFormID()));
								name = std::string_view{ "" };
								if ((act->GetFormID() >> 24) != 0xFE) {
									file = datahandler->LookupLoadedModByIndex((uint8_t)(act->GetFormID() >> 24));
									if (file == nullptr) {
										iter++;
										loginfo("invalid plugin");
										continue;
									}
									name = file->GetFilename();
								}
								//loginfo("iter 5.1");
								if (name.empty()) {
									//name = datahandler->LookupLoadedLightModByIndex((uint16_t)(((npc->GetFormID() << 8)) >> 20))->GetFilename();
									file = datahandler->LookupLoadedLightModByIndex((uint16_t)(((act->GetFormID() & 0x00FFF000)) >> 12));
									if (file == nullptr) {
										iter++;
										loginfo("invalid plugin");
										continue;
									}
									name = file->GetFilename();
									lightplugin = true;
								}
								//loginfo("iter 5.2");
								loginfo("[Settings] [CheckCellForActors] {} named {} from {}", Utility::GetHex(act->GetFormID()), act->GetName(), name);
								//loginfo("iter 5.3");
							}
							//loginfo("iter 6");
							if (Distribution::ExcludedNPC(act)) {
								excluded = true;
								LOG_1("{}[CheckCellForActors] excluded");
							}
							//loginfo("iter 7");
							// get rule
							ActorInfo* acinfo = new ActorInfo(act, 0, 0, 0, 0, 0);
							Distribution::Rule* rl = Distribution::CalcRule(acinfo);
							delete acinfo;
							//loginfo("iter 8");
							//loginfo("check 23");
							//logwarn("[CheckActorsForRules] got rule");
							// lookup plugin of the actor red
							//loginfo("check 24");
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
							//loginfo("check 25");
							// we found an actor that does not have a rule, so print that to the output
							out << act->GetName() << ";"
								<< "0x" << Utility::GetHex(act->GetActorBase()->GetFormID()) << ";"
								<< "0x" << Utility::GetHex(act->GetFormID()) << ";";
							//logwarn("[CheckActorsForRules] Actor: {} {} {} does not have a valid rule", act->GetName(), Utility::GetHex(act->GetActorBase()->GetFormID()), Utility::GetHex(act->GetFormID()));

							//loginfo("check 26");
							if (act->GetRace())
								out << act->GetRace()->GetFormEditorID() << ";"
									<< "0x" << Utility::GetHex(act->GetRace()->GetFormID()) << ";";
							else
								out << ";;";
							//loginfo("check 27");
							//logwarn("[CheckActorsForRules] Race: {} {}", act->GetRace()->GetFormEditorID(), Utility::GetHex(act->GetRace()->GetFormID()));
							if (act->GetSaveParentCell())
								out << act->GetSaveParentCell()->GetName();

							//loginfo("check 28");
							//logwarn("[CheckActorsForRules] factions");
							act->VisitFactions([&out](RE::TESFaction* a_faction, std::int8_t) {
								if (a_faction)
									out << ";"
										<< "0x" << Utility::GetHex(a_faction->GetFormID());
								//logwarn("[CheckActorsForRules] Faction: {}", Utility::GetHex(a_faction->GetFormID()));
								return false;
							});
							//loginfo("check 29");
							//logwarn("[CheckActorsForRules] end");
							out << "\n";
							out.flush();
						}
					}
					//loginfo("end iter");
				}
			} catch (...) {
				//logwarn("catch");
				out << ";";
			}
			try {
				iter++;
				//coun++;
			} catch (...) {
				//logwarn("catch finished");
				break;
			}
		}
	}
	lockcells.release();
	loginfo("[Settings] [CheckCellForActors] end");
}

void Settings::ApplySkillBoostPerks()
{
	auto datahandler = RE::TESDataHandler::GetSingleton();
	auto npcs = datahandler->GetFormArray<RE::TESNPC>();
	for(auto& npc : npcs) {
		if (npc && npc->GetFormID() != 0x7 && !Distribution::ExcludedNPC(npc) ){//&& !((npc->actorData.templateUseFlags & RE::ACTOR_BASE_DATA::TEMPLATE_USE_FLAG::kSpells))) {
			npc->AddPerk(Settings::AlchemySkillBoosts, 1);
			npc->AddPerk(Settings::PerkSkillBoosts, 1);
			LOGL1_3("{}[Settings] [AddPerks] Added perks to npc {}", npc->GetName());
		}
	}
	/*
	if (actor->HasPerk(Settings::AlchemySkillBoosts) == false) {
		actor->AddPerk(Settings::AlchemySkillBoosts, 1);
		loginfo("Adding Perk AlchemySkillBoosts");
	}
	if (actor->HasPerk(Settings::PerkSkillBoosts) == false) {
		actor->AddPerk(Settings::PerkSkillBoosts, 1);
		loginfo("Adding Perk PerkSkillBoosts");
	}*/

}

void Settings::ClassifyItems()
{
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
	_foodmagicka.clear();
	_foodstamina.clear();
	_foodhealth.clear();
	_foodall.clear();

	Data* data = Data::GetSingleton();
	data->ResetAlchItemEffects();

	std::vector<std::tuple<std::string, std::string>> ingredienteffectmap;

	// start sorting items

	auto begin = std::chrono::steady_clock::now();
	auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
	auto end = hashtable->end();
	auto iter = hashtable->begin();
	RE::AlchemyItem* item = nullptr;
	RE::IngredientItem* itemi = nullptr;
	while (iter != end) {
		if ((*iter).second && (*iter).second->IsMagicItem()) {
			item = (*iter).second->As<RE::AlchemyItem>();
			if (item) {
				LOGL1_4("{}[Settings] [ClassifyItems] Found AlchemyItem {}", item->GetName());
				// unnamed items cannot appear in anyones inventory normally so son't add them to our lists
				if (item->GetName() == nullptr || item->GetName() == (const char*)"" || strlen(item->GetName()) == 0 ||
					std::string(item->GetName()).find(std::string("Dummy")) != std::string::npos ||
					std::string(item->GetName()).find(std::string("dummy")) != std::string::npos) {
					iter++;
					continue;
				}
				// check whether item is excluded, or whether it is not whitelisted when in whitelist mode
				// if it is excluded and whitelisted it is still excluded
				if (Distribution::excludedItems()->contains(item->GetFormID()) ||
					Settings::_CompatibilityWhitelist && !Distribution::whitelistItems()->contains(item->GetFormID())) {
					iter++;
					continue;
				}
				// check whether the plugin is excluded
				if ((item->GetFormID() >> 24) != 0xFE && Distribution::excludedPlugins()->contains(uint8_t(item->GetFormID() >> 24)) == true) {
					iter++;
					continue;
				}
				// check whether the light plugin is excluded
				if ((item->GetFormID() >> 24) == 0xFE && Distribution::excludedPluginsLight()->contains((uint16_t)((item->GetFormID() & 0x00FFF000) >> 12)) == true) {
					iter++;
					continue;
				}

				auto clas = ClassifyItem(item);
				// set medicine flag for those who need it
				if (item->IsFood() == false && item->IsPoison() == false) {  //  && item->IsMedicine() == false
					item->data.flags = RE::AlchemyItem::AlchemyFlag::kMedicine | item->data.flags;
					if (EnableLoadLog && LogLevel >= 4) {
						LOGLE1_1("Item: {}", item->GetName());
						if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kCostOverride)
							LOGLE_1("\tFlag: CostOverride");
						if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kFoodItem)
							LOGLE_1("\tFlag: FoodItem");
						if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kExtendDuration)
							LOGLE_1("\tFlag: ExtendedDuration");
						if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kMedicine)
							LOGLE_1("\tFlag: Medicine");
						if (item->data.flags & RE::AlchemyItem::AlchemyFlag::kPoison)
							LOGLE_1("\tFlag: Poison");
					}
					LOGLE1_1("[Settings] [ClassifyItems] [AssignPotionFlag] {}", item->GetName());
				}

				// determine the type of item
				if (std::get<2>(clas) == ItemType::kFood) {
					// we will only classify food which works on stamina, magicka or health for now
					if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
						(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealRate)) > 0) {
						_foodhealth.insert(_foodhealth.end(), { std::get<0>(clas), item });
					}
					if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagicka)) > 0 ||
						(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagickaRate)) > 0) {
						_foodmagicka.insert(_foodmagicka.end(), { std::get<0>(clas), item });
					}
					if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStamina)) > 0 ||
						(std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStaminaRate)) > 0) {
						_foodstamina.insert(_foodstamina.end(), { std::get<0>(clas), item });
					}
					_foodall.insert(_foodall.end(), { std::get<0>(clas), item });
				} else if (std::get<2>(clas) == ItemType::kPoison) {
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
				} else if (std::get<2>(clas) == ItemType::kPotion) {
					if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kBlood)) > 0)
						_potionsBlood.insert(_potionsBlood.end(), { std::get<0>(clas), item });
					else if ((std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kHealth)) > 0 ||
							 (std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kMagicka)) > 0 ||
							 (std::get<0>(clas) & static_cast<uint64_t>(AlchemyEffect::kStamina)) > 0) {
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
					} else if (std::get<0>(clas) != static_cast<uint64_t>(AlchemyEffect::kNone)) {
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
				}
				// add item into effect map
				data->SetAlchItemEffects(item->GetFormID(), std::get<0>(clas), std::get<3>(clas), std::get<4>(clas));
			}

			itemi = (*iter).second->As<RE::IngredientItem>();
			if (itemi) {
				LOGL1_4("{}[Settings] [ClassifyItems] Found IngredientItem {}", itemi->GetName());
				for (int i = 0; i < (int)itemi->effects.size(); i++) {
					auto sett = itemi->effects[i]->baseEffect;
					// just retrieve the effects, we will analyze them later
					if (sett) {
						ingredienteffectmap.push_back({ itemi->GetName(), Utility::ToString(ConvertToAlchemyEffectPrimary(sett)) });
					}
				}
			}
		}
		iter++;
	}
	PROF1_1("[PROF] [ClassifyItems] execution time: {} µs", std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()));

	// items initialised
	_itemsInit = true;

	LOGL1_1("{}[Settings] [ClassifyItems] _potionsWeak_main {}", potionsWeak_main()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsWeak_rest {}", potionsWeak_rest()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsStandard_main {}", potionsStandard_main()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsStandard_rest {}", potionsStandard_rest()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsPotent_main {}", potionsPotent_main()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsPotent_rest {}", potionsPotent_rest()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsInsane_main {}", potionsInsane_main()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsInsane_rest {}", potionsInsane_rest()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _potionsBlood {}", potionsBlood()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _poisonsWeak {}", poisonsWeak()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _poisonsStandard {}", poisonsStandard()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _poisonsPotent {}", poisonsPotent()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _poisonsInsane {}", poisonsInsane()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _foodmagicka {}", foodmagicka()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _foodstamina {}", foodstamina()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _foodhealth {}", foodhealth()->size());
	LOGL1_1("{}[Settings] [ClassifyItems] _foodall {}", foodall()->size());

	if (EnableLoadLog && LogLevel >= 4) {
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
		out << "foodmagicka\n";
		it = foodmagicka()->begin();
		while (it != foodmagicka()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "foodstamina\n";
		it = foodstamina()->begin();
		while (it != foodstamina()->end()) {
			if (!visited.contains(std::get<1>(*it)->GetFormID()))
				out << ";" << std::get<1>(*it)->GetName() << "\n"
					<< "1|7|<" << Utility::GetHex(std::get<1>(*it)->GetFormID()) << ",>\n";
			visited.insert(std::get<1>(*it)->GetFormID());
			it++;
		}
		out << "foodhealth\n";
		it = foodhealth()->begin();
		while (it != foodhealth()->end()) {
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

		/*for (int i = 0; i < potionsWeak_main()->size(); i++) {
				out << "\t" << std::get<1>(potionsWeak_main()[i])->GetFormID() << "\t\t" << std::get<1>(potionsWeak_main()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsWeak_rest\n";
			for (int i = 0; i < potionsWeak_rest()->size(); i++) {
				out << "\t" << std::get<1>(potionsWeak_rest()[i])->GetFormID() << "\t\t" << std::get<1>(potionsWeak_rest()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsStandard_main\n";
			for (int i = 0; i < potionsStandard_main()->size(); i++) {
				out << "\t" << std::get<1>(potionsStandard_main()[i])->GetFormID() << "\t\t" << std::get<1>(potionsStandard_main()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsStandard_rest\n";
			for (int i = 0; i < potionsStandard_rest()->size(); i++) {
				out << "\t" << std::get<1>(potionsStandard_rest()[i])->GetFormID() << "\t\t" << std::get<1>(potionsStandard_rest()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsPotent_main\n";
			for (int i = 0; i < potionsPotent_main()->size(); i++) {
				out << "\t" << std::get<1>(potionsPotent_main()[i])->GetFormID() << "\t\t" << std::get<1>(potionsPotent_main()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsPotent_rest\n";
			for (int i = 0; i < potionsPotent_rest()->size(); i++) {
				out << "\t" << std::get<1>(potionsPotent_rest()[i])->GetFormID() << "\t\t" << std::get<1>(potionsPotent_rest()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsInsane_main\n";
			for (int i = 0; i < potionsInsane_main()->size(); i++) {
				out << "\t" << std::get<1>(potionsInsane_main()[i])->GetFormID() << "\t\t" << std::get<1>(potionsInsane_main()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsInsane_rest\n";
			for (int i = 0; i < potionsInsane_rest()->size(); i++) {
				out << "\t" << std::get<1>(potionsInsane_rest()[i])->GetFormID() << "\t\t" << std::get<1>(potionsInsane_rest()[i])->GetFormEditorID() << "\n";
			}
			out << "potionsBlood\n";
			for (int i = 0; i < potionsBlood()->size(); i++) {
				out << "\t" << std::get<1>(potionsBlood()[i])->GetFormID() << "\t\t" << std::get<1>(potionsBlood()[i])->GetFormEditorID() << "\n";
			}
			out << "poisonsWeak\n";
			for (int i = 0; i < poisonsWeak()->size(); i++) {
				out << "\t" << std::get<1>(poisonsWeak()[i])->GetFormID() << "\t\t" << std::get<1>(poisonsWeak()[i])->GetFormEditorID() << "\n";
			}
			out << "poisonsStandard\n";
			for (int i = 0; i < poisonsStandard()->size(); i++) {
				out << "\t" << std::get<1>(poisonsStandard()[i])->GetFormID() << "\t\t" << std::get<1>(poisonsStandard()[i])->GetFormEditorID() << "\n";
			}
			out << "poisonsPotent\n";
			for (int i = 0; i < poisonsPotent()->size(); i++) {
				out << "\t" << std::get<1>(poisonsPotent()[i])->GetFormID() << "\t\t" << std::get<1>(poisonsPotent()[i])->GetFormEditorID() << "\n";
			}
			out << "poisonsInsane\n";
			for (int i = 0; i < poisonsInsane()->size(); i++) {
				out << "\t" << std::get<1>(poisonsInsane()[i])->GetFormID() << "\t\t" << std::get<1>(poisonsInsane()[i])->GetFormEditorID() << "\n";
			}
			out << "foodmagicka\n";
			for (int i = 0; i < foodmagicka()->size(); i++) {
				out << "\t" << std::get<1>(foodmagicka()[i])->GetFormID() << "\t\t" << std::get<1>(foodmagicka()[i])->GetFormEditorID() << "\n";
			}
			out << "foodstamina\n";
			for (int i = 0; i < foodstamina()->size(); i++) {
				out << "\t" << std::get<1>(foodstamina()[i])->GetFormID() << "\t\t" << std::get<1>(foodstamina()[i])->GetFormEditorID() << "\n";
			}
			out << "foodhealth\n";
			for (int i = 0; i < foodhealth()->size(); i++) {
				out << "\t" << std::get<1>(foodhealth()[i])->GetFormID() << "\t\t" << std::get<1>(foodhealth()[i])->GetFormEditorID() << "\n";
			}
			out << "foodall\n";
			for (int i = 0; i < foodall()->size(); i++) {
				out << "\t" << std::get<1>(foodall()[i])->GetFormID() << "\t\t" << std::get<1>(foodall()[i])->GetFormEditorID() << "\n";
			}*/
	}
}

std::tuple<uint64_t, ItemStrength, ItemType, int, float> Settings::ClassifyItem(RE::AlchemyItem* item)
{
	RE::EffectSetting* sett = nullptr;
	if ((item->avEffectSetting) == nullptr && item->effects.size() == 0) {
		return { 0, ItemStrength::kStandard, ItemType::kFood, 0, 0.0f};
	}
	// we look at max 4 effects
	AlchemyEffectBase av[4]{
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
	// we will not abort the loop, since the number of effects on one item is normally very
	// limited, so we don't have much iterations
	AlchemyEffectBase tmp = 0;
	if (item->effects.size() > 0) {
		for (uint32_t i = 0; i < item->effects.size() && i < 4; i++) {
			sett = item->effects[i]->baseEffect;
			// just retrieve the effects, we will analyze them later
			if (sett) {
				mag[i] = item->effects[i]->effectItem.magnitude;
				dur[i] = item->effects[i]->effectItem.duration;

				uint32_t formid = sett->GetFormID();
				if ((tmp = (static_cast<uint64_t>(ConvertToAlchemyEffectPrimary(sett)))) > 0) {

					av[i] |= tmp;
				}
				if (sett->data.archetype == RE::EffectArchetypes::ArchetypeID::kDualValueModifier && (tmp = ((static_cast<uint64_t>(ConvertToAlchemyEffectSecondary(sett))))) > 0) {

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
		switch (item->avEffectSetting->data.primaryAV) {
		case RE::ActorValue::kHealth:
			av[0] = static_cast<uint64_t>(ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV));
			mag[0] = err.magnitude;
			dur[0] = 1;
			break;
		case RE::ActorValue::kMagicka:
			av[0] = static_cast<uint64_t>(ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV));
			mag[0] = err.magnitude;
			dur[0] = 1;
			break;
		case RE::ActorValue::kStamina:
			av[0] = static_cast<uint64_t>(ConvertToAlchemyEffect(item->avEffectSetting->data.primaryAV));
			mag[0] = err.magnitude;
			dur[0] = 1;
			break;
		}
	}
	// analyze the effect types
	AlchemyEffectBase alch = 0;
	ItemStrength str = ItemStrength::kWeak;
	float maxmag = 0;
	int maxdur = 0;
	for (int i = 0; i < 4; i++) {
		if ((av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kHealth)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kMagicka)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kStamina)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kHealRate)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kMagickaRate)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kStaminaRate)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kHealRateMult)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kMagickaRateMult)) > 0 ||
			(av[i] & static_cast<AlchemyEffectBase>(AlchemyEffect::kStaminaRateMult)) > 0) {
			if (mag[i] * dur[i] > maxmag) {
				maxmag = mag[i] * dur[i];
				maxdur = dur[i];
			}
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
	else if (maxmag <= _MaxMagnitudeWeak)
		str = ItemStrength::kWeak;
	else if (maxmag <= _MaxMagnitudeStandard)
		str = ItemStrength::kStandard;
	else if (maxmag <= _MaxMagnitudePotent)
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
		alch = static_cast<uint64_t>(AlchemyEffect::kBlood);
	}

	ItemType type = ItemType::kPotion;
	if (item->IsFood())
		type = ItemType::kFood;
	else if (item->IsPoison())
		type = ItemType::kPoison;

	return {
		alch,
		str,
		type,
		maxdur,
		maxmag
	};
}

#pragma endregion
