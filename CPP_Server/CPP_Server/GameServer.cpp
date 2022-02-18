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
	
	//[Knkght] [RefCOunt] ==> �ι��� ������ �Ҵ�
	shared_ptr<Knight> spr(new Knight());

	// [knight | RefCOunt ]  ==> �Ҵ� 1�� 
	shared_ptr<Knight> spr = make_shared<Knight>();
	weak_ptr<Knight> wpr = spr;

	//weak_Ptr�� ����ϱ� ���� ���Ῡ�θ� Ȯ���ؾ���
	bool expired = wpr.expired();
	//�����ٸ� �ٽ� shared_ptr�� ĳ���� �� �� ����.
	shared_ptr<Knight> spr2 = wpr.lock(); //���� expired�� ���¸�  nullptr



}

