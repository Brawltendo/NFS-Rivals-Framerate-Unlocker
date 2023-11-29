#pragma once
#include <Engine/Game/Common/Core/Entity/GameComponentEntity.h>
#include <Core/Types.h>


namespace fb
{

class EntityBusData;
class ClientDestructionManager;


class ClientEntityCreationInfo : public GameEntityCreationInfo
{
public:
	const EntityBusData* entityBusData;
	ClientDestructionManager* destructionManager;
	uint remoteGhostId;
};

}