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
DECLSPEC_ALIGN(16)  //16바이트 정렬
struct SListEntry
{
	SListEntry* next;
};
DECLSPEC_ALIGN(16)  //16바이트 정렬
struct SListHeader
{
	SListHeader()
	{
		alignment = 0;
		region = 0;
	}
	union
	{
		struct
		{
			uint64 alignment; // 카운터
			uint64 region; //데이터 주소
		}DUMMYSTRUCTNAME;
		struct
		{
			uint64 depth : 16; //데이터개수
			uint64 sequence : 48; //push / pop 
			uint64 reserved : 4;
			uint64 next : 60; 
		}HeaderX64;
	};

	SListEntry* next = nullptr;
};
void InitializeHeader(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);