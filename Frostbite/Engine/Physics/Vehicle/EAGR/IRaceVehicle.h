#pragma once


namespace fb
{

struct LinearTransform;


class IRaceVehicle
{
public:
	float brake()
	{
		typedef float(__fastcall* native)(IRaceVehicle*);
		return reinterpret_cast<native>(__vftable[25])(this);
	}

	float rpm()
	{
		typedef float(__fastcall* native)(IRaceVehicle*);
		return reinterpret_cast<native>(__vftable[175])(this);
	}

	float getForwardSpeed()
	{
		typedef float(__fastcall* native)(IRaceVehicle*);
		return reinterpret_cast<native>(__vftable[254])(this);
	}

	LinearTransform& localGroundTransform()
	{
		char* bytes = reinterpret_cast<char*>(this);
		return *reinterpret_cast<LinearTransform*>(bytes + 0x1870);
	}

protected:
	void** __vftable;
};

}