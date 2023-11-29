#include <Engine/Core/Memory/MemoryArena.h>


namespace fb
{

MemoryArena* s_levelArena = reinterpret_cast<MemoryArena*>(0x141C929D0);

}

void* operator new(size_t sz, const fb::MemoryArena& arena)
{
	auto native = reinterpret_cast<void*(__fastcall*)(size_t, const fb::MemoryArena&)>(0x1401456A0);
	return native(sz, arena);
}