#pragma once

#include <atomic>

class Spinlock
{
	std::atomic_flag* _flag;

public:
	Spinlock(std::atomic_flag& flag)
	{
		_flag = &flag;
		while (flag.test_and_set(std::memory_order_acquire));
	}

	Spinlock(const Spinlock&) = delete;

	~Spinlock()
	{
		_flag->clear(std::memory_order_release);
	}
};

class SpinlockA
{
	std::atomic_flag* _flag;

public:
	SpinlockA(std::atomic_flag& flag)
	{
		_flag = &flag;
		while (flag.test_and_set(std::memory_order_acquire))
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
			flag.wait(true, std::memory_order_relaxed)
#endif
				;
	}

	SpinlockA(const SpinlockA&) = delete;

	~SpinlockA()
	{
		_flag->clear(std::memory_order_release);
#if defined(__cpp_lib_atomic_wait) && __cpp_lib_atomic_wait >= 201907L
		_flag->notify_one();
#endif
	}
};
