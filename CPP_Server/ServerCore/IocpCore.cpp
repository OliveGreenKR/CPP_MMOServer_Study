#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"

IocpCore::IocpCore() {
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0); //CP 생성
	ASSERT_CRASH(_iocpHandle != INVALID_HANDLE_VALUE);
}

IocpCore::~IocpCore() {
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject) {
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*key*/0, 0); //reinterpret_cast<ULONG_PTR>(iocpObject)였었음
}

//WorkerThread의 할일 :  일감 검사
bool IocpCore::Dispatch(uint32 timeoutMs) {

	DWORD numOfBytes = 0;
	//반드시 Ref Count를 통한 제어 필요.
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (::GetQueuedCompletionStatus(_iocpHandle, OUT & numOfBytes, OUT &key,
									OUT reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef iocpObject = iocpEvent->owner;
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
				IocpObjectRef iocpObject = iocpEvent->owner;
				iocpObject->Dispatch(iocpEvent, numOfBytes);
				break;
		}
	}

	return false;
}
