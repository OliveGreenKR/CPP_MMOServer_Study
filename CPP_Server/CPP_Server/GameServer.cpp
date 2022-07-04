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

#include <Memory.h>

void HandleError(const char* cause)
{
    int32 errCode = ::WSAGetLastError();
    cout << cause << "ErrCode" <<  errCode << endl;
}

const int32 BUFSIZE = 1000;

struct Session // ������ ������ Ŭ���̾�Ʈ�� ������ ����ü�� �����Ұ� ( 1000��Ŭ�� -> 1000�� Session)
{
	//WSAOVERLAPPED overlapped = {};
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
};

enum IO_TYPE
{
	READ,
	WRITE,
	ACCEPT,
	CONNECT,
};
struct OverlappedEx
{
	WSAOVERLAPPED overlapped = {};
	int32 type = 0; //IO_TYPE
};

void WorkerThreadMain(HANDLE iocpHandle) {
	while (true)
	{

		DWORD bytesTransferred = 0;
		Session* session = nullptr;
		OverlappedEx* overlappedEx = nullptr;
		//IOCP�� �Ϸ��ȣ �Դ��� üũ
		bool ret =  ::GetQueuedCompletionStatus(iocpHandle, &bytesTransferred,
									/*key val*/
									(ULONG_PTR*)&session,
									(LPOVERLAPPED*)&overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
		{
			//TODO :  �������
			continue;
		}

		ASSERT_CRASH(overlappedEx->type == IO_TYPE::READ);
		cout << "recv data IOCP = " << bytesTransferred << endl;


		//�ٽ� read�� �ϱ� ���� recv ��ȣ��
		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;

		::WSARecv(session->socket, &wsaBuf, 1, &recvLen, &flags,
				  &overlappedEx->overlapped,
				  NULL);
	}
}

int main()
{
	WSAData wsaData;
	if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
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
					IOCP
	----------------------------------------------------------*/

	vector<Session*> sessionManager;

	//CP ����
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);

	//WorkerThreads ����
	for (int32 i = 0; i < 5; i++)
		GThreadManager->Launch([=]() { WorkerThreadMain(iocpHandle); });

	//Main Thread =  Accept ���
	while (true)
	{
		sockaddr_in clientAddr;
		int32 addrLen = sizeof(clientAddr);
		SOCKET clientSocket = ::accept(listenSocket, (sockaddr*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
			return 0;
		Session* session = xnew<Session>();
		session->socket = clientSocket;
		sessionManager.push_back(session);

		cout << "Client Connected!" << endl;

		//������ CP�� ���
		::CreateIoCompletionPort((HANDLE)clientSocket,
								 iocpHandle,
								 /*key��: ���α׷������Ƿ� �ƹ��ų� ����, ���α׷��Ӱ� ������ Ű�� ����� �Ѵ�.*/
								 (ULONG_PTR)session,
								 /*�ִ� ������ ����,  0: �ִ��ھ��*/
								 0);
		WSABUF wsaBuf;
		wsaBuf.buf = session->recvBuffer;
		wsaBuf.len = BUFSIZE;

		DWORD recvLen = 0;
		DWORD flags = 0;

		//������ ���� �����Ҵ�
		OverlappedEx* overlappedEx = new OverlappedEx();
		overlappedEx->type = IO_TYPE::READ;

		::WSARecv(clientSocket, &wsaBuf, 1, &recvLen, &flags,
				  /*LPWSAoverlapped ptr*/
				  &overlappedEx->overlapped,
				  /*COmpletion routine :�ݹ��Լ�*/
				  NULL);
	}
	
	GThreadManager->Join();
	// ���� ����
	::WSACleanup();
}
