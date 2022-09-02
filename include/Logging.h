#pragma once



#define LOGE_1(s)            \
	if (Logging::EnableLog) \
		logger::info(s);

#define LOGE1_1(s, t)        \
	if (Logging::EnableLog) \
		logger::info(s, t);

#define LOGE2_1(s, t, u)       \
	if (Logging::EnableLog) \
		logger::info(s, t, u);

#define LOGE2_1(s, t, u, v)    \
	if (Logging::EnableLog) \
		logger::info(s, t, u, v);

#define LOGE2_1(s, t, u, v, w) \
	if (Logging::EnableLog) \
		logger::info(s, t, u, v, w);
#define LOGE_2(s)                                       \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s);

#define LOGE1_2(s, t)                                   \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, t);

#define LOGE2_2(s, t, u)                                \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, t, u);

#define LOGE3_2(s, t, u, v)                             \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, t, u, v);

#define LOGE4_2(s, t, u, v, w)                           \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, t, u, v, w);

#define LOGE5_2(s, t, u, v, x, y)                       \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, t, u, v, x, y);

#define LOG_1(s)             \
	if (Logging::EnableLog) \
		logger::info(s, Logging::TimePassed() + " | ");

#define LOG1_1(s, t)         \
	if (Logging::EnableLog) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define LOG2_1(s, t, u)      \
	if (Logging::EnableLog) \
		logger::info(s, Logging::TimePassed() + " | ", t, u);

#define LOG3_1(s, t, u, v)   \
	if (Logging::EnableLog) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v);

#define LOG4_1(s, t, u, v, w)  \
	if (Logging::EnableLog) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);

#define LOG5_1(s, t, u, v, w, x) \
	if (Logging::EnableLog)     \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w, x);

#define LOG_2(s)                                        \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ");

#define LOG1_2(s, t)                                    \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define LOG2_2(s, t, u)                                 \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ", t, u);

#define LOG3_2(s, t, u, v)                               \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);

#define LOG4_2(s, t, u, v, w)                               \
	if (Logging::EnableLog && Logging::LogLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);

#define LOG_3(s)                                        \
	if (Logging::EnableLog && Logging::LogLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ");

#define LOG1_3(s, t)                                    \
	if (Logging::EnableLog && Logging::LogLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define LOG2_3(s, t, u)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ", t, u);

#define LOG3_3(s, t, u, v)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v);

#define LOG4_3(s, t, u, v, w)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);

#define LOG_4(s)                                        \
	if (Logging::EnableLog && Logging::LogLevel >= 3) \
		logger::info(s, Logging::TimePassed() + " | ");

#define LOG1_4(s, t)                                    \
	if (Logging::EnableLog && Logging::LogLevel >= 3) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define LOG2_4(s, t, u)                                 \
	if (Logging::EnableLog && Logging::LogLevel >= 3) \
		logger::info(s, Logging::TimePassed() + " | ", t, u);

#define LOG3_4(s, t, u, v)                               \
	if (Logging::EnableLog && Logging::LogLevel >= 3) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v);

#define LOG4_4(s, t, u, v, w)                                    \
	if (Logging::EnableLog && Logging::LogLevel >= 3) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);

#define LOG4_4(s, t, u, v, w)                           \
	if (Logging::EnableLog && Logging::LogLevel >= 3) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);

#define LOG_5(s)                                      \
	if (Logging::EnableLog && Logging::LogLevel >= 4) \
		logger::info(s, Logging::TimePassed() + " | ");

#define LOG1_5(s, t)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 4) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define LOG2_5(s, t, u)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 4) \
		logger::info(s, Logging::TimePassed() + " | ", t, u);

#define LOG3_5(s, t, u, v)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 4) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v);

#define LOG4_5(s, t, u, v, w)                                  \
	if (Logging::EnableLog && Logging::LogLevel >= 4) \
		logger::info(s, Logging::TimePassed() + " | ", t, u, v, w);


#define PROF_1(s)                  \
	if (Logging::EnableProfiling) \
		logger::info(s, Logging::TimePassed() + " | ");

#define PROF1_1(s, t)              \
	if (Logging::EnableProfiling) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define PROF_2(s)                                                 \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ");

#define PROF1_2(s, t)                                             \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 1) \
		logger::info(s, Logging::TimePassed() + " | ", t);

#define PROF_3(s)                                                 \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ");

#define PROF1_3(s, t)                                             \
	if (Logging::EnableProfiling && Logging::ProfileLevel >= 2) \
		logger::info(s, Logging::TimePassed() + " | ", t);

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
	static inline bool EnableProfiling = false;
	static inline int LogLevel = 0;
	static inline int ProfileLevel = 0;
};
