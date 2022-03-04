#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

/*---------------------
	Memory Header	//편리한 관리를 위한 메모리 헤더 (데이터 크기 등등..)
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
		//header의 다음 주소 반환, 즉 data의 시작 주소를 반환한다.
		return reinterpret_cast<void*>(++header);
	}
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		//MemoryHeader 크기만큼 뒤로 가기위해 casting  후에 -1  연산을 한다.
		return header;
	}

public:
	int32 allocSize;
	//필요한 debug 정보 추가
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
	//헤더가 제일먼저 위치해야함
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	atomic<int32> _useCount = 0; 
	atomic<int32> _reserveCount = 0; 

};

