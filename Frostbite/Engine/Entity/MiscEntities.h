#pragma once
#include <Engine/Core/Reflection/PropertyWriter.h>
#include <Engine/Core/Math/LinearTransform.h>


namespace fb
{

class EntityBus;
class TransformMultiplierEntityData;
class DefaultEntityCreator;
class EntityFactoryParams;
class EntityCreationInfo;
class EntityInitInfo;
struct UpdateInfo;
class EntityDeinitInfo;
class TransformMultiplierEntityUpdater;


class TransformMultiplierEntity
{
public:
	TransformMultiplierEntity(const EntityCreationInfo& info, const TransformMultiplierEntityData* data)
	{
		auto base_ctor = reinterpret_cast<void*(__fastcall*)(void*, const EntityCreationInfo&, const TransformMultiplierEntityData*)>(0x14020F9A0);
		base_ctor(this, info, data);
		__vftable = (void**)0x141412CE0;
		m_outCache = LinearTransform(LinearTransform::IdentityType::Identity);
		m_prevOrientation = Quat();
		m_currentOrientation = Quat();
		m_prevPosition = vecZero();
		m_currentPosition = vecZero();
	}

	static void ApplyPatches();

	static TransformMultiplierEntity* EntityCreator_create_Client(DefaultEntityCreator* creator, EntityFactoryParams& params);
	static TransformMultiplierEntity* EntityCreator_create_Server(DefaultEntityCreator* creator, EntityFactoryParams& params);

	void onCreate(const EntityCreationInfo& info);
	void onInit(const EntityInitInfo& info);
	void onDeinit(const EntityDeinitInfo& info);
	void writeOut();
	void update(const UpdateInfo& info);

	typedef TransformMultiplierEntity*(__fastcall* t_EntityCreator_create_Client)(DefaultEntityCreator*, EntityFactoryParams&);
	static t_EntityCreator_create_Client o_EntityCreator_create_Client;

	typedef TransformMultiplierEntity*(__fastcall* t_EntityCreator_create_Server)(DefaultEntityCreator*, EntityFactoryParams&);
	static t_EntityCreator_create_Server o_EntityCreator_create_Server;

	typedef void(__fastcall* t_onCreate)(TransformMultiplierEntity*, const EntityCreationInfo&);
	static t_onCreate o_onCreate;

	typedef void(__fastcall* t_onInit)(TransformMultiplierEntity*, const EntityInitInfo&);
	static t_onInit o_onInit;

	typedef void(__fastcall* t_writeOut)(TransformMultiplierEntity*);
	static t_writeOut o_writeOut;

protected:
	void** __vftable;
private:
	char gap_8[24];
	EntityBus* m_entityBus;
	TransformMultiplierEntityData* m_data;
	PropertyReader<LinearTransform> m_in1;
	PropertyReader<LinearTransform> m_in2;
	PropertyWriter<LinearTransform> m_out;

	// additional fields
	PropertyWriter<LinearTransform> m_interpolatedOut;
	LinearTransform m_outCache;
	Quat m_prevOrientation;
	Quat m_currentOrientation;
	Vec3 m_prevPosition;
	Vec3 m_currentPosition;

	static TransformMultiplierEntityUpdater* s_updater;
};
static_assert(sizeof(TransformMultiplierEntity) == 0xE0, "Wrong size!");

}