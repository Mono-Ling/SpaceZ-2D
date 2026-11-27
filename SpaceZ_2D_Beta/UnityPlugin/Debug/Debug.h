#pragma once
#include <string>

namespace UnityPlugin::Debug
{
    void Log(const std::string& message);
    void LogWarning(const std::string& message);
    void LogError(const std::string& message);
    void MemoryLogError(const char* message);
}