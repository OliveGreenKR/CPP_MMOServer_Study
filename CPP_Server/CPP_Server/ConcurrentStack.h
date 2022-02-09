#pragma once
#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack()
	{}
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack& other) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;
		//�Ϲ����� �̱۽����忡�� ó�� 
		//empty�� Ȯ���ϰ� top, pop����
		value = ::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this]() { return _stack.empty() == false; });

		value = ::move(_stack.top());
		_stack.pop();
	}

	bool Empty()
	{
		lock_guard<mutex> lock(_mutex);
		return _stack.empty();

	}
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value)), next(nullptr) 
		{}
		shared_ptr<T> data;
		shared_ptr<Node> next;

#ifdef POPCOUNT
		Node(const T& value) : data(value), next(nullptr)
		{}

		T data;
		Node* next;
#endif // POPCOUNT
	};

public:

	void Push(const T& value)
	{
		shared_ptr<Node> node = make_shared<Node>(value);

		node->next = std::atomic_load(&_head);
		while(std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
			//_head�� ���->�ؽ�Ʈ�� ���� ��, _head�� node�� �ٲ�� �ȴ�
		{ }

	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&_head);
		//�׳� �ҷ����� �ȵǰ� atomic�ϰ� �ҷ��;���..-> shared_ptr�� ��ü������ ī������ �ϱ� �����̴�.

		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
		{ }

		if (oldHead == nullptr)
			return shared_ptr<T>();
		
		return oldHead->data;

	}


#ifdef POPCOUNT
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		while (_head.compare_exchange_weak(node->next, node) == false)
		{

		}

		_head = node;
	}

	bool TryPop(T& value)
	{
		Node* oldHead = _head;
		++_popCount;
		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
		}

		if (oldHead == nullptr)
		{
			--_popCount;
			return false;
		}

		value = oldHead->data;

		//delete oldHead;  
		TryDelete(oldHead);
		return true;
	}
	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

	void ChainPendingList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{
			//�̾���̰� ó���� _pendingList�� �������ִ°��� atomic�ϰ� ���ֱ� ���� 
		}

	}

	void ChainPendingList(Node* node)
	{
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingList(node, node);
	}

	void TryDelete(Node* oldHead)
	{
		//popcount üũ
		if (_popCount == 1)
		{
			//�� ȥ�ڴ�->delete OldHead ����

			//������ �����ϱ� ������ , ��������� �ٸ� �����͵鵵 ���� �غ����Ѵ�.
			Node* node = _pendingList.exchange(nullptr);
			// ������ pendingList�� Nullptr�� �ǰ�, node�� �޾ƿ� ����

			if (--_popCount == 0)//_popcount�� atomicŬ������ ������ ������ ������ϰ� �̷�����.
			{
				//����� ���̰� ������ ������ �����ص� �ȴ�
				//�� ���Ŀ��� ����� �����ʹ� �и��� �ξ����� ����� ����.
				DeleteNodes(node);
			}
			else if (node)
			{
				//���� ���������� �ٽ� ������ ����
				ChainPendingNode(node);
			}
			//������ ����
			delete oldHead;
		}
		else
		{
			//�����ֳ�? ->pendinglist�� �߰�
			ChainPendingNode(oldHead);
			--_popCount;
		}
	}
#endif // POPCOUNT
private:
	shared_ptr<Node> _head;

#ifdef POPCOUNT
	atomic<Node*> _head;
	atomic<uint32> _popCount = 0;
	atomic<Node*> _pendingList; //�����Ǿ���� ������ ù��° ��� (�������� Ÿ��Ÿ���� ��������)  
#endif // POPCOUNT

};