#ifndef _TEST_1_H_
#define _TEST_1_H_

#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <future>
#include <functional>

using namespace std;

//��������
int add(int i, int j) {
	//cout << "������������" << endl;
	this_thread::sleep_for(chrono::seconds(1));
	//cout << i + j << endl;
	return i + j;
}

//ȫ�־�̬����
static float minusBy5(float i) {
	cout << "����ȫ�־�̬����" << endl;
	return i - 5.0f;
}

class MyFunction {
public:
	//��Ǿ�̬��Ա����
	string pow2(string& str) const {
		cout << "������Ǿ�̬��Ա����" << endl;
		int result = 1;
		int pow = power_;
		while (pow--) {
			result *= atoi(str.c_str());
		}
		return "multiply result is : " + to_string(result);
	}
	//��̬��Ա����
	static int divide(int i, int j) {
		cout << "���Ǿ�̬��Ա����" << endl;
		if (0 == j) {
			return 0;
		}
		return i / j;
	}

private:
	int power_ = 2;
};

using Funcptr = string(*)(string, int);

class Base {
public:
	string operator()() {
		cout << "���Ƿº���" << endl;
		return string("����·�ɣ�����Ҫ��Ϊ������������");
	}

	operator Funcptr() {
		cout << "���ǽ���������ת���õ��ĺ���ָ��" << endl;
		return showMsg;
	}

	static string showMsg(string msg, int num) {
		string str = msg + to_string(num);
		return str;
	}
};


void test1(ThreadPool* tp) {
	int i = 6;
	int j = 3;
	string str = "5";
	MyFunction mf;

	string msg = "����·�ɣ��ҽ���";
	int num = 18;

	cout << "------------------" << endl;

	//��������
	auto ans1 = tp->commit([i, j] {return add(i, j); });
	//��������
	auto ans2 = tp->commit(add, i, j);
	//ȫ�־�̬����
	future<float> ans3 = tp->commit(bind(minusBy5, 8.5f));
	//�Ǿ�̬��Ա����
	auto ans4 = tp->commit(bind(&MyFunction::pow2, mf, std::ref(str)));
	//��̬��Ա����
	future<int> ans5 = tp->commit(/*bind(*/&MyFunction::divide, i, j/*)*/);
	//�º���
	Base b1;
	auto ans6 = tp->commit(b1);
	//����������ת���õ��ĺ���ָ��
	Base b2;
	auto ans7 = tp->commit(b2, msg, num);

	cout << ans1.get() << endl;
	cout << ans2.get() << endl;
	cout << ans3.get() << endl;
	cout << ans4.get() << endl;
	cout << ans5.get() << endl;
	cout << ans6.get() << endl;
	cout << ans7.get() << endl;

}


#endif // !_TEST_1_H_



