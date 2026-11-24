#include "Core/Memory/GlobalMemory.h"
#include <stdexcept>
using namespace Core::Memory;
using namespace std;

namespace Core::Memory
{
    MemoryMgr* g_memoryMgr = nullptr;

    MgrAllocator<MemoryMgr> GetMemoryMgrAllocator()
    {
        static MgrAllocator<MemoryMgr> allocator;
        return allocator;
    }
    void EnableGlobalMemoryMgr()
    {
        if(g_memoryMgr)
            return;
        g_memoryMgr = GetMemoryMgrAllocator().allocate(1);
        g_memoryMgr = new(g_memoryMgr) MemoryMgr();
    }
    void DisableGlobalMemoryMgr()
    {
        if(!g_memoryMgr)
            return;
        g_memoryMgr->~MemoryMgr();
        GetMemoryMgrAllocator().deallocate(g_memoryMgr, 1);
        g_memoryMgr = nullptr;
    }

    void MemoryLogError(const char* msg)
    {
        if(g_memoryLogError)
            g_memoryLogError(msg);
    }
}

void* ::operator new(size_t size)
{
    if(!g_memoryMgr)
        throw bad_alloc();
    return g_memoryMgr->Alloc(size);
}
void* ::operator new(size_t size, const nothrow_t&) noexcept
{
    if(!g_memoryMgr)
    {
        MemoryLogError("【全局内存管理】管理器未就绪，nothrow new 返回null");
        return nullptr;
    }
    try
    {
        return g_memoryMgr->Alloc(size);
    }
    catch(...)
    {
        MemoryLogError("【全局内存管理】Alloc异常，nothrow new 返回null");
        return nullptr;
    }
}
void ::operator delete(void* ptr) noexcept
{
    if(!g_memoryMgr)
    {
        MemoryLogError("【全局内存管理】Free异常，内存管理器已失活");
        return;
    }
    try
    {
        g_memoryMgr->Free(ptr);
    }
    catch(const exception& e)
    {
        MemoryLogError(e.what());
    }
    catch(...)
    {
        MemoryLogError("【全局内存管理】Unknow Error");
    }
}

void* ::operator new[](size_t size)
{
    if(!g_memoryMgr)
        throw bad_alloc();
    return g_memoryMgr->Alloc(size);
}
void* ::operator new[](size_t size, const nothrow_t&) noexcept
{
    if(!g_memoryMgr)
    {
        MemoryLogError("【全局内存管理】管理器未就绪，nothrow new[] 返回null");
        return nullptr;
    }
    try
    {
        return g_memoryMgr->Alloc(size);
    }
    catch(...)
    {
        MemoryLogError("【全局内存管理】Alloc异常，nothrow new[] 返回null");
        return nullptr;
    }
}
void  ::operator delete[](void* ptr) noexcept
{
    if(!g_memoryMgr)
    {
        MemoryLogError("【全局内存管理】Free异常，内存管理器已失活");
        return;
    }
    try
    {
        g_memoryMgr->Free(ptr);
    }
    catch(const exception& e)
    {
        MemoryLogError(e.what());
    }
    catch(...)
    {
        MemoryLogError("【全局内存管理】Unknow Error");
    }
}