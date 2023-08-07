#pragma once

#include "Logging.h"
#include "Utility.h"
using ScriptObjectPtr = RE::BSTSmartPointer<RE::BSScript::Object>;
using ScriptArrayPtr = RE::BSTSmartPointer<RE::BSScript::Array>;
using ScriptCallbackPtr = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>;
using ScriptArgs = std::unique_ptr<RE::BSScript::IFunctionArguments>;


namespace ScriptObject
{
	ScriptObjectPtr FromForm(const RE::TESForm* a_form, const std::string& a_scriptName);
}
