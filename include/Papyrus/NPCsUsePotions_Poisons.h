#pragma once

namespace Papyrus
{
	namespace Poisons
	{
		void AnimatedPoisons_Callback(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor, int poisonDosage);

		void AnimatedPoisons_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		void AnimatedPoisons_PlaySound(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		void Register(RE::BSScript::Internal::VirtualMachine* a_vm);
	}
}
