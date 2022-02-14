#include "pch.h"
#include "DeadLockProfiler.h"

/*------------------------
	DeadLock Prodfiler
--------------------------*/

void DeadLockProfiler::PushLock(const char* lockname)
{
	LockGuard guard(_lock);

	//# 아이디를 찾거나 발급한다.
	int32 lockid = 0;
	auto findit = _nameToId.find(lockname);
	if (findit == _nameToId.end())
	{
		lockid = static_cast<int32>(_nameToId.size());
		_nameToId[lockname] = lockid;
		_idToName[lockid] = lockname;
	}
	else
	{
		lockid = findit->second;
	}

	// # 잡고 있는 락이 있었다면,
	if (_lockStack.empty() == false)
	{
		// 기존에 발견되지 않은 케이스 라면, 데드락 여부 다시 확인
		const int32 previd = _lockStack.top();
		if (lockid != previd)
		{
			set<int32>& history = _lockHistory[previd];
			//처음발견한 간선(연결)
			if (history.find(lockid) == history.end())
			{
				history.insert(lockid);
				CheckCycle();
			}
		}

	}
	_lockStack.push(lockid);
}

void DeadLockProfiler::PopLock(const char* lockname)
{
	LockGuard guard(_lock);

	if (_lockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockid = _nameToId[lockname];
	if (_lockStack.top() != lockid)
	{
		CRASH("INVALID_UNLOCK");
	}

	_lockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	//등록된 lock의 개수
	const int32 lockNum = static_cast<int32>(_nameToId.size());
	//초기화
	_discoveredOrder = vector<int32>(lockNum, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockNum, false);
	_parent = vector<int32>(lockNum, -1);


	for (int32 lockid = 0; lockid < lockNum; lockid++)
		Dfs(lockid);
	//연산이 끝났으면 정리한다.
	_discoveredOrder.clear();
	_parent.clear();
	_finished.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	//방문된적이 있는지 검사
	if (_discoveredOrder[here] != -1)
	{
		return;
	}
	_discoveredOrder[here] = _discoveredCount++;

	//인접 정점 존재 확인
	auto findit = _lockHistory.find(here);
	if (findit == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}
	//모든 인접한 정점을 순회-> 싸이클이 있는지 판별
	set<int32>& nextSet = findit->second;
	for (int32 there : nextSet)
	{
		//아직 방문한적이 없다면 방문
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		//방문한 적이 있다면 순방향, 역방향 체크

		//here가 there보다 먼저 발견되었다 -> there는 here의 자손 ->순방향
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		//순방향이 아닌데, 같은 그룹내이다? -> Cycle을 유발하는 역방향간선
		//같은 그룹 내 ==  DFS(there)가 아직 종료되지 않음(탐색중)
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);
			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];

				if (now == there)//역방향 간선이므로 there는 here의 조상 중 하나
					break;
			}

			CRASH("DEAD_LOCK_DETECTED");
		}
	}
	_finished[here] = true;
}
