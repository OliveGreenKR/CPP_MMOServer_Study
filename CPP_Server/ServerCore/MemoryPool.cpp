#include "pch.h"
#include "MemoryPool.h"

/*---------------------
	Memory Pool          
----------------------*/

MemoryPool::MemoryPool(int32 allocsize) : _allocSize(allocsize)
{
}

MemoryPool::~MemoryPool()
{
	while (_queue.empty() == false)
	{
		MemoryHeader* header = _queue.front();
		_queue.pop();
		::free(header);
	}
}

void MemoryPool::Push(MemoryHeader* ptr)
{
	WRITE_LOCK;
	ptr->allocSize = 0; //������� �ƴ϶�� ǥ�÷� Ȱ��
	
	_queue.push(ptr);//Pool�� �޸� �ݳ�

	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;
	{
		WRITE_LOCK;
		//Pool�� ������ üũ
		if (_queue.empty() == false)
		{
			//������ ��������
			header = _queue.front();
			_queue.pop();
		}
	}
	
	//���� ������? �����Ҵ�(��å�� ���� ��������)
	if (header == nullptr)
	{
		header = reinterpret_cast<MemoryHeader*>(::malloc(_allocSize));
	}
	else
	{
		ASSERT_CRASH(header->allocSize == 0);
	}

	_allocCount.fetch_add(1);

	return header;
}
