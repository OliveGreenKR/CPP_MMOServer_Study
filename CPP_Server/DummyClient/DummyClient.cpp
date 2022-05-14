#include "pch.h"
#include <iostream>

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
    SOCKET clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Socket Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }

    //-------������ ������ ����
    SOCKADDR_IN serverAddr; //IPv4
    ::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = ::inet_addr("127.0.0.1"); //���Ĺ���
    ::inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); //���������ּҰ� ������.
    //DNS������ �̿��� �����ּҸ� ã�� ����� ����� �����̴�
    serverAddr.sin_port = htons(7777);

    if(::connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR )
    {
        int32 errCode = ::WSAGetLastError();
        cout << "Connect Error Code : " << errCode << endl;
        ::WSACleanup();
        return 0;
    }

    cout << "Connection Succeed!\n";
    while (true)
    {
        //todo
        this_thread::sleep_for(1s);
    }
    //socket resource ��ȯ
    ::closesocket(clientSocket);
    //winsock ����
    ::WSACleanup();
}