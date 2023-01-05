#pragma once

namespace Papyrus
{
	namespace Potions
	{
		void AnimatedPotions_Callback(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		void AnimatedPotions_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		void AnimatedPotions_RestorePoison(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		void Register(RE::BSScript::Internal::VirtualMachine* a_vm);
	}
}
