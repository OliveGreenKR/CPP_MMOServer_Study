#include "pch.h"
#include "AccountManager.h"

#include "UserManager.h"

void AccountManager::ProcessLogin()
{
	//accountlock
	::lock_guard<mutex> guard(_mutex);

	//userlock�� ��Եȴ�
	User* user = UserManager::Instance()->GetUser(100);

	//todo
}
