#include "pch.h"

#include <iostream>
using namespace std;

#include "CorePch.h"
#include <thread>
#include<atomic>
#include <mutex>
#include <future>
#include <queue>
#include <stack>
#include <Windows.h>
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"

class Player
{
public:
	Player() { cout << "Player()\n"; }
	~Player() { cout << "~Player()\n"; }
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
	Knight* k1 = (Knight*)xnew<Player>();

	//k1->_hp = 100;

	xdelete(k1);
}

