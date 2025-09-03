#pragma once

#include <chrono>
#include <semaphore>
#include <sstream>
#include <string>
#include <math.h>
#include <atomic>
#include <memory>

#include "Threading.h"
#include "Types.h"

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#	define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

/* #define loginfo(a_fmt, ...) \
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
*/


#define loginfo(...)                                      \
		static_cast<void>(info(__func__ __VA_OPT__(, ) __VA_ARGS__)); \

#define logwarn(...)                                    \
		static_cast<void>(warn(__func__ __VA_OPT__(, ) __VA_ARGS__)); \

#define logcritical(...)                                \
		static_cast<void>(critical(__func__ __VA_OPT__(, ) __VA_ARGS__)); \

// load logging with / without time stamps

#define LOGL_1(a_fmt, ...) \
	if (Logging::EnableLoadLog) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));
		

#define LOGL_2(a_fmt, ...)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOGL_3(a_fmt, ...)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 2) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOGL_4(a_fmt, ...)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 3) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

// regular logging with timestamps

#define LOG_1(a_fmt, ...)             \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_2(a_fmt, ...)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_3(a_fmt, ...)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_4(a_fmt, ...)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

#define LOG_5(a_fmt, ...)                                      \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		loginfo(__func__, a_fmt __VA_OPT__(, ) __VA_ARGS__);
		//static_cast<void>(logger::info(fmt::runtime(fmt::format("{} {:<30} ", Logging::TimePassed(), "[" + std::string(__func__) + "]") + std::string(a_fmt)) __VA_OPT__(, ) __VA_ARGS__));

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
		RE::ConsoleLog::GetSingleton()->Print(c_str); // ((void)0);
#endif


#define StartProfiling \
	auto $$profiletimebegin$$ = std::chrono::steady_clock::now();
#define ResetProfiling \
	$$profiletimebegin$$ = std::chrono::steady_clock::now();

#define TimeProfiling \
	$$profiletimebegin$$


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
	/// returns time passed formatted
	/// </summary>
	/// <returns></returns>
	static std::string TimePassedFormatted()
	{
		return FormatTime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - execstart).count());
	}

	/// <summary>
	/// Formats microseconds into a proper time string
	/// </summary>
	/// <returns></returns>
	static std::string FormatTime(int64_t microseconds)
	{
		std::stringstream ss;
		int64_t tmp = 0;
		if (microseconds >= 60000000) {
			tmp = (int64_t)trunc((long double)microseconds / 60000000);
			ss << std::setw(6) << tmp << "m ";
			microseconds -= tmp * 60000000;
		} //else
			//ss << "        ";
		if (microseconds >= 1000000) {
			tmp = (int64_t)trunc((long double)microseconds / 1000000);
			ss << std::setw(2) << tmp << "s ";
			microseconds -= tmp * 1000000;
		} //else
			//ss << "    ";
		if (microseconds >= 1000) {
			tmp = (int64_t)trunc((long double)microseconds / 1000);
			ss << std::setw(3) << tmp << "ms ";
			microseconds -= tmp * 1000;
		} //else
			//ss << "      ";
		ss << std::setw(3) << microseconds << "μs";
		return ss.str();
	}

	/// <summary>
	/// Formats nanoseconds into a proper time string
	/// </summary>
	/// <returns></returns>
	static std::string FormatTimeNS(int64_t nanoseconds)
	{
		std::stringstream ss;
		int64_t tmp = 0;
		if (nanoseconds > 60000000000) {
			tmp = (int64_t)trunc((long double)nanoseconds / 60000000000);
			ss << std::setw(6) << tmp << "m ";
			nanoseconds -= tmp * 60000000000;
		} else
			ss << "        ";
		if (nanoseconds > 1000000000) {
			tmp = (int64_t)trunc((long double)nanoseconds / 1000000000);
			ss << std::setw(2) << tmp << "s ";
			nanoseconds -= tmp * 1000000000;
		} else
			ss << "    ";
		if (nanoseconds > 1000000) {
			tmp = (int64_t)trunc((long double)nanoseconds / 1000000);
			ss << std::setw(3) << tmp << "ms ";
			nanoseconds -= tmp * 1000000;
		} else
			ss << "      ";
		if (nanoseconds > 1000) {
			tmp = (int64_t)trunc((long double)nanoseconds / 1000);
			ss << std::setw(3) << tmp << "μs ";
			nanoseconds -= tmp * 1000;
		} else
			ss << "      ";
		ss << std::setw(3) << nanoseconds << "ns";
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

	enum class MessageType
	{
		Log,
		Profile,
		Usage,
		Exclusion
	};

private:
	static inline std::jthread* logger = nullptr;

	static inline ts_deque<std::pair<MessageType, std::string>> _queue;

	static inline std::condition_variable _queueWait;
	static inline std::mutex _queueMutex;

	static void AsyncLogger(std::stop_token stop);

public:

	static void BeginLogging()
	{
		if (logger != nullptr) {
			// if the thread is there, then destroy and delete it
			// if it is joinable and not running it has already finished, but needs to be joined before
			// it can be destroyed savely
			logger->request_stop();
			logger->join();
			logger->~jthread();
			delete logger;
			logger = nullptr;
		}
		logger = new std::jthread(AsyncLogger);
		//logger->detach();
	}

	static void EndLogging()
	{
		if (logger != nullptr) {
			logger->request_stop();
			logger->join();
			logger->~jthread();
			delete logger;
			logger = nullptr;
		}
	}

	static void LogMessage(MessageType type, std::string message)
	{
		_queue.push_back({ type, message });
	}
};


