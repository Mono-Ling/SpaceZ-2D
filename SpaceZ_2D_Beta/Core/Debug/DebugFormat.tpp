#pragma once
#include "Core/Debug/DebugFormat.h"

namespace Core::Debug
{
    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    std::string StringFormat(Args... args)
    {
        std::string str;
        (str.append(ToString(args)), ...);
        return str;
    }

    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    void LogFormat(Args... args)
    {
        Log<std::string>(StringFormat(args...));
    }

    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    void LogWarningFormat(Args... args)
    {
        LogWarning<std::string>(StringFormat(args...));
    }

    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    void LogErrorFormat(Args... args)
    {
        LogError<std::string>(StringFormat(args...));
    }
}