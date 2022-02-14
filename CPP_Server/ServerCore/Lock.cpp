#include "pch.h"
#include "Lock.h"
#include "CoreTLS.h"
#include "DeadLockProfiler.h"

void Lock::WriteLock(const char* name)
{	
#if _DEBUG
	GDeadLockProfiler->PushLock(name);
#endif
	// ������ �����尡 �����ϰ� �ִٸ� ������ ����(����� ����㰡)
	const uint32 lockThreadId = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;
	if (LThreadId == lockThreadId)
	{
		_writeCount++;
		return;
	}

	//�ƹ��� ����(Write) �� ����(Read)�ϰ� ���� ���� ��, ���տ��� �������� ��´�.
	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{	
			//Read�κ��� 0�̾ƴ϶�� EXPECTED���� �ɼ� �����Ƿ�,READ,WRITE �˻簡��
			uint32 expected = EMPTY_FLAG;
			if (_lockFlag.compare_exchange_strong(OUT expected, desired))
			{
				_writeCount++;
				return;
			}
		}
		//������ �ð��� ������ ������ crash
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
	//ReadLock�� �� Ǯ�� �������� WriteUnlock�� �� ���� ����.
	if ((_lockFlag.load() & READ_COUNT_MASK) !=0)
		CRASH("INVALID_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)//���� ���� ���̻� �������� ���� ����
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

	//������ �����尡 ����(Write)�ϰ� �ִ�
	if (LThreadId == lockThreadId)
	{
		_lockFlag.fetch_add(1); //ReadCount+1 (���� �������̹Ƿ� ���վ���)
		return;
	}

	//�ƹ��� �������� �ʰ� �������� ����.->���� ��  ReadCount �÷��� ���̴�
	const int64 beginTick = ::GetTickCount64();
	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			//Write�� 0 �� �ƴ϶�� expected�� �� �� ����.
			uint32 expected = (_lockFlag.load() & READ_COUNT_MASK);
			if (_lockFlag.compare_exchange_strong(OUT expected, expected + 1))
				return;
		}

		//������ �ð��� ������ ������ crash
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
	if((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0) //�̹� 0�̾���?
		CRASH("MULTIPLE_UNLOCK")
}