class Log
{
	static inline std::ofstream* _stream = nullptr;
	static inline std::binary_semaphore lock{ 1 };
	static inline std::mutex _m_lock;

public:
	/// <summary>
	/// Inits profile log
	/// </summary>
	/// <param name="pluginname"></param>
	static void Init(std::string pluginname, bool append = false)
	{
		std::unique_lock<std::mutex> guard(_m_lock);
		//lock.acquire();
		//auto path = SKSE::log::log_directory();
		//if (path.has_value()) {
		//	_stream = new std::ofstream(path.value() / pluginname / (pluginname + "log.log"), std::ios_base::out | std::ios_base::trunc);
		//}

		_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + ".log"), std::ios_base::out | std::ios_base::trunc);
		if (_stream == nullptr || _stream->is_open() == false)
			std::cout << "Cannot create Log!\n";
		//lock.release();
	}

	/// <summary>
	/// Closes log log
	/// </summary>
	static void Close()
	{
		std::unique_lock<std::mutex> guard(_m_lock);
		//lock.acquire();
		if (_stream != nullptr) {
			_stream->flush();
			_stream->close();
			delete _stream;
			_stream = nullptr;
		}
		//lock.release();
	}

	/// <summary>
	/// writes to the log log
	/// </summary>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="message"></param>
	template <class... Args>
	static void write(std::string message)
	{
		std::unique_lock<std::mutex> guard(_m_lock);
		//lock.acquire();
		if (_stream) {
			try {
				_stream->write(message.c_str(), message.size());
				_stream->flush();
			} catch (std::exception& e) {
				std::cout << "Error in logging stream: " << e.what() << "\n";
			}
		}
		//lock.release();
	}
};

template <class... Args>
struct [[maybe_unused]] info
{
	info() = delete;

	explicit info(
		std::string func,
		fmt::format_string<Args...> a_fmt,
		Args&&... a_args,
		std::source_location a_loc = std::source_location::current())
	{
		std::string mes = fmt::format("{:<25} {} {} {:<30}\t{}", std::filesystem::path(a_loc.file_name()).filename().string() + "(" + std::to_string(static_cast<int>(a_loc.line())) + "):", "[info]    ", Logging::TimePassed(), "[" + func + "]", fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n");
		Logging::LogMessage(Logging::MessageType::Log, mes);
	}
};

template <class... Args>
info(std::string, fmt::format_string<Args...>, Args&&...) -> info<Args...>;

template <class... Args>
struct [[maybe_unused]] warn
{
	warn() = delete;

	explicit warn(
		std::string func,
		fmt::format_string<Args...> a_fmt,
		Args&&... a_args,
		std::source_location a_loc = std::source_location::current())
	{
		std::string mes = fmt::format("{:<25} {} {} {:<30}\t{}", std::filesystem::path(a_loc.file_name()).filename().string() + "(" + std::to_string(static_cast<int>(a_loc.line())) + "):", "[warning] ", Logging::TimePassed(), "[" + func + "]", fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n");
		Logging::LogMessage(Logging::MessageType::Log, mes);
	}
};

template <class... Args>
warn(std::string, fmt::format_string<Args...>, Args&&...) -> warn<Args...>;

template <class... Args>
struct [[maybe_unused]] critical
{
	critical() = delete;

	explicit critical(
		std::string func,
		fmt::format_string<Args...> a_fmt,
		Args&&... a_args,
		std::source_location a_loc = std::source_location::current())
	{
		std::string mes = fmt::format("{:<25} {} {} {:<30}\t{}", std::filesystem::path(a_loc.file_name()).filename().string() + "(" + std::to_string(static_cast<int>(a_loc.line())) + "):", "[critical]", Logging::TimePassed(), "[" + func + "]", fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n");
		Logging::LogMessage(Logging::MessageType::Log, mes);
	}
};

template <class... Args>
critical(std::string, fmt::format_string<Args...>, Args&&...) -> critical<Args...>;

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
		std::string mes = fmt::format("{:<25} {} {} {:<30} [ExecTime:{:>11}]\t{}", std::filesystem::path(a_loc.file_name()).filename().string() + "(" + std::to_string(static_cast<int>(a_loc.line())) + "):", "[prof]", Logging::TimePassed(), "[" + func + "]", Logging::FormatTimeNS(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - begin).count()), fmt::format(a_fmt, std::forward<Args>(a_args)...) + "\n");
		Logging::LogMessage(Logging::MessageType::Profile, mes);
	}
};

template <class... Args>
profile(std::string, std::chrono::time_point<std::chrono::steady_clock>, fmt::format_string<Args...>, Args&&...) -> profile<Args...>;


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
		Logging::LogMessage(Logging::MessageType::Usage, mes);
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
		Logging::LogMessage(Logging::MessageType::Exclusion, mes);
	}
};

template <class... Args>
logexcl(fmt::format_string<Args...>, Args&&...) -> logexcl<Args...>;
