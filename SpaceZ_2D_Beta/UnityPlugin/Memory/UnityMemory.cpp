#include "UnityPlugin/UnityPlugin/UnityPlugin.h"
#include "UnityPlugin/Memory/IUnityMemoryManager.h"
#include "Core/Memory/Memory.h"
#include "Core/Memory/GlobalMemory.h"
#include <stdexcept>

namespace UnityPlugin
{
    static IUnityMemoryManager* g_memoryMgr = nullptr;
    static UnityAllocator* g_allocator = nullptr;

    static void LoadMemoryInterface(IUnityInterfaces* ptr)
    {
        if(!ptr)
            return;
        g_memoryMgr = ptr->Get<IUnityMemoryManager>();
        if(!g_memoryMgr)
            return;

        g_allocator = g_memoryMgr->CreateAllocator("SpaceZ-2D-Beta Native", "Global Heap Allocator");

        Core::Memory::EnableGlobalMemoryMgr();

        g_isGlobalMemoryReady = g_allocator != nullptr;
    }
    static void UnloadMemoryInterface()
    {
        if(!g_memoryMgr || !g_allocator)
            return;

        g_isGlobalMemoryReady = false;
        
        Core::Memory::DisableGlobalMemoryMgr();

        g_memoryMgr->DestroyAllocator(g_allocator);
        g_memoryMgr = nullptr;
        g_allocator = nullptr;
    }

    static void* Alloc(std::size_t size)
    {
        if(!g_memoryMgr || !g_allocator)
            throw std::bad_alloc();
        return g_memoryMgr->Allocate(g_allocator, size, __STDCPP_DEFAULT_NEW_ALIGNMENT__ , __FILE__, __LINE__);
    }
    static void Free(void* ptr)
    {
        if(!ptr)
            return;
        if(!g_memoryMgr || !g_allocator)
            throw std::runtime_error("【UnityPlugin Memory】内存释放异常，内存管理器 | 分配器为空");
        g_memoryMgr->Deallocate(g_allocator, ptr, __FILE__, __LINE__);
    }

    static bool init = []() -> bool
    {
        OnUnityLoadCallbacks().emplace_back(LoadMemoryInterface);
        OnUnityUnloadCallbacks().emplace_back(UnloadMemoryInterface);

        Core::Memory::allocFunc = Alloc;
        Core::Memory::freeFunc = Free;
        return true;
    }();
}