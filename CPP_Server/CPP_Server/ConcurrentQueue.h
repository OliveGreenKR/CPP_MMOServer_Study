#pragma once

#include <mutex>

template<typename T>
class LockQueue
{
public:
	LockQueue()
	{}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue& other) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
			return false;
		//일반적인 싱글스레드에서 처럼 
		//empty를 확인하고 top, pop진행
		value = ::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this]() { return _queue.empty() == false; });

		value = ::move(_queue.front());
		_queue.pop();
	}

	bool Empty()
	{
		lock_guard<mutex> lock(_mutex);
		return _queue.empty();

	}
private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};

