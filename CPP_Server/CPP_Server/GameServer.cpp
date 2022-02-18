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

using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight 
{
public:
	Knight() 
	{
		cout << "Knight()\n";
	}
	~Knight() { cout << "~Knight()\n"; }

	KnightRef _target = nullptr;
	InventoryRef _invnetory = nullptr;
};

class Inventory
{
public:
	Inventory(KnightRef knight) : _master(**knight), __master(make_shared<Knight>(knight))
	{}

public:
	Knight& _master;
	weak_ptr<Knight> __master;
};

int main()
{
	
	//[Knkght] [RefCOunt] ==> 두번에 나누어 할당
	shared_ptr<Knight> spr(new Knight());

	// [knight | RefCOunt ]  ==> 할당 1번 
	shared_ptr<Knight> spr = make_shared<Knight>();
	weak_ptr<Knight> wpr = spr;

	//weak_Ptr은 사용하기 전에 만료여부를 확인해야함
	bool expired = wpr.expired();
	//귀찮다면 다시 shared_ptr로 캐스팅 할 수 있음.
	shared_ptr<Knight> spr2 = wpr.lock(); //만약 expired된 상태면  nullptr



}

