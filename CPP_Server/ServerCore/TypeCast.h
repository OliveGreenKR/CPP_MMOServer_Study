#pragma once
#include "Types.h"

#pragma region TypeList

template<typename... T>
struct TypeList;

template<typename T, typename U>
struct TypeList<T, U>
{
	using Head = T;
	using Tail = U;
};

template<typename T, typename... U>
struct TypeList<T, U...>
{
	using Head = T;
	using Tail = TypeList<U...>;
};
#pragma endregion

#pragma region Length
template<typename T>
struct Length;

template<>
struct Length<TypeList<>>
{
	enum { value = 0 };
};

template<typename T, typename...U>
struct Length<TypeList<T, U...>>
{
	enum { value = 1 + Length<TypeList<U...>>::value };
};
#pragma endregion

#pragma region TypeAt
template<typename T, int32 index>
struct TypeAt;

template<typename Head, typename ...Tail>
struct TypeAt<TypeList<Head, Tail...>, 0>
{
	using Result = Head;
};

template<typename Head, typename...Tail, int32 index>
struct TypeAt<TypeList<Head,Tail...>,index>
{
	using Result = typename TypeAt<TypeList<Tail...>,index -1 >::Result
};

#pragma endregion

#pragma region IndexOf
template<typename TL, typename T>
struct IndexOf;

template<typename ...Tail, typename T>
struct IndexOf<TypeList<T, Tail...>, T>
{
	enum { value = 0};
};

template<typename T>
struct IndexOf<TypeList<>, T>
{
	enum { value = -1 };
};

template<typename Head, typename ...Tail, typename T>
struct IndexOf<TypeList<Head, Tail...>, T>
{
private:
	enum {temp = IndexOf<TypeList<Tail...>,T>::value};
public:
	enum { value = (temp == -1)? -1: temp+1 };
};

#pragma endregion

#pragma region Conversion
template<typename From, typename To>
class Conversion
{
private:
	using Small = __int8;
	using Big = __int32;

	static Small Test(const To&) { return 0; }
	static Big Test(...) { return 0; }
	static From MakeFrom() { return 0; }

public:
	enum
	{
		exists =  sizeof(Test(MakeFrom())) ==  sizeof(Small)
	};
	//�����Ϸ��� �ִ��� �����ϰ� �ؼ� -> From�� To�� ĳ������ �ȴٸ�, Test�ռ��� SmallŸ���� �����ϰԵȴ�
	//�׷��Ƿ� ������ Ÿ��ĳ������ �����ϱ� ������ exists�� true�� �ȴ�.

	//���, �����Ϸ����� ������ �Ǻ��Ǳ� ������, Test �Լ��� �����ΰ� �������� �ʾƵ� �۵��Ѵ�.
};
#pragma endregion

#pragma region TypeCast

template<typename TL>
class TypeConversion
{

};
#pragma endregion