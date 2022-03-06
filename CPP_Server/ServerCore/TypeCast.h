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
	//컴파일러는 최대한 가능하게 해석 -> From이 To로 캐스팅이 된다면, Test합수는 Small타입을 리턴하게된다
	//그러므로 같으면 타입캐스팅이 가능하기 때문에 exists는 true가 된다.

	//사실, 컴파일러에서 모든것이 판별되기 때문에, Test 함수의 구현부가 존재하지 않아도 작동한다.
};
#pragma endregion

#pragma region TypeCast

template<typename TL>
class TypeConversion
{

};
#pragma endregion