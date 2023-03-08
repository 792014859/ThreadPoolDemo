#pragma once
#include "UnCopyAble.h"
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <assert.h>



template<typename T>
class StealTaskQueue :
    private UnCopyAble
{
public:
    StealTaskQueue() {
        m_que1 = new std::queue<std::unique_ptr<T>>();
        m_que2 = new std::queue<std::unique_ptr<T>>();
    }

    ~StealTaskQueue() {
        if (m_que1 != nullptr)
        {
            delete m_que1;
            m_que1 = nullptr;
        }
        if (m_que2 != nullptr) {
            delete m_que2;
            m_que2 = nullptr;
        }
    }

    void push(T&& value) {
        while (true) {
            if (m_mutex.try_lock()) {
                std::unique_ptr<T> task(std::make_unique<T>(std::move(value)));
                m_que1->push(std::move(task));
                m_cond.notify_one();
                m_mutex.unlock();
                break;
            }
            else {
                std::this_thread::yield();
            }
        }
    }

    bool pop(T& value) {
        std::lock_guard <std::mutex> locker(m_mutex);
        if (m_que1->empty()) return false;
        value = std::move(*(m_que1->front()));
        m_que1->pop();
        return true;
    }

    bool empty() {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_que1->empty();
    }

    bool steal(StealTaskQueue<T>& tq) {
        if (tq.empty()) return false;
        tq.adjust();
        std::lock_guard<std::mutex> locker(m_mutex);
        std::swap(m_que1, tq.m_que2);
        return true;
    }
private:
    void adjust() {
        std::lock_guard<std::mutex> locker(m_mutex);
        int size1 = m_que1->size();
        int size2 = m_que2->size();
        while (size1 - size2 >= 2) {
            m_que2->push(std::move(m_que1->front()));
            m_que1->pop();
            size1--;
            size2++;
        }
    }

private:
    std::mutex m_mutex;
    std::queue<std::unique_ptr<T>>* m_que1; //正常队列
    std::queue<std::unique_ptr<T>>* m_que2; //被偷取队列
    std::condition_variable m_cond;

};


