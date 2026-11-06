#pragma once
#include "System/PhySystem.h"
#include "Debug/NativeDebug.h"
#include <stdexcept>

extern Core::PhySystem* phySystem;

#define TRY_CATCH_DEBUG(code, title, message) \
    do  \
    {   \
        try code \
        catch (const std::exception& e) \
        {   \
            ::Debug::LogErrors("【", title, "】", message, e.what());    \
        }   \
        catch (...) \
        {   \
            ::Debug::LogErrors("【", title, "】", message, "Unknown exception");    \
        }   \
    } while (0)

#define EXPORT_FUNCTION extern "C" __declspec(dllexport)

bool IsEngineAvailable();