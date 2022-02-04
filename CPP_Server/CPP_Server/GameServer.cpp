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
		bool expected = false; // ����
		bool desired = true;   // ���ϴ� ��
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
			expected = false; //&�� �ޱ� ������ �ʱ�ȭ�� �Ź� ���־����.
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
