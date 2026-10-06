#pragma once
#include <string>

namespace Core
{
    template<typename T>
    std::string ToString(const T& value);

    template<typename T>
    requires requires(T a) { std::to_string(a); }
    std::string ToString(const T& value)
    {
        return std::to_string(value);
    }

    template<>
    std::string ToString<std::string>(const std::string& value)
    {
        return value;
    }
    template<>
    std::string ToString<const char*>(const char* const& value)
    {
        return std::string(value);
    }
}