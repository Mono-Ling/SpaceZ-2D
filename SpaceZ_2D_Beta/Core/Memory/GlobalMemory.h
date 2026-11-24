#pragma once
#include "Core/Memory/MemoryMgr.h"
#include "Core/Memory/MgrAllocator.h"

namespace Core::Memory
{
    MgrAllocator<MemoryMgr> GetMemoryMgrAllocator();
    void EnableGlobalMemoryMgr();
    void DisableGlobalMemoryMgr();

    typedef void(*MemoryLogErrorFunc)(const char* msg);
    inline extern MemoryLogErrorFunc g_memoryLogError = nullptr;
    void MemoryLogError(const char* msg);
}