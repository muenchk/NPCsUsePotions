
#include "Papyrus/NPCsUsePotions_Poisons.h"
#include "Logging.h"
#include "Data.h"
#include "Utility.h"
#include "Compatibility.h"
#include "Statistics.h"

namespace Papyrus
{
	namespace Poisons
	{
		const std::string script = "NPCsUsePotions_Poisons";

		static Compatibility* comp = nullptr;
		static Data* data = nullptr;

		/// <summary>
		/// Game audiomanager which plays sounds.
		/// </summary>
		static RE::BSAudioManager* audiomanager;

		void AnimatedPoisons_Callback(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor, int poisonDosage)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG1_2("{}[Papyrus] [Poison] [AnimatedPoisons_Callback] called for actor {}", Utility::PrintForm(actor));

			RE::AlchemyItem* poison = comp->AnPois_FindActorPoison(actor->GetFormID());
			if (poison == nullptr) {
				LOG_4("{}[Papyrus] [Poison] [AnimatedPoisons_Callback] poison not found");
				return;
			}

			comp->AnPois_RemoveActorPoison(actor->GetFormID());

			int dosage = poisonDosage;
			if (!Settings::Compatibility::AnimatedPoisons::_UsePoisonDosage || dosage == 0)
				dosage = data->GetPoisonDosage(poison);

			// save statistics
			Statistics::Misc_PoisonsUsed++;

			LOG1_2("{}[Papyrus] [Poison] [AnimatedPoisons_Callback] Use Poison {}", Utility::PrintForm(poison));
			RE::ExtraDataList* extra = new RE::ExtraDataList();
			extra->Add(new RE::ExtraPoison(poison, dosage));
			auto ied = actor->GetEquippedEntryData(false);
			if (ied) {
				ied->AddExtraList(extra);
				actor->RemoveItem(poison, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
			} else {
				ied = actor->GetEquippedEntryData(true);
				if (ied) {
					ied->AddExtraList(extra);
					actor->RemoveItem(poison, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);
				}
			}
		}

		void AnimatedPoisons_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG1_2("{}[Papyrus] [Poison] [AnimatedPoisons_Abort] called for actor {}", Utility::PrintForm(actor));

			comp->AnPois_RemoveActorPoison(actor->GetFormID());
		}

		void AnimatedPoisons_PlaySound(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor)
		{
			if (!actor) {
				a_vm->TraceStack("Actor not found", a_stackID);
				return;
			}
			LOG1_2("{}[Papyrus] [Poison] [AnimatedPoisons_PlaySound] called for actor {}", Utility::PrintForm(actor));

			RE::AlchemyItem* poison = comp->AnPois_FindActorPoison(actor->GetFormID());
			if (poison == nullptr) {
				LOG_4("{}[Papyrus] [Poison] [AnimatedPoisons_PlaySound] poison not found");
				return;
			}

			if (!audiomanager)
				audiomanager = RE::BSAudioManager::GetSingleton();
			{
				// play poison sound
				RE::BSSoundHandle handle;
				if (poison->data.consumptionSound)
					audiomanager->BuildSoundDataFromDescriptor(handle, poison->data.consumptionSound->soundDescriptor);
				else if (Settings::PoisonUse)
					audiomanager->BuildSoundDataFromDescriptor(handle, Settings::PoisonUse->soundDescriptor);
				handle.SetObjectToFollow(actor->Get3D());
				handle.SetVolume(1.0);
				handle.Play();
			}
		}

		void Register(RE::BSScript::Internal::VirtualMachine* a_vm)
		{
			comp = Compatibility::GetSingleton();
			data = Data::GetSingleton();

			a_vm->RegisterFunction(std::string("AnimatedPoisons_Callback"), script, AnimatedPoisons_Callback);
			a_vm->RegisterFunction(std::string("AnimatedPoisons_Abort"), script, AnimatedPoisons_Abort);
			a_vm->RegisterFunction(std::string("AnimatedPoisons_PlaySound"), script, AnimatedPoisons_PlaySound);
		}
	}
}
