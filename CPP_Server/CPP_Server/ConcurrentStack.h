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
		Node(const T& value) :  data(value){}

		T data;
		Node* next;
	};

public:

	//����带 �����
	//������� next =  ���� ���
	//��� =  ���� ���
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;	//�� �ܰ迡������ ��Ƽ������ ȯ�濡�� ������ ���� �� ����
							//head�� ������ ���� ���� �߰��� ��尪�� �ٲ� �� ����.

		while (_head.compare_exchange_weak(node->next, node) == false)
		{
			//head�� node->next�϶�, head�� node�� �ȴ�.
			//����(false)�ϸ� ��� �õ�
		}

		_head = node;
	}

	/*����� �����͸� �а� �����ϴ� ���� ��ǥ
	 * 1.������ ����� �����͸� �д´�
	 * 2. head->next�� �д´�.
	 * 3. head =  head->next
	 * 4. ���� head ������ ���� �� ��ȯ
	 * 5. ���� ��� ����
	 * 
	 * (���ۺ��� head�� �����ϱ� ������ �����ϴ�, head�� ������ ������ �� �ֱ� ����...)
	 */
	bool TryPop(T& value)
	{
		Node* oldHead = _head;
		
		//oldHead�� nullptr�϶��� üũ�� �ؾ��Ѵ�.

		while(oldHead && _head.compare_exchange_weak(oldHead,oldHead->next) == false)
		{ }

		if (oldHead == nullptr)
		{
			return false;
		}
		
		value = oldHead->data;

		
		//���� �̷��� delete�� �ع�����, ������ oldHead�� �����ϴ� �����忡 ������ �����!!
		//C#�̳� Javaó�� GC�� ������ (�޸� ������ �ƹ��� �������� ������ �����ִ°�)��� ���⼭ ������ ��...
		//delete oldHead;  

		return true;
	 }
private:
	atomic<Node*> _head;
};