#pragma once
#include <cstdint>
#include <Engine/Core/Types.h>


namespace fb
{

struct CacheData
{
	void callListeners(bool isFirstTimeWritten)
	{
		auto native = reinterpret_cast<void(__fastcall*)(CacheData*, bool)>(0x140150CB0);
		native(this, isFirstTimeWritten);
	}

	// have to template this because we don't have Frostbite type info ready at compile time
	template<class T>
	void setValue(const T* sourceValue, uint valueFlags);

	enum { IsValueWritten    = 1 << 0 };
	enum { ValuePointerMask  = -4 };
	enum { IsValueSet        = 1 << 1 };

	uintptr_t value;
	uintptr_t listener;
	union
	{
		PropertyId listenerName;
		uint listenerCount;
	};
	uintptr_t typeInfo;
};

class PropertyReaderBase
{
public:
	bool hasConnection() const { return m_cache != nullptr; }
	bool hasConnectionValue() const { return (m_cache->value & CacheData::IsValueWritten) != 0; }

protected:
	const void* get()
	{
		if (!m_cache || !m_cache->value || (m_cache->value & CacheData::IsValueWritten) == 0)
		{
			return m_defaultValue;
		}
		
		return reinterpret_cast<const void*>(m_cache->value & uintptr_t(CacheData::ValuePointerMask));
	}

	CacheData* m_cache;
	const void* m_defaultValue;
};

template<typename FieldType>
class PropertyReader : public PropertyReaderBase
{
public:
	const FieldType& get() { return *(const FieldType*)PropertyReaderBase::get(); }
};


template<class T>
void CacheData::setValue(const T* sourceValue, uint valueFlags)
{
	T* rawPtr = reinterpret_cast<T*>(value & uintptr_t(ValuePointerMask));
	value |= valueFlags | IsValueSet;
	*rawPtr = *sourceValue;

	bool isFirstTimeWritten = (value & IsValueWritten) == 0;
	callListeners(isFirstTimeWritten);
}

}