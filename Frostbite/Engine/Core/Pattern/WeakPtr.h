#pragma once
#include <cstdint>


namespace fb
{

class WeakToken
{
public:
	void* realptr() const { return m_realptr; }

private:
	void* m_realptr;
	volatile int m_refcount;
};

class WeakPtrBase
{
public:
	const void* get() const
	{
		if (m_token)
		{
			if (uintptr_t realptr = reinterpret_cast<uintptr_t>(m_token->realptr()))
			{
				return reinterpret_cast<void*>(realptr - sizeof(uintptr_t));
			}
		}
		return nullptr;
	}

	bool isValid() const { return m_token && m_token->realptr(); }

protected:
	WeakToken* m_token;
};

template <typename T>
class ConstWeakPtr : public WeakPtrBase
{
public:
	const T* get() const { return static_cast<const T*>(WeakPtrBase::get()); }
};

template <typename T>
class WeakPtr : public ConstWeakPtr<T>
{
public:
	T* get() const { return static_cast<T*>(WeakPtrBase::get()); }
};

}