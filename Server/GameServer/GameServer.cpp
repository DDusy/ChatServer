#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <string>

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

int main()
{
	
	vector<std::thread> Thread_Vec;

	//정적 Lib의 경우 디버깅도 가능

	// cout 의 경우 System Call(OS 커널에 요청)
	// Window / unix에 따라 다름
	// 한군데에 종속적이면 옮길때 힘듬
	// C++11 부터 스레드생성이 생기며 환경이 상관없어짐 
	// 한국은 windows Server이용 c++ iocp 서버 사용 / 해외는 Unix 거의 사용

	HelloWorld();

	//std::thread T(HelloThread2,"안녕하세요오오");  // 동시에 실행

	//동기화를 안할경우 순서는 뒤죽박죽
	for (int i = 0; i < 10; ++i)
	{
		Thread_Vec.push_back(std::thread(HelloThread2, i));
	}

	
	cout << "Hello Main" << endl;

	//int32 iCnt = T.hardware_concurrency(); //Cpu 코어개수 정확하지 않음 
	//auto ID = T.get_id();  // 스레드마다의 ID 어떤 스레드인지 구분할 때 사용
	
	//T.detach();		// 실제 구동 스레드와 실제 스레드와 연결 고리 끊기
					// Std::thread 객체에서 실제 쓰레드를 분리
					// 변수를 이용해 상태나 정보추출이 불가능

	
	for (auto& iter : Thread_Vec)
	{
		if (iter.joinable()) // 연동된 쓰레드가 있는지 판별
			iter.join(); // 스레드처리가 끝날때까지 기다리기

	}

	cout << "순서는 뭐에요" << endl;
	

	//Atomic 다 실행이 되거나 하나도 실행되지 않거나 All - or - Nothing
	//공유 데이터 관련한 문제
	//순서가 뒤죽 박죽스

	//DB 
	// A , B 가 교환할때 따로 따로 진행될경우
	// 서버가 터질 경우 정상적인 교환이 일어나지 않음
	//

	std::thread t1(Add);
	std::thread t2(Sub);

	t1.join();
	t2.join();


	cout << sum << endl;
}