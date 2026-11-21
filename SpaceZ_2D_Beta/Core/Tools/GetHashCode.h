#pragma once
#include <functional>
#include <concepts>

namespace Core
{
    template<typename T>
    std::size_t GetHashCode(const T& value) = delete;

    template<typename T>
    concept StdHashable = requires(T value)
    {
        { std::hash<T>{}(std::declval<const T&>()) } -> std::same_as<std::size_t>;
    };

    template<StdHashable T>
    constexpr std::size_t GetHashCode(const T& value) { return std::hash<T>{}(value); }

    template<typename T>
    requires (!StdHashable<T>) && requires(const T& value)
    {
        {value.GetHashCode()} -> std::same_as<std::size_t>;
    }
    constexpr std::size_t GetHashCode(const T& value) { return value.GetHashCode(); }

    template<typename T>
    concept Hashable = requires(const T& value)
    {
        {GetHashCode(value)} -> std::same_as<std::size_t>;
    };
}