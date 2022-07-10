#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Accept,
	//PreRecv, 0 byte recv
	Recv,
	Send,
};
/*-------------
	IocpEvent
---------------*/

//OverLapped Ȯ��
class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type);

	void Init();
	EventType GetType() { return eventType; }

public:
	EventType		eventType;
	IocpObjectRef	owner;

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

	void SetSession(Session* session) { session = session; }
	SessionRef GetSession() { return session; }

public:
	//todo : accept�� ��� �߰����� ���ڰ� �ʿ��� ���� �ִ�, 
	//session���� �����ʿ� :  Listenr�� Register�� ���ؼ�
	SessionRef session = nullptr;
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