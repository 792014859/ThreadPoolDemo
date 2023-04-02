#pragma once
#include <thread>
#include <mutex>
#include <memory>

using UNIQUE_LOCK = std::unique_lock<std::mutex>;
using GUARD_LOCK = std::lock_guard<std::mutex>;

static const int CPU_NUM = static_cast<int>(std::thread::hardware_concurrency());							//CPU核数

enum class ThreadType : char { primary, second };
//static const int THREAD_TYPE_PRIMARY = 1;																	//主线程标志
//static const int THREAD_TYPE_SECOND = 2;																	//辅线程标志

static const int PRIMARY_THREAD_NUMBERS = (CPU_NUM > 8) ? CPU_NUM : 8;										//默认主线程个数
static const int MAX_THREAD_SIZE = (PRIMARY_THREAD_NUMBERS * 2);									//最大的线程个数
static const int MAX_TASK_STEAL_RANGE = 16;																	//盗取机制相邻原则
static const int SECONDARY_THREAD_TTL = 3;															// 辅助线程ttl，单位为s
static const int MONITOR_SPAN = 1;																	// 监控线程执行间隔，单位为s
static const int BATCHSIZE = 3;																	//批量执行任务的任务数
static const int BATCHTASKENABLE = false;														//是否开启批量处理

static const unsigned int MAX_BLOCK_TTL = 10000000;                                         // 最大阻塞时间，单位为ms
