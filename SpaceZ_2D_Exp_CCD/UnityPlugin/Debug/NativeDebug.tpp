#pragma once
#include "Debug/NativeDebug.h"

namespace Debug
{
    using Core::ToString;

    template<Core::Debug::ToStringAbility... Args>
    requires ( sizeof...(Args) >= 1 )
    void Logs(Args... args)
    {
        std::string ans;
        (ans.append(ToString(args)), ...);
        Log(ans);
    }

    template<Core::Debug::ToStringAbility... Args>
    requires ( sizeof...(Args) >= 1 )
    void LogWarnings(Args... args)
    {
        std::string ans;
        (ans.append(ToString(args)), ...);
        LogWarning(ans);
    }

    template<Core::Debug::ToStringAbility... Args>
    requires ( sizeof...(Args) >= 1 )
    void LogErrors(Args... args)
    {
        std::string ans;
        (ans.append(ToString(args)), ...);
        LogError(ans);
    }
}