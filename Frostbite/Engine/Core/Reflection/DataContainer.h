#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Reflection/Reflection.h>


namespace fb
{

class DataContainer : public ITypedObject
{
protected:
	union
	{
		struct
		{
			u16 m_flags;
			u16 m_refCnt;
		};
		u32 m_refCountAndId;
	};
};

}