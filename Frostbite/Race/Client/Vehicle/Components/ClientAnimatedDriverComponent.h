#pragma once


namespace fb
{

struct EntityUpdateInfo;
class IRaceVehicle;

class ClientAnimatedDriverComponent
{
public:
	static void ApplyPatches();

	void updateVehicleRuntimeParameters()
	{
		auto native = reinterpret_cast<void(__fastcall*)(ClientAnimatedDriverComponent*)>(0x140872630);
		native(this);
	}

	IRaceVehicle* getRaceVehicle()
	{
		char* bytes = reinterpret_cast<char*>(this);
		return *reinterpret_cast<IRaceVehicle**>(bytes + 0x248);
	}

	void onFrameInterpolationUpdate(const EntityUpdateInfo& updateInfo);

};

}