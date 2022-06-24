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
int main()
{
    //WinSock Lib Init
    WSAData wsaData;
    if (::WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return 0;

    //socekt
    SOCKET serverSocket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        HandleError("Socekt");
        return 0;
    }

    //�ɼ��� �ؼ��ϰ� ó���ϴ� ��ü : level
    //�����ڵ� -> SOL_SOCKERT
    //IPv4 -> IPPROTO_IP
    //TCP �������� -> IPPROTO_TCP

    //�ش� level�� option ���� :  opt
    //�پ��� �ɼ��� ����. MS DOC ����  -> �� �ɼǸ��� ���� optval�� �ٸ��� ������ �� Ȯ������
    
    //SO_KEEPALIVE = �ֱ������� ���� ���� Ȯ�� ����(TCP_ONLY)
    //������ ������ �Ϲ������� ���� ��쿡 ���� ��ó�� ����
    //�ֱ������� ������¸� Ȯ���ϴ� ��Ŷ�� ������ ������ ������ �����Ѵ�.
    bool enable = true;
    setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

    //SO_LINGER : �����ϴ�
    // ���� ���ҽ� ��ȯ ���� ���´� senddata�� ó�� ����
    // �۽Ź��ۿ� �ִ� �������͸� �������ΰ�?
    LINGER linger;
    // linger onoff(default = 0) : 
    // 0 �̸� closesocket()�� �ٷ� ����;
    // 1 �̸� linger sec ��ŭ ��� �� ����;
    linger.l_onoff = 1;
    linger.l_linger = 5; //5 sec
    setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger , sizeof(linger));


    //HALF-CLOSE
    //SD_SEND : send ���´�.
    //SD_RECEIVE : recv ���´�
    //SD_BOTH :  �Ѵ� ���´�.
    //::shutdown(serverSocket, SD_SEND);

    // SO_SNDBUF, SO_RCVBUF : �ۼ��� ���� ũ����� ���.
    int32 sendBufferSize;
    int32 optionLen = sizeof(sendBufferSize);
    ::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);

    //SO_REUSEADDR : IP�ּ� �� Port�� ����
    {
        bool enable = true;
        ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
    }

    //IPPROTO_TCP 
    //TCP_NODELAY : Nagle �˰��� �۵�����
    //*Nagle : �����Ͱ� ����� ũ�� ������, �׷��� ������ ���� ������ ���.
    //==> ���ʿ��� packet ���� ����, But ����ϸ鼭 ���� �ð� ����.
    //���� ���� ������� ���������� �� ���ִ� ���� ����!!
    {
        bool enable = true;
        ::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
    }

    //���� ���ҽ� ��ȯ
    ::closesocket(serverSocket); 

    //winsock ����
    ::WSACleanup();
}
