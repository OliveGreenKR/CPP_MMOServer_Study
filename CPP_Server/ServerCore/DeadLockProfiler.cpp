#include "pch.h"
#include "DeadLockProfiler.h"

/*------------------------
	DeadLock Prodfiler
--------------------------*/

void DeadLockProfiler::PushLock(const char* lockname)
{
	LockGuard guard(_lock);

	//# ���̵� ã�ų� �߱��Ѵ�.
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

	// # ��� �ִ� ���� �־��ٸ�,
	if (_lockStack.empty() == false)
	{
		// ������ �߰ߵ��� ���� ���̽� ���, ����� ���� �ٽ� Ȯ��
		const int32 previd = _lockStack.top();
		if (lockid != previd)
		{
			set<int32>& history = _lockHistory[previd];
			//ó���߰��� ����(����)
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
	//��ϵ� lock�� ����
	const int32 lockNum = static_cast<int32>(_nameToId.size());
	//�ʱ�ȭ
	_discoveredOrder = vector<int32>(lockNum, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockNum, false);
	_parent = vector<int32>(lockNum, -1);


	for (int32 lockid = 0; lockid < lockNum; lockid++)
		Dfs(lockid);
	//������ �������� �����Ѵ�.
	_discoveredOrder.clear();
	_parent.clear();
	_finished.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	//�湮������ �ִ��� �˻�
	if (_discoveredOrder[here] != -1)
	{
		return;
	}
	_discoveredOrder[here] = _discoveredCount++;

	//���� ���� ���� Ȯ��
	auto findit = _lockHistory.find(here);
	if (findit == _lockHistory.end())
	{
		_finished[here] = true;
		return;
	}
	//��� ������ ������ ��ȸ-> ����Ŭ�� �ִ��� �Ǻ�
	set<int32>& nextSet = findit->second;
	for (int32 there : nextSet)
	{
		//���� �湮������ ���ٸ� �湮
		if (_discoveredOrder[there] == -1)
		{
			_parent[there] = here;
			Dfs(there);
			continue;
		}
		//�湮�� ���� �ִٸ� ������, ������ üũ

		//here�� there���� ���� �߰ߵǾ��� -> there�� here�� �ڼ� ->������
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;

		//�������� �ƴѵ�, ���� �׷쳻�̴�? -> Cycle�� �����ϴ� �����Ⱓ��
		//���� �׷� �� ==  DFS(there)�� ���� ������� ����(Ž����)
		if (_finished[there] == false)
		{
			printf("%s -> %s\n", _idToName[here], _idToName[there]);
			int32 now = here;
			while (true)
			{
				printf("%s -> %s\n", _idToName[_parent[now]], _idToName[now]);
				now = _parent[now];

				if (now == there)//������ �����̹Ƿ� there�� here�� ���� �� �ϳ�
					break;
			}

			CRASH("DEAD_LOCK_DETECTED");
		}
	}
	_finished[here] = true;
}
