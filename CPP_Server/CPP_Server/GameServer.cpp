#include "pch.h"

#include <iostream>
using namespace std;

#include "CorePch.h"
#include <thread>
#include<atomic>
#include <mutex>
#include <future>

#include <Windows.h>


thread_local int32 LThreadid = 0;

void ThreadMain(int32 threadid)
{
	LThreadid = threadid;

	while (true)
	{
		cout << "Hi!, I am Thread" << LThreadid << endl;
		this_thread::sleep_for(1s);
	}

}

int main()
{
	vector<thread> threads;

	for (int i = 0; i < 10; i++)
	{
		threads.push_back(thread(ThreadMain, i));
	}

	for (thread& t : threads)
	{
		if(t.joinable())
		{
			t.join();
		}
	}

}
