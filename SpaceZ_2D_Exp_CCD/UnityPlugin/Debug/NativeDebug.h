#pragma once
#include "Debug/Debug.h"
#include <string>

namespace Debug
{
    void Log(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);

    template<Core::Debug::ToStringAbility... Args>
    requires ( sizeof...(Args) >= 1 )
    void Logs(Args... args);

    template<Core::Debug::ToStringAbility... Args>
    requires ( sizeof...(Args) >= 1 )
    void LogWarnings(Args... args);

    template<Core::Debug::ToStringAbility... Args>
    requires ( sizeof...(Args) >= 1 )
    void LogErrors(Args... args);
}
#include "Debug/NativeDebug.tpp"