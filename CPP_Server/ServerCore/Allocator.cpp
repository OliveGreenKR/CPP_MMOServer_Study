#include "pch.h"
#include "Allocator.h"

/*------------------------------
		BaseAllocator
--------------------------------*/

void* BaseAllocator::Alloc(int32 size)
{
	return ::malloc(size);
}

void BaseAllocator::Release(void* ptr)
{
	::free(ptr);
}
/*------------------------------
		Stomp Allocator
--------------------------------*/
void* StompAllocator::Alloc(int32 size)
{
	const int pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	//예약과 동시에 사용할 것, r/w 가능
	return ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

void StompAllocator::Release(void* ptr)
{
	::VirtualFree(ptr, 0, MEM_RELEASE);
}
