#pragma once

#include <mutex>

class Account
{
	//data...
};
class AccountManager
{
public:
	static AccountManager* Instance()
	{
		static AccountManager instance;
		return &instance;
	}

	Account* GetAccount(int32 id)
	{
		::lock_guard<mutex> guard(_mutex);

		//데이터 접근

		return nullptr;
	}

	void ProcessLogin();
private:
	mutex _mutex; //class의 자물쇠
	//map<int32, Account*> _accounts;
};

