#pragma once

/*----------------------
	IocpObject
---------------------*/
class IocpObject  : public enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract; //���� �����Լ� (�Ļ����� �ݵ�� �������ؾ���)
	virtual void Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) abstract; //Event�� ���� �ϰ� ������ �ľ� �� ó��
	
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

	bool	Register(IocpObjectRef iocpObject);
	bool	Dispatch(uint32 timeoutMs= INFINITE);

private:
	HANDLE	_iocpHandle;
};
