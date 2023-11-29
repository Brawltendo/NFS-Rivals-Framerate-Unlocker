#pragma once
#include <Engine/Entity/Updater.h>
#include <Engine/Core/Resource/BaseTypesDefs.h>
#include <Engine/Entity/EntityDataDefs.h>


namespace fb
{

class SimpleUpdaterBase : public Updater
{
public:
	SimpleUpdaterBase(Realm realm, UpdatePass updatePass, const char* debugName, uint prio)
	{
		auto native = reinterpret_cast<SimpleUpdaterBase*(__fastcall*)(SimpleUpdaterBase*,Realm,UpdatePass,const char*,uint)>(0x140247920);
		native(this, realm, updatePass, debugName, prio);
	}

protected:
	void flushCommandBuffer()
	{
		auto native = reinterpret_cast<void(__fastcall*)(SimpleUpdaterBase*)>(0x14024B430);
		native(this);
	}

	typedef void Instance;
	struct Instances // eastl::vector<Instance*>
	{
		Instance** mpBegin;
		Instance** mpEnd;
		Instance** mpCapacity;
		void* mAllocator;
	} m_instances;

	enum Action
	{
		Action_Add = 1,
		Action_Remove = -1
	};
	struct Command
	{
		Instance* instance;
		Action action;
	};
	struct CommandBuffer // eastl::fixed_vector<Command, 2>
	{
		Command* mpBegin;
		Command* mpEnd;
		Command* mpCapacity;
		// eastl::fixed_vector_allocator
		struct { void* mOverflowAllocator; void* mpPoolBegin; } mAllocator;
		char mBuffer[sizeof(Command) * 2];
	} m_commandBuffer;

	Realm m_realm;
	UpdatePass m_updatePass;
};
static_assert(sizeof(SimpleUpdaterBase) == 0x88, "Wrong size!");

}