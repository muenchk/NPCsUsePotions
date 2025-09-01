#pragma once

#include <chrono>
#include <semaphore>
#include <sstream>
#include <string>
#include <math.h>

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#	define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define loginfo(a_fmt, ...) \
	{                \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__)); \
	}

#define logwarn(a_fmt, ...) \
	if (Logging::EnableLog) \
		static_cast<void>(logger::warn(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define logcritical(a_fmt, ...) \
	static_cast<void>(logger::critical(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#ifndef NDEBUG
#define logdebug(a_fmt, ...) \
	((void)0);
#else
#define logdebug(a_fmt, ...) \
	static_cast<void>(logger::debug(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));
#endif

// load logging with / without time stamps

#define LOGL_1(a_fmt, ...) \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOGL_2(a_fmt, ...)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOGL_3(a_fmt, ...)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOGL_4(a_fmt, ...)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

// regular logging with timestamps

#define LOG_1(a_fmt, ...)             \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_2(a_fmt, ...)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_3(a_fmt, ...)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_4(a_fmt, ...)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_5(a_fmt, ...)                                      \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

// profiling

#define PROF_1(...)                 \
	if (Logging::EnableProfiling) \
		static_cast<void>(profile(__func__, __VA_ARGS__));
#define PROF_2(...)                 \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 1) \
		static_cast<void>(profile(__func__, __VA_ARGS__));
#define PROF_3(...)                                             \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 2) \
		static_cast<void>(profile(__func__, __VA_ARGS__));


// exclusions

#define EXCL(s, t) \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logexcl(s, t));

// debug

#ifndef NDEBUG
#	define LogConsole(c_str) \
		((void)0);
#else
#	define LogConsole(c_str) \
		((void)0); // RE::ConsoleLog::GetSingleton()->Print(c_str);
#endif


class Logging
{
public:
	/// <summary>
	/// time the game was started
	/// </summary>
	static inline std::chrono::time_point<std::chrono::system_clock> execstart = std::chrono::system_clock::now();



	/// <summary>
	/// calculates and returns the time passed sinve programstart
	/// </summary>
	/// <returns></returns>
	static std::string TimePassed()
	{
		std::stringstream ss;
		ss << "[" << std::setw(12) << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - execstart) << "]";
		return ss.str();
	}

	/// <summary>
	/// Formats microseconds into a proper time string
	/// </summary>
	/// <returns></returns>
	static std::string FormatTime(long long microseconds)
	{
		std::stringstream ss;
		long long tmp = 0;
		if (microseconds >= 60000000) {
			tmp = (long long)trunc((long double)microseconds / 60000000);
			ss << std::setw(6) << tmp << "m ";
			microseconds -= tmp * 60000000;
		} //else
			//ss << "        ";
		if (microseconds >= 1000000) {
			tmp = (long long)trunc((long double)microseconds / 1000000);
			ss << std::setw(2) << tmp << "s ";
			microseconds -= tmp * 1000000;
		} //else
			//ss << "    ";
		if (microseconds >= 1000) {
			tmp = (long long)trunc((long double)microseconds / 1000);
			ss << std::setw(3) << tmp << "ms ";
			microseconds -= tmp * 1000;
		} //else
			//ss << "      ";
		ss << std::setw(3) << microseconds << "μs";
		return ss.str();
	}

	/// <summary>
	/// Whether logging is enables
	/// </summary>
	static inline bool EnableLog = false;
	/// <summary>
	/// Whether logging during loading the game is enabled
	/// </summary>
	static inline bool EnableLoadLog = false;
	/// <summary>
	/// Whether profiling is enabled
	/// </summary>
	static inline bool EnableProfiling = false;
	/// <summary>
	/// Whether generic Logging is enabled
	/// </summary>
	static inline bool EnableGenericLogging = true;
	/// <summary>
	/// The level of events to log
	/// </summary>
	static inline int LogLevel = 0;
	/// <summary>
	/// The level of functions to profile
	/// </summary>
	static inline int ProfileLevel = 0;
	/// <summary>
	/// Directory for logfiles
	/// </summary>
	static inline std::filesystem::path log_directory = "";
};

class Profile
{
	static inline std::ofstream* _stream = nullptr;
	static inline std::binary_semaphore lock{ 1 };

public:
	/// <summary>
	/// Inits profile log
	/// </summary>
	/// <param name="pluginname"></param>
	static void Init(std::string pluginname)
	{
		lock.acquire();
		_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + "_profile.log"), std::ios_base::out | std::ios_base::trunc);
		lock.release();
	}

	/// <summary>
	/// Closes profile log
	/// </summary>
	static void Close()
	{
		lock.acquire();
		if (_stream != nullptr) {
			_stream->flush();
			_stream->close();
			delete _stream;
			_stream = nullptr;
		}
		lock.release();
	}

	/// <summary>
	/// writes to the profile log
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="message"></param>
	template <class... Args>
	static void write(std::string message)
	{
		lock.acquire();
		if (_stream) {
			_stream->write(message.c_str(), message.size());
			_stream->flush();
		}
		lock.release();
	}
};
template <class... Args>
struct [[maybe_unused]] profile
{
	profile() = delete;

	explicit profile(
		std::string func,
		std::chrono::time_point<std::chrono::steady_clock> begin,
		fmt::format_string<Args...> a_fmt,
		Args&&... a_args,
		std::source_location a_loc = std::source_location::current())
	{
		std::string mes = fmt::format("{:<25} {} {} {:<30} [ExecTime:{:>11}]\t{}", std::filesystem::path(a_loc.file_name()).filename().string() + "(" + std::to_string(static_cast<int>(a_loc.line())) + "):", "[prof]", Logging::TimePassed(), "[" + func + "]", Logging::FormatTime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin).count()), fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n");
		Profile::write(mes);
	}
};

