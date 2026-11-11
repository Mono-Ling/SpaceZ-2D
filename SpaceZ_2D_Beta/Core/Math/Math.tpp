#pragma once
#include "Core/Math/Math.h"

namespace Core::Math
{
    template<LessComparable T>
    constexpr T Min(const T& a, const T& b) { return a < b ? a : b; }
    template<GreaterComparable T>
    constexpr T Max(const T& a, const T& b) { return a > b ? a : b; }

    template<typename T>
    requires LessComparable<T> && GreaterComparable<T>
    constexpr T Clamp(const T& value, const T& min, const T& max)
    {
        return Min(Max(value, min), max);
    }

    template<typename T>
    requires LessComparable<T> && GreaterComparable<T> && requires(T type)
    {
        { T{0} } -> std::same_as<T>;
        { T{1} } -> std::same_as<T>;
    }
    constexpr T Clamp01(const T& value)
    {
        return Clamp(value, T{0}, T{1});
    }

    template<LessComparable T>
    requires requires(T value)
    {
        { T{0} } -> std::same_as<T>;
        { -value } -> std::convertible_to<T>;
    }
    constexpr T Abs(const T& value)
    {
        return value < T{0} ? -value : value;
    }

    template<typename T>
    requires requires(T type)
    {
        { type + type } -> std::same_as<T>;
        { type - type } -> std::same_as<T>;
        { type * 0.0f } -> std::same_as<T>;
    }
    constexpr T Lerp(const T& a, const T& b, float n)
    {
        n = Clamp01(n);
        return a + (b - a) * n;
    }

    template<typename T>
    requires std::floating_point<T>
    constexpr bool Near(const T& a, const T& b, float eps)
    {
        return Abs(a - b) < eps;
    }

    template<typename T>
    requires std::floating_point<T>
    constexpr bool GreaterNear(const T& a, const T& b, float eps)
    {
        return a > b || Near(a, b, eps);
    }

    template<typename T>
    requires std::floating_point<T>
    constexpr bool LessNear(const T& a, const T& b, float eps)
    {
        return a < b || Near(a, b, eps);
    }
}