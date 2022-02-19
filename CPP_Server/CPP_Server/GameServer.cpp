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

class Knight 
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

	//static void* operator new(size_t size)
	//{
	//	cout << "new!" << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//static void operator delete(void* ptr)
	//{
	//	cout << "delete!" << endl;
	//	::free(ptr);
	//}

public:
	int _hp = 100;
	int _mp = 10;
};


void* operator new[](size_t size)
{
	cout << "new[]!" << size << endl;
	void* ptr = ::malloc(size);
	return ptr;
}

void operator delete[](void* ptr)
{
	cout << "delete!" << endl;
	::free(ptr);
}


int main()
{
	Knight* k1 = xnew<Knight>(10);

	xdelete(k1);
}

