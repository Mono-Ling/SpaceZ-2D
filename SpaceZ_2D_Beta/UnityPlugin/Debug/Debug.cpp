#include "UnityPlugin/Debug/Debug.h"
#include "UnityPlugin/UnityPlugin/UnityPlugin.h"
#include "UnityPlugin/Debug/IUnityLog.h"

static IUnityLog* logPtr = nullptr;

static void LoadLogInterface(IUnityInterfaces* ptr)
{
    if (ptr)
        logPtr = ptr->Get<IUnityLog>();
}
static void UnloadLogInterface() { logPtr = nullptr; }

static bool loadCallback = []()->bool
{
    OnUnityLoadCallbacks().emplace_back(LoadLogInterface);
    return true;
}();
static bool unloadCallback = []()->bool
{
    OnUnityUnloadCallbacks().emplace_back(UnloadLogInterface);
    return true;
}();

namespace Debug
{
    void Log(const std::string& message)
    {
        if(logPtr)
            UNITY_LOG(logPtr, message.data());
    }
    void LogWarning(const std::string& message)
    {
        if(logPtr)
            UNITY_LOG_WARNING(logPtr, message.data());
    }
    void LogError(const std::string& message)
    {
        if(logPtr)
            UNITY_LOG_ERROR(logPtr, message.data());
    }
}