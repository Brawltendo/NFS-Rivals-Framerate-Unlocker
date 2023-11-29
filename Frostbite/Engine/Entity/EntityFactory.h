#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Core/Math/LinearTransform.h>
#include <Engine/Entity/EntityBusDataDefs.h>
#include <Engine/Entity/EntityBus/EntityBusPeer.h>


namespace fb
{

class SaveInStream;
class EntityBusPeer;
class Ghost;
class ComponentCollection;
class SubLevel;
class GameObjectData;
class SaveInStreamContext;


class EntityCreationInfo : public EntityBusPeerCreationInfo
{
public:
	enum RenderFlags
	{
		CastSunShadowFlag      = 1 << 0,
		CastReflectionFlag     = 1 << 1,
		CastDynamicEnvmapFlag  = 1 << 2,
		CastRVMFlag            = 1 << 3,
	};

	LinearTransform transform;
	SaveInStream* stream;
	EntityBusPeer* parent;
	Ghost* ghost;
	u32 objectVariationNameHash;
	u32 renderFlags;
	RadiosityTypeOverride radiosityTypeOverride;
	bool rootOnCreateIsCalled;
	bool isEntityOwner;
	ComponentCollection* collection;
	u8 collectionCount;
	u8 transformCount;
	bool isAutoLoaded;
	SubLevel* subLevel;
};

class EntityFactoryParams
{
protected:
	void** __vftable;
public:
	struct CreatedEntities
	{
		class // eastl::segmented_vector
		{
			void* mAllocator;
			void* mFirstSegment;
			void* mLastSegment;
			size_t mSegmentCount;
		} initPass[2];
	};

	EntityCreationInfo base;
	const GameObjectData* data;
	SaveInStreamContext* loadContext;
	bool skipNetworkId;
	bool wantsGhost;
	CreatedEntities* createdEntities;
};

}