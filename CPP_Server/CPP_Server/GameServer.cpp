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

void Producer()
{
	while (true)
	{
		{
			//lcok_guard와는 다르게 lock의 생성시기를 조절가능
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		 
		//handle를 signal 상태로 바꿔준다
		::SetEvent(handle);

		this_thread::sleep_for(100ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE); //무한대기
		//>>handle이  signal이 될때까지 무한 대기
		//Auto Reset이기 때문에  signal이 된것을 확인한 순간, 아래의 코드로 진행이 되게 되면서
		// 자동으로 handle의 상태는 다시 non-signal로 닫힌다.

		/*::ResetEvent(handle) << 수동으로 설정 시, 이렇게 직접 닫아주어야한다.*/

		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << endl;
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
