#pragma once
#include "Core/Memory/MgrAllocator.h"
#include "Core/Memory/MemoryPage.h"
#include "Core/Memory/MemoryPageTree.h"
#include <vector>

#ifndef NDEBUG
#include <unordered_set>
#endif

namespace Core::Memory
{
    constexpr std::size_t ALLOC_PAGE_SIZE = 1024 * 1024;
    constexpr std::size_t HEADER_SIZE = alignof(std::max_align_t);

    class MemoryMgr
    {
        MemoryPageTree _freePages;
        std::vector<void*, MgrAllocator<void*>> _allocPage;
#ifndef NDEBUG
        std::unordered_set<void*, std::hash<void*>, std::equal_to<void*>, MgrAllocator<void*>> _usingMemorySet;
#endif

    public:
        MemoryMgr() = default;
        MemoryMgr(const MemoryMgr& other) = delete;
        MemoryMgr(MemoryMgr&& other) = delete;
        ~MemoryMgr();

        MemoryMgr& operator=(const MemoryMgr& other) = delete;
        MemoryMgr& operator=(MemoryMgr&& other) = delete;

        void* Alloc(std::size_t size);
        void Free(void* ptr);

        void* Alloc(std::size_t size, std::align_val_t al);
        void Free(void* ptr, std::align_val_t al);

    private:
        MemoryPage AllocPage(std::size_t size);
    };
}