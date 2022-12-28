#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
/*--------------
	Session
---------------*/
class Session : public IocpObject
{
	//specific classes that can access private
	friend class Listener;
	friend class IocpCore;
	friend class Service;

public:
	Session();
	virtual ~Session();

public:
	/*정보 관련*/
	void				SetNetAddress(NetAddress address) { _netAddress = address; }
	NetAddress			GetNetAddress() { return _netAddress; }
	SOCKET				GetSocket() { return _socket; }
	bool				IsConnected() { return _connected;}
	SessionRef			GetSessionRef() { return static_pointer_cast<Session>(shared_from_this()); }

public:
	/*인터페이스 구현*/
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
		/*전송관련*/


public:
	//temp
	char _recvBuffer[100] = {};

protected:
	SOCKET				_socket = INVALID_SOCKET;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;

};

