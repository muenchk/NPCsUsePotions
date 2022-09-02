#include "Hooks.h"
#include "Events.h"
#include "Settings.h"
#include "Console.h"
#include "Game.h"
#include "NUPInterface.h"


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
		//spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
		spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);
	}
}

// VR
extern "C" DLLEXPORT bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	InitializeLog();
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_VR_1_4_15) {
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
		// Debug stuff
		if (Settings::_CheckActorsWithoutRules)
			Settings::CheckActorsForRules();
		// classify currently loaded game items
		Settings::ClassifyItems();
		logger::info("Items classified");
		// register eventhandlers
		Events::RegisterAllEventHandlers();
		logger::info("Registered Events");
		//Console::RegisterConsoleCommands();
		//logger::info("Registered Console Commands");
		
	}
}


extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	//SKSE::AllocTrampoline(1<<4);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		logger::error("[LOADPLUGIN] couldn't register listener");
		return false;
	}

	auto serialization = (SKSE::SerializationInterface*)a_skse->QueryInterface(SKSE::LoadInterface::kSerialization);
	if (!serialization) {
		logger::error("[LOADPLUGIN] couldn't get serialization interface");
		return false;
	}

	Game::SaveLoad::GetSingleton()->Register(serialization, 0xFD34899E);

	//Hooks::InstallHooks();

	return true;
}

extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI()
{
	auto api = NPCsUsePotions::NUPInterface::GetSingleton();

	logger::info("[API] NUP API requested and returned");

	return static_cast<void*>(api);
}
