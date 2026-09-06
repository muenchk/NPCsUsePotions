#include "UI/UIMonitorActorsWindow.h"
#include "ActorInfo.h"
#include "Events.h"
#include "Data.h"

namespace NPCsUsePotions
{
	namespace UserInterface
	{
		class UIColumns
		{
		public:
			enum ColumnID
			{
				ColName,
				ColFormID,
				ColHealth,
				ColDurHealth,
				ColMagicka,
				ColDurMagicka,
				ColStamina,
				ColDurStamina,
				ColDurFortify,
				ColDurRegen,
				ColNextFoodTime,
				ColDurCombat,
				ColGlobalCooldown,
			};
			std::string name = "";
			uint32_t formid = 0;
			std::string health = "";
			int durHealth = 0.f;
			std::string magicka = "";
			int durMagicka = 0.f;
			std::string stamina = "";
			int durStamina = 0.f;
			int durFortify = 0.f;
			int durRegen = 0.f;
			float nextFoodTime = 0.f;
			int durCombat = 0.f;
			int globalCooldown = 0.f;
		};


		void ActorMonitor::Draw()
		{
			static int32_t MAX_ITEMS = 100;
			static std::vector<UIColumns> elements(MAX_ITEMS);
			static int32_t count = 0;
			static std::shared_ptr<ActorInfo> playerinfo = Data::GetSingleton()->FindActor(RE::PlayerCharacter::GetSingleton());

			if (Events::Main::GetLastActorsUpdateTime() != _lastActorsUpdate) {
				_lastActorsUpdate = Events::Main::GetLastActorsUpdateTime();
				_actors.clear();
				Events::Main::GetActors(_actors);
			}

			// player
			elements[0].name = playerinfo->GetName();
			elements[0].formid = playerinfo->GetFormID();
			elements[0].health = fmt::format("{:.1f}/{:.1f} | {:.2f}", playerinfo->GetAV(RE::ActorValue::kHealth), playerinfo->GetAVMax(RE::ActorValue::kHealth), playerinfo->GetAVPercentage(RE::ActorValue::kHealth));
			elements[0].durHealth = playerinfo->GetDurHealth();
			elements[0].magicka = fmt::format("{:.1f}/{:.1f} | {:.2f}", playerinfo->GetAV(RE::ActorValue::kMagicka), playerinfo->GetAVMax(RE::ActorValue::kMagicka), playerinfo->GetAVPercentage(RE::ActorValue::kMagicka));
			elements[0].durMagicka = playerinfo->GetDurMagicka();
			elements[0].stamina = fmt::format("{:.1f}/{:.1f} | {:.2f}", playerinfo->GetAV(RE::ActorValue::kStamina), playerinfo->GetAVMax(RE::ActorValue::kStamina), playerinfo->GetAVPercentage(RE::ActorValue::kStamina));
			elements[0].durStamina = playerinfo->GetDurStamina();
			elements[0].durFortify = playerinfo->GetDurFortify();
			elements[0].durRegen = playerinfo->GetDurRegeneration();
			elements[0].nextFoodTime = playerinfo->GetNextFoodTime();
			elements[0].durCombat = playerinfo->GetDurCombat();
			elements[0].globalCooldown = playerinfo->GetGlobalCooldownTimer();
			count = 1;

			auto itr = _actors.begin();
			while (count < 100 && itr != _actors.end()) {
				elements[count].name = (*itr)->GetName();
				elements[count].formid = (*itr)->GetFormID();
				elements[count].health = fmt::format("{:.1f}/{:.1f} | {:.2f}", (*itr)->GetAV(RE::ActorValue::kHealth), (*itr)->GetAVMax(RE::ActorValue::kHealth), (*itr)->GetAVPercentage(RE::ActorValue::kHealth));
				elements[count].durHealth = (*itr)->GetDurHealth();
				elements[count].magicka = fmt::format("{:.1f}/{:.1f} | {:.2f}", (*itr)->GetAV(RE::ActorValue::kMagicka), (*itr)->GetAVMax(RE::ActorValue::kMagicka), (*itr)->GetAVPercentage(RE::ActorValue::kMagicka));
				elements[count].durMagicka = (*itr)->GetDurMagicka();
				elements[count].stamina = fmt::format("{:.1f}/{:.1f} | {:.2f}", (*itr)->GetAV(RE::ActorValue::kStamina), (*itr)->GetAVMax(RE::ActorValue::kStamina), (*itr)->GetAVPercentage(RE::ActorValue::kStamina));
				elements[count].durStamina = (*itr)->GetDurStamina();
				elements[count].durFortify = (*itr)->GetDurFortify();
				elements[count].durRegen = (*itr)->GetDurRegeneration();
				elements[count].nextFoodTime = (*itr)->GetNextFoodTime();
				elements[count].durCombat = (*itr)->GetDurCombat();
				elements[count].globalCooldown = (*itr)->GetGlobalCooldownTimer();
				count++;
				itr++;
			}

			ImGuiWindowFlags window_flags = 0;
			window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration;

			ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiCond_Appearing, ImVec2{ 0, 0 });
			ImGui::SetNextWindowSize(ImVec2{ (float)RE::BSGraphics::Renderer::GetScreenSize().width * 1.0f, (float)RE::BSGraphics::Renderer::GetScreenSize().height * 0.5f }, ImGuiCond_Appearing);

			auto [globalFont, globalFontSize] = LibImGuiUI::LibImGuiUI_APIv1::instance->GetFontByType(FontType::SmallFont);
			ImGui::PushFont(globalFont, globalFontSize);

			ImGuiStyle* style = ImGui::GetStyle();

			auto col = style->Colors[ImGuiCol_WindowBg];
			col.w = 0.2f;  //0.1f;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, col);

			if (ImGui::Begin("NUP Actor Monitor", nullptr, window_flags)) {
				static ImGuiTableFlags flags =
					ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_ScrollY;

				if (ImGui::BeginTable("itemtable", 13, flags, ImVec2(0.0f, 0.0f), 0.0f)) {  // ImGui::GetTextLineHeightWithSpacing() * rownum
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColName);
					ImGui::TableSetupColumn("FormID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColFormID);
					ImGui::TableSetupColumn("Health", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColHealth);
					ImGui::TableSetupColumn("Dur Health", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColDurHealth);
					ImGui::TableSetupColumn("Magicka", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColMagicka);
					ImGui::TableSetupColumn("Dur Magicka", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColDurMagicka);
					ImGui::TableSetupColumn("Stamina", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColStamina);
					ImGui::TableSetupColumn("Dur Stamina", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColDurStamina);
					ImGui::TableSetupColumn("Fortify", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColDurFortify);
					ImGui::TableSetupColumn("Regen", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColDurRegen);
					ImGui::TableSetupColumn("NextFoodTime", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColNextFoodTime);
					ImGui::TableSetupColumn("Combat", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColDurCombat);
					ImGui::TableSetupColumn("Global Cooldown", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthStretch, 50.f, UIColumns::ColumnID::ColGlobalCooldown);
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableHeadersRow();

					// DO SOME SORTING

					// use clipper for large vertical lists
					ImGuiListClipper clipper;
					clipper.Begin(count < (int32_t)elements.size() ? count : (int32_t)elements.size());
					while (clipper.Step()) {
						for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
							auto item = &elements[i];
							ImGui::PushID(i);
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(item->name.c_str());
							ImGui::TableNextColumn();
							ImGui::Text("%x", item->formid);
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(item->health.c_str());
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->durHealth);
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(item->magicka.c_str());
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->durMagicka);
							ImGui::TableNextColumn();
							ImGui::TextUnformatted(item->stamina.c_str());
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->durStamina);
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->durFortify);
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->durRegen);
							ImGui::TableNextColumn();
							ImGui::Text("%f", item->nextFoodTime);
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->durCombat);
							ImGui::TableNextColumn();
							ImGui::Text("%d", item->globalCooldown);
							ImGui::PopID();
						}
					}
					ImGui::EndTable();
				}

				ImGui::End();
			}
			ImGui::PopStyleColor();
			ImGui::PopFont();
		}

		void ActorMonitor::OnWindowOpened()
		{

		}

		void ActorMonitor::OnWindowClosed()
		{
		}

		void ActorMonitor::RevertGameCallback(SKSE::SerializationInterface * a_intfc)
		{
			loginfo("Reverting game, ActorMonitor");

		}

		ActorMonitor* ActorMonitor::GetSingleton()
		{
			static ActorMonitor* window = new ActorMonitor();
			return window;
		}

		util::shared_ptr<ActorMonitor> ActorMonitor::GetSingletonShared()
		{
			static util::shared_ptr<ActorMonitor> sharedwindow = util::create_shared<ActorMonitor>(GetSingleton());
			return sharedwindow;
		}

		Window::KeyInformation* ActorMonitor::GetCloseKeys()
		{
			return _closeKeys;
		}
		Window::KeyInformation* ActorMonitor::GetOpenKeys()
		{
			return _openKeys;
		}

		void ActorMonitor::Register()
		{
			loginfo("Register Diary Window");
			Init();

			Game::SaveLoad::GetSingleton()->RegisterForRevertCallback(0x02000002, std::bind(&ActorMonitor::RevertGameCallback, this, std::placeholders::_1));
			LOG_1("Registered {}", typeid(&ActorMonitor::RevertGameCallback).name());

			LibImGuiUI::LibImGuiUI_APIv1::instance->RegisterWindow(GetSingletonShared());
		}

		bool ActorMonitor::Open()
		{
			if (_open)
				return false;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _opened) > std::chrono::milliseconds(500)) {
				Window::Open();
				loginfo("Opening Diary Window");
				OnWindowOpened();
				_opened = std::chrono::steady_clock::now();
				return true;
			}
			return false;
		}

		bool ActorMonitor::Close()
		{
			if (!_open)
				return false;
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _opened) > std::chrono::milliseconds(500)) {
				_opened = std::chrono::steady_clock::now();
				OnWindowClosed();
				Window::Close();
				loginfo("Closing Diary Window");
				return true;
			}
			return false;
		}
	}
}
