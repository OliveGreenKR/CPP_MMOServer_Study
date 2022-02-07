#include "pch.h"

#include <iostream>
using namespace std;

#include "CorePch.h"
#include <thread>
#include<atomic>
#include <mutex>
#include <future>

#include <Windows.h>

int32 buffer[10000][10000];

int main()
{
	::memset(buffer, 0, sizeof(buffer));

	{

		uint64 start = GetTickCount64();
		int64 sum = 0;

		for (int32 i = 0; i < 10000; i++)
		{
			for (int32 j = 0; j < 10000; j++)
			{
				sum += buffer[i][j];
			}
		}

		uint64 end = GetTickCount64();

		cout << "Elapsed Tick : " << end - start << endl;
	}

	{
		uint64 start = GetTickCount64();
		int64 sum = 0;

		for (int32 i = 0; i < 10000; i++)
		{
			for (int32 j = 0; j < 10000; j++)
			{
				sum += buffer[j][i];
			}
		}

		uint64 end = GetTickCount64();

		cout << "Elapsed Tick : " << end - start << endl;
	}

	//2번째 방법이 3배정도 느리다..
	/*
	* 바로 그것이 cache의 동작 때문이다..
	*
	* *캐시가 데이터를 가져올 때,  캐시 철학에 따라,  주변의 데이터들을 블록 단위로 가져오게 된다.
	*
	* --> 그 후에 근처의 값을 사용할 때, 만약 이미 캐시에 있다면--> cache hit 이 발생하여 속도가 매우 빠르다..
	*
	*
	*
	*	>> 두 번 째 방법은 처음 buffer[0][0]을 가져올 때, 주변의 것들 [0][1] / [0][2] .. 들을 가져온다.
	*	   따라서 cache hit이 자주 발생하지 않고, 렘까지 가야하기 때문에 발동 시간이 오래 걸리는 것
	*
	*/
}
