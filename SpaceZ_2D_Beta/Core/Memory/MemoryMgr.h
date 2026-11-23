#pragma once
#include "Core/Memory/MgrAllocator.h"
#include "Core/Memory/MemoryPage.h"
#include <set>
#include <vector>

#ifndef NDEBUG
#include <unordered_set>
#endif

namespace Core::Memory
{
#ifndef NDEBUG
    constexpr bool IS_DEBUG = true;
#else
    constexpr bool IS_DEBUG = false;
#endif

    constexpr std::size_t ALLOC_PAGE_SIZE = 1024 * 1024;
    constexpr std::size_t HEADER_SIZE = alignof(std::max_align_t);

    class MemoryMgr
    {
        std::set<MemoryPage, std::less<MemoryPage>, MgrAllocator<MemoryPage>> _freePageSet;
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

    private:
        MemoryPage AllocPage(std::size_t size);
    };
}