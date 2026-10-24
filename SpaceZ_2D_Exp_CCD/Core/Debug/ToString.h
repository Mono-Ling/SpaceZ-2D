#pragma once
#include <string>
#include <string_view>

namespace Core
{
    template<typename T>
    std::string ToString(const T& obj);

    template<typename T>
    requires requires(T a) { std::to_string(a); }
    std::string ToString(const T& value)
    {
        return std::to_string(value);
    }

    template<>
    constexpr std::string ToString<std::string>(const std::string& value)
    {
        return value;
    }
    template<>
    constexpr std::string ToString<const char*>(const char* const& value)
    {
        return std::string(value);
    }
    template<std::size_t size>
    constexpr std::string ToString(const char (&value)[size])
    {
        return std::string(value);
    }
    constexpr std::string ToString(std::string_view value)
    {
        return std::string(value);
    }
}