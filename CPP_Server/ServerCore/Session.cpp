#include "pch.h"
#include "Session.h"
#include "SocketUtils.h"
#include "Service.h"

Session::Session() {
	_socket = SocketUtils::CreateSocket();
}

Session::~Session() {
	SocketUtils::Close(_socket);
}

void Session::Disconnect(const WCHAR* cause) {
	if(_connected.exchange(false) == false)
		return; //원래 false였음

	//원래 true
	wcout << "Disconnect : " << cause << endl;

	OnDIsconnected(); //컨텐츠 코드에서 오버로딩
	SocketUtils::Close(_socket);
	GetService()->ReleaseSession(GetSessionRef());
}

HANDLE Session::GetHandle() {
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes) {
	switch (iocpEvent->eventType) {
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		break;
		ProcessSend(numOfBytes);
		break;

	default:
		break;
	}
}

void Session::RegisterConnect() {
}

void Session::RegisterRecv() {

	if(IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); //add_ref

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer);
	wsaBuf.len = len32(_recvBuffer);

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, OUT &numOfBytes, OUT &flags, &_recvEvent, nullptr)) {
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING) {
			HandleError(errorCode);
			_recvEvent.owner = nullptr; //release_ref
		}
	}
}

void Session::RegisterSend() {
}

void Session::ProcessConnect() {
	_connected.store(true);
	//session register
	GetService()->AddSession(GetSessionRef());

	//contents code's overloading
	OnConnected();

	//수신등록
	RegisterRecv();
}

void Session::ProcessRecv(int32 numOfBytes) {
	_recvEvent.owner = nullptr; //release_ref :  더 이상 예약을 거는 개체가 없으므로 release

	if (numOfBytes == 0) {
		Disconnect(L"Recv 0");
		return;
	}
	//todo
	cout << "Recv Data len = " << numOfBytes << endl;

	//수신등록
	RegisterRecv();
}

void Session::ProcessSend(int32 numOfBytes) {
}

void Session::HandleError(int32 errorCode) {
	switch (errorCode) {
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		//todo: log
		cout << "Handle Error : " << errorCode << endl;
		break;
	}
}
