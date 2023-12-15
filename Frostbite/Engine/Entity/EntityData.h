#pragma once
#include <Engine/Core/Resource/BaseTypes.h>
#include <Engine/Core/Math/LinearTransform.h>


namespace fb
{

class GameObjectData : public DataBusPeer
{
public:
	bool getIsGhost() const
	{
		typedef bool(__fastcall* native)(const GameObjectData*);
		return reinterpret_cast<native>(__vftable[2])(this);
	}
};
static_assert(sizeof(GameObjectData) == 0x18, "Wrong size!");

class EntityData : public GameObjectData
{
};

class SpatialEntityData : public EntityData
{
protected:
	LinearTransform m_transform;
};

class ComponentEntityData : public SpatialEntityData
{
protected:
	GameObjectData** m_components;
	u8 m_clientRuntimeComponentCount;
	u8 m_serverRuntimeComponentCount;
	u8 m_clientRuntimeTransformationCount;
	u8 m_serverRuntimeTransformationCount;
};

class GameComponentEntityData : public ComponentEntityData
{
protected:
	bool m_enabled;
};

class DataContainerPolicyAsset : public Asset
{
};

class ComponentData : public GameObjectData
{
protected:
	GameObjectData** m_components;
	LinearTransform m_transform;
	bool m_excluded;
};
static_assert(sizeof(ComponentData) == 0x70, "Wrong size!");

class GameComponentData : public ComponentData
{
};
static_assert(sizeof(GameComponentData) == 0x70, "Wrong size!");

}