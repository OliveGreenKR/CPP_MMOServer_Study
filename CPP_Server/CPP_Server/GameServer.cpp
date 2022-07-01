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

struct Session // ������ ������ Ŭ���̾�Ʈ�� ������ ����ü�� �����Ұ� ( 1000��Ŭ�� -> 1000�� Session)
{
	WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

void CALLBACK RecvCallback(DWORD error, DWORD recvLen, LPWSAOVERLAPPED overlapped, DWORD flags) {
	
	cout << "data recvLen Callback = " << recvLen << endl;
	//todo : eco ������ ����ٸ� ���⼭ wsasend�� ����

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
		//�񵿱� ���� ����
		sockaddr_in clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket;
		while (true)
		{
			//accept�� �񵿱Ⱑ ���������� ��� �� �غ������ �����Ͽ� �̹����ǿ����� ����� accept�� ���
			clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
				break;
			if (::WSAGetLastError() == WSAEWOULDBLOCK)
				continue;
			//������Ȳ
			return 0;
		}

		Session session = Session{ clientSocket };
		//�̺�Ʈ ����.
		WSAEVENT wsaEvent = ::WSACreateEvent();
		session.overlapped.hEvent = wsaEvent;

		cout << "Client connected!" << endl;

		while (true)
		{
			WSABUF wsaBuf;//�񵿱� recv�� ���� ����ü
			wsaBuf.buf = session.recvBuffer;
			wsaBuf.len = BUFSIZE;

			DWORD recvLen = 0;
			DWORD flags = 0;
			//�񵿱� recv
			if (::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags, &session.overlapped,RecvCallback )
				== SOCKET_ERROR)
			{
				//pending ��������?
				if (::WSAGetLastError() == WSA_IO_PENDING)
				{
					//pending ==> callback�� ������ ��
					//alertable state�� �ٲٱ�
					//1. event������ 1:1�����ؾ��ϹǷ� ���� ������ ����
					//::WSAWaitForMultipleEvents(1, &wsaEvent, true, WSA_INFINITE, "TRUE");
					//2. apcQ �� ����Ǵ� ��� �ݹ� ó��, Ŭ���̾�Ʈ�� ������ ũ�� ������� �ʴ´�.
					::SleepEx(INFINITE, true); 
				}
				else
				{
					//todo : ������Ȳ
					break;
				}
			}
			else //recvcallback���� �ٷ� �������� ��
				cout << "Data Recv Len = " << recvLen << endl;
		}
		::closesocket(session.socket);
	}
	
	// ���� ����
	::WSACleanup();
}
