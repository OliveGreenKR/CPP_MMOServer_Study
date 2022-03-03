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
	int _hp = rand() % 100;
};

int main()
{
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
