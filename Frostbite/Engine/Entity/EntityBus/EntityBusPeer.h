#pragma once
#include <cstdint>
#include <Engine/Core/Resource/BaseTypesDefs.h>
#include <Engine/Entity/EntityDataDefs.h>


namespace fb
{

class EntityBus;


class EntityBusPeerCreationInfo
{
public:
	EntityBus* entityBus;
	uintptr_t userData;
	Realm realm;
	SubRealm subRealm;
	EntityCreatorType creatorType;
};

}