#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Math/LinearTransform.h>


namespace fb
{

class ComponentCollection
{
public:
	LinearTransform* localTransforms();
	LinearTransform* worldTransforms();
	LinearTransform* worldTransform(u8 idx);
	LinearTransform* localTransform(u8 idx);

private:
	void* owner;
	u8 totalCount;
	u8 offsetCount;
	u8 transformCount;
};

class Component
{
public:
	void setTransform(LinearTransform* localTransform, bool isExternalModification)
	{
		auto native = reinterpret_cast<void(__fastcall*)(void*, LinearTransform*,bool)>(0x1437A8530);
		native(this, localTransform, isExternalModification);
	}

	void ownerTransformChanged()
	{
		auto native = reinterpret_cast<void(__fastcall*)(Component*)>(0x140258270);
		native(this);
	}
};

inline LinearTransform* ComponentCollection::localTransforms()
{
	u8* data = reinterpret_cast<u8*>(this + 1);
	LinearTransform* localTransforms = reinterpret_cast<LinearTransform*>(data + (16 * offsetCount));
	return localTransforms;
}

inline LinearTransform* ComponentCollection::worldTransforms()
{
	u8* data = reinterpret_cast<u8*>(this + 1);
	u8* localTransforms = data + (16 * offsetCount);
	LinearTransform* worldTransforms = reinterpret_cast<LinearTransform*>(localTransforms + sizeof(LinearTransform) * transformCount);
	return worldTransforms;
}

inline LinearTransform* ComponentCollection::worldTransform(u8 idx)
{
	// 0xFF gets the root world transform
	if (idx == 0xFF)
	{
		return &worldTransforms()[0];
	}
	// child transforms need the index + 1
	return &worldTransforms()[idx+1];
}

inline LinearTransform* ComponentCollection::localTransform(u8 idx)
{
	return &localTransforms()[idx];
}


}