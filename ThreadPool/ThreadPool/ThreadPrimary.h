#pragma once
#include "ThreadBase.h"
#include "StealTaskQueue.h"
#include <vector>

class ThreadPrimary : public ThreadBase {
public:
	explicit ThreadPrimary() : ThreadBase() {
		m_index = 1;
		pool_threads = nullptr;
		m_thread_type = ThreadType::primary;
	}

protected:
	void init() override {
		if (is_init == false) {
			is_init = true;
			m_this_thread = std::move(std::thread(&ThreadPrimary::run, this));
		}
	}

	bool setThreadPoolInfo(int index, TaskQueue<OrdinaryTask>* pool_taskQ, std::vector<ThreadPrimary*>* poolThreads, ThreadPoolConfig* config) {
		if (is_init != false || pool_taskQ == nullptr || poolThreads == nullptr || config == nullptr) return false;
		this->m_index = index;
		this->m_pool_taskQ = pool_taskQ;
		this->pool_threads = poolThreads;
		this->m_config = config;
		return true;
	}

	void run() override {
		if (is_init == true && pool_threads != nullptr && m_config != nullptr) {
			if (std::any_of(pool_threads->begin(), pool_threads->end(), [](ThreadPrimary* thd) {return nullptr == thd; })) return;
			if (!m_config->batch_task_enable) {
				while (is_shutdown == false) {
					processTask();
				}
			}
			else {
				while (is_shutdown == false) {
					processTasks();
				}
			}
			
		}
	}

	//ִ�е�������
	void processTask() {
		OrdinaryTask task;
		if (popTask(task) || popPoolTask(task) || stealTask(task)) {
			runTask(task);
		}
		else {
			std::this_thread::yield();
		}
	}

	//����ִ������
	void processTasks() {
		std::vector<OrdinaryTask> tasks;
		if (popTask(tasks) || popPoolTasks(tasks) || stealTask(tasks)) {
			runTask(tasks);
		}
		else {
			std::this_thread::yield();
		}
	}

	//�ӱ���������е�����������
	bool popTask(OrdinaryTask& task) {
		return m_taskQ.popTask(task);
	}

	//�ӱ���������е���һ������
	bool popTask(std::vector<OrdinaryTask>& tasks) {
		return m_taskQ.popTasks(tasks, m_config->batch_size);
	}

	//͵ȡ����
	bool stealTask(OrdinaryTask& task) {
		if (pool_threads->size() < m_config->default_primary_thread_numbers) return false;
		int range = m_config->calcStealRange();
		for (int i = 0; i < range; ++i) {
			int curIndex = (m_index + i + 1) % m_config->default_primary_thread_numbers;
			if (curIndex != m_index && nullptr != (*pool_threads)[curIndex] && (*pool_threads)[curIndex]->m_taskQ.m_que1->size() >= 2) {
				if (m_taskQ.steal((*pool_threads)[curIndex]->m_taskQ) && popTask(task)) {
					return true;
				}
			}
		}
		return false;
	}

	//͵ȡ����
	bool stealTask(std::vector<OrdinaryTask>& tasks) {
		if (pool_threads->size() < m_config->default_primary_thread_numbers) return false;
		int range = m_config->calcStealRange();
		for (int i = 0; i < range; ++i) {
			int curIndex = (m_index + i + 1) % m_config->default_primary_thread_numbers;
			if (curIndex != m_index && nullptr != (*pool_threads)[curIndex]) {
				if (m_taskQ.steal((*pool_threads)[curIndex]->m_taskQ) && popTask(tasks)) {
					return true;
				}
			}
		}
		return false;
	}

private:
	int m_index; //��ǰ�̵߳ı��
	std::vector<ThreadPrimary*>* pool_threads; //���ڴ���̳߳��е���Ҫ�̵߳���Ϣ
	StealTaskQueue<OrdinaryTask> m_taskQ; //��ǰ�߳��е��������


	friend class ThreadPool;
	template<typename T>
	friend class StealTaskQueue;
};

