#pragma once
#include "UnityPlugin/IUnityInterface.h"
#include "UnityPlugin/Memory/NativeAllocator.h"
#include <vector>

typedef void (*OnUnityPluginLoadFunc)(IUnityInterfaces* interfacePtr);
typedef void (*OnUnityPluginUnloadFunc)();

namespace UnityPlugin
{
    inline bool g_isGlobalMemoryReady = false;

    template<typename T>
    using Action = std::vector<T, NativeAllocator<T>>;

    inline Action<OnUnityPluginLoadFunc>& OnUnityLoadCallbacks()
    {
        static Action<OnUnityPluginLoadFunc> callbacks;
        return callbacks;
    }
    inline Action<OnUnityPluginUnloadFunc>& OnUnityUnloadCallbacks()
    {
        static Action<OnUnityPluginUnloadFunc> callbacks;
        return callbacks;
    }
}