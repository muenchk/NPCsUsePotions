#include "VM.h"

namespace ScriptObject
{
	ScriptObjectPtr FromForm(const RE::TESForm* a_form, const std::string& a_scriptName)
	{
		ScriptObjectPtr object;

		if (!a_form) {
			logger::warn("Cannot retrieve script object from a None form."sv);

			return object;
		}

		const auto vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
		if (!vm) {
			return object;
		}

		const auto typeID = static_cast<RE::VMTypeID>(a_form->GetFormType());
		const auto policy = vm->GetObjectHandlePolicy();
		const auto handle = policy ? policy->GetHandleForObject(typeID, a_form) : 0;

		if (!a_scriptName.empty()) {
			// Just call the virtual function if we know the script name
			vm->FindBoundObject(handle, a_scriptName.c_str(), object);

			if (!object) {
				LOG2_1("{}[ScriptObject] [FromForm] Script {} is not attached to form. {}"sv, a_scriptName, Utility::GetHex(a_form->GetFormID()));
			}
		} else {
			// Script name wasn't specified, so look for one in the internal structure
			RE::BSSpinLockGuard lk{ vm->attachedScriptsLock };
			if (auto it = vm->attachedScripts.find(handle); it != vm->attachedScripts.end()) {
				auto& scriptArray = it->second;
				const auto size = scriptArray.size();
				if (size == 1) {
					object = ScriptObjectPtr(scriptArray[0].get());
				} else if (size == 0) {
					LOG1_1("{}[ScriptObject] [FromForm] No scripts attached to. {}"sv, Utility::GetHex(a_form->GetFormID()));
					return nullptr;
				} else {
					LOG1_1("{}[ScriptObject] [FromForm] Multiple scripts are attached to. {}"sv, Utility::GetHex(a_form->GetFormID()));
					return nullptr;
				}
			}
		}

		return object;
	}
}
