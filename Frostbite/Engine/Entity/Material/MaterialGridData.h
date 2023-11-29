#pragma once
#include <Engine/Core/Types.h>


namespace fb
{

struct MaterialDecl
{
public:
	u32 getPacked() const { return m_packed; }

protected:
	u32 m_packed;
};

}