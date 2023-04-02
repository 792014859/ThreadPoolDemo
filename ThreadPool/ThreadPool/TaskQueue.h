#pragma once
#include "UnCopyAble.h"
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
template<typename T>
class TaskQueue : private UnCopyAble
{
public:
	TaskQueue() = default;
	~TaskQueue() = default;
	
	//弹出一个任务
	bool popTask(T& value) {
		bool result = false;
		if (m_mutex.try_lock()) {
			if (!m_queue.empty()) {
				value = std::move(*(m_queue.front()));
				m_queue.pop();
				result = true;
			}
			m_mutex.unlock();
		}
		return result;
	}
	//弹出一批任务
	bool popTasks(std::vector<T>& tasks, int batchSize) {
		bool result = false;
		if (m_mutex.try_lock()) {
			while (!m_queue.empty() && batchSize--) {
				tasks.push_back(std::move(*(m_queue.front())));
				m_queue.pop();
				result = true;
			}
			m_mutex.unlock();
		}
		return result;
	}

	//添加一个任务
	void push(T&& value) {
		std::unique_ptr<T> task(std::make_unique<T>(std::move(value)));
		std::lock_guard<std::mutex> locker(m_mutex);
		m_queue.push(std::move(task));
		m_cond.notify_one();
	}

	bool empty() {
		std::lock_guard<std::mutex> locker(m_mutex);
		return m_queue.empty();
	}

private:
	std::mutex m_mutex;
	std::queue<std::unique_ptr<T>> m_queue;
	std::condition_variable m_cond;
};


