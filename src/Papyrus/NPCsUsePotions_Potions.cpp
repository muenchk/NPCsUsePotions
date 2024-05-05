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
			LOG_2("called for actor {}", Utility::PrintForm(actor));

			RE::AlchemyItem* potion = comp->AnPoti_FindActorPotion(actor->GetFormID());
			if (potion == nullptr) {
				LOG_4("potion not found");
				return;
			}

			logusage("Actor:\t{}\tItem:\t{}", Utility::PrintForm(actor), Utility::PrintForm(potion));

			comp->AnPoti_RemoveActorPotion(actor->GetFormID());
			RE::ExtraPoison* poison = nullptr;
			bool poisonf = Utility::GetAppliedPoison(actor, poison);
			if (poisonf)
				comp->AnPoti_AddActorPoison(actor->GetFormID(), poison->poison, poison->count);

			// save statistics
			Statistics::Misc_PotionsAdministered++;

			actor->DrinkPotion(potion, nullptr);
			//RE::ActorEquipManager::GetSingleton()->EquipObject(actor, potion, extra, 1, nullptr, true, false, false);

			LOG_2("Use Potion {}", Utility::PrintForm(potion));
			
		}

		void AnimatedPotions_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG_2("called for actor {}", Utility::PrintForm(actor));

			comp->AnPoti_RemoveActorPotion(actor->GetFormID());
		}

		void AnimatedPotions_RestorePoison(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG_2("called for actor {}", Utility::PrintForm(actor));

			auto [poison, count] = comp->AnPoti_FindActorPoison(actor->GetFormID());
			if (poison == nullptr || count == 0) {
				LOG_4("no poison to be applied");
				return;
			}

			comp->AnPoti_RemoveActorPoison(actor->GetFormID());

			LOG_2("Restore Poison {}", Utility::PrintForm(poison));
			//RE::ExtraDataList* extra = new RE::ExtraDataList();
			//extra->Add(new RE::ExtraPoison(poison, count));
			auto ied = actor->GetEquippedEntryData(false);
			if (ied) {
				ied->PoisonObject(poison, count);
			} else {
				ied = actor->GetEquippedEntryData(true);
				if (ied) {
					ied->PoisonObject(poison, count);
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
