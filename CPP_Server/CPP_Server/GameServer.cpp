// CPP_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>
using namespace std;
#include "CorePch.h"

#include <thread>

void HelloThread()
{
	cout << "Hello Thread!" << endl;
}

void HelloThread_2(int32 num)
{
	cout << num << endl;
}
int main()
{
	vector<thread> v;

	for (int32 i = 0; i < 10; i++)
	{
		v.push_back(::thread(HelloThread_2, i));
	}

	for (int32 i = 0; i < 10; i++)
	{
		if (v[i].joinable())
			v[i].join();
	}

	cout << "Hello Main!" << endl;

}
