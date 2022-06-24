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

    //옵션을 해석하고 처리하는 주체 : level
    //소켓코드 -> SOL_SOCKERT
    //IPv4 -> IPPROTO_IP
    //TCP 프로토콜 -> IPPROTO_TCP

    //해당 level의 option 설정 :  opt
    //다양한 옵션이 존재. MS DOC 참고  -> 각 옵션마다 맏는 optval이 다르기 때문에 꼭 확인하자
    
    //SO_KEEPALIVE = 주기적으로 연결 상태 확인 여부(TCP_ONLY)
    //상대방이 연결을 일방적으로 끊는 경우에 대한 대처가 가능
    //주기적으로 연결상태를 확인하는 패킷을 보내서 끊어진 연결을 감지한다.
    bool enable = true;
    setsockopt(serverSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&enable, sizeof(enable));

    //SO_LINGER : 지연하다
    // 소켓 리소스 반환 전에 보냈던 senddata의 처리 여부
    // 송신버퍼에 있는 데ㅐ이터를 보낼것인가?
    LINGER linger;
    // linger onoff(default = 0) : 
    // 0 이면 closesocket()이 바로 리턴;
    // 1 이면 linger sec 만큼 대기 후 리턴;
    linger.l_onoff = 1;
    linger.l_linger = 5; //5 sec
    setsockopt(serverSocket, SOL_SOCKET, SO_LINGER, (char*)&linger , sizeof(linger));


    //HALF-CLOSE
    //SD_SEND : send 막는다.
    //SD_RECEIVE : recv 막는다
    //SD_BOTH :  둘다 막는다.
    //::shutdown(serverSocket, SD_SEND);

    // SO_SNDBUF, SO_RCVBUF : 송수신 버퍼 크기관련 사용.
    int32 sendBufferSize;
    int32 optionLen = sizeof(sendBufferSize);
    ::getsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&sendBufferSize, &optionLen);

    //SO_REUSEADDR : IP주소 및 Port를 재사용
    {
        bool enable = true;
        ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
    }

    //IPPROTO_TCP 
    //TCP_NODELAY : Nagle 알고리즘 작동여부
    //*Nagle : 데이터가 충분히 크면 보내고, 그렇지 않으면 쌓일 때까지 대기.
    //==> 불필요한 packet 생성 감소, But 대기하면서 반응 시간 손해.
    //게임 같은 고반응성 서버에서는 꼭 꺼주는 것이 좋다!!
    {
        bool enable = true;
        ::setsockopt(serverSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable));
    }

    //소켓 리소스 반환
    ::closesocket(serverSocket); 

    //winsock 종료
    ::WSACleanup();
}
