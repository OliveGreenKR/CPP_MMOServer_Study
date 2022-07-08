#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

//TEMP
IocpCore GIocpCore;

IocpCore::IocpCore() {
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); //CP 생성
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore() {
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObject* iocpObject) {
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/reinterpret_cast<ULONG_PTR>(iocpObject), 0);
}

//WorkerThread의 할일 :  일감 검사
bool IocpCore::Dispatch(uint32 timeoutMs) {

	DWORD numOfBytes = 0;
	//반드시 Ref Count를 통한 제어 필요.
	IocpObject* iocpObject = nullptr;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT reinterpret_cast<PULONG_PTR>(&iocpObject),
									OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		//Event로 일감 종류 확인 및 처리
		iocpObject->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		//실패
		int32 errCode = ::WSAGetLastError();
		switch (errCode)
		{
			case WAIT_TIMEOUT:
				return false;
			default:
				//TODO :  logiing
				iocpObject->Dispatch(iocpEvent, numOfBytes);
				break;
		}
	}

	return false;
}
