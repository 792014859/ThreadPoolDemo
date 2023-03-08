#pragma once
#include <vector>
#include <memory>
#include "UnCopyAble.h"

class OrdinaryTask : private UnCopyAble
{
public:
	template<typename T>
	OrdinaryTask(T&& f) : m_ptr(std::make_unique<taskDerided<T>>(std::forward<T>(f))) {}
	OrdinaryTask() : m_ptr(nullptr) {}
	OrdinaryTask(OrdinaryTask&& task) noexcept : m_ptr(std::move(task.m_ptr)) {}
	OrdinaryTask& operator=(OrdinaryTask&& task) noexcept {
		m_ptr = std::move(task.m_ptr);
		return *this;
	}
	void operator()() {
		if (m_ptr != nullptr) m_ptr->call();
	}

private:
	struct taskBased {
		virtual void call() = 0;
		virtual ~taskBased() = default;
	};

	template<typename T>
	struct taskDerided : public taskBased {
		T func;
		explicit taskDerided(T&& f) : func(std::move(f)) {}
		void call() override { func(); }
	};

	std::unique_ptr<taskBased> m_ptr;
};

