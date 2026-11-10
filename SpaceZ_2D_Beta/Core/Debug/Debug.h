#pragma once
#include "Debug/ToString.h"
#include <functional>

namespace Core::Debug
{
    using DebugFunction = std::function<void(const std::string&)>;

    inline DebugFunction debugLog;
    inline DebugFunction debugLogWarning;
    inline DebugFunction debugErrorLog;

    template<typename T>
    void Log(const T& message) = delete;

    template<typename T>
    void LogWarning(const T& message) = delete;

    template<typename T>
    void LogError(const T& message) = delete;


    template<>
    void Log<std::string>(const std::string& message);

    template<>
    void LogWarning<std::string>(const std::string& message);

    template<>
    void LogError<std::string>(const std::string& message);


    template<ToStringAbility T>
    void Log(const T& message);

    template<ToStringAbility T>
    void LogWarning(const T& message);

    template<ToStringAbility T>
    void LogError(const T& message);
}
#include "Core/Debug/Debug.tpp"