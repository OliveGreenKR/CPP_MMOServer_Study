#include "pch.h"
#include "UserManager.h"
#include "AccountManager.h"

void UserManager::ProcessSave()
{
	//accountlock�� ����
	Account* account = AccountManager::Instance()->GetAccount(100);

	//userLock
	lock_guard<mutex> guard(_mutex);

	//todo
}
