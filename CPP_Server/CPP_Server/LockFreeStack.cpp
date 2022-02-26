#include "pch.h"
#include "LockFreeStack.h"

#ifdef FIRST one thread
/*___________________________
1�� �õ�
-----------------------------*/

//[Header]->[SLE | data]->[SLE | data]  ...
// header : ù��° �����͸� ����Ų��.

void InitializeHeader(SListHeader* header)
{
	header->next = nullptr;
}

//���ο� SLE����
void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next; //������ ���� entry�������ο���
	header->next = entry; //����� ����
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
2�� �õ�
-----------------------------*/
void InitializeHeader(SListHeader* header)
{
	header->next = nullptr;
}
void PushEntrySList(SListHeader* header, SListEntry* entry)
{
	entry->next = header->next;
	//dest(ptr) ,  desired(val), expected(val)
	//expectd�� ������ dest�� desired�� �ٲ۵�, dest�� �����ּҸ� ��ȯ
	//�ٸ��� ��ȯ���� �ʰ� dest�� ���� �ּҸ� ��ȯ
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
