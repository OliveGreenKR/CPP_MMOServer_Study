#pragma once

#include<stack>
#include<map>
#include<vector>

/*------------------------
	DeadLock Prodfiler
--------------------------*/

class DeadLockProfiler
{
public:
	void PushLock(const char* lockname);
	void PopLock(const char* lockname);
	void CheckCycle();
private:
	void Dfs(int32 idx);

private:
	unordered_map<const char*, int32>	_nameToId;
	unordered_map<int32, const char*>	_idToName;
	stack<int32>						_lockStack;
	//��� ��ȣ��, �� ��尡 ������ ����(set)
	map<int32, set<int32>>				_lockHistory;

	Mutex _lock;

private:
	//��尡 �߰ߵ� ������ ����ϴ� ����
	vector<int32>	_discoveredOrder;
	//��尡 �߰ߵ� ����
	int32			_discoveredCount = 0;
	// Dfs(i)�� ����Ǿ����� ����
	vector<bool>	_finished;
	//�θ�� ����
	vector<int32>	_parent;
	
};
