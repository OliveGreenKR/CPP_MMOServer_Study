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


//소수 구하기 문제
extern atomic<int> cnt = 0;
extern atomic<int64> Ticks = 0;

void CntPrime(int start, int end)
{
	auto st = GetTickCount64();

	if (end < start)
		return;
	if (start < 2)
		start = 2;

	for (int n = start; n < end; n++)
	{
		for (int i = 2; i * i <= n; i++)
		{
			if (n % i == 0)
			{
				//lock_guard<mutex> _lock(m);
				cnt.fetch_sub(1);
				break;
			}
		}
		//lock_guard<mutex> _lock(m);
		cnt.fetch_add(1);
	}
	auto fin = GetTickCount64();
	Ticks.fetch_add(fin - st);
}

int main()
{
	int N = 100'00;
	
	vector<thread> threads;

	int coreCount = thread::hardware_concurrency();
	int jobCount = (N / coreCount) + 1; //코어만큼 N나누기

	for (int i = 0; i < coreCount; i++)
	{
		int start = (i * jobCount) + 1;
		int end = min(N, (i + 1) * jobCount);

		threads.push_back(thread(CntPrime, start, end));
	}

	for (thread& t : threads)
	{
		t.join();
	}

	cout << cnt << endl;
	cout << Ticks << "ms" << endl;
}

