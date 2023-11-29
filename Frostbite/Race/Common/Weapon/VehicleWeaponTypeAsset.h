#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Entity/EntityData.h>
#include <Engine/Entity/Material/MaterialGridData.h>


namespace fb
{

class VehicleWeaponTypeAsset : DataContainerPolicyAsset
{
	MaterialDecl Material;
	uint WeaponId;
};

}
