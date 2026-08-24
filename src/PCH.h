#pragma once

#pragma warning(push)

#include <RE/Skyrim.h>
#include <REL/Relocation.h>
#include <SKSE/SKSE.h>
#include <xbyak/xbyak.h>

#include <imgui.h>
#include "imgui_internal.h"

#include <LibSUtils/LibSUtils.h>
#include <LibImGuiUI/LibImGuiUI_API.h>

#include <nlohmann/json.hpp>

#ifdef NDEBUG
#	include <spdlog/sinks/basic_file_sink.h>
#else
#	include <spdlog/sinks/msvc_sink.h>
#endif
#pragma warning(pop)

using namespace std::literals;

using namespace LibSUtils;

namespace logger = SKSE::log;

namespace util
{
	using SKSE::stl::report_and_fail;
}

#define DLLEXPORT __declspec(dllexport)

#include "Plugin.h"
