#pragma once
#include "Types.h"

/*-------------------------
		RW Spin Lock
---------------------------*/

/**
 * @brief 
 * 
 * 32��Ʈ�� ����ϴµ�
 * [WWWWWWWW][WWWWWWWW][RRRRRRRR][RRRRRRRR]
 * 
 * ����16��Ʈ, ����16��Ʈ�� �ǹ̸� ������ ����� ����..!
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

	void WriteLock(const char* name);
	void WriteUnlock(const char* name);
	void ReadLock(const char* name);
	void ReadUnlock(const char* name);

private:
	Atomic<uint32> _lockFlag = EMPTY_FLAG;

	//����� ������ ����ϱ� ���� WriteCount�� ���� �����Ѵ�.
	//�̴� ����ϴ� �����尡 ���������� ����ϱ� ������, atomic���� 
	//�ʾƵ� �ȴ�.
	uint16 _writeCount = 0;

	//����� lock�� �����ϸ�, _writeCount�� ������Ű�� ������
	//������ �����ϱ� ����
};

/*--------------------------
		LockGuards		
----------------------------*/

//RAII
class ReadLockGuard
{
public:
	ReadLockGuard(Lock& lock, const char* name) : 
		_lock(lock), _name(name)
	{
		_lock.ReadLock(name);
	}
	~ReadLockGuard() { _lock.ReadUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};

class WriteLockGuard
{
public:
	WriteLockGuard(Lock& lock, const char* name) : 
		_lock(lock), _name(name)
	{
		_lock.WriteLock(name);
	}
	~WriteLockGuard() { _lock.WriteUnlock(_name); }

private:
	Lock& _lock;
	const char* _name;
};