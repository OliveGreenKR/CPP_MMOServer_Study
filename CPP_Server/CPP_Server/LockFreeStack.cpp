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

#ifdef SECOND ABAproblem
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
#endif // SECOND 

/*___________________________
		3�� �õ�
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

	//���� 4��Ʈ�� 0�̹Ƿ�(16����Ʈ����) 60��Ʈ������ �ּ�ǥ������
	desired.HeaderX64.next = (((uint64)entry) >> 4); 

	while (true)
	{
		expected = *header;

		//�߰��� �����Ͱ� ����ɼ� ����
		//==> region(�������ּ�)�� alignment(ī����)�� ��� ���ƾ� SWAP

		entry->next = (SListEntry*)(((uint64)expected.HeaderX64.next) << 4); //entry->next = header->next
		desired.HeaderX64.depth = expected.HeaderX64.depth + 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;
		//dest, desired1,desired2, expeced
		if (InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)//��ȯ����
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
		//Use-After_Free ����
		desired.HeaderX64.next = (((uint64)entry->next) >> 4);
		desired.HeaderX64.depth = expected.HeaderX64.depth - 1;
		desired.HeaderX64.sequence = expected.HeaderX64.sequence + 1;

		if (::InterlockedCompareExchange128((int64*)header, desired.region, desired.alignment, (int64*)&expected) == 1)
			break;
	}

	return entry;
}
