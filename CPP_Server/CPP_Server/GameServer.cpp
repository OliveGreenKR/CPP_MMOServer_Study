#include "pch.h"

#include <iostream>

#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include <Windows.h>
#include "ThreadManager.h"

#include <WinSock2.h>
#include <MSWSock.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << "ErrCode" <<  errCode << endl;
}

const int32 BUFSIZE = 1000;

struct Session // 서버에 접속한 클라이언트의 정보를 구조체로 관리할것 ( 1000개클라 -> 1000개 Session)
{
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags) {
	
	cout << "data recvLen Callback = " << recvLen << endl;
	//todo : eco 서버를 만든다면 여기서 wsasend를 실행

}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return 0;

	u_long on = 1;
	if (::ioctlsocket(listenSocket, FIONBIO, &on) == INVALID_SOCKET)
		return 0;

	SOCKADDR_IN serverAddr;
	::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	serverAddr.sin_port = ::htons(7777);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
		return 0;

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		return 0;

	cout << "Accept" << endl;

	/*------------------------------------------------------------
					Overlapped  I/O Model (CallBack)
	----------------------------------------------------------*/
	while (true)
	{
		//비동기 소켓 생성
		sockaddr_in clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket;
		while (true)
		{
			//accept도 비동기가 존재하지만 사용 전 준비사항이 복잡하여 이번강의에서는 동기식 accept를 사용
			clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			//문제상황
			return 0;
		}

		Session session = Session{ clientSocket };
		//이벤트 생성.
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client connected!" << endl;

		while (true)
		{
			WSABUF wsaBuf;//비동기 recv를 위한 구조체
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			//비동기 recv
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped,RecvCallback )
				== SOCKET_ERROR)
			{
				//pending 상태인지?
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					//pending ==> callback로 통제할 것
					//alertable state로 바꾸기
					//1. event개수를 1:1대응해야하므로 개수 제한이 존재
					//::WSAWaitForMultipleEvents(1, &wsaEvent, true, WSA_INFINITE, "TRUE");
					//2. apcQ 로 예약되는 모든 콜백 처리, 클라이언트의 개수에 크게 영향받지 않는다.
					::SleepEx(INFINITE, true); 
				}
				else
				{
					//todo : 문제상황
					break;
				}
			}
			else //recvcallback없이 바로 보내졌을 때
				cout << "Data Recv Len = " << recvLen << endl;
		}
		::closesocket(session.socket);
	}
	
	// 윈속 종료
	::WSACleanup();
}
