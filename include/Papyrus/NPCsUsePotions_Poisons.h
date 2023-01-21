#pragma once

namespace Papyrus
{
	namespace Poisons
	{
		/// <summary>
		/// [Papyrus function] Applies poison to an actors weapon
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="actor">The actor to apply poison to</param>
		/// <param name="poisonDosage">the amount of dosages to apply</param>
		void AnimatedPoisons_Callback(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor, int poisonDosage);

		/// <summary>
		/// [Papyrus Function] Removes information about poisons to apply for an actor
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="actor">The actor, for which information is erased</param>
		void AnimatedPoisons_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		/// <summary>
		/// [Papyrus Function] Playes the sound of a poison being applied on an actor
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="actor">The actor the sound should be played for</param>
		void AnimatedPoisons_PlaySound(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		/// <summary>
		/// Registers papyrus functions
		/// </summary>
		/// <param name="a_vm"></param>
		void Register(RE::BSScript::Internal::VirtualMachine* a_vm);
	}
}