template <class... Args>
profile(std::string, std::chrono::time_point<std::chrono::steady_clock>, fmt::format_string<Args...>, Args&&...) -> profile<Args...>;


#define StartProfiling \
	auto begin = std::chrono::steady_clock::now();

#define TimeProfiling \
	begin


class LogUsage
{
	static inline std::ofstream* _stream = nullptr;
	static inline std::binary_semaphore lock{ 1 };

public:
	/// <summary>
	/// Inits item usage log
	/// </summary>
	/// <param name="pluginname"></param>
	static void Init(std::string pluginname)
	{
		lock.acquire();
		_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + "_usage.log"), std::ios_base::out | std::ios_base::trunc);
		lock.release();
	}

	/// <summary>
	/// Closes item usage log
	/// </summary>
	static void Close()
	{
		lock.acquire();
		if (_stream != nullptr) {
			_stream->flush();
			_stream->close();
			delete _stream;
			_stream = nullptr;
		}
		lock.release();
	}

	/// <summary>
	/// writes to the item usage log
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="message"></param>
	template <class... Args>
	static void write(std::string message)
	{
		lock.acquire();
		if (_stream) {
			_stream->write(message.c_str(), message.size());
			_stream->flush();
		}
		lock.release();
	}
};
template <class... Args>
struct [[maybe_unused]] logusage
{
	logusage() = delete;

	explicit logusage(
		fmt::format_string<Args...> a_fmt,
		Args&&... a_args)
	{
		std::string mes = "[usage] [" + Logging::TimePassed() + "] " + fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n";
		LogUsage::write(mes);
	}
};

template <class... Args>
logusage(fmt::format_string<Args...>, Args&&...) -> logusage<Args...>;


class LogExcl
{
	static inline std::ofstream* _stream = nullptr;
	static inline std::binary_semaphore lock{ 1 };

public:
	/// <summary>
	/// Inits item usage log
	/// </summary>
	/// <param name="pluginname"></param>
	static void Init(std::string pluginname)
	{
		lock.acquire();
		_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + "_excl.log"), std::ios_base::out | std::ios_base::trunc);
		lock.release();
	}

	/// <summary>
	/// Closes item usage log
	/// </summary>
	static void Close()
	{
		lock.acquire();
		if (_stream != nullptr) {
			_stream->flush();
			_stream->close();
			delete _stream;
			_stream = nullptr;
		}
		lock.release();
	}

	/// <summary>
	/// writes to the item usage log
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="message"></param>
	template <class... Args>
	static void write(std::string message)
	{
		lock.acquire();
		if (_stream) {
			_stream->write(message.c_str(), message.size());
			_stream->flush();
		}
		lock.release();
	}
};
template <class... Args>
struct [[maybe_unused]] logexcl
{
	logexcl() = delete;

	explicit logexcl(
		fmt::format_string<Args...> a_fmt,
		Args&&... a_args)
	{
		std::string mes = "[Exclusion] [" + Logging::TimePassed() + "] " + fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n";
		LogExcl::write(mes);
	}
};

template <class... Args>
logexcl(fmt::format_string<Args...>, Args&&...) -> logexcl<Args...>;
