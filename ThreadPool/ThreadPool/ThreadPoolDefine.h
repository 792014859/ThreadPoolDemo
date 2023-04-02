#pragma once
#include <thread>
#include <mutex>
#include <memory>

using UNIQUE_LOCK = std::unique_lock<std::mutex>;
using GUARD_LOCK = std::lock_guard<std::mutex>;

static const int CPU_NUM = static_cast<int>(std::thread::hardware_concurrency());							//CPU����

enum class ThreadType : char { primary, second };
//static const int THREAD_TYPE_PRIMARY = 1;																	//���̱߳�־
//static const int THREAD_TYPE_SECOND = 2;																	//���̱߳�־

static const int PRIMARY_THREAD_NUMBERS = (CPU_NUM > 8) ? CPU_NUM : 8;										//Ĭ�����̸߳���
static const int MAX_THREAD_SIZE = (PRIMARY_THREAD_NUMBERS * 2);									//�����̸߳���
static const int MAX_TASK_STEAL_RANGE = 16;																	//��ȡ��������ԭ��
static const int SECONDARY_THREAD_TTL = 3;															// �����߳�ttl����λΪs
static const int MONITOR_SPAN = 1;																	// ����߳�ִ�м������λΪs
static const int BATCHSIZE = 3;																	//����ִ�������������
static const int BATCHTASKENABLE = false;														//�Ƿ�����������

static const unsigned int MAX_BLOCK_TTL = 10000000;                                         // �������ʱ�䣬��λΪms
