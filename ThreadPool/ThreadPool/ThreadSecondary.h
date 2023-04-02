#pragma once
#include "ThreadBase.h"

class ThreadSecondary : public ThreadBase {
public:
	explicit ThreadSecondary() {
		cur_ttl = 0;
		m_thread_type = ThreadType::second;
	}

protected:
	void init() override {
		if (is_init != false || m_config == nullptr) return;
		cur_ttl = m_config->second_thread_ttl;
		is_init = true;
		m_this_thread = std::move(std::thread(&ThreadSecondary::run, this));
	}

	void run() override {
		if (is_init != true || m_config == nullptr) return;
		if (!m_config->batch_task_enable) {
			while (is_shutdown != true) {
				processTask();
			}
		}
		else {
			while (is_shutdown != true) {
				processTasks();
			}
		}
		
	}

	void setThreadPoolInfo(TaskQueue<OrdinaryTask>* pool_taskQ, ThreadPoolConfig* config) {
		if (is_init != false || pool_taskQ == nullptr || config == nullptr) return;
		m_pool_taskQ = pool_taskQ;
		m_config = config;
	}

	//执行单个任务
	void processTask() {
		OrdinaryTask task;
		if (popPoolTask(task)) {
			runTask(task);
		}
		else {
			std::this_thread::yield();
		}
	}

	//批量执行任务
	void processTasks() {
		std::vector<OrdinaryTask> tasks;
		if (popPoolTasks(tasks)) {
			runTask(tasks);
		}
		else {
			std::this_thread::yield();
		}
	}


	bool freeze() {
		if (is_running) {
			cur_ttl++;
			cur_ttl = std::min(cur_ttl, m_config->second_thread_ttl);
		}
		else {
			cur_ttl--;
		}
		return cur_ttl <= 0;
	}

private:
	int cur_ttl; //当前生存周期
	friend class ThreadPool;
};

