#pragma once
#include "System/PhySystem.h"
#include "Debug/NativeDebug.h"
#include <stdexcept>

extern Core::PhySystem* phySystemPtr;

#define TRY_CATCH_DEBUG(code, debugTitle, debugInfo)  \
    do  \
    {   \
        try { code } \
        catch (const std::exception& e) \
        {   \
            Debug::LogError("【" + std::string(debugTitle) + "】" + std::string(debugInfo) + std::string(e.what())); \
        }   \
        catch(...) \
        {   \
            Debug::LogError("【" + std::string(debugTitle) + "】" + std::string(debugInfo) + "Unknown exception"); \
        }   \
    } while(false)

#define EXPORT_FUNCTION extern "C" __declspec(dllexport)

inline bool IsEngineAvailable()
{
    if(!phySystemPtr)
    {
        Debug::LogWarning("【物理引擎】引擎未启动");
        return false;
    }
    return true;
}