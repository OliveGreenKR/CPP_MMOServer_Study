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

	//return이 없는 이유는...
	// task의 실행결과인 int64는 future를 통해 받아오게 되기 때문
}

int main()
{
	//std::future : 언젠가 '미래'에 값을 뱉어주겠다.
	{
		//async방식 - 멀티 쓰레드 환경이됨
		//deferred -  단일 쓰레드 환경임
		::future<int64> future = ::async(::launch::async, Calculate);
		//::future<int64> future = ::async(::launch::deferred, Calculate);

		//todo

		int64 sum = future.get(); // 이제 결과물이 필요하다.
	}

	//std::promise
	{
		//미래에 결과물을 반환할 것이라는 '약속'
		::promise<string> promise;
		
		future<string> future = promise.get_future();
		
		/*
		promise 는 다른 thread에,
		future는 우리가 가지고 있는 형태
		
		무전기처럼 promise와 연동..
		만약 다른 thread에서 promise의 데이터를 수정한다면
		future.get()으로 가져올 수 있음
		*/

		thread t(PromiseWorker, ::move(promise));
		t.join();

		string str = future.get();
		cout << str << endl;
	}
	
	//std::packaged_task
	{
		//'함수의 시그니처'를 맞추어 생성해야함!
		::packaged_task<int64(void)> task(Calculate);
	
		::future<int64> future = task.get_future();

		thread t1(TaskWorkder, ::move(task));
		t1.join();

		int64 sum= future.get();
		cout << sum << endl;

	}
}
