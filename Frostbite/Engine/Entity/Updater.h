#pragma once
#include <Entity/EntityDataDefs.h>
#include <Engine/Core/Types.h>


namespace fb
{

class Updater
{
public:

protected:
	void** __vftable;
private:
	const char* m_name;
	uint m_priority;
	int m_addedToManagerCount;
};

}