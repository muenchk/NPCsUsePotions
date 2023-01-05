#include <unordered_map>

#include "Compatibility.h"
#include "Data.h"
#include "Logging.h"
#include "Papyrus/NPCsUsePotions_Poisons.h"
#include "Statistics.h"
#include "Utility.h"

namespace Papyrus
{
	namespace Potions
	{
		const std::string script = "NPCsUsePotions_Potions";

		static Compatibility* comp = nullptr;

		/// <summary>
		/// Game audiomanager which plays sounds.
		/// </summary>
		static RE::BSAudioManager* audiomanager;

		void AnimatedPotions_Callback(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG1_2("{}[Papyrus] [Potion] [AnimatedPotions_Callback] called for actor {}", Utility::PrintForm(actor));

			RE::AlchemyItem* potion = comp->AnPoti_FindActorPotion(actor->GetFormID());
			if (potion == nullptr) {
				LOG_4("{}[Papyrus] [Potion] [AnimatedPotions_Callback] potion not found");
				return;
			}

			comp->AnPoti_RemoveActorPotion(actor->GetFormID());
			RE::ExtraPoison* poison = nullptr;
			bool poisonf = Utility::GetAppliedPoison(actor, poison);
			if (poisonf)
				comp->AnPoti_AddActorPoison(actor->GetFormID(), poison->poison, poison->count);

			// save statistics
			Statistics::Misc_PotionsAdministered++;
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->SetOwner(actor);

			RE::ActorEquipManager::GetSingleton()->EquipObject(actor, potion, extra, 1, nullptr, true, false, false);

			LOG1_2("{}[Papyrus] [Potion] [AnimatedPotions_Callback] Use Potion {}", Utility::PrintForm(potion));
			
		}

		void AnimatedPotions_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG1_2("{}[Papyrus] [Potion] [AnimatedPotions_Abort] called for actor {}", Utility::PrintForm(actor));

			comp->AnPoti_RemoveActorPotion(actor->GetFormID());
		}

		void AnimatedPotions_RestorePoison(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG1_2("{}[Papyrus] [Potion] [AnimatedPotions_RestorePoison] called for actor {}", Utility::PrintForm(actor));

			auto [poison, count] = comp->AnPoti_FindActorPoison(actor->GetFormID());
			if (poison == nullptr || count == 0) {
				LOG_4("{}[Papyrus] [Poison] [AnimatedPotions_RestorePoison] no poison to be applied");
				return;
			}

			comp->AnPoti_RemoveActorPoison(actor->GetFormID());

			LOG1_2("{}[Papyrus] [Poison] [AnimatedPotions_RestorePoison] Restore Poison {}", Utility::PrintForm(poison));
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->Add(new RE::ExtraPoison(poison, count));
			auto ied = actor->GetEquippedEntryData(false);
			if (ied) {
				ied->AddExtraList(extra);
			} else {
				ied = actor->GetEquippedEntryData(true);
				if (ied) {
					ied->AddExtraList(extra);
				}
			}
		}

		void Register(RE::BSScript::Internal::VirtualMachine* a_vm)
		{
			comp = Compatibility::GetSingleton();

			a_vm->RegisterFunction(std::string("AnimatedPotions_Callback"), script, AnimatedPotions_Callback);
			a_vm->RegisterFunction(std::string("AnimatedPotions_Abort"), script, AnimatedPotions_Abort);
			a_vm->RegisterFunction(std::string("AnimatedPotions_RestorePoison"), script, AnimatedPotions_RestorePoison);
		}
	}
}
