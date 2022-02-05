#include "pch.h"
#include <iostream>
using namespace std;
#include "CorePch.h"

#include <thread>
#include<atomic>
#include <mutex>

enum
{
	COUNT = 10'0000
};

class SpinLock
{
public:
	void lock()
	{
		//CAS(Compare And-Swap)
		bool expected = false; // 예상값
		bool desired = true;   // 원하는 값
		/* CAS puseudo code
		if(_locked == expected)
		{
			expected = _locked;
			_locked = desired;
			return true;
		}
		else
		{
			expected = _locked;
			return false;
		}
		****/

		while (_locked.compare_exchange_strong(expected, desired) == false)
		{
			expected = false; //&를 받기 때문에 초기화를 매번 해주어야함.

			/*auto waiting_time = ::chrono::milliseconds(100);
			this_thread::sleep_for(waiting_time);*/

			this_thread::sleep_for(100ms); //일정기간 정지
			
			this_thread::yield(); //양보
			/*
			* sleep_for :  몇 초동안 스케쥴링에서 빠지게 된다.
			* yield : 언제든지 다시 스케쥴링이 될 수 있음. 죽, 현재 이 순간만 빠지게됨.
			*/
			
		}
	}

	void unlock()
	{
		//_locked = false;
		_locked.store(false);
	}

private:
	atomic<bool> _locked = false;
};


int32 sum = 0;
mutex m;
SpinLock spinLock;

void Add()
{
	for (int32 i = 0; i < COUNT; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < COUNT; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}


int main()
{
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();


	cout << sum << endl;

}
