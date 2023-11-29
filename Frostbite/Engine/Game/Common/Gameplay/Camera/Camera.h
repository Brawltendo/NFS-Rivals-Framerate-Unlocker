#pragma once
#include "Core/Resource/BaseTypesDefs.h"
#include "Core/Math/LinearTransform.h"
#include "Core/Math/Vec2.h"
#include "CameraEffects/FovEffect.h"


namespace fb
{

class MemoryArena;
class IPhysicsManager;


struct CameraUpdateInfo
{
	float deltaTime;
	float interpolationFactor;
	float filteredDeltaTime;
	Realm realm;
	bool isDebugUpdate;
};

class Camera
{
public:

	struct CreationInfo
	{
		MemoryArena& memoryArena;
		IPhysicsManager* physicsManager;
		void* userData;
	};

	const LinearTransform& getTransform() { return m_transform; }

	enum CameraType
	{
		CameraType_Base,
		CameraType_Target,
		CameraType_Chase,
		CameraType_Free,
		CameraType_Vehicle,
		CameraType_Fps,
		CameraType_Other
	};

	void setTransform(const LinearTransform& t)
	{
		auto native = reinterpret_cast<void(__fastcall*)(Camera*, const LinearTransform&)>(0x1403C4600);
		native(this, t);
	}
	void setViewportOffset(Vec2Ref offset)
	{
		// MSVC optimized this function to just pass in a 64 bit value instead of a Vec2 ref
		// so just make a type pun union
		union { Vec2 v; uint64_t i; } conv{};
		conv.v = offset;
		auto native = reinterpret_cast<void(__fastcall*)(Camera*, uint64_t)>(0x1403C46D0);
		native(this, conv.i);
	}

protected:
	void** __vftable;

private:
	LinearTransform m_transform;
	Vec2 m_viewportOffset;
	float m_screenExposureAreaOffset;

protected:
	FovEffect m_fovInEffect;
	FovEffect m_fovOutEffect;
	const class CameraData* m_data;
	float m_fov;
	float m_dofFocusDistance;
};
static_assert(sizeof(Camera) == 0xC0, "Wrong size!");

}