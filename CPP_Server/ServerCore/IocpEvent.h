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

//OverLapped Ȯ��
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
	//todo : accept�� ��� �߰����� ���ڰ� �ʿ��� ���� �ִ�, 
	//session���� �����ʿ� :  Listenr�� Register�� ���ؼ�
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