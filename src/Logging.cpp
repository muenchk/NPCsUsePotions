#include "Logging.h"

std::string Logging::TimePassed()
{
	std::stringstream ss;
	ss << "[" << std::setw(12) << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - execstart) << "]";
	return ss.str();
}

std::string Logging::TimePassedFormatted()
{
	return FormatTime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - execstart).count());
}

std::string Logging::FormatTime(int64_t microseconds)
{
	std::stringstream ss;
	int64_t tmp = 0;
	if (microseconds >= 60000000) {
		tmp = (int64_t)trunc((long double)microseconds / 60000000);
		ss << std::setw(6) << tmp << "m ";
		microseconds -= tmp * 60000000;
	}  //else
	//ss << "        ";
	if (microseconds >= 1000000) {
		tmp = (int64_t)trunc((long double)microseconds / 1000000);
		ss << std::setw(2) << tmp << "s ";
		microseconds -= tmp * 1000000;
	}  //else
	//ss << "    ";
	if (microseconds >= 1000) {
		tmp = (int64_t)trunc((long double)microseconds / 1000);
		ss << std::setw(3) << tmp << "ms ";
		microseconds -= tmp * 1000;
	}  //else
	//ss << "      ";
	ss << std::setw(3) << microseconds << "μs";
	return ss.str();
}

std::string Logging::FormatTimeNS(int64_t nanoseconds)
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


void Logging::AsyncLogger(std::stop_token stop)
{
	while (stop.stop_requested() == false) {
		// main loop for writing logs
		std::unique_lock<std::mutex> guard(_queueMutex);
		_queueWait.wait_for(guard, std::chrono::milliseconds(100), [] { return _queue.empty() == false; });
		try {
			std::pair<MessageType, std::string> pair = _queue.get_pop_front();
			switch (pair.first) {
			case MessageType::Log:
				Log::write(pair.second);
				break;
			case MessageType::Profile:
				Profile::write(pair.second);
				break;
			case MessageType::Usage:
				LogUsage::write(pair.second);
				break;
			case MessageType::Exclusion:
				LogExcl::write(pair.second);
				break;
			}
		} catch (std::exception&) {
		}
	}
}

void Logging::BeginLogging()
{
	if (logger != nullptr) {
		// if the thread is there, then destroy and delete it
		// if it is joinable and not running it has already finished, but needs to be joined before
		// it can be destroyed savely
		logger->request_stop();
		logger->detach();
		logger->~jthread();
		delete logger;
		logger = nullptr;
	}
	logger = new std::jthread(AsyncLogger);
	//logger->detach();
}

void Logging::EndLogging()
{
	if (logger != nullptr) {
		logger->request_stop();
		logger->detach();
		logger->~jthread();
		delete logger;
		logger = nullptr;
	}
}

void Logging::LogMessage(MessageType type, std::string message)
{
	_queue.push_back({ type, message });
	_queueWait.notify_one();
}


void Log::Init(std::string pluginname, bool append)
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

void Log::Close()
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

void Log::write(std::string message)
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

void Profile::Init(std::string pluginname)
{
	lock.acquire();
	_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + "_profile.log"), std::ios_base::out | std::ios_base::trunc);
	lock.release();
}

void Profile::Close()
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

void Profile::write(std::string message)
{
	lock.acquire();
	if (_stream) {
		_stream->write(message.c_str(), message.size());
		_stream->flush();
	}
	lock.release();
}

void LogUsage::Init(std::string pluginname)
{
	lock.acquire();
	_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + "_usage.log"), std::ios_base::out | std::ios_base::trunc);
	lock.release();
}

void LogUsage::Close()
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
void LogUsage::write(std::string message)
{
	lock.acquire();
	if (_stream) {
		_stream->write(message.c_str(), message.size());
		_stream->flush();
	}
	lock.release();
}

void LogExcl::Init(std::string pluginname)
{
	lock.acquire();
	_stream = new std::ofstream(Logging::log_directory / pluginname / (pluginname + "_excl.log"), std::ios_base::out | std::ios_base::trunc);
	lock.release();
}
void LogExcl::Close()
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
void LogExcl::write(std::string message)
{
	lock.acquire();
	if (_stream) {
		_stream->write(message.c_str(), message.size());
		_stream->flush();
	}
	lock.release();
}
