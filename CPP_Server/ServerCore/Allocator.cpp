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
	const int64 pageCount = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	const int64 dataOffset = pageCount * PAGE_SIZE - size;

	//예약과 동시에 사용할 것, r/w 가능
	void* baseAddress = ::VirtualAlloc(NULL, pageCount * PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	// 주소 + 연산을 위해서는 포인터 타입이 정해져야한다.
	//1바이트 단위로 주소 연산을 하기위해 int8* 로 캐스팅 한 후 offset을 더한다.

	return static_cast<void*>(static_cast<int8*>(baseAddress) + dataOffset);

}

void StompAllocator::Release(void* ptr)
{
	const int64 address = reinterpret_cast<int64>(ptr);
	//data는 Page_size 단위이기 때문에, offset은 항상 page_size보다 작다.
	//즉, "address % page_size"를 통해 offset을 역산할 수 있게되고, 이를 빼면 본래의 시작위치주소를 알 수 있다.

	const int64 baseAddress = address - (address % PAGE_SIZE);
	::VirtualFree(reinterpret_cast<void*>(baseAddress), 0, MEM_RELEASE);
}
