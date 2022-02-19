#pragma once
#include "Allocator.h"

template<typename Type, typename ...Args>
Type* xnew(Args&&... args) //������ ������ �ƴ� '��������=��������'
{
	Type* memory = static_cast<Type*>(XAlloc(sizeof(Type)));
	//placement new
	new(memory) Type(::forward<Args>(args)...);
	return memory;
}

template<typename Type>
void xdelete(Type* obj)
{
	obj->~Type();
	XRelease(obj);
}