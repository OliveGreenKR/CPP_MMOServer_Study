#pragma once


#ifdef FIRST
/*___________________________
		1�� �õ�
-----------------------------*/
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};
void InitializeHeader(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);

#endif // FIRST

#ifdef SECOND
/*___________________________
		2�� �õ�
-----------------------------*/
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};
void InitializeHeader(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
#endif // SECOND
/*___________________________
		3�� �õ�
-----------------------------*/
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};
void InitializeHeader(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);