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

#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

LockQueue<int32> q;
LockFreeStack<int32> s;

#define SHARED

#ifdef POPCOUNT
void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);
	}
}
void Pop()
{
	while (true)
	{
		int32 data = 0;
		s.TryPop(data);
		cout << data << endl;
	}
}
#endif 

#ifdef SHARED
void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		s.Push(value);
	}
}
void Pop()
{
	while (true)
	{
		auto data = s.TryPop();
		if (data != nullptr)
			cout << (*data) << endl;
	}
}
#endif // SHARED



int main()
{
	thread t1(Push);
	thread t2(Pop);
	
	t1.join();
	t2.join();
	

}
