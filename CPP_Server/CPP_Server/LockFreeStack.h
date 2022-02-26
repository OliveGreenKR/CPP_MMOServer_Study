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
DECLSPEC_ALIGN(16)  //16����Ʈ ����
struct SListEntry
{
	SListEntry* next;
};
DECLSPEC_ALIGN(16)  //16����Ʈ ����
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
			uint64 alignment; // ī����
			uint64 region; //������ �ּ�
		}DUMMYSTRUCTNAME;
		struct
		{
			uint64 depth : 16; //�����Ͱ���
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