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
		//attachheader로 헤더다음, 데이터의 시작주소를 반환
		Ty* memory = static_cast<Ty*>(MemoryHeader::AttachHeader(s_pool.Pop(), s_allocSize));
		new(memory)Ty(::forward<Args>(args)...); //생성자의 인자도 넣어줄 수 있다.
		return memory;
	}

	static void Push(Ty* obj)
	{
		obj->~Ty(); //소멸자 호출

		// 헤더가들어갈, 헤더의 시작주소를 구하고, Pool에 넣어준다.
		s_pool.Push(MemoryHeader::DetachHeader(obj)); 
	}

	static shared_ptr<Ty> MakeShared()
	{
		shared_ptr<Ty> ptr = { Pop(),Push };
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
