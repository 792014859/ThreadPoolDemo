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

	//����һ������
	bool popPoolTask(OrdinaryTask& task) {
		return m_pool_taskQ && m_pool_taskQ->popTask(task);
	}
	//����һ������
	bool popPoolTasks(std::vector<OrdinaryTask>& tasks) {
		return m_pool_taskQ && m_pool_taskQ->popTasks(tasks, m_config->batch_size);
	}
	//ִ��һ������
	void runTask(OrdinaryTask& task) {
		is_running = true;
		task();
		m_total_task_nums++;
		is_running = false;
	}
	//����ִ������
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
	bool is_shutdown; //����߳��Ƿ�ر�
	bool is_init; //��ǳ�ʼ��״̬
	bool is_running; //����߳��Ƿ�����ִ��
	ThreadType m_thread_type; //��ʾ�����̻߳��Ǹ��߳�
	unsigned long long m_total_task_nums; //�̴߳�������������

	TaskQueue<OrdinaryTask>* m_pool_taskQ; //�̳߳��������
	ThreadPoolConfig* m_config; //���ò�����Ϣ

	std::thread m_this_thread; //��ǰ�߳�
};


