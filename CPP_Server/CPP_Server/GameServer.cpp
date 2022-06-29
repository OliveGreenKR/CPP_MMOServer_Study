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

struct Session // 서버에 접속한 클라이언트의 정보를 구조체로 관리할것 ( 1000개클라 -> 1000개 Session)
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
		//Accept를 기달리고 있는 listenSocket은 Read를 검사하는 set에 넣어주어야한다.
		FD_SET(listenSocket, &reads);

		//socket 등록
		for (Session& s : sessions)
		{
			//소켓 정책에 따라 조건을 나누어 set에 삽입
			if (s.recvBytes <= s.sendBytes) 
				FD_SET(s.socket, &reads);
			else
				FD_SET(s.socket, &writes);
		}
		//[0:window라서 사용안함][set1][set2][set3][Option]
		// [Option] : 마지막 time out 인자 설정가능
		timeval timeout;
		timeout.tv_sec; //set
		timeout.tv_usec; //micro sec
		int retVal = ::select(0, &reads, &writes, nullptr, nullptr);  //무한대기로 설정
		if (retVal == SOCKET_ERROR)	 break;

		//select는 준비되지 않은 것은 제거 -> 이제 남은 것들은 모두 가능한 socket일것이다.

		//Listenr socekt 체크 : accept 준비상태 확인
		if (FD_ISSET(listenSocket, &reads))
		{
			//accept 가능 -> 요청한 client가 존재
			SOCKADDR_IN clientAddr;
			int32 addrLen = sizeof(clientAddr);
			SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSocket != INVALID_SOCKET)
			{
				cout << "Client Connected!" << endl;
				sessions.push_back(Session{ clientSocket });
			}
		}
		//나머지 소켓 체크

		for (Session& s : sessions)
		{
			//read 체크
			if (FD_ISSET(s.socket, &reads))
			{
				int32 recvLen = ::recv(s.socket, s.recvBuffer, BUFSIZE, 0);	
				if (recvLen <= 0)
				{
					//todo : sessions제거
					continue;
				}
				s.recvBytes = recvLen;
			}

			//write체크
			if (FD_ISSET(s.socket, &writes))
			{
				//블로킹 소켓-> 모든 데이터 발송
				//논블로킹 소켓 -> 일부만 발송 가능(상대방 수신버퍼에 따라)
				int32 sendLen = send(s.socket, &s.recvBuffer[s.sendBytes], s.recvBytes - s.sendBytes, 0);
				if (sendLen == SOCKET_ERROR)
				{
					//TODO : session 제거
					continue;
				}
				s.sendBytes += sendLen;
				if (s.recvBytes == s.sendBytes) //다보냄
				{
					s.recvBytes = 0;
					s.sendBytes = 0;
				}
			}
		}


	}

	
	// 윈속 종료
	::WSACleanup();
}
