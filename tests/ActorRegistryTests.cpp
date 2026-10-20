#include "ActorRegistry.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

using namespace std::chrono_literals;

struct Actor
{
	std::function<void()> onDestroy;
	~Actor() { if (onDestroy) onDestroy(); }
};
using Registry = ActorRegistry<Actor>;
constexpr auto accept = [](const auto&) { return true; };
constexpr auto reject = [](const auto&) { return false; };

void check(bool value, const char* message)
{
	if (!value) throw std::runtime_error(message);
}

// Bounded negative control for the original lock scope. Without the timeout,
// this reproduces the engine-lock -> registry-lock -> engine-lock cycle.
class LegacyRegistry
{
public:
	template <class Validator> void validate(Validator validator)
	{
		std::lock_guard lock(mutex);
		validator();
	}
	void registerEvent() { std::lock_guard lock(mutex); }
private:
	std::mutex mutex;
};

void legacyDeadlockControl()
{
	LegacyRegistry registry;
	std::timed_mutex engineLock;
	std::unique_lock engineOwner(engineLock);
	std::promise<void> validationEntered;
	bool acquired = false;
	std::thread worker([&] {
		registry.validate([&] {
			validationEntered.set_value();
			acquired = engineLock.try_lock_for(250ms);
			if (acquired) engineLock.unlock();
		});
	});
	validationEntered.get_future().wait();
	registry.registerEvent();
	engineOwner.unlock();
	worker.join();
	check(!acquired, "negative control did not reproduce the original cycle");
}

void fixedLockCycle()
{
	for (int round = 0; round < 100; ++round) {
		Registry registry;
		auto actor = std::make_shared<Actor>();
		registry.insert(actor, 1);
		std::timed_mutex engineLock;
		std::unique_lock engineOwner(engineLock);
		std::promise<void> validationEntered;
		bool acquired = false;
		Registry::WeakSet selected;
		std::thread worker([&] {
			selected = registry.validate([&](const auto&) {
				validationEntered.set_value();
				acquired = engineLock.try_lock_for(2s);
				if (acquired) engineLock.unlock();
				return acquired;
			}, accept);
		});
		validationEntered.get_future().wait();
		// Registration callback arrives while the engine owns its actor lock.
		const bool registered = registry.contains(actor);
		engineOwner.unlock();
		worker.join();
		check(registered && acquired && selected.size() == 1,
		      "engine event was blocked behind actor validation");
	}
}

void membershipAndExpiry()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	auto b = std::make_shared<Actor>();
	check(!registry.insert({}, 0), "registered null actor");
	check(registry.insert(a, 1), "first insert failed");
	check(!registry.insert(a, 1), "duplicate insert succeeded");
	registry.insert(b, 2);
	a.reset();
	check(registry.validate(accept, accept).size() == 1, "expired actor selected");
	check(registry.size() == 1, "expired registration retained");
	registry.erase(std::uint32_t{2});
	check(registry.size() == 0, "form-ID removal failed");
}

void invalidVersusTemporarilyExcluded()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	registry.insert(a, 1);
	check(registry.validate(accept, reject).empty(), "excluded actor selected");
	check(registry.contains(a), "temporarily excluded actor unregistered");
	bool selectorCalled = false;
	check(registry.validate(reject, [&](const auto&) {
		selectorCalled = true;
		return true;
	}).empty(), "invalid actor selected");
	check(!selectorCalled && !registry.contains(a), "invalid actor handling failed");
}

void unregisterDuringValidation()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	registry.insert(a, 1);
	auto result = registry.validate([&](const auto& actor) {
		registry.erase(actor);
		return true;
	}, accept);
	check(result.empty() && registry.size() == 0, "removed actor resurrected");
}

void reregisterDuringValidation()
{
	for (const bool validationResult : { false, true }) {
		Registry registry;
		auto a = std::make_shared<Actor>();
		registry.insert(a, 1);
		auto result = registry.validate([&](const auto& actor) {
			registry.erase(actor);
			registry.insert(actor, 1);
			return validationResult;
		}, accept);
		check(result.empty(), "stale validation selected a new registration");
		check(registry.contains(a), "stale validation deleted a new registration");
		check(registry.validate(accept, accept).size() == 1, "new registration lost");
	}
}

void clearDuringValidation()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	registry.insert(a, 1);
	auto result = registry.validate([&](const auto& actor) {
		registry.clear();
		registry.insert(actor, 1);
		return false;
	}, accept);
	check(result.empty() && registry.contains(a), "clear reused a generation");
}

