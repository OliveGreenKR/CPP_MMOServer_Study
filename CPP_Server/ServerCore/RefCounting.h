#pragma once

/*--------------------
	RefCountable	
--------------------*/

class RefCountable
{
public:
	RefCountable() : _refCount(1) {}
	virtual ~RefCountable() {}

	int32 GetRefCount() { return _refCount; }

	int32 AddRef() { return ++_refCount; }//������� ���� int32��  ��ȯ�ϰ� �Ѵ�
	int32 ReleaseRef() 
	{
		int32 refCount = --_refCount;
		if (_refCount == 0)
			delete this;

		return refCount;
	}


protected:
	atomic<int32> _refCount;
};

/*--------------------
	SharedPtr
--------------------*/

template <typename T>
class TSharedPtr
{
public:
	TSharedPtr() {}
	TSharedPtr(T* ptr) { Set(ptr); }

	// ����
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }

	//�̵�
	TSharedPtr(TSharedPtr&& rhs)
	{
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
	}
	//��� ���� ����
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }
public:

	//���� ������
	TSharedPtr& operator= (const TSharedPtr& rhs)
	{
		if (rhs._ptr != _ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}
	//�̵� ������
	TSharedPtr& operator=(TSharedPtr&& rhs) noexcept
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}
	// �� ������
	bool		operator==(TSharedPtr& rhs) { return _ptr == rhs._ptr; }
	bool		operator==(T* rhs) { return _ptr == rhs; }
	bool		operator!=(TSharedPtr& rhs) { return _ptr != rhs._ptr; }
	bool		operator!=(T* rhs) { return _ptr != rhs; }
	bool		operator<(const TSharedPtr& rhs) { return _ptr < rhs._ptr; }
	//�����Ϳ���
	T&			operator*() { return *_ptr; }
	const T&	operator*() const { return *_ptr; }
				operator T* () const { return _ptr; } //ĳ����?
	T*			operator ->() { return _ptr; }
	const T*	operator->() const { return _ptr; }

	bool IsNull() { return _ptr == nullptr; }

private:
	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}
private:
	T* _ptr = nullptr; //RefCounting�� �����Ǵ� �����Ϳ����Ѵ�
};
