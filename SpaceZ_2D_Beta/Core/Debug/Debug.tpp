#pragma once
#include "Core/Debug/Debug.h"

namespace Core::Debug
{
    template<>
    void Log<std::string>(const std::string& message)
    {
        if(debugLog)
            debugLog(message);
    }

    template<>
    void LogWarning<std::string>(const std::string& message)
    {
        if(debugLogWarning)
            debugLogWarning(message);
    }

    template<>
    void LogError<std::string>(const std::string& message)
    {
        if(debugErrorLog)
            debugErrorLog(message);
    }

    template<ToStringAbility T>
    void Log(const T& message)
    {
        Log<std::string>(ToString(message));
    }

    template<ToStringAbility T>
    void LogWarning(const T& message)
    {
        LogWarning<std::string>(ToString(message));
    }

    template<ToStringAbility T>
    void LogError(const T& message)
    {
        LogError<std::string>(ToString(message));
    }
}