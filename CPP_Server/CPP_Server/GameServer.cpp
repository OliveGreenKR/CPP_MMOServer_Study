#include "pch.h"

#include <iostream>

#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <Windows.h>
#include "ThreadManager.h"

#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

class Knight
{
public:
	Knight() {};
	Knight(int hp) : _hp(hp) {};
public:
	int _hp = rand() % 100;
};

class Monster
{
public:
	int64 _id = 0;
};

int main()
{
	Knight* knights[100];
	for (int32 i = 0; i < 100; i++)
		knights[i] = ObjectPool<Knight>::Pop();
	for (int32 i = 0; i < 100; i++)
	{
		ObjectPool<Knight>::Push(knights[i]);
		knights[i] = nullptr;
	}

	//멤버 함수 사용
	shared_ptr<Knight>sptr = ObjectPool<Knight>::MakeShared();

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Knight* knight = xnew<Knight>();

					cout << knight->_hp << endl;

					this_thread::sleep_for(10ns);

					xdelete(knight);
				}
			});
	}

	GThreadManager->Join();
}
