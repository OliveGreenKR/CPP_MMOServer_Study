#include "pch.h"

#include <iostream>
using namespace std;

#include "CorePch.h"
#include <thread>
#include<atomic>
#include <mutex>
#include <future>
#include <queue>
#include <stack>
#include <Windows.h>
#include "ThreadManager.h"
#include "RefCounting.h"

class Wraight : public RefCountable
{
public:
	int _hp = 150;
	int _posX = 0;
	int _posY = 0;
};

using WraightRef = TSharedPtr<Wraight>;

class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
		_target = target;
		//_target->AddRef();
	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_posX;
		int posY = _target->_posY;
		// TODO ..쫒아가기

		if (_target->_hp == 0)
		{
			//_target->ReleaseRef();
			_target = nullptr;
			return true;
		}
		return false;
	}

public:
	WraightRef _target = nullptr;
};

using MissileRef = TSharedPtr<Missile>;

int main()
{
	WraightRef wraight(new Wraight()); //wraight ref =2
	//제작버전에서는 초기시에는 수동으로 Ref를 낮추어야함
	wraight->ReleaseRef();
	MissileRef missile(new Missile());
	missile->ReleaseRef();


	missile->SetTarget(wraight);

	//레이스가 체력이 0이됨.
	wraight->_hp = 0;

	//delete wraight;
	//wraight->ReleaseRef(); 
	wraight = nullptr;
	//이제 위에처럼만 해주어도, nullptr로 sharedPtr이 생성되면서.(복사생성자)
	//기존의 것은 Release하고 다시 nullptr로 생성하게된다 (복사연산자 오버로딩 확인)

	// nmullptr 또는 다른 ptr로 밀어주면 자연스럽게 
	//refcount가 동작하면서 shared_ptr기능을 할 수 있게 되었다.

	while (true)
	{
		if (missile)
		{
			if (missile->Update()) { 
				//missile->ReleaseRef();
				missile = nullptr;
			}
		}
	}
								
	//delete missile;
	/*missile->ReleaseRef();*/
	missile = nullptr;

}

