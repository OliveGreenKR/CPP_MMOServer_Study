#include "pch.h"
#include <iostream>
using namespace std;
#include "CorePch.h"

#include <thread>
#include<atomic>
#include <mutex>
#include <Windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

//참고) CV는 User_Level Object이다.
condition_variable cv;

void Producer()
{
	while (true)
	{
		// 1) lock을 잡고
		// 2) 공유 변수 값을 수정
		// 3) Lock 해제
		// 4) 조건변수 통해 다른 쓰레드에 통지
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		 
		////handle를 signal 상태로 바꿔준다
		//::SetEvent(handle);
		cv.notify_one();

		/*this_thread::sleep_for(100ms);*/
	}
}

void Consumer()
{
	while (true)
	{
		//::WaitForSingleObject(handle, INFINITE); //무한대기
		
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; });

		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << q.size() << endl;
		}
	}
}

int main()
{
	//HANDLE 구조는 정수형 데이터이지만, Event의 ID와 같은 역활을 한다.
	handle = ::CreateEvent(NULL/*보안속성*/,
							FALSE /*수동 리셋여부 (false => Auto)*/,
							FALSE/*초기화상태*/,
							NULL/*이름*/);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
