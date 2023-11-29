#pragma once
#include <cstdint>
#include <Core/Resource/BaseTypesDefs.h>


namespace fb
{

class SaveInStream;


class EntityInitInfo
{
public:
	void** __vftable;
	SaveInStream* stream;
	uintptr_t userData;
	Realm realm;
	bool autoAddToSpatialQuerymanager;
	bool isSubClass;
	bool isUsed;
};

}