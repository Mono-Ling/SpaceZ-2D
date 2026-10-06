#include"IUnityLog.h"
#include"Debug/NativeDebug.h"
#include"UnityInterface/UnityPlugin.h"

static IUnityLog* unityLogPtr = nullptr;

static void OnNativeDebugLoad(IUnityInterfaces* interfacePtr)
{
    if (interfacePtr)
        unityLogPtr = interfacePtr->Get<IUnityLog>();
}
static void OnNativeDebugUnload()
{
    unityLogPtr = nullptr;
}

static bool registered = [](){
    UnityPluginLoadCallbacks().push_back(OnNativeDebugLoad);
    UnityPluginUnloadCallbacks().push_back(OnNativeDebugUnload);
    return true;
}();


namespace Debug
{
    void Log(const std::string& str)
    {
        if(registered && unityLogPtr)
            UNITY_LOG(unityLogPtr, str.data());
    }
    void LogWarning(const std::string& str)
    {
        if(registered && unityLogPtr)
            UNITY_LOG_WARNING(unityLogPtr, str.data());
    }
    void LogError(const std::string& str)
    {
        if(registered && unityLogPtr)
            UNITY_LOG_ERROR(unityLogPtr, str.data());
    }
}
