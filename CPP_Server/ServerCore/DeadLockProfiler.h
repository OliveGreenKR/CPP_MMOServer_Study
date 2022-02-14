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
	//노드 번호와, 그 노드가 연결한 노드들(set)
	map<int32, set<int32>>				_lockHistory;

	Mutex _lock;

private:
	//노드가 발견된 순서를 기록하는 벡터
	vector<int32>	_discoveredOrder;
	//노드가 발견된 순서
	int32			_discoveredCount = 0;
	// Dfs(i)가 종료되었는지 여부
	vector<bool>	_finished;
	//부모님 추적
	vector<int32>	_parent;
	
};
