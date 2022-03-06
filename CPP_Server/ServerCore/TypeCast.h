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

template<typename V>
struct Int2Type
{
	enum { Value =  V};
};

template<typename TL>
class TypeConversion
{
public:
	enum
	{
		length = Length<TL>::value
	};

	TypeCast()
	{
		MakeTable(Int2Type<0>(), Int2Type<0>());
	}

	template <int32 i, int32 j>
	static void MakeTable(Int2Type<i>, Int2Type<j>)
	{
		using FromType = typename TypeAt<TL, i>::Result;
		using ToType = typename TypeAt<TL, j>::Result;

		if (Conversion<const FromType*, const ToType*>::exists)
			s_convert[i][j] = true;
		else
			s_convert[i][j] = false;

		MakeTable(Int2Type<i>(), Int2Type<j+1>());
	}

public:
	static bool s_convert[length][length];
};

template <typename TL>
bool TypeConversion<TL>::s_convert[length][length];


#pragma endregion