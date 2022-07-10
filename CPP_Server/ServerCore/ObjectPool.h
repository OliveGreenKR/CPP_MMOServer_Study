#pragma once
#include "Types.h"
#include "MemoryPool.h"

template<typename Ty>
class ObjectPool
{
public:
	template<typename ...Args>
	static Ty* Pop(Args&& ...args)
	{
#ifdef _STOMP
		MemoryHeader* ptr = reinterpret_cast<MemoryHeader*>(StompAllocator::Alloc(s_allocSize));
		Ty* memory = static_cast<Ty*>(MemoryHeader::AttachHeader(ptr, s_allocSize));
#else
		//attachheader�� �������, �������� �����ּҸ� ��ȯ
		Ty* memory = static_cast<Ty*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
#endif
		new(memory)Ty(::forward<Args>(args)...); //�������� ���ڵ� �־��� �� �ִ�.
		return memory;
	}

	static void Push(Ty* obj)
	{
		obj->~Ty(); //�Ҹ��� ȣ��
#ifdef _STOMP
		StompAllocator::Release(MemoryHeader::DetachHeader(obj));
#else
		// �������, ����� �����ּҸ� ���ϰ�, Pool�� �־��ش�.
		s_pool.Push(MemoryHeader::DetachHeader(obj));
#endif
	}
	template<typename ...Args>
	static shared_ptr<Ty> MakeShared(Args&&... args)
	{
		shared_ptr<Ty> ptr = { Pop(::forward<Args>(args)...),Push };
		return ptr;
	}
private:
	static int32 s_allocSize;
	static MemoryPool s_pool;
};

template <typename Ty>
int32 ObjectPool<Ty>::s_allocSize = sizeof(Ty) + sizeof(MemoryHeader);
template <typename Ty>
MemoryPool ObjectPool<Ty>::s_pool(s_allocSize);
