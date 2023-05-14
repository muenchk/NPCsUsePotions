#include <unordered_map>
#include <unordered_set>

#include "Tests.h"
#include "Utility.h"

namespace Tests
{

	/// <summary>
	/// Teleports the player to every cell in the game and calculates the distribution rules for all actors
	/// </summary>
	void TestHandler()
	{
		std::this_thread::sleep_for(10s);
		RE::UI* ui = RE::UI::GetSingleton();
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrder.csv";
		std::string pathid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrderID.csv";
		std::string pathfail = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellFail.csv";
		std::string pathfailid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellFailID.csv";
		std::unordered_set<uint32_t> done;
		std::unordered_set<std::string> excluded;
		std::unordered_set<uint32_t> excludedid;
		if (std::filesystem::exists(path)) {
			std::ifstream inp(path);
			std::ifstream inpid(pathid);
			std::ifstream dones("Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellCalculation.csv");
			std::string lineinp;
			std::string lastdone;
			std::string tmp;
			std::string id;
			uint32_t formid = 0;
			while (std::getline(dones, tmp))
				lastdone = tmp;
			size_t pos = lastdone.find(';');
			if (pos != std::string::npos) {
				lastdone = lastdone.substr(0, pos);
			}
			bool flag = false;
			while (std::getline(inp, lineinp) && std::getline(inpid, id)) {
				if (flag == true) {
					break;
				}
				if (lastdone == lineinp && lineinp != "Wilderness")
					flag = true;
				try {
					formid = static_cast<uint32_t>(std::stol(id, nullptr, 16));
					done.insert(formid);
				} catch (std::exception&) {
					//loginfo("tryna fail");
				}
			}
			excluded.insert(lineinp);
			if (std::filesystem::exists(pathfail)) {
				std::ifstream fail(pathfail);
				std::ifstream failid(pathfailid);
				while (std::getline(fail, tmp)) {
					excluded.insert(tmp);
				}
				while (std::getline(failid, tmp)) {
					try {
						formid = static_cast<uint32_t>(std::stol(tmp, nullptr, 16));
						excludedid.insert(formid);
					} catch (std::exception&) {
						//loginfo("tryna fail fail");
					}
				}
				fail.close();
				failid.close();
			}
			inp.close();
			dones.close();
			std::ofstream failout(pathfail);
			std::ofstream failoutid(pathfailid);
			auto it = excluded.begin();
			while (it != excluded.end()) {
				failout << *it << "\n";
				it++;
			}
			auto itr = excludedid.begin();
			while (itr != excludedid.end()) {
				failoutid << Utility::GetHex(*itr) << "\n";
				itr++;
			}
		}
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::ofstream outid = std::ofstream(pathid, std::ofstream::out);

		RE::TESObjectCELL* cell = nullptr;
		std::vector<RE::TESObjectCELL*> cs;
		{
			const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
			const RE::BSReadLockGuard locker{ lock };
			if (hashtable) {
				RE::TESObjectCELL* cell = nullptr;
				for (auto& [id, form] : *hashtable) {
					if (form) {
						cell = (form)->As<RE::TESObjectCELL>();
						if (cell) {
							cs.push_back(cell);
							out << cell->GetFormEditorID() << "\n";
							outid << Utility::GetHex(cell->GetFormID()) << "\n";
						}
					}
				}
			}
		}
		//auto hashtable = std::get<0>(RE::TESForm::GetAllForms());
		//auto iter = hashtable->begin();

		//loginfo("tryna start");
		//while (iter != hashtable->end()) {
		//	if ((*iter).second) {
		//		cell = ((*iter).second)->As<RE::TESObjectCELL>();
		//		if (cell) {
		//			out << cell->GetFormEditorID() << "\n";
		//			outid << Utility::GetHex(cell->GetFormID()) << "\n";
		//		}
		//	}
		//	iter++;
		//}
		out.close();
		outid.close();
		LogConsole("Start Test");
		loginfo("Start Test");
		//iter = hashtable->begin();
		//while (iter != hashtable->end()) {
		//	if ((*iter).second) {
		//		cell = ((*iter).second)->As<RE::TESObjectCELL>();
		for (size_t i = 0; i < cs.size(); i++) {
			cell = cs[(int)i];
			if (cell) {
				if (excludedid.contains(cell->GetFormID()) || done.contains(cell->GetFormID()) || std::string(cell->GetFormEditorID()) == "Wilderness") {
					//				iter++;
					continue;
				}
				while (ui->GameIsPaused()) {
					std::this_thread::sleep_for(100ms);
				}
				if (cell->GetRuntimeData().references.size() > 0) {
					char buff[70] = "Moving to cell:\t";
					strcat_s(buff, 70, cell->GetFormEditorID());
					LogConsole(buff);
					loginfo("Moving to cell:\t{}", cell->GetFormEditorID());
					RE::PlayerCharacter::GetSingleton()->MoveTo((*(cell->GetRuntimeData().references.begin())).get());
				}
				std::this_thread::sleep_for(7s);
			}
			//	}
			//	iter++;
		}
		LogConsole("Finished Test");
		loginfo("Finished Test");
		//loginfo("tryna end");
	}

	/// <summary>
	/// Calculates the distribution rules for all actors in all cells in the game
	/// </summary>
	void TestAllCells()
	{
		std::string path = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrder.csv";
		std::string pathid = "Data\\SKSE\\Plugins\\NPCsUsePotions\\NPCsUsePotions_CellOrderID.csv";
		std::ofstream out = std::ofstream(path, std::ofstream::out);
		std::ofstream outid = std::ofstream(pathid, std::ofstream::out);
		//loginfo("tryna start");
		{
			const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
			const RE::BSReadLockGuard locker{ lock };
			if (hashtable) {
				RE::TESObjectCELL* cell = nullptr;
				for (auto& [id, form] : *hashtable) {
					if (form) {
						cell = (form)->As<RE::TESObjectCELL>();
						if (cell) {
							out << cell->GetFormEditorID() << "\n";
							outid << Utility::GetHex(cell->GetFormID()) << "\n";
						}
					}
				}
			}
		}

		std::this_thread::sleep_for(10s);
		RE::UI* ui = RE::UI::GetSingleton();
		{
			const auto& [hashtable, lock] = RE::TESForm::GetAllForms();
			const RE::BSReadLockGuard locker{ lock };
			if (hashtable) {
				RE::TESObjectCELL* cell = nullptr;
				for (auto& [id, form] : *hashtable) {
					if (form) {
						cell = (form)->As<RE::TESObjectCELL>();
						if (cell) {
							while (ui->GameIsPaused()) {
								std::this_thread::sleep_for(100ms);
							}
							Settings::CheckCellForActors(cell->GetFormID());
						}
					}
				}
			}
		}
	}
}
