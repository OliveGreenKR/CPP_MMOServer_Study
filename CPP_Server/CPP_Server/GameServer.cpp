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

    // socket = ��ȭ ��ȣ
    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }
    //-----������ ������ ����
    SOCKADDR_IN serverAddr; //IPv4
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY); //<�˾Ƽ� �����~  << �̷������� ������ ��� �ּҶ� ���ᰡ��
    //DNS������ �̿��� �����ּҸ� ã�� ����� ����� �����̴�
    serverAddr.sin_port = htons(7777);

    //�ȳ��� �� ����, "�Ĵ��� ��ǥ��ȣ". => Bind
    if (::bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Bind Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }
    //�������� => listen
    if (::listen(listenSocket, 10) == SOCKET_ERROR)// backlog: �ִ� ��⿭ �ѵ�
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
        //accpet�� ���� Client�� �ּҸ� �޾� Ŭ���̾�Ʈ ������ ��ȯ�Ѵ�.
        SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen); //���� �ΰ��� Opt�̴�. Null�� �о��.
        if (clientSocket == INVALID_SOCKET)
        {
            int32 errCode = ::WSAGetLastError();
            cout << "Accept Error Code : " << errCode << endl;
            ::WSACleanup();
            return 0;
        }

        //�մ�����
        char ipAddress[16];
        ::inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddress, sizeof(ipAddress));
        cout << "Client Connected! IP = " << ipAddress << endl;

        //todo
    }




    //winsock ����
    ::WSACleanup();
}
