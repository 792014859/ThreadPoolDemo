#include "ThreadPool.h"
#include "test1.h"
#include <iostream>
#include <time.h>
#include <string>

using namespace std;

int main() {

    
    
    for (int i = 0; i < 1; ++i) {
        clock_t startThread, endThread;
        ThreadPoolConfig config{};
        config.batch_task_enable = false;
        config.batch_size = 5;
        //config.max_thread_size = config.default_primary_thread_numbers;
        auto tp = std::make_unique<ThreadPool>(true, config);    // 构造一个线程池类的智能指针
        startThread = clock();
        test2(tp.get());
        endThread = clock();
        cout << "Thread time = " << double(endThread - startThread) / CLOCKS_PER_SEC << "s" << endl;
       /* clock_t start, end;
        start = clock();
        test3();
        end = clock();
        cout << "time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl << endl;*/
        //test1(tp.get());
    }
    
    return 0;
}