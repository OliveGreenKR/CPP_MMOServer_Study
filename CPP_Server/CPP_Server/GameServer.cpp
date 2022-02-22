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

class Player
{
public:
	Player() { cout << "Player()\n"; }
	virtual ~Player() { cout << "~Player()\n"; }
};

class Knight : public Player
{
public:
	Knight() 
	{
		cout << "Knight()\n";
	}

	Knight(int hp) : _hp(hp)
	{
		cout << "Knight(hp)\n";
	}

	~Knight() { cout << "~Knight()\n"; }

public:
	int _hp = 100;
	int _mp = 10;
};

int main()
{
	for (int i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
					Vector<Knight> v(10);
					Map<int32, Knight> m;
					m[100] = Knight();

					this_thread::sleep_for(10ms);
				}
			});
	}

	GThreadManager->Join();
	return 0;
}

