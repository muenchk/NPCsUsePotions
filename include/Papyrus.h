#pragma once

namespace Papyrus
{
	/// <summary>
	/// Registers all available papyrus functions
	/// </summary>
	/// <param name="a_vm"></param>
	/// <returns></returns>
	bool Register(RE::BSScript::Internal::VirtualMachine* a_vm);
}
