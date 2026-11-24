#include "Core/Memory/MemoryMgr.h"
#include "Core/Memory/Memory.h"
#include "Core/Math/Math.h"

#ifndef NDEBUG
#include <stdexcept>
#endif

using namespace Core::Math;
using namespace std;

namespace Core::Memory
{
    MemoryMgr::~MemoryMgr()
    {
        for(auto& ptr : _allocPage)
            Memory::Free(ptr);
    }
    void* MemoryMgr::Alloc(size_t size)
    {
        if(!size)
            size = 1;
        if(size >= SIZE_MAX - 2 * HEADER_SIZE)
            throw bad_alloc();

        const size_t allocSize = (HEADER_SIZE + size + HEADER_SIZE - 1) & ~(HEADER_SIZE - 1);

        void* allocPtr = _freePages.GetMemory(allocSize);
        if(!allocPtr)
        {
            auto page = AllocPage(allocSize);
            allocPtr = page.Cut(allocSize);

            if(page)
                _freePages.PutMemory(page);
        }

        if(!allocPtr)
            throw bad_alloc();
        size_t* header = reinterpret_cast<size_t*>(allocPtr);
        *header = allocSize;

        uintptr_t ans = reinterpret_cast<uintptr_t>(allocPtr) + HEADER_SIZE;
        void* ansPtr = reinterpret_cast<void*>(ans);

#ifndef NDEBUG
        _usingMemorySet.insert(ansPtr);
#endif

        return ansPtr;
    }
    void MemoryMgr::Free(void* ptr)
    {
        if(!ptr)
            return;

#ifndef NDEBUG
        auto it = _usingMemorySet.find(ptr);
        if(it == _usingMemorySet.end())
            throw invalid_argument("【MemoryMgr】无效地址，释放失败");
        _usingMemorySet.erase(it);
#endif

        uintptr_t freeAddr = reinterpret_cast<uintptr_t>(ptr) - HEADER_SIZE;
        size_t* header = reinterpret_cast<size_t*>(freeAddr);
        size_t size = *header;

        MemoryPage page(freeAddr, size);
        if(!page)
            return;
        _freePages.PutMemory(page);
    }

    MemoryPage MemoryMgr::AllocPage(std::size_t size)
    {
        size = Max(size, ALLOC_PAGE_SIZE);
        auto ptr = Memory::Alloc(size);
        if(!ptr)
            throw bad_alloc();

        _allocPage.emplace_back(ptr);
        return MemoryPage(reinterpret_cast<uintptr_t>(ptr), size);
    }
}