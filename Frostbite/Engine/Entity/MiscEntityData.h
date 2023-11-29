#pragma once
#include <Engine/Entity/EntityData.h>
#include <Engine/Core/Resource/BaseTypesDefs.h>
#include <Engine/Core/Math/LinearTransform.h>


namespace fb
{

class TransformMultiplierEntityData : public EntityData
{
public:
	LinearTransform getIn1() { return m_in1; }
	LinearTransform getIn2() { return m_in2; }

protected:
	Realm m_realm;
	LinearTransform m_in1;
	LinearTransform m_in2;
};

}