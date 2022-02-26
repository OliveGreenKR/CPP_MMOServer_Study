#include "pch.h"
#include "LockFreeStack.h"

#ifdef FIRST one thread
/*___________________________
1차 시도
-----------------------------*/

//[Header]->[SLE | data]->[SLE | data]  ...
// header : 첫번째 데이터를 가리킨다.

void InitializeHeader(SListHeader* header)
{
	header->next = nullptr;
}

//새로운 SLE삽입
void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next; //기존의 것을 entry다음으로연결
	header->next = entry; //헤더에 연결
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* first = header->next;

	if (first != nullptr)
		header->next = first->next;

	return first;
}
#endif // FIRST

#ifdef SECOND ABAproblem
/*___________________________
2차 시도
-----------------------------*/
void InitializeHeader(SListHeader* header)
{
	header->next = nullptr;
}
void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;
	//dest(ptr) ,  desired(val), expected(val)
	//expectd와 같으면 dest는 desired로 바꾼뒤, dest의 시작주소를 반환
	//다르면 변환하지 않고 dest의 시작 주소를 반환
	while (::InterlockedCompareExchange64((int64*)&header->next, (int64)entry, (int64)entry->next) != (int64)entry)
	{

	}
}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListEntry* expected = header->next;

	//dest ,  desired, expected
	while (expected && ::InterlockedCompareExchange64((int64*)&header->next, (int64)expected->next, (int64)expected) != (int64)expected->next)
	{
	}

	return expected;
}
#endif // SECOND 

/*___________________________
		3차 시도
-----------------------------*/
void InitializeHeader(SListHeader* header)
{
	header->alignment = 0;
	header->region = 0;
}
void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	SListHeader expected = {};
	SListHeader desired = {};

	//하위 4비트는 0이므로(16바이트정렬) 60비트만으로 주소표현가능
	desired.HeaderX64.next = (((uint64)entry) >> 4); 

	while (true)
	{
		expected = *header;

		//중간에 데이터가 변경될수 잇음
		//==> region(데이터주소)와 alignment(카운터)가 모두 같아야 SWAP

		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4); //entry->next = header->next
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;
		//dest, desired1,desired2, expeced
		if (InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)//변환성공
			break;
	}

}

SListEntry* PopEntrySList(SListHeader* header)
{
	SListHeader expected = {};
	SListHeader desired = {};
	SListEntry* entry = nullptr;

	while (true)
	{
		expected = *header;
		entry = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4);  //entry = header->next
		if (entry == nullptr)
			break;
		//Use-After_Free 가능
		desired.HeaderX64.next = (((uint64)entry->next) >> 4);
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}
