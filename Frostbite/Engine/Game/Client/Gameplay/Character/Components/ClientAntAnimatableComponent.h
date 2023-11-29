#pragma once
#include <cstdint>


namespace fb
{

struct ComponentCollection;
class AntAnimatableComponentData;

class ClientAntAnimatableComponent
{
public:

private:
	void** __vftable;
	ComponentCollection* m_collection;
	AntAnimatableComponentData* m_data;
	uint8_t m_componentIndex;
	uint8_t gap_19[49];
	uint64_t m_handler; // GameAnimatable
};

}