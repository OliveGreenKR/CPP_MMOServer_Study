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
	ptr->allocSize = 0; //사용중이 아니라는 표시로 활용
	
	_queue.push(ptr);//Pool에 메모리 반납

	_allocCount.fetch_sub(1);
}

MemoryHeader* MemoryPool::Pop()
{
	MemoryHeader* header = nullptr;
	{
		WRITE_LOCK;
		//Pool의 데이터 체크
		if (_queue.empty() == false)
		{
			//있으면 꺼내오기
			header = _queue.front();
			_queue.pop();
		}
	}
	
	//만약 없으면? 새로할당(정책에 따라 선택하자)
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
