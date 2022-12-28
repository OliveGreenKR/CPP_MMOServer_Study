#pragma once
#include "IocpCore.h"
#include "IocpEvent.h"
#include "NetAddress.h"
/*--------------
	Session
---------------*/
class Service;

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
	/*외부사용 유틸리티*/
	void				Disconnect(const WCHAR* cause);

	shared_ptr<Service> GetService() { return _service.lock();}
	void				SetService(shared_ptr<Service> service){ _service = service; }


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
	void				RegisterConnect();
	void				RegisterRecv();
	void				RegisterSend();

	void				ProcessConnect();
	void				ProcessRecv(int32 numOfBytes);
	void				ProcessSend(int32 numOfBytes);

	void				HandleError(int32 errorCode);

protected:
	/*컨텐츠 코드에서 오버로딩*/
	virtual void		OnConnected(){}
	virtual int32		OnRecv(BYTE* buffer, int32 len) {return len;}
	virtual void		OnSend(int32 len) {}
	virtual void		OnDIsconnected() {}


public:
	//temp
	char				_recvBuffer[1000] = {};

private:
	weak_ptr<Service>	_service;  //순환을 줄이고, 서비스는 항상 존재할것이므로 weakptr사용
	SOCKET				_socket = INVALID_SOCKET;
	NetAddress			_netAddress = {};
	Atomic<bool>		_connected = false;

private:
	USE_LOCK;
	/*수신관련*/

	/*통신관련*/
private:

	/*IocpEvent 재사용*/
	RecvEvent			_recvEvent;

};

