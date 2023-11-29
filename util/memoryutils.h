#pragma once
#include <cstdint>
#include <vector>

// Hooking code below comes from here: https://github.com/khalladay/hooking-by-example

// allocates memory close enough to the provided targetAddr argument to be reachable
// from the targetAddr by a 32 bit jump instruction
void* _AllocatePageNearAddress(void* targetAddr);

uint32_t _WriteAbsoluteJump64(void* absJumpMemory, void* addrToJumpTo);

#define check(expr) if (!(expr)){ DebugBreak(); exit(-1); }

void InjectHook(ptrdiff_t address, void* hook);

void PatchBytes(ptrdiff_t address, uint8_t patch[], size_t patchSize);
