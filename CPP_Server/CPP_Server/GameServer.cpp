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

using TL = TypeList<class Player, class Knight, class Archer>;

class Player
{
public:
	DECLARE_TL

public:
	Player()
	{
		INIT_TL(Player)
	}
	virtual ~Player() {};

	
};

class Knight : public Player
{
public:
	Knight() { INIT_TL(Knight) };
public:
	int _hp = rand() % 100;
};

class Archer :public Player
{
public:
	Archer() { INIT_TL(Archer) }
	int64 _hp = 0;
};

class Mage :public Player
{
public:
	Mage() { INIT_TL(Mage) }
	int64 _mp = 0;
};


class Dog
{
public:
	int64 _id = 0;
};


int main()
{
	/*Player* player = new Knight();

	bool canCast = CanCast<Knight*>(player);
	Knight* knight = TypeCast<Knight*>(player);

	delete player;*/

	shared_ptr<Knight> knight = MakeShared<Knight>();
	shared_ptr<Player> player = TypeCast<Player>(knight);
	bool cancast = CanCast<Player>(knight);


	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([]()
			{
				while (true)
				{
				}
			});
	}

	GThreadManager->Join();
}
