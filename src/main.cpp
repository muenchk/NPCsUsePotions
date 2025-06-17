#include "AlchemyEffect.h"
#include "Hooks.h"
#include "Events.h"
#include "Settings.h"
#include "Console.h"
#include "Game.h"
#include "NUPInterface.h"
#include "DataStorage.h"
#include "ActorManipulation.h"
#include "Compatibility.h"
#include "Papyrus.h"

#include <string>
#include <ShlObj_core.h>

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
		Settings::log_directory = path.value();
		Logging::log_directory = Settings::log_directory;
		auto spath = path.value();
		spath /= Settings::PluginNamePlain;
		spath /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(spath.string(), true);
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

		Profile::Init(Settings::PluginNamePlain);
		LogUsage::Init(Settings::PluginNamePlain);
		LogExcl::Init(Settings::PluginNamePlain);
	}
}

extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);
	v.AuthorName("KoeniglichePM");

	v.UsesAddressLibrary();
	v.CompatibleVersions({ SKSE::RUNTIME_SSE_LATEST });
	v.UsesNoStructs();

	return v;
}();

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
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
	if (ver < SKSE::RUNTIME_LATEST_VR) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}
	return true;
} 

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	StartProfiling;
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kPreLoadGame:
		Settings::InitGameStuff();
		break;
	case SKSE::MessagingInterface::kDataLoaded:
		// init ActorInfo's statics
		ActorInfo::Init();
		// init Data
		Data::GetSingleton()->Init();
		// init game objects and load pluginnames
		Settings::InitGameStuff();
		// load settings
		Settings::Load();
		Settings::Save();
		loginfo("Settings loaded");
		// init ACM data access
		ACM::Init();
		// load distribution settings
		Settings::LoadDistrConfig();
		loginfo("Distribution configuration loaded");
		// before classifying items make sure compatibility loads everything it can
		Compatibility::GetSingleton()->Load();
		// prepare alchemic effect
		AlchemicEffect::Init();
		// classify currently loaded game items
		Settings::ClassifyItems();
		Settings::CleanAlchemyEffects();
		loginfo("Items classified");
		// register data storage
		// datastorage must always register game callbacks before events, to ensure read data is present
		Storage::Register();
		// register compatibility
		Compatibility::Register();
		// register eventhandlers
		Events::RegisterAllEventHandlers();
		loginfo("Registered Events");
		// register console commands
		Console::RegisterConsoleCommands();
		logger::info("Registered Console Commands");
		PROF_1(TimeProfiling, "DataLoad execution time.");
		break;
	case SKSE::MessagingInterface::kPostLoad:
		Settings::Interfaces::RequestAPIs();
		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	// find logging folder

	InitializeLog();

	loginfo("kAnyFood\t{}", Utility::GetHex((uint64_t)AlchemyBaseEffectSecond::kAnyFood));
	loginfo("kAnyFortify\t{}", Utility::GetHex((uint64_t)AlchemyBaseEffectSecond::kAnyFortify));
	loginfo("kAnyPoison\t{}", Utility::GetHex((uint64_t)AlchemyBaseEffectSecond::kAnyPoison));
	loginfo("kAnyPotion\t{}", Utility::GetHex((uint64_t)AlchemyBaseEffectSecond::kAnyPotion));
	loginfo("kAnyRegen\t{}", Utility::GetHex((uint64_t)AlchemyBaseEffectSecond::kAnyRegen));
	loginfo("kAnyFortifyRegen\t{}", Utility::GetHex(((uint64_t)AlchemyBaseEffectSecond::kAnyRegen | (uint64_t)AlchemyBaseEffectSecond::kAnyFortify)));
	loginfo("kAllPotions\t{}", Utility::GetHex((uint64_t)AlchemyBaseEffectSecond::kAllPotions));

	loginfo("Game Version: {}", a_skse->RuntimeVersion().string());

	loginfo("{} v{}", Plugin::NAME, Plugin::VERSION.string());
	profile(__func__, std::chrono::steady_clock::now(), "{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());
	logusage("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);

	SKSE::AllocTrampoline(1<<8);

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

	// register game load save events
	Game::SaveLoad::GetSingleton()->Register(serialization, 0xFD34899E);

	// register papyrus functions
	SKSE::GetPapyrusInterface()->Register(Papyrus::Register);

	// do not install fast travel hooks, since fasttravelend event is not available on VR
	//Hooks::InstallHooks();

	return true;
}

extern "C" DLLEXPORT void* SKSEAPI RequestPluginAPI()
{
	auto api = NPCsUsePotions::NUPInterface::GetSingleton();

	loginfo("[API] NUP API requested and returned");

	return static_cast<void*>(api);
}
