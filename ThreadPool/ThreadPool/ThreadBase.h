#pragma once
#include <thread>
#include "TaskQueue.h"
#include "OrdinaryTask.h"
#include "ThreadPoolDefine.h"
#include "ThreadPoolConfig.h"
class ThreadBase
{
protected:
	explicit ThreadBase() {
		is_shutdown = false;
		is_init = false;
		is_running = false;
		m_total_task_nums = 0;
		m_config = nullptr;
		m_pool_taskQ = nullptr;
		m_thread_type = ThreadType::primary;
	}

	virtual ~ThreadBase() { reset(); }

	void destroy() {
		if (is_shutdown == false) reset();
	}

	virtual void init() {}
	virtual void run() {}

	//弹出一个任务
	bool popPoolTask(OrdinaryTask& task) {
		return m_pool_taskQ && m_pool_taskQ->popTask(task);
	}
	//弹出一批任务
	bool popPoolTasks(std::vector<OrdinaryTask>& tasks) {
		return m_pool_taskQ && m_pool_taskQ->popTasks(tasks, m_config->batch_size);
	}
	//执行一个任务
	void runTask(OrdinaryTask& task) {
		is_running = true;
		task();
		m_total_task_nums++;
		is_running = false;
	}
	//批量执行任务
	void runTask(std::vector<OrdinaryTask>& tasks) {
		is_running = true;
		for (auto& task : tasks) {
			task();
			m_total_task_nums++;
		}
		is_running = false;
	}
	void reset() {
		is_shutdown = true;
		if (m_this_thread.joinable()) m_this_thread.join();
		is_init = false;
		is_running = false;
		m_total_task_nums = 0;
	}

protected:
	bool is_shutdown; //标记线程是否关闭
	bool is_init; //标记初始化状态
	bool is_running; //标记线程是否正在执行
	ThreadType m_thread_type; //表示是主线程还是辅线程
	unsigned long long m_total_task_nums; //线程处理的任务的数量

	TaskQueue<OrdinaryTask>* m_pool_taskQ; //线程池任务队列
	ThreadPoolConfig* m_config; //配置参数信息

	std::thread m_this_thread; //当前线程
};


