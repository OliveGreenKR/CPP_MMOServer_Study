#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*---------------------
	Memory Header	//���� ������ ���� �޸� ��� (������ ũ�� ���..)
----------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader  : public SLIST_ENTRY
{
public:
	//[Header][Data]
	MemoryHeader(int32 size) : allocSize(size)
	{}
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); //placement new
		//header�� ���� �ּ� ��ȯ, �� data�� ���� �ּҸ� ��ȯ�Ѵ�.
		return reinterpret_cast<void*>(++header);
	}
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		//MemoryHeader ũ�⸸ŭ �ڷ� �������� casting  �Ŀ� -1  ������ �Ѵ�.
		return header;
	}

public:
	int32 allocSize;
	//�ʿ��� debug ���� �߰�
};

/*---------------------
	Memory Pool            
----------------------*/
DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
public:
	MemoryPool(int32 allocsize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();

private:
	//����� ���ϸ��� ��ġ�ؾ���
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	atomic<int32> _useCount = 0; 
	atomic<int32> _reserveCount = 0; 

};

