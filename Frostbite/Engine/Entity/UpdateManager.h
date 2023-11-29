#pragma once
#include <Entity/EntityDataDefs.h>


namespace fb
{

class UpdateManagerImpl;
class Updater;
class AtomicLinearAllocator;


class UpdateManager
{
public:
	void addUpdater(Updater* updater)
	{
		auto native = reinterpret_cast<void(__fastcall*)(UpdateManager*, Updater*)>(0x140249750);
		native(this, updater);
	}

	void removeUpdater(Updater* updater)
	{
		auto native = reinterpret_cast<void(__fastcall*)(UpdateManager*, Updater*)>(0x14024DAE0);
		native(this, updater);
	}

private:
	UpdateManagerImpl* m_impl;
};

struct UpdateManagers
{
	UpdateManager manager[UpdatePass_Count];
	AtomicLinearAllocator* m_realmAllocator;
};

extern UpdateManagers* g_updateManagers;

}