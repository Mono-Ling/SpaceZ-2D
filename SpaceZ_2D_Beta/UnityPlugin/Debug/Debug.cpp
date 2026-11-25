#include "UnityPlugin/Debug/Debug.h"
#include "UnityPlugin/UnityPlugin/UnityPlugin.h"
#include "UnityPlugin/Debug/IUnityLog.h"
#include "Core/Memory/GlobalMemory.h"

namespace UnityPlugin
{
    static IUnityLog* logPtr = nullptr;

    static void LoadLogInterface(IUnityInterfaces* ptr)
    {
        if (ptr)
            logPtr = ptr->Get<IUnityLog>();
    }
    static void UnloadLogInterface()
    {
        logPtr = nullptr;
    }
    static void MemoryLogError(const char* message)
    {
        if (logPtr)
            UNITY_LOG_ERROR(logPtr, message);
    }

    static bool init = []() -> bool
    {
        OnUnityLoadCallbacks().emplace_back(LoadLogInterface);
        OnUnityUnloadCallbacks().emplace_back(UnloadLogInterface);

        Core::Memory::g_memoryLogError = MemoryLogError;
        return true;
    }();
}

namespace UnityPlugin::Debug
{
    void Log(const std::string& message)
    {
        if (logPtr)
            UNITY_LOG(logPtr, message.data());
    }
    void LogWarning(const std::string& message)
    {
        if (logPtr)
            UNITY_LOG_WARNING(logPtr, message.data());
    }
    void LogError(const std::string& message)
    {
        if (logPtr)
            UNITY_LOG_ERROR(logPtr, message.data());
    }
}