#pragma once
#include <Engine/Entity/EntityFactory.h>


namespace fb
{

class GameWorld;


class GameEntityCreationInfo : public EntityFactoryParams
{
public:
	GameWorld& gameWorld;
};

}