#pragma once



class Spinlock
{
	std::atomic_flag* _flag;

public:
	Spinlock(std::atomic_flag& flag)
	{
		_flag = &flag;
		while (flag.test_and_set(std::memory_order_acquire))
			;
	}

	Spinlock(const Spinlock&) = delete;

	~Spinlock()
	{
		_flag->clear(std::memory_order_release);
	}
};
