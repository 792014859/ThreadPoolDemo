#pragma once
#include <vector>
#include <list>
#include <future>
#include <thread>
#include <algorithm>
#include <memory>
#include <functional>
#include <iostream>

#include "ThreadPoolConfig.h"
#include "TaskQueue.h"
#include "OrdinaryTask.h"
#include "ThreadPrimary.h"
#include "ThreadSecondary.h"
#include "UnCopyAble.h"
class ThreadPool : public UnCopyAble
{
public:
	explicit ThreadPool(bool autoInit = true, const ThreadPoolConfig& config = ThreadPoolConfig()) noexcept {
		cur_index = 0;
		is_init = false;
		setConfig(config);
		is_monitor = true;
		monitor_thread = std::move(std::thread(&ThreadPool::monitor, this));
		if (autoInit) init();
	}
	~ThreadPool() {
		is_monitor = false;
		if (monitor_thread.joinable()) monitor_thread.join();
		destroy();
	}

	void setConfig(const ThreadPoolConfig& config) {
		if (is_init != false) return;
		m_config = config;
	}

	void init() {
		if (is_init) return;
		primary_threads.reserve(m_config.default_primary_thread_numbers);
		for (int i = 0; i < m_config.default_primary_thread_numbers; ++i) {
			ThreadPrimary* ptr = new ThreadPrimary();
			ptr->setThreadPoolInfo(i, &taskQ, &primary_threads, &m_config);
			ptr->init();
			primary_threads.emplace_back(ptr);
		}
		is_init = true;
	}

	void destroy() {
		if (is_init == false) return;
		is_init = false;
		int sum = 0;
		for (auto& pt : primary_threads) {
			sum += pt->m_total_task_nums;
			std::cout << pt->m_index << ":" << pt->m_total_task_nums << std::endl;
			pt->destroy();
			if (pt != nullptr) {
				delete pt;
				pt = nullptr;
			}
		}
		std::cout << "total_task_nums:" << sum << std::endl;
		primary_threads.clear();
		for (auto& st : secondary_threads) {
			st->destroy();
		}
		secondary_threads.clear();
	}

	//执行任务
	template<typename Func, typename... Args>
	auto commit(Func&& func, Args... args)->std::future<decltype((std::forward<Func>(func))(std::forward<Args>(args)...))> {
		using resultType = decltype((std::forward<Func>(func))(std::forward<Args>(args)...));
		std::function<resultType()> ff = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

		std::packaged_task<resultType()> task(ff);
		std::future<resultType> result(task.get_future());

		int realIndex = dispatch(-1);
		if (realIndex >= 0 && realIndex < m_config.default_primary_thread_numbers) {
			primary_threads[realIndex]->m_taskQ.push(std::move(task));
		}
		else {
			taskQ.push(std::move(task));
		}
		return result;
	}


	/*template<typename F, typename...Args>
	void submit(F&& f, Args... args)
	{
		if (is_init == false) return;
		using resultType = decltype((std::forward<F>(f))(std::forward<Args>(args)...));
		std::future<resultType> fut = commit(std::forward<F>(f), std::forward<Args>(args)...);
		std::chrono::system_clock::time_point deadline =
			std::chrono::system_clock::now() + std::chrono::milliseconds(this->m_config.second_thread_ttl);

		const auto& futStatus = fut.wait_until(deadline);
		switch (futStatus)
		{
			case std::future_status::ready: std::cout << "函数处理完成" << std::endl; break;
			case std::future_status::timeout: std::cout << "函数处理超时" << std::endl; break;
			case std::future_status::deferred: std::cout << "函数还未开始处理" << std::endl; break;
			default: break;
		}
	}*/

protected:
	int dispatch(int origIndex) {
		int realIndex = 0;
		if (-1 == origIndex) {
			realIndex = cur_index++;
			if (cur_index > m_config.max_thread_size || cur_index < 0) {
				cur_index = 0;
			}
		}
		else {
			realIndex = origIndex;
		}
		return realIndex;
	}

	void monitor() {
		while (is_monitor) {
			while (is_monitor && !is_init) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}
			int span = m_config.monitor_span;
			while (is_monitor && is_init && (span--) > 0) {
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			bool busy = std::all_of(primary_threads.begin(), primary_threads.end(),
				[](ThreadPrimary* ptr) {return nullptr != ptr && ptr->is_running; });
			if (busy && ((secondary_threads.size() + m_config.default_primary_thread_numbers) < m_config.max_thread_size)) {
				auto ptr = std::make_unique<ThreadSecondary>();
				ptr->setThreadPoolInfo(&taskQ, &m_config);
				ptr->init();
				secondary_threads.emplace_back(std::move(ptr));
			}

			for (auto iter = secondary_threads.begin(); iter != secondary_threads.end(); ) {
				if ((*iter)->freeze()) {
					secondary_threads.erase(iter++);
				}
				else {
					iter++;
				}
			}
		}
	}

private:
	bool is_init = false;
	bool is_monitor = true;
	int cur_index;
	TaskQueue<OrdinaryTask> taskQ;
	std::vector<ThreadPrimary*> primary_threads;
	std::list<std::unique_ptr<ThreadSecondary>> secondary_threads;
	ThreadPoolConfig m_config;
	std::thread monitor_thread;

};


