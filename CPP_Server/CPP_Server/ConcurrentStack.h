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
		//일반적인 싱글스레드에서 처럼 
		//empty를 확인하고 top, pop진행
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
			//_head가 노드->넥스트랑 같을 때, _head가 node로 바뀌게 된다
		{ }

	}

	shared_ptr<T> TryPop()
	{
		shared_ptr<Node> oldHead = std::atomic_load(&_head);
		//그냥 불러오면 안되고 atomic하게 불러와야함..-> shared_ptr은 자체적으로 카운팅을 하기 때문이다.

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
			//이어붙이고 처음을 _pendingList로 지정해주는것을 atomic하게 해주기 위함 
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
		//popcount 체크
		if (_popCount == 1)
		{
			//나 혼자다->delete OldHead 가능

			//삭제가 가능하기 때문에 , 삭제예약된 다른 데이터들도 삭제 해봐야한다.
			Node* node = _pendingList.exchange(nullptr);
			// 기존의 pendingList는 Nullptr이 되고, node가 받아온 형태

			if (--_popCount == 0)//_popcount는 atomic클래스로 묶여서 연산이 아토믹하게 이뤄진다.
			{
				//끼어든 아이가 없으니 삭제를 진행해도 된다
				//이 이후에는 끼어들어도 데이터는 분리해 두었으니 상관이 없다.
				DeleteNodes(node);
			}
			else if (node)
			{
				//누가 끼어들었으니 다시 가져다 놓자
				ChainPendingNode(node);
			}
			//데이터 삭제
			delete oldHead;
		}
		else
		{
			//누가있네? ->pendinglist에 추가
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
	atomic<Node*> _pendingList; //삭제되어야할 노드들의 첫번째 노드 (나머지는 타고타고가서 삭제가능)  
#endif // POPCOUNT

};