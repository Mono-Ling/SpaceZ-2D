#include "IUnityInterface.h"

IUnityInterfaces* g_UnityInterfaces = nullptr;

#pragma region Unity注入
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginLoad(IUnityInterfaces* interfacePtr)
{
    g_UnityInterfaces = interfacePtr;
}
extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API
UnityPluginUnload()
{
    g_UnityInterfaces = nullptr;
}
#pragma endregion

extern "C" UNITY_INTERFACE_EXPORT IUnityInterfaces* GetUnityInterface()
{
    return g_UnityInterfaces;
}