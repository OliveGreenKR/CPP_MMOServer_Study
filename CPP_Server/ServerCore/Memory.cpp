#include "pch.h"
#include "Memory.h"
#include "MemoryPool.h"


/*---------------------------
	Memory Manager
---------------------------*/

MemoryManager::MemoryManager()
{
	int32 size = 0;
	int32 tableindex = 0;

	for (size = 32; size <= 1024; size += 32)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableindex <= size)
		{
			_poolTable[tableindex] = pool;
			tableindex++; //0~32 -> 1 , ~64 -> 2 ...
		}
	}
	for (; size <= 2048; size += 128 )
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableindex <= size)
		{
			_poolTable[tableindex] = pool;
			tableindex++; 
		}
	}
	for (; size <= 4096; size += 256)
	{
		MemoryPool* pool = new MemoryPool(size);
		_pools.push_back(pool);

		while (tableindex <= size)
		{
			_poolTable[tableindex] = pool;
			tableindex++; 
		}
	}
}

MemoryManager::~MemoryManager()
{
	for (MemoryPool* pool : _pools)
		delete pool;

	_pools.clear();
}

void* MemoryManager::Allocate(int32 size)
{
	MemoryHeader* header = nullptr;
	const int32 allocsize = size + sizeof(MemoryHeader);

	if (allocsize > MAX_ALLOC_SIZE)//�ʹ� ũ�� -> �Ϲ��Ҵ�
	{
		header = reinterpret_cast<MemoryHeader*>(::_aligned_malloc(allocsize,SLIST_ALIGNMENT));
	}
	else
	{
		///�޸� Ǯ���� ��������.
		header = _poolTable[allocsize]->Pop();
	}
	return MemoryHeader::AttachHeader(header, allocsize); //�����ͽ����ּ� ��ȯ
}
void MemoryManager::Release(void* ptr)
{
	MemoryHeader* header = MemoryHeader::DetachHeader(ptr); //��� �����ּ� ��ȯ
	const int32 allocsize = header->allocSize;
	
	ASSERT_CRASH(allocsize >0);

	if (allocsize > MAX_ALLOC_SIZE)
	{
		::_aligned_free(header);
	}
	else
	{
		_poolTable[allocsize]->Push(header);
	}

}

