#pragma once
#include "Allocator.h"

/*---------------------------
	Memory Manager
---------------------------*/
class MemoryPool;

class MemoryManager
{
	enum//~1024 : 32 | ~2048 : 128 | ~4096 :  256
	{
		POOL_COUNT = (1024 / 32) + (1024 / 128) + (2048 / 256),
		MAX_ALLOC_SIZE = 4096
	};
public:
	MemoryManager();
	~MemoryManager();

	void*		Allocate(int32 size);
	void		Release(void* ptr);

private:
	vector <MemoryPool*> _pools;

	// 메모리 크기 <-> 메모리 pool*
	// 빠른 검색을 위한 helperTable생성(인덱스 검색 -> 상수복잡도)
	MemoryPool* _poolTable[MAX_ALLOC_SIZE + 1];
};

/*--------------
	XAllocator
-----------------*/
template<typename Type, typename ...Args>
Type* xnew(Args&&... args) //현재 poolAllocator사용
{
	Type* memory = static_cast<Type*>(PoolAllocator::Alloc(sizeof(Type)));
	//placement new
	new(memory) Type(::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	PoolAllocator::Release(obj);
}

template<typename Type>
shared_ptr<Type> MakeShared()
{
	return shared_ptr<Type>{xnew<Type>(), xdelete<Type>};
}