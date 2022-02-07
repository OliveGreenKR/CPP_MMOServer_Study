#include "pch.h"

#include <iostream>
using namespace std;

#include "CorePch.h"
#include <thread>
#include<atomic>
#include <mutex>
#include <future>

int64 result = 0;

int64 Calculate()
{
	int64 sum = 0;
	for (int32 i = 0; i < 100'000; i++)
		sum += i;

	return result = sum;
}

void PromiseWorker(::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorkder(::packaged_task<int64(void)>&& task)
{
	task();

	//return�� ���� ������...
	// task�� �������� int64�� future�� ���� �޾ƿ��� �Ǳ� ����
}

int main()
{
	//std::future : ������ '�̷�'�� ���� ����ְڴ�.
	{
		//async��� - ��Ƽ ������ ȯ���̵�
		//deferred -  ���� ������ ȯ����
		::future<int64> future = ::async(::launch::async, Calculate);
		//::future<int64> future = ::async(::launch::deferred, Calculate);

		//todo

		int64 sum = future.get(); // ���� ������� �ʿ��ϴ�.
	}

	//std::promise
	{
		//�̷��� ������� ��ȯ�� ���̶�� '���'
		::promise<string> promise;
		
		future<string> future = promise.get_future();
		
		/*
		promise �� �ٸ� thread��,
		future�� �츮�� ������ �ִ� ����
		
		������ó�� promise�� ����..
		���� �ٸ� thread���� promise�� �����͸� �����Ѵٸ�
		future.get()���� ������ �� ����
		*/

		thread t(PromiseWorker, ::move(promise));
		t.join();

		string str = future.get();
		cout << str << endl;
	}
	
	//std::packaged_task
	{
		//'�Լ��� �ñ״�ó'�� ���߾� �����ؾ���!
		::packaged_task<int64(void)> task(Calculate);
	
		::future<int64> future = task.get_future();

		thread t1(TaskWorkder, ::move(task));
		t1.join();

		int64 sum= future.get();
		cout << sum << endl;

	}
}
