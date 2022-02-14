#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{	
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// 동일한 쓰레드가 소유하고 있다면 무조건 성공(재귀적 사용허가)
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	//아무도 소유(Write) 및 공유(Read)하고 있지 않을 때, 경합에서 소유권을 얻는다.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{	
			//Read부분이 0이아니라면 EXPECTED값이 될수 없으므로,READ,WRITE 검사가능
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}
		//과도한 시간이 지나면 인위적 crash
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	//ReadLock이 다 풀기 전까지는 WriteUnlock은 할 수가 없다.
	if ((_lockFlag.load() & READ_COUNT_MASK) !=0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)//이제 내가 더이상 소유하지 않을 것임
	{
		_lockFlag.store(EMPTY_FLAG);
	}
}

void Lock::ReadLock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;

	//동일한 쓰레드가 소유(Write)하고 있다
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1); //ReadCount+1 (내가 소유중이므로 경합없음)
		return;
	}

	//아무도 소유하지 않고 있을때는 가능.->경합 후  ReadCount 올려줄 것이다
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//Write가 0 이 아니라면 expected가 될 수 없다.
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		//과도한 시간이 지나면 인위적 crash
		if (::GetTickCount64() - beginTick >= ACQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}

}

void Lock::ReadUnlock(const char* name)
{
#if _DEBUG
	GDeadLockProfiler->PopLock(name);
#endif
	//ReadCount --;
	if((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) //이미 0이었다?
		CRASH("MULTIPLE_UNLOCK")
}
