#pragma once


namespace fb
{

struct Vec3;

class ClientRaceWheelComponentSimulation
{
public:
	Vec3& contactNormal()
	{
		typedef Vec3& (__fastcall* native)(ClientRaceWheelComponentSimulation*);
		return reinterpret_cast<native>(__vftable[7])(this);
	}

	Vec3& contactPosition()
	{
		typedef Vec3&(__fastcall* native)(ClientRaceWheelComponentSimulation*);
		return reinterpret_cast<native>(__vftable[8])(this);
	}

protected:
	void** __vftable;
};

}