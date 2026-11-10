#pragma once
#include "UnityPlugin/IUnityInterface.h"
#include <vector>

typedef void(*OnUnityPluginLoad)(IUnityInterfaces* interfacePtr);
typedef void(*OnUnityPluginUnload)();

inline std::vector<OnUnityPluginLoad>& OnUnityLoadCallbacks()
{
    static std::vector<OnUnityPluginLoad> callbacks;
    return callbacks;
}
inline std::vector<OnUnityPluginUnload>& OnUnityUnloadCallbacks()
{
    static std::vector<OnUnityPluginUnload> callbacks;
    return callbacks;
}