#pragma once


namespace fb
{

struct LinearTransform;

class ClientRaceVehicleComponentSimulation
{
public:
	void worldTransform(const LinearTransform& transform)
	{
		typedef void(__fastcall* native)(ClientRaceVehicleComponentSimulation*, const LinearTransform&);
		reinterpret_cast<native>(__vftable[3])(this, transform);
	}

protected:
	void** __vftable;
};

}