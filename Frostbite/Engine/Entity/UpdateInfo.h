#pragma once
#include <Engine/Core/Types.h>
#include <Engine/Entity/EntityDataDefs.h>


namespace fb
{

struct UpdateInfo
{
	void* queryResult;
	float deltaTime;
	float interpolationFactor;
	uint ticks;
	UpdatePass updatePass;
	uint currentTick;
};

struct EntityUpdateInfo : UpdateInfo
{
};

}