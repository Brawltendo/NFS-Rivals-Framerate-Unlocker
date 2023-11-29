#include <Race/Client/Weapon/ClientVehicleWeaponEntity.h>
#include <Race/Common/Weapon/VehicleWeaponEntityData.h>

#include <Engine/Entity/UpdateInfo.h>
#include <Engine/Entity/Component.h>
#include <Engine/Game/Client/Core/Entity/ClientGameComponentEntity.h>

#include <memoryutils.h>
#include <safetyhook.hpp>
#include <MinHook.h>


namespace
{

const int fieldhash_InterpolatedTransform = 0xDCAE85AA;

}

namespace fb
{

void ClientVehicleWeaponEntity::applyInterpolation(float interpolationFactor, const LinearTransform& currentTransform)
{
	if (!VecIsTrueXYZ(m_prevPosition == m_currentPosition))
	{
		Vec interpPosition = VecLerp(m_prevPosition, m_currentPosition, interpolationFactor);
		Quat interpOrientation = slerp(m_prevOrientation, m_currentOrientation, interpolationFactor);
		LinearTransform interpT(interpOrientation, interpPosition);
		//if (interpT != currentTransform)
			m_interpolatedTransform = interpT;
	}
}

void ClientVehicleWeaponEntity::frameInterpolationUpdate(const EntityUpdateInfo& info)
{
	auto base_frameInterpolationUpdate = reinterpret_cast<void(__fastcall*)(void*, const EntityUpdateInfo&)>(0x1402BA9C0);

	if (/*m_vehicle.isValid() &&*/ m_interpolatedTransform.hasOutgoingConnections())
	{
		const LinearTransform& currTrans = m_transformOutput.get();

		// interpolate values and expose them to Schematics
		applyInterpolation(info.interpolationFactor, currTrans);
	}
	
	base_frameInterpolationUpdate(this, info);
}

void ClientVehicleWeaponEntity::ApplyPatches()
{
	// increase size of fb::ClientVehicleWeaponEntity from 0x710 to 0x750
	// with this we can add extra fields for interpolation
	{
		uint8_t patch1[] = { 0x50, 0x07 };
		PatchBytes(0x1408FA5AF, patch1, sizeof(patch1));
		uint8_t patch2[] = { 0x50, 0x07 };
		PatchBytes(0x141FB5402, patch2, sizeof(patch2));
		uint8_t patch3[] = { 0x50, 0x07 };
		PatchBytes(0x1408FCD11, patch3, sizeof(patch3));
	}

	// mid function hook into sub_1408F2830 (called inside of ClientVehicleWeaponEntity's ComponentEntityCreator)
	static SafetyHookMid vehicleWeaponEntityPatch1{};
	vehicleWeaponEntityPatch1 = safetyhook::create_mid(reinterpret_cast<void*>(0x1408F2878),
		[](SafetyHookContext& ctx)
	{
		auto EntityBus_internalCreateFieldOverride = reinterpret_cast<PropertyWriterBase * (__fastcall*)(void*, PropertyWriterBase*, const void*, int, uintptr_t, void*)>(0x14023FC00);

		ClientVehicleWeaponEntity* entity = reinterpret_cast<ClientVehicleWeaponEntity*>(ctx.rdi);
		EntityFactoryParams* params = reinterpret_cast<EntityFactoryParams*>(ctx.rsi);

		if (entity)
		{
			// have to do this in here because hooking the constructor crashes the game
			entity->m_prevOrientation = Quat();
			entity->m_currentOrientation = Quat();
			entity->m_prevPosition = vecZero();
			entity->m_currentPosition = vecZero();

			entity->m_interpolatedTransform = PropertyWriter<LinearTransform>();
			LinearTransform t(LinearTransform::IdentityType::Identity);
			EntityBus_internalCreateFieldOverride(params->base.entityBus, &entity->m_interpolatedTransform, params->data, fieldhash_InterpolatedTransform, 0x1419F5640, &t);
		}
	});

	// mid function hook into ClientVehicleWeaponEntity::postPhysicsUpdate
	static SafetyHookMid vehicleWeaponEntityPatch2{};
	vehicleWeaponEntityPatch2 = safetyhook::create_mid(reinterpret_cast<void*>(0x1408FFD95),
		[](SafetyHookContext& ctx)
	{
		ClientVehicleWeaponEntity* thisPtr = reinterpret_cast<ClientVehicleWeaponEntity*>(ctx.rbx);
		EntityUpdateInfo* updateInfo = reinterpret_cast<EntityUpdateInfo*>(ctx.rbp);

		if (thisPtr->m_interpolatedTransform.hasOutgoingConnections())
		{
			const LinearTransform& currTrans = thisPtr->m_transformOutput.get();
			thisPtr->m_prevPosition = thisPtr->m_currentPosition;
			thisPtr->m_currentPosition = currTrans.trans;
			thisPtr->m_prevOrientation = thisPtr->m_currentOrientation;
			thisPtr->m_currentOrientation = Quat(currTrans);

			// interpolate values and expose them to Schematics
			thisPtr->applyInterpolation(updateInfo->interpolationFactor, currTrans);
		}
	});

	// redirect fb::ClientVehicleWeaponEntity::needFrameInterpolationUpdate vfptr
	{
		uint8_t patch[] = { 0x40, 0x98, 0x75, 0x40, 0x01, 0x00, 0x00, 0x00 };
		PatchBytes(0x141571470, patch, sizeof(patch));
	}

	// redirect fb::ClientVehicleWeaponEntity::onFrameInterpolationUpdate vfptr to our patched version
	{
		auto f = &ClientVehicleWeaponEntity::frameInterpolationUpdate;
		union { void* ptr; uint8_t arr[8]; } patch{};
		patch.ptr = *(void**)&f;
		PatchBytes(0x141571478, patch.arr, sizeof(patch));
	}
}

}