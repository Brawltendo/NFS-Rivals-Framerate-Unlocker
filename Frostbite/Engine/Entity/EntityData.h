#pragma once
#include <Engine/Core/Resource/BaseTypes.h>


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

}