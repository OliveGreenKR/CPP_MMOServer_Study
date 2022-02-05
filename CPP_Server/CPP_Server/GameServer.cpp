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

void Producer()
{
	while (true)
	{
		{
			//lcok_guard�ʹ� �ٸ��� lock�� �����ñ⸦ ��������
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		 
		//handle�� signal ���·� �ٲ��ش�
		::SetEvent(handle);

		this_thread::sleep_for(100ms);
	}
}

void Consumer()
{
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE); //���Ѵ��
		//>>handle��  signal�� �ɶ����� ���� ���
		//Auto Reset�̱� ������  signal�� �Ȱ��� Ȯ���� ����, �Ʒ��� �ڵ�� ������ �ǰ� �Ǹ鼭
		// �ڵ����� handle�� ���´� �ٽ� non-signal�� ������.

		/*::ResetEvent(handle) << �������� ���� ��, �̷��� ���� �ݾ��־���Ѵ�.*/

		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << endl;
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