void laterCallbackRemovesEarlierResult()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	auto b = std::make_shared<Actor>();
	registry.insert(a, 1);
	registry.insert(b, 2);
	int calls = 0;
	auto result = registry.validate([&](const auto&) {
		if (++calls == 2) registry.clear();
		return true;
	}, accept);
	check(calls == 2 && result.empty(), "removed earlier result survived commit");
}

void registrationDuringValidation()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	auto b = std::make_shared<Actor>();
	registry.insert(a, 1);
	auto result = registry.validate([&](const auto&) {
		registry.insert(b, 2);
		return true;
	}, accept);
	check(result.size() == 1 && registry.contains(b), "new actor lost or validated early");
	check(registry.validate(accept, accept).size() == 2, "new actor missed next cycle");
}

void exceptionSafety()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	registry.insert(a, 1);
	try {
		registry.validate([](const auto&) -> bool { throw std::runtime_error("test"); }, accept);
		throw std::logic_error("validator did not throw");
	} catch (const std::runtime_error&) {}
	check(registry.contains(a), "throwing validation changed live membership");
	check(registry.validate(accept, accept).size() == 1, "mutex leaked after exception");
}

void lifetimeAndDestructorReentry()
{
	Registry registry;
	auto a = std::make_shared<Actor>();
	bool destroyed = false;
	a->onDestroy = [&] {
		check(registry.size() == 0, "destructor sees stale registration");
		destroyed = true;
	};
	registry.insert(a, 1);
	auto result = registry.validate([&](const auto& actor) {
		registry.erase(actor);
		a.reset();
		check(!destroyed, "snapshot failed to retain actor lifetime");
		return false;
	}, accept);
	check(result.empty() && destroyed, "snapshot reference was leaked");
}

void concurrentMembership()
{
	Registry registry;
	std::vector<std::shared_ptr<Actor>> actors;
	for (unsigned i = 0; i < 64; ++i) actors.push_back(std::make_shared<Actor>());
	std::atomic<bool> start = false;
	std::vector<std::thread> threads;
	for (unsigned worker = 0; worker < 4; ++worker) {
		threads.emplace_back([&, worker] {
			while (!start.load()) std::this_thread::yield();
			for (unsigned n = 0; n < 5000; ++n) {
				const auto id = (n + worker * 17) % 64;
				registry.insert(actors[id], id);
				(void)registry.contains(actors[id]);
				if (n % 3 == 0) registry.erase(actors[id]);
				if (n % 7 == 0) registry.erase(id);
			}
		});
	}
	threads.emplace_back([&] {
		while (!start.load()) std::this_thread::yield();
		for (unsigned n = 0; n < 2000; ++n) {
			(void)registry.validate(accept, accept);
			(void)registry.copy();
			if (n % 17 == 0) registry.clear();
		}
	});
	start = true;
	for (auto& thread : threads) thread.join();
	registry.clear();
	for (unsigned i = 0; i < actors.size(); ++i) registry.insert(actors[i], i);
	check(registry.copy().size() == actors.size(), "concurrent registry corrupted");
	check(registry.validate(accept, accept).size() == actors.size(), "final validation failed");
}

int main()
{
	const std::pair<const char*, void(*)()> tests[] = {
		{"original lock-cycle negative control", legacyDeadlockControl},
		{"fixed lock cycle (100 interleavings)", fixedLockCycle},
		{"membership, duplicates, expiry, form-ID removal", membershipAndExpiry},
		{"invalid versus temporarily excluded", invalidVersusTemporarilyExcluded},
		{"unregister during validation", unregisterDuringValidation},
		{"re-register during valid and invalid validation", reregisterDuringValidation},
		{"clear and re-register during validation", clearDuringValidation},
		{"later removal of an earlier candidate", laterCallbackRemovesEarlierResult},
		{"new registration waits for next cycle", registrationDuringValidation},
		{"validation exception safety", exceptionSafety},
		{"snapshot lifetime and destructor reentry", lifetimeAndDestructorReentry},
		{"concurrent membership stress", concurrentMembership}
	};
	try {
		for (const auto& [name, run] : tests) {
			run();
			std::cout << "PASS " << name << '\n';
		}
	} catch (const std::exception& e) {
		std::cerr << "FAIL " << e.what() << '\n';
		return 1;
	}
	std::cout << "All 12 actor-registry checks passed.\n";
}
