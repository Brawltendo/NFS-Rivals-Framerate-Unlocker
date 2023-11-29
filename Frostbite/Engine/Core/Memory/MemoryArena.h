#pragma once
#include <Engine/Core/Types.h>


namespace fb
{

class MemoryArena
{
public:
	void* alloc(size_t size, size_t alignment) const
	{
		auto native = reinterpret_cast<void*(__fastcall*)(const MemoryArena*, size_t, size_t)>(0x140146020);
		return native(this, size, alignment);
	}

protected:
	enum MemoryArenaType
	{
		MemoryArenaType_Unknown,
		MemoryArenaType_ScratchPadArena,
		MemoryArenaType_GlobalHeapArena,
		MemoryArenaType_HeapArenaWithOtherSmallblock,
		MemoryArenaType_HeapArena,
		MemoryArenaType_HeapArenaWithGlobalSmallblock,
		MemoryArenaType_NonLocalHeapArena,
		MemoryArenaType_StackMemoryArena,
		MemoryArenaType_StackArenaBase,
		MemoryArenaType_HeapArenaBase,
		MemoryArenaType_StaticHeapMemoryArena,
		MemoryArenaType_VirtualAllocArena
	};

	MemoryArenaType m_arenaType;
	uint m_flags;
};

extern MemoryArena* s_levelArena;

}

void* operator new(size_t sz, const fb::MemoryArena& arena);

__forceinline void* operator new(size_t sz, size_t alignment, const fb::MemoryArena& arena)
{
	return arena.alloc(sz, alignment);
}