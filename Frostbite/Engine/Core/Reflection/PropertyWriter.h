#pragma once
#include <Engine/Core/Reflection/PropertyReader.h>


namespace fb
{

class PropertyWriterBase
{
public:
	bool hasOutgoingConnections() const { return m_cache != nullptr; }
	bool hasValue() const { return m_cache && (m_cache->value & CacheData::IsValueWritten) != 0; }

protected:
	CacheData* m_cache;
};

template<typename FieldType>
class PropertyWriter : public PropertyWriterBase
{
public:
	PropertyWriter()
	{
		m_cache = nullptr;
	}

	void operator=(const FieldType& newValue)
	{
		if (!m_cache) { return; }
		m_cache->setValue(&newValue, CacheData::IsValueWritten);
	}

	const FieldType& get() { return *reinterpret_cast<const FieldType*>(m_cache->value & uintptr_t(CacheData::ValuePointerMask)); }

};

}