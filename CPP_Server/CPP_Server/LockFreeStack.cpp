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

#ifdef SECOND abaproblem
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
#endif // SECOND abaproblem
