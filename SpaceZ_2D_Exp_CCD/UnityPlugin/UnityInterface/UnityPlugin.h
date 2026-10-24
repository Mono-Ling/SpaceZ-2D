#pragma once
#include "UnityInterface/IUnityInterface.h"
#include <vector>

extern IUnityInterfaces* g_UnityInterfaces;

typedef void(*OnUnityPluginLoad)(IUnityInterfaces* interfacePtr);
typedef void(*OnUnityPluginUnload)();

std::vector<OnUnityPluginLoad>& UnityPluginLoadCallbacks();
std::vector<OnUnityPluginUnload>& UnityPluginUnloadCallbacks();