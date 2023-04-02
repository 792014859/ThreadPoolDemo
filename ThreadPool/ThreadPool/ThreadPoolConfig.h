#pragma once
#include "ThreadPoolDefine.h"

class ThreadPoolConfig {
public:
	int default_primary_thread_numbers = PRIMARY_THREAD_NUMBERS;
	int max_thread_size = MAX_THREAD_SIZE;
	int max_task_steal_range = MAX_TASK_STEAL_RANGE;
	int second_thread_ttl = SECONDARY_THREAD_TTL;
	int monitor_span = MONITOR_SPAN;
	int batch_size = BATCHSIZE;
	bool batch_task_enable = BATCHTASKENABLE;

private:
	friend class ThreadPrimary;
	friend class ThreadSecondary;

	int calcStealRange() const {
		int range = std::min(this->max_task_steal_range, this->default_primary_thread_numbers - 1);
		return range;
	}
};
