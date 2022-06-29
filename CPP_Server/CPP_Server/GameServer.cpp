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

const int BUFSIZE = 1000;

struct Session // ������ ������ Ŭ���̾�Ʈ�� ������ ����ü�� �����Ұ� ( 1000��Ŭ�� -> 1000�� Session)
{
	SOCKET socket = INVALID_SOCKET;
	char recvBuffer[BUFSIZE] = {};
	int32 recvBytes = 0;
	int32 sendBytes = 0;
};

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
							Select Model
	----------------------------------------------------------*/
	vector<Session> sessions;
	sessions.reserve(100);

	fd_set reads, writes;

	while (true)
	{
		//***socket set init
		FD_ZERO(&reads);
		FD_ZERO(&writes);

		//**ListenSocket Enroll  : 
		//Accept�� ��޸��� �ִ� listenSocket�� Read�� �˻��ϴ� set�� �־��־���Ѵ�.
		FD_SET(listenSocket, &reads);

		//socket ���
		for (Session& s : sessions)
		{
			//���� ��å�� ���� ������ ������ set�� ����
			if (s.recvBytes <= s.sendBytes) 
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}
		//[0:window�� ������][set1][set2][set3][Option]
		// [Option] : ������ time out ���� ��������
		timeval timeout;
		timeout.tv_sec; //set
		timeout.tv_usec; //micro sec
		int retVal = ::select(0, &reads, &writes, nullptr, nullptr);  //���Ѵ��� ����
		if (retVal == SOCKET_ERROR)	 break;

		//select�� �غ���� ���� ���� ���� -> ���� ���� �͵��� ��� ������ socket�ϰ��̴�.

		//Listenr socekt üũ : accept �غ���� Ȯ��
		if (FD_ISSET(listenSocket, &reads))
		{
			//accept ���� -> ��û�� client�� ����
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected!" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}
		//������ ���� üũ

		for (Session& s : sessions)
		{
			//read üũ
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);	
				if (recvLen <= 0)
				{
					//todo : sessions����
					continue;
				}
				s.recvBytes = recvLen;
			}

			//writeüũ
			if (FD_ISSET(s.socket, &writes))
			{
				//���ŷ ����-> ��� ������ �߼�
				//����ŷ ���� -> �Ϻθ� �߼� ����(���� ���Ź��ۿ� ����)
				int32 sendLen = send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					//TODO : session ����
					continue;
				}
				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes) //�ٺ���
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}


	}

	
	// ���� ����
	::WSACleanup();
}
