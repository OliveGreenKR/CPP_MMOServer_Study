#pragma once

// C++11 apply
template<int... Remains>
struct seq
{};

template<int N, int... Remains>
struct gen_seq : gen_seq<N-1, N-1, Remains...>
{};

template<int... Remains>
struct gen_seq<0, Remains...> : seq<Remains...>
{};

//gens_seq은 재귀의 과정을 통해 넣어준 처음에 넣어준 숫자만큼의 seq을 return하게됨
// gen_seq<3) --->  sea<0,1,2>가 된다

template<typename Ret, typename... Args>
void xapply(Ret(*func)(Args...), std::tuple<Args...>& tup)
{
	xapply_helper(func, gen_seq<sizeof...(Args)>(), tup);
						//sizeof(...Arg)는 인자의 개수를 뱉어낸다.
}

template<typename F, typename... Args, int... ls>
void xapply_helper(F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(func)(std::get<ls>(tup)...); 
	//그렇게 tuple의 인자를 하나씩 빼서 각각으로 전달할 수 있게 된다.
}

//멤버함수 호출을 위해,  객체를 받는 T 추가
template<typename T, typename Ret, typename... Args>
void xapply(T* obj, Ret(T::*func)(Args...), std::tuple<Args...>& tup)
{
	xapply_helper(obj, func, gen_seq<sizeof...(Args)>(), tup);
}

template<typename T, typename F, typename... Args, int... ls>
void xapply_helper(T* obj, F func, seq<ls...>, std::tuple<Args...>& tup)
{
	(obj->*func)(std::get<ls>(tup)...);
}

// 함수자 (Functor)
class IJob
{
public:
	virtual void Execute() { }
};

template<typename Ret, typename... Args>
class FuncJob : public IJob
{
	using FuncType = Ret(*)(Args...);

public:
	FuncJob(FuncType func, Args... args) : _func(func), _tuple(args...)
	{

	}

	virtual void Execute() override
	{
		//std::apply(_func, _tuple); // C++17
		xapply(_func, _tuple);
	}

private:
	FuncType _func;
	std::tuple<Args...> _tuple;
};

//멤버함수 호출 job
template<typename T, typename Ret, typename... Args>
class MemberJob : public IJob
{
						//객체 안의 멤버함수 포인터 이므로 T::추가
	using FuncType = Ret(T::*)(Args...);

public:
	MemberJob(T* obj, FuncType func, Args... args) : _obj(obj), _func(func), _tuple(args...)
	{

	}

	virtual void Execute() override
	{
		//std::apply(_func, _tuple); // C++17
		xapply(_obj, _func, _tuple);
	}

private:
	T*			_obj;
	FuncType	_func;
	std::tuple<Args...> _tuple;
};
/// /////////////
