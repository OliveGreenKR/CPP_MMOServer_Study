#pragma once
#include <mutex>

class User
{
	//data
};

class UserManager
{
public:
	static UserManager* Instance()
	{
		static UserManager instance;
		return &instance;
	}

	User* GetUser(int32 id)
	{
		lock_guard<mutex> guard(_mutex);
		//데이터 접근
		return nullptr;
	}

	void ProcessSave();

private:
	mutex _mutex;
};

