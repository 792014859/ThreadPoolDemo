#include "ThreadPool.h"
#include "test1.h"
#include <iostream>
#include <time.h>

using namespace std;

int main() {

    

    auto tp = std::make_unique<ThreadPool>();    // ����һ���̳߳��������ָ��
    clock_t start, end;
    start = clock();
    test1(tp.get());
    end = clock();
    cout << "time = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    return 0;
}