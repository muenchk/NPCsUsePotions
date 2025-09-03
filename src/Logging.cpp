#include "Logging.h"

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
