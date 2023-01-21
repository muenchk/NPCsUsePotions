#pragma once

namespace Papyrus
{
	namespace Potions
	{
		/// <summary>
		/// [Papyrus Function] Applies a potion to an actor
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="actor">the actor to apply the potion to</param>
		void AnimatedPotions_Callback(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		/// <summary>
		/// [Papyrus Function] Erases information about an actor and potion
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="actor">actor to erase information for</param>
		void AnimatedPotions_Abort(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		/// <summary>
		/// [Papyrus Function] Restores a poison that was applied to an actors weapons prior
		/// </summary>
		/// <param name="a_vm"></param>
		/// <param name="a_stackID"></param>
		/// <param name=""></param>
		/// <param name="actor">the actor to restore the poison for</param>
		void AnimatedPotions_RestorePoison(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::StaticFunctionTag*, RE::Actor* actor);

		/// <summary>
		/// Registers papyrus functions
		/// </summary>
		/// <param name="a_vm"></param>
		void Register(RE::BSScript::Internal::VirtualMachine* a_vm);
	}
}
