#pragma once

/*----------------------
	IocpObject
---------------------*/
class IocpObject
{
public:
	virtual HANDLE GetHandle() abstract; //순수 가상함수 (파생에서 반드시 재정의해야함)
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract; //Event를 통해 일감 종류를 파악 및 처리
	
};
/*----------------------
	IocpCOre
---------------------*/

class IocpCore
{
public:
	IocpCore();
	~IocpCore();

	HANDLE	GetHandle() { return _iocpHandle; }

	bool	Register(class IocpObject* iocpObject);
	bool	Dispatch(uint32 timeoutMs= INFINITE);

private:
	HANDLE	_iocpHandle;
};

//TEMP for debugging
extern IocpCore GIocpCore;