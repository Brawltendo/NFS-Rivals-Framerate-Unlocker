#include <Race/Common/Core/Proxy/ProxyPropertyWriter.h>
#include <Core/Math/LinearTransform.h>

namespace fb
{

//
// need these to be specialized so we can use the native game functions
//

template<>
void ProxyPropertyWriter<float>::operator=(const float& value)
{
	auto native = reinterpret_cast<void(__fastcall*)(ProxyPropertyWriter<float>*, const float&)>(0x1407B7270);
	native(this, value);
}

template<>
void ProxyPropertyWriter<LinearTransform>::operator=(const LinearTransform& value)
{
	auto native = reinterpret_cast<void(__fastcall*)(ProxyPropertyWriter<LinearTransform>*, const LinearTransform&)>(0x14081A610);
	native(this, value);
}


//
// need these to be specialized so we can use the native vtables
//

template<>
ProxyPropertyWriter<float>::ProxyPropertyWriter()

{
	__vftable = reinterpret_cast<void**>(0x14152E268);
	m_source = nullptr;
	m_propertyId = 0;
	m_index = 0;
	m_writer = PropertyWriter<float>();
}

template<>
ProxyPropertyWriter<LinearTransform>::ProxyPropertyWriter()
	
{
	__vftable = reinterpret_cast<void**>(0x141540490);
	m_source = nullptr;
	m_propertyId = 0;
	m_index = 0;
	m_writer = PropertyWriter<LinearTransform>();
}

}