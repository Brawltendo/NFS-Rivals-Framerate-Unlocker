#pragma once
#include <Core/Types.h>
#include <Race/Common/Core/Proxy/ProxyPropertyWriter.h>


namespace fb
{

class VehicleWeaponUpgradable
{
	uint m_upgradeLevel;
	ProxyPropertyWriter<int> m_upgradeLevelOutput;
};

}