#pragma once


#ifdef FIRST
/*___________________________
		1차 시도
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
		2차 시도
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
		3차 시도
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