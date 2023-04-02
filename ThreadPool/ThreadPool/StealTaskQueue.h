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
        m_que1 = std::make_unique<std::queue<std::unique_ptr<T>>>(std::queue<std::unique_ptr<T>>());
    }

    ~StealTaskQueue() {
    }

    //添加一个任务
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

    //弹出一个任务
    bool popTask(T& value) {
        bool result = false;
        if (m_mutex.try_lock()) {
            if (!m_que1->empty()) {
                value = std::move(*(m_que1->front()));
                m_que1->pop();
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
            while (!m_que1->empty() && batchSize--) {
                tasks.push_back(std::move(*(m_que1->front())));
                m_que1->pop();
                result = true;
            }
            m_mutex.unlock();
        }
        return result;
    }

    bool empty() {
        std::lock_guard<std::mutex> locker(m_mutex);
        return m_que1->empty();
    }

    bool steal(StealTaskQueue<T>& tq) {
        bool result = false;
        if (m_mutex.try_lock()) {
            result = tq.adjust(m_que1);
            m_mutex.unlock();
        }
        return result;
    }
private:
    bool adjust(std::unique_ptr<std::queue<std::unique_ptr<T>>>& tem) {
        bool result = false;
        if (m_mutex.try_lock()) {
            int size = m_que1->size() / 2;
            int cnt = 0;
            while (!m_que1->empty() && size--) {
                tem->push(std::move(m_que1->front()));
                m_que1->pop();
                cnt++;
                result = true;
            }
            std::cout << "偷到任务数：" << cnt << std::endl;
            m_mutex.unlock();
        }
        return result;
    }

private:
    std::mutex m_mutex;
    std::unique_ptr<std::queue<std::unique_ptr<T>>> m_que1; //正常队列
    std::condition_variable m_cond;

    friend class ThreadPrimary;
};


