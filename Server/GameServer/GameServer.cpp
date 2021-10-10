#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <atomic>
#include <future>

#include <thread>
#include <string>
#include <windows.h>

#include "ConCurrentQueue.h"
#include "ConCurrentStack.h"

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

int sum = 0;

void Add()
{
	for (int32 i = 0; i < 100'00000; ++i)
		sum++;
}

void Sub()
{
	for (int32 i = 0; i < 100'00000; ++i)
		sum--;
}
//? 무슨 템플릿?
void HelloThread2(int32 _str) // 가변인자 이용해서 사용
{
	//Functor나 람다도 가능

	cout << _str << endl;
}

//TLS 사용 법
// c++ 이전 방식 __declspec(thread) int32 value;
thread_local int32 LThreadId;


void ThreadMain(int32 _threadID)
{
	LThreadId = _threadID;

	while (true)
	{
		cout << "Hi I'm Thread" << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}


LcokQueue<int32> q;
LockStack<int32> s;

void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		q.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void pop()
{
	while (true)
	{
		int32 data = 0;
	if (q.TryPop(OUT data))
		cout << data << endl;
		
	}
}

int main()
{


	//정적 Lib의 경우 디버깅도 가능

	// cout 의 경우 System Call(OS 커널에 요청)
	// Window / unix에 따라 다름
	// 한군데에 종속적이면 옮길때 힘듬
	// C++11 부터 스레드생성이 생기며 환경이 상관없어짐 
	// 한국은 windows Server이용 c++ iocp 서버 사용 / 해외는 Unix 거의 사용


	/*  TLS 스레드 인덱싱 테스트
	 vector<std::thread> Thread_Vec;


	for (int32 i = 0; i < 10; ++i)
	{
		int32 threadID = i + 1;
		Thread_Vec.push_back(thread(ThreadMain, threadID));
	}
	*/
	
	
	//Lock 이용한 스택 큐 사용
	thread t1(Push);
	thread t2(pop);
	thread t3(pop);

	t1.join();
	t2.join();
	t3.join();



	
}