#include "pch.h"

#include <iostream>
using namespace std;

#include "CorePch.h"
#include <thread>
#include<atomic>
#include <mutex>
#include <future>

#include <Windows.h>

atomic<bool> ready;
int32 value;

void Producer()
{
	value = 10;

	ready.store(true);

	::atomic_thread_fence(memory_order::memory_order_release);
	//-----------------release로 인한 절취선------------
}

void Comsumer()
{
	//---------------acquire 절취선 -------------
	::atomic_thread_fence(memory_order::memory_order_acquire);
	while (ready.load() == false)
		;
	cout << value << endl;
}


int main()
{
	ready = false;
	value = 0;
	thread t1(Producer);
	thread t2(Comsumer);

	t1.join();
	t2.join();

}
