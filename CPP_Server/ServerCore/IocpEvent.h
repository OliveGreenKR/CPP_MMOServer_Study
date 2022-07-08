#pragma once

class Session;

/*-------------
	IocpEvent
---------------*/

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv, 0 byte recv
	Recv,
	Send,
};

//OverLapped 확장
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return _type; }

protected:
	EventType _type;

};

/*----------------------
	Connect Event
-----------------------*/

class ConnectEvent : public IocpEvent
{
public:
	ConnectEvent() : IocpEvent(EventType::Connect) {};
};

/*----------------------
	Accept Event
-----------------------*/

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::Accept) {};

	void SetSession(Session* session) { _session = session; }
	Session* GetSession() { return _session; }

private:
	//todo : accept의 경우 추가적인 인자가 필요할 수도 있다, 
	//session정보 연동필요 :  Listenr의 Register를 위해서
	Session* _session = nullptr;
};


/*----------------------
	Recv Event
-----------------------*/
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {};
};

/*----------------------
	Send Event
-----------------------*/
class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {};
};