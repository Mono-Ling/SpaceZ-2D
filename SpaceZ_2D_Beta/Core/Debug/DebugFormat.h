#pragma once
#include "Core/Debug/Debug.h"
#include "Core/Debug/ToString.h"
#include <string>

namespace Core::Debug
{
    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    void LogFormat(Args... args);

    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    void LogWarningFormat(Args... args);

    template<ToStringAbility... Args>
    requires (sizeof...(Args) >= 1)
    void LogErrorFormat(Args... args);
}
#include "Core/Debug/DebugFormat.tpp"