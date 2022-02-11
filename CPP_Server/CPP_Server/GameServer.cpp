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
CoreGlobal Core;


void ThreadMain()
{
	while (true)
	{
		cout << "hello I am Thread ..." <<
			LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}
int main()
{
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadMain);
	}

	GThreadManager->Join();


}
