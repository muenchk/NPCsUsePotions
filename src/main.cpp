#include "Hooks.h"
#include "Events.h"
#include "Settings.h"

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif
		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}

// VR
extern "C" DLLEXPORT bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info)
{
	InitializeLog();
	info->infoVersion = SEKSE::PluginInfo::kInfoVersion;
	info->name = Plugin::NAME.data();
	info->version = Plugin::VERSION[0];

	if (skse->isEditor) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	logger::critical(FMT_STRING("Unsupported runtime version {}"), RUNTIME_VR_1_4_15.string());
	logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
	if (skse->runtimeVersion < RUNTIME_VR_1_4_15) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}
	return true;
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
		// load settings
		Settings::Load(); // also resaves the file
		logger::info("Settings loaded");
		// load distribution settings
		Settings::LoadDistrConfig();
		logger::info("Distribution configuration loaded");
		if (Settings::_CheckActorsWithoutRules)
			Settings::CheckActorsForRules();
		// classify currently loaded game items
		Settings::ClassifyItems();
		logger::info("Items classified");
		Events::RegisterAllEventHandlers();
		logger::info("Registered Events");
	}
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	//SKSE::AllocTrampoline(1<<4);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	//Hooks::InstallHooks();

	return true;
}
