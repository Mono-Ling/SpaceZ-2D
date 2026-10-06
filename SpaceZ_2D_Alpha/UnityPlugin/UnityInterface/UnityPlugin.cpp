#include "UnityInterface/UnityPlugin.h"
#include <vector>

IUnityInterfaces* g_UnityInterfaces = nullptr;

std::vector<OnUnityPluginLoad>& UnityPluginLoadCallbacks()
{
    static std::vector<OnUnityPluginLoad> callbacks;
    return callbacks;
}
std::vector<OnUnityPluginUnload>& UnityPluginUnloadCallbacks()
{
    static std::vector<OnUnityPluginUnload> callbacks;
    return callbacks;
}

#pragma region Unity日志注入
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces* interfacePtr)
{
    g_UnityInterfaces = interfacePtr;
    for (const auto& callback : UnityPluginLoadCallbacks())
        callback(interfacePtr);
    UnityPluginLoadCallbacks().clear();
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload()
{
    g_UnityInterfaces = nullptr;
    for (const auto& callback : UnityPluginUnloadCallbacks())
        callback();
    UnityPluginUnloadCallbacks().clear();
}
#pragma endregion
