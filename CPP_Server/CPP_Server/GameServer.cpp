#include "pch.h"
#include <iostream>
using namespace std;
#include "CorePch.h"

#include <thread>
#include<atomic>
#include <mutex>
#include <Windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

//����) CV�� User_Level Object�̴�.
condition_variable cv;

void Producer()
{
	while (true)
	{
		// 1) lock�� ���
		// 2) ���� ���� ���� ����
		// 3) Lock ����
		// 4) ���Ǻ��� ���� �ٸ� �����忡 ����
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		 
		////handle�� signal ���·� �ٲ��ش�
		//::SetEvent(handle);
		cv.notify_one();

		/*this_thread::sleep_for(100ms);*/
	}
}

void Consumer()
{
	while (true)
	{
		//::WaitForSingleObject(handle, INFINITE); //���Ѵ��
		
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; });

		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << q.size() << endl;
		}
	}
}

int main()
{
	//HANDLE ������ ������ ������������, Event�� ID�� ���� ��Ȱ�� �Ѵ�.
	handle = ::CreateEvent(NULL/*���ȼӼ�*/,
							FALSE /*���� ���¿��� (false => Auto)*/,
							FALSE/*�ʱ�ȭ����*/,
							NULL/*�̸�*/);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
