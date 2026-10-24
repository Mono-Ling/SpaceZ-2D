#pragma once
#include <string>
#include <vector>
#include <functional>
#include <concepts>

namespace Core::Debug
{
    using LogFuncPtr = std::function<void(const std::string&)>;

    inline std::vector<LogFuncPtr> debugLog;
    inline std::vector<LogFuncPtr> debugLogWarning;
    inline std::vector<LogFuncPtr> debugLogError;

    inline void Log(const std::string& message);
    inline void LogWarning(const std::string& message);
    inline void LogError(const std::string& message);

    template<typename T>
    concept ToStringAbility = requires(T message)
    {
        { ToString<T>(message) } -> std::same_as<std::string>;
    };

    template<ToStringAbility T>
    void Log(const T& message);

    template<ToStringAbility T>
    void LogWarning(const T& message);

    template<ToStringAbility T>
    void LogError(const T& message);
}
#include "Debug/Debug.tpp"