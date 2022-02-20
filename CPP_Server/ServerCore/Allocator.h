#pragma once

/*------------------------------
		BaseAllocator
--------------------------------*/

class BaseAllocator
{
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);

};

/*------------------------------
		Stomp Allocator
--------------------------------*/
class StompAllocator
{
	enum { PAGE_SIZE = 0x1000 };//4KB,4096
public:
	static void* Alloc(int32 size);
	static void Release(void* ptr);

};
