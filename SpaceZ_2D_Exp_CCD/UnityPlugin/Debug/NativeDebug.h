#pragma once
#include <string>

namespace Debug
{
    void Log(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
}