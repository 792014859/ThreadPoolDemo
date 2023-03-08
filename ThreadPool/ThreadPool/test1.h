#ifndef _TEST_1_H_
#define _TEST_1_H_

#include "ThreadPool.h"
#include <iostream>
#include <string>
#include <future>
#include <functional>

using namespace std;

//有名函数
int add(int i, int j) {
	//cout << "我是有名函数" << endl;
	this_thread::sleep_for(chrono::seconds(1));
	//cout << i + j << endl;
	return i + j;
}

//全局静态函数
static float minusBy5(float i) {
	cout << "我是全局静态函数" << endl;
	return i - 5.0f;
}

class MyFunction {
public:
	//类非静态成员函数
	string pow2(string& str) const {
		cout << "我是类非静态成员函数" << endl;
		int result = 1;
		int pow = power_;
		while (pow--) {
			result *= atoi(str.c_str());
		}
		return "multiply result is : " + to_string(result);
	}
	//静态成员函数
	static int divide(int i, int j) {
		cout << "我是静态成员函数" << endl;
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
		cout << "我是仿函数" << endl;
		return string("我是路飞，我是要成为海贼王的男人");
	}

	operator Funcptr() {
		cout << "我是将类对象进行转换得到的函数指针" << endl;
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

	string msg = "我是路飞，我今年";
	int num = 18;

	cout << "------------------" << endl;

	//匿名函数
	auto ans1 = tp->commit([i, j] {return add(i, j); });
	//有名函数
	auto ans2 = tp->commit(add, i, j);
	//全局静态函数
	future<float> ans3 = tp->commit(bind(minusBy5, 8.5f));
	//非静态成员函数
	auto ans4 = tp->commit(bind(&MyFunction::pow2, mf, std::ref(str)));
	//静态成员函数
	future<int> ans5 = tp->commit(/*bind(*/&MyFunction::divide, i, j/*)*/);
	//仿函数
	Base b1;
	auto ans6 = tp->commit(b1);
	//将类对象进行转换得到的函数指针
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



