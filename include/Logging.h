#pragma once


#define loginfo(...) \
	{                \
		static_cast<void>(logger::info(__VA_ARGS__)); \
	}

#define logwarn(...) \
	static_cast<void>(logger::warn(__VA_ARGS__));

#define logcritical(...) \
	static_cast<void>(logger::critical(__VA_ARGS__));

#ifndef NDEBUG
#define logdebug(...) \
	((void)0);
#else
#define logdebug(...) \
	static_cast<void>(logger::debug(__VA_ARGS__));
#endif

// logging without time stamps

#define LOGE_1(...) \
	if (Logging::EnableLog) \
		static_cast<void>(logger::info(__VA_ARGS__));

#define LOGE1_1(s, t)        \
	if (Logging::EnableLog) \
		static_cast<void>(logger::info(s, t));

#define LOGE2_1(s, t, u)       \
	if (Logging::EnableLog) \
		static_cast<void>(logger::info(s, t, u));

#define LOGE2_1(s, t, u, v)    \
	if (Logging::EnableLog) \
		static_cast<void>(logger::info(s, t, u, v));

#define LOGE2_1(s, t, u, v, w) \
	if (Logging::EnableLog) \
		static_cast<void>(logger::info(s, t, u, v, w));

#define LOGE_2(...)                                       \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(__VA_ARGS__));

#define LOGE1_2(s, t)                                   \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t));

#define LOGE2_2(s, t, u)                                \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u));

#define LOGE3_2(s, t, u, v)                             \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u, v));

#define LOGE4_2(s, t, u, v, w)                           \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u, v, w));

#define LOGE5_2(s, t, u, v, x, y)                       \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u, v, x, y));

// load logging with / without time stamps

#define LOGL_1(s)           \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOGL1_1(s, t)        \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOGL2_1(s, t, u)     \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOGL3_1(s, t, u, v)  \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v));

#define LOGL4_1(s, t, u, v, w) \
	if (Logging::EnableLoadLog)   \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOGL5_1(s, t, u, v, w, x) \
	if (Logging::EnableLoadLog)      \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w, x));

#define LOGL_2(s)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOGL1_2(s, t)                                  \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOGL2_2(s, t, u)                               \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOGL3_2(s, t, u, v)                            \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOGL4_2(s, t, u, v, w)                         \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOGL_3(s)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOGL1_3(s, t)                                  \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOGL2_3(s, t, u)                               \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOGL_4(s)                                      \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOGL1_4(s, t)                                  \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOGL2_4(s, t, u)                               \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOGLE_1(...)         \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(__VA_ARGS__));

#define LOGLE1_1(s, t)       \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, t));

#define LOGLE2_1(s, t, u)    \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, t, u));

#define LOGLE2_1(s, t, u, v) \
	if (Logging::EnableLoadLog) \
		static_cast<void>(logger::info(s, t, u, v));

#define LOGLE2_1(s, t, u, v, w) \
	if (Logging::EnableLoadLog)    \
		static_cast<void>(logger::info(s, t, u, v, w));

#define LOGLE_2(...)                                   \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(__VA_ARGS__));

#define LOGLE1_2(s, t)                                 \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t));

#define LOGLE2_2(s, t, u)                              \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u));

#define LOGLE3_2(s, t, u, v)                           \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u, v));

#define LOGLE4_2(s, t, u, v, w)                        \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u, v, w));

#define LOGLE5_2(s, t, u, v, x, y)                     \
	if (Logging::EnableLoadLog && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, t, u, v, x, y));


// regular logging with timestamps

#define LOG_1(s)             \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOG1_1(s, t)         \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOG2_1(s, t, u)      \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOG3_1(s, t, u, v)   \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v));

#define LOG4_1(s, t, u, v, w)  \
	if (Logging::EnableLog && Logging::EnableGenericLogging) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOG5_1(s, t, u, v, w, x) \
	if (Logging::EnableLog && Logging::EnableGenericLogging)     \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w, x));

#define LOG_2(s)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOG1_2(s, t)                                    \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOG2_2(s, t, u)                                 \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOG3_2(s, t, u, v)                               \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOG4_2(s, t, u, v, w)                               \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOG_3(s)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOG1_3(s, t)                                    \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOG2_3(s, t, u)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOG3_3(s, t, u, v)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v));

#define LOG4_3(s, t, u, v, w)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOG_4(s)                                        \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOG1_4(s, t)                                    \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOG2_4(s, t, u)                                 \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOG3_4(s, t, u, v)                               \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v));

#define LOG4_4(s, t, u, v, w)                                    \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOG4_4(s, t, u, v, w)                           \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 3) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));

#define LOG_5(s)                                      \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define LOG1_5(s, t)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define LOG2_5(s, t, u)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u));

#define LOG3_5(s, t, u, v)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v));

#define LOG4_5(s, t, u, v, w)                                  \
	if (Logging::EnableLog && Logging::EnableGenericLogging && Logging::LogLevel >= 4) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t, u, v, w));


#define PROF_1(s)                  \
	if (Logging::EnableProfiling) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define PROF1_1(s, t)              \
	if (Logging::EnableProfiling) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define PROF_2(s)                                                 \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define PROF1_2(s, t)                                             \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 1) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#define PROF_3(s)                                                 \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | "));

#define PROF1_3(s, t)                                             \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 2) \
		static_cast<void>(logger::info(s, Logging::TimePassed() + " | ", t));

#ifdef NDEBUG
#	define LogConsole(c_str) \
		RE::ConsoleLog::GetSingleton()->Print(c_str);
#else
#	define LogConsole(c_str) \
		((void)0);
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
		ss << std::setw(12) << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - execstart);
		return ss.str();
	}

	static inline bool EnableLog = false;
	static inline bool EnableLoadLog = false;
	static inline bool EnableProfiling = false;
	static inline bool EnableGenericLogging = true;
	static inline int LogLevel = 0;
	static inline int ProfileLevel = 0;
};
