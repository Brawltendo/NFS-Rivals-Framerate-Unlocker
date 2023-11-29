#pragma once
#include <Engine/Entity/SimpleUpdaterBase.h>


namespace fb
{

class UpdateQueryCollector;


template<class InstanceType>
class SimpleUpdater : protected SimpleUpdaterBase
{
public:
	SimpleUpdater(Realm realm, UpdatePass updatePass, const char* debugName, uint prio)
		: SimpleUpdaterBase(realm, updatePass, debugName, prio)
	{
	}

	void addInstance(InstanceType* e)
	{
		auto native = reinterpret_cast<void(__fastcall*)(SimpleUpdaterBase*, void*)>(0x140249700);
		native(this, (void*)e);
	}

	void removeInstance(InstanceType* e)
	{
		auto native = reinterpret_cast<void(__fastcall*)(SimpleUpdaterBase*, void*)>(0x14024C850);
		native(this, (void*)e);
	}

	struct Instances // eastl::vector<InstanceType*>
	{
		InstanceType** mpBegin;
		InstanceType** mpEnd;
		InstanceType** mpCapacity;
		void* mAllocator;
	};
	const Instances& getInstances() const
	{
		return *reinterpret_cast<const Instances*>(&m_instances);
	}

	void* preQueryUpdate(const UpdateInfo& info, UpdateQueryCollector& collector)
	{
		flushCommandBuffer();
		InstanceType** it = getInstances().mpBegin;
		InstanceType** end = getInstances().mpEnd;
		for (; it != end; ++it)
			(*it)->update(info);
		return nullptr;
	}

	void postQueryUpdate(const UpdateInfo& info, void* preQueryUpdateResult)
	{
		flushCommandBuffer();
		InstanceType** it = getInstances().mpBegin;
		InstanceType** end = getInstances().mpEnd;
		for (; it != end; ++it)
			(*it)->update(info);
	}

};

template<class InstanceType>
class SimpleUpdaterWithAutoDelete : public SimpleUpdater<InstanceType>
{
public:
	SimpleUpdaterWithAutoDelete(Realm realm, UpdatePass updatePass, const char* debugName, uint prio, Updater** ptrToThis)
		: SimpleUpdater<InstanceType>(realm, updatePass, debugName, prio)
		, m_ptrToThis(ptrToThis)
	{
	}

	void dtor(bool d)
	{
		auto baseDtor = reinterpret_cast<void(__fastcall*)(SimpleUpdaterBase*)>(0x140248300);
		auto nativeDelete = reinterpret_cast<void(__fastcall*)(void*)>(0x140116300);
		baseDtor(this);
		if (d)
			nativeDelete(this);
	}

private:
	Updater** m_ptrToThis;
};

}