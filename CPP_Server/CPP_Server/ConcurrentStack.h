#pragma once
#include <mutex>
#include <atomic>

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
		Node(const T& value) :  data(value){}

		T data;
		Node* next;
	};

public:

	//새노드를 만들고
	//새노드이 next =  기존 헤드
	//헤드 =  기존 노드
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;	//이 단계에서부터 멀티스레드 환경에서 문제가 생길 수 있음
							//head는 누구나 접근 가능 중간에 헤드값이 바뀔 수 있음.

		while (_head.compare_exchange_weak(node->next, node) == false)
		{
			//head가 node->next일때, head는 node가 된다.
			//실패(false)하면 계속 시도
		}

		_head = node;
	}

	/*헤드의 데이터를 읽고 삭제하는 것이 목표
	 * 1.기존의 헤드의 데이터를 읽는다
	 * 2. head->next를 읽는다.
	 * 3. head =  head->next
	 * 4. 기존 head 데이터 추출 후 반환
	 * 5. 기존 노드 삭제
	 * 
	 * (시작부터 head에 접근하기 때문에 복잡하다, head는 누구나 접근할 수 있기 때문...)
	 */
	bool TryPop(T& value)
	{
		Node* oldHead = _head;
		
		//oldHead가 nullptr일때도 체크를 해야한다.

		while(oldHead && _head.compare_exchange_weak(oldHead,oldHead->next) == false)
		{ }

		if (oldHead == nullptr)
		{
			return false;
		}
		
		value = oldHead->data;

		
		//만약 이렇게 delete를 해버리면, 기존의 oldHead를 참고하던 스레드에 문제가 생긴다!!
		//C#이나 Java처럼 GC가 있으면 (메모리 삭제를 아무도 참고하지 않으면 지워주는것)사실 여기서 끝나는 것...
		//delete oldHead;  

		return true;
	 }
private:
	atomic<Node*> _head;
};