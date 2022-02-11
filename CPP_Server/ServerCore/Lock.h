#pragma once
#include "Types.h"

/*-------------------------
		RW Spin Lock
---------------------------*/

/**
 * @brief 
 * 
 * 32비트를 사용하는데
 * [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
 * 
 * 하위16비트, 상위16비트의 의미를 나누어 사용할 것임..!
 * 
 * W : Writing Flag (Exclusive Lock Owner ThreadId)
 * R : Reading Flag (Shared Lock Count)
*/

class Lock
{
public:
	enum : uint32
	{
		ACQUIRE_TIMEOUT_TICK = 1000,
		MAX_SPIN_COUNT = 5000,
		WRITE_THREAD_MASK = 0xFFFF'0000,
		READ_COUNT_MASK = 0x0000'FFFF,
		EMPTY_FLAG = 0x0000'0000
	};

	void WriteLock();
	void WriteUnlock();
	void ReadLock();
	void ReadUnlock();

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;

	//재귀적 동작을 허락하기 위해 WriteCount를 따로 관리한다.
	//이는 사용하는 스레드가 독립적으로 사용하기 때문에, atomic하지 
	//않아도 된다.
	uint16 _writeCount = 0;

	//재귀적 lock을 실행하면, _writeCount를 증가시키는 식으로
	//동작이 가능하기 때문
};

/*--------------------------
		LockGuards		
----------------------------*/

//RAII
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock) : _lock(lock) 
	{
		_lock.ReadLock();
	}
	~ReadLockGuard() { _lock.ReadUnlock(); }

private:
	Lock& _lock;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock) : _lock(lock)
	{
		_lock.WriteLock();
	}
	~WriteLockGuard() { _lock.WriteUnlock(); }

private:
	Lock& _lock;
};