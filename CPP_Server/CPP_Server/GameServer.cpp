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
		// TODO ..�i�ư���

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
	//���۹��������� �ʱ�ÿ��� �������� Ref�� ���߾����
	wraight->ReleaseRef();
	MissileRef missile(new Missile());
	missile->ReleaseRef();


	missile->SetTarget(wraight);

	//���̽��� ü���� 0�̵�.
	wraight->_hp = 0;

	//delete wraight;
	//wraight->ReleaseRef(); 
	wraight = nullptr;
	//���� ����ó���� ���־, nullptr�� sharedPtr�� �����Ǹ鼭.(���������)
	//������ ���� Release�ϰ� �ٽ� nullptr�� �����ϰԵȴ� (���翬���� �����ε� Ȯ��)

	// nmullptr �Ǵ� �ٸ� ptr�� �о��ָ� �ڿ������� 
	//refcount�� �����ϸ鼭 shared_ptr����� �� �� �ְ� �Ǿ���.

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

