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

	Player() {};
	virtual ~Player() {};
private:
	int64 _id = 0;
};

class Knight : Player
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

class Archer : Player
{
public:
	int64 _hp = 0;
};

#include "TypeCast.h"

int main()
{

	using TL = TypeList<Monster, Knight, Archer>;
	int32 len1 = Length<TL>::value; //3
	TypeAt<TL, 2>::Result player; //archer선택됨

	int32 knihgtidx = IndexOf<TL, Knight>::value; //1
	int32 grbgidx = IndexOf<TL, Player>::value; //-1

	Conversion<Knight, Player>::exists; // true -> 변환가능
	Conversion<Player, Knight>::exists; // false -> 변환불가


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
