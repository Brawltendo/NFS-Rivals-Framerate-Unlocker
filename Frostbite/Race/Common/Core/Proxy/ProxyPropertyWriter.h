#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Reflection/PropertyWriter.h>


namespace fb
{

struct IProxySource;

class ProxyPropertyWriterBase
{
public:

protected:
	void** __vftable;
	IProxySource* m_source;
	PropertyId m_propertyId;
	uint m_index;
};
static_assert(sizeof(ProxyPropertyWriterBase) == 0x18, "Wrong size!");

template<typename FieldType>
class ProxyPropertyWriter : public ProxyPropertyWriterBase
{
public:
	ProxyPropertyWriter();

	void operator=(const FieldType& value);
	__forceinline FieldType get()
	{
		return m_value;
	}

private:
	FieldType m_value;
	PropertyWriter<FieldType> m_writer;
};

}