#pragma once
#include "Core/System/PhySystem.h"
#include "Core/Debug/DebugFormat.h"
#include "UnityPlugin/Debug/Debug.h"
#include "UnityPlugin/IUnityInterface.h"

namespace UnityPlugin
{
    inline Core::PhySystem* g_phySystem = nullptr;

    inline bool PhyAvailable()
    {
        if(!g_phySystem)
        {
            Debug::LogWarning("【Export Interface】物理系统未启动");
            return false;
        }
        return true;
    }
}

#define TRY_CATCH_DEBUG(code, title, info)\
    do  \
    {   \
       try  code    \
       catch(const std::exception& e)   \
       {    \
            Core::Debug::LogErrorFormat("【", title, "】", info, e.what(), __FILE__, __LINE__);  \
       }    \
       catch(...)   \
       {    \
            Core::Debug::LogErrorFormat("【", title, "】", info, "Unknown Error", __FILE__, __LINE__);    \
       }    \
    } while (0);
    
#define EXPORT_FUNCTION extern "C" UNITY_INTERFACE_EXPORT