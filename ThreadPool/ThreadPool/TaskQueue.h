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

	bool pop(T& value) {
		std::lock_guard<std::mutex> locker(m_mutex);
		if (m_queue.empty()) return false;
		value = std::move(*(m_queue.front()));
		m_queue.pop();
		return true;
	}

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


