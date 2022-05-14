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
int main()
{
    //WinSock Lib Init
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    // socket = 전화 번호
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }
    //-----연결할 목적지 설정
    SOCKADDR_IN serverAddr; //IPv4
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); //<알아서 골라줘~  << 이론적으로 가능한 모든 주소랑 연결가능
    //DNS서버를 이용해 서버주소를 찾는 방법을 사용할 예정이다
    serverAddr.sin_port = htons(7777);

    //안내원 폰 개통, "식당의 대표번호". => Bind
    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }
    //영업시작 => listen
    if (::listen(listenSocket, 10) == SOCKET_ERROR)// backlog: 최대 대기열 한도
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Listen Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }

    while (true)
    {
        SOCKADDR_IN clientAddr; //IPv4
        ::memset(&clientAddr, 0, sizeof(clientAddr));
        int32 addrLen = sizeof(clientAddr);
        //accpet를 통해 Client의 주소를 받아 클라이언트 소켓을 반환한다.
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen); //뒤의 두개는 Opt이다. Null로 밀어도됨.
        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Accept Error Code : " << errCode << endl;
            ::WSACleanup();
            return 0;
        }

        //손님입장
        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;

        //todo
    }




    //winsock 종료
    ::WSACleanup();
}
