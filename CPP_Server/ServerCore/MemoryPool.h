#pragma once

/*---------------------
	Memory Header
----------------------*/

struct MemoryHeader//편리한 관리를 위한 메모리 헤더 (데이터 크기 등등..)
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

class MemoryPool
{
public:
	MemoryPool(int32 allocsize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr);
	MemoryHeader*	Pop();

private:
	int32 _allocSize = 0;
	atomic<int32> _allocCount = 0; //메모리 풀에서 뱉어준 메모리의 개수(디버깅용)

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};

