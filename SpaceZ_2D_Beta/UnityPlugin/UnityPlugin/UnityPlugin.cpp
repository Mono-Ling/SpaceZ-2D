#include "UnityPlugin/UnityPlugin.h"

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces* interfacePtr)
{
    for(const auto& func : OnUnityLoadCallbacks())
        func(interfacePtr);
    OnUnityLoadCallbacks().clear();
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload()
{
    for(const auto& func : OnUnityUnloadCallbacks())
        func();
    OnUnityUnloadCallbacks().clear();
}