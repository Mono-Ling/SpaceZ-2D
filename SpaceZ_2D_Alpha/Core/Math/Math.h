#pragma once
#include<concepts>
#include <cmath>
#include <cstdint>
#include <limits>
#include<algorithm>

namespace Core::Math
{
    constexpr float PI         = 3.14159265358979323846f;
    constexpr float Deg2Rad    = PI / 180.0f;
    constexpr float Rad2Deg    = 180.0f / PI;
    constexpr float Epsilon    = 0.00001f;
    constexpr float Infinity   = std::numeric_limits<float>::infinity();
    constexpr float NegativeInfinity = -std::numeric_limits<float>::infinity();
    constexpr float NaN        = std::numeric_limits<float>::quiet_NaN();

    template<typename T>
    concept LessComparable = requires(T a, T b)
    {
        { a < b } -> std::convertible_to<bool>;
    };
    template<typename T>
    concept GreaterComparable = requires(T a, T b)
    {
        { a > b } -> std::convertible_to<bool>;
    };
    template<typename T>
    concept Arithmetic = requires(T a, T b)
    {
        { a + b } -> std::convertible_to<T>;
        { a - b } -> std::convertible_to<T>;
        { a * b } -> std::convertible_to<T>;
        { a / b } -> std::convertible_to<T>;
    };

    template<LessComparable T>
    constexpr T Clamp(T value, T min, T max)
    {
        return std::clamp(value,min,max);
    }
    template<LessComparable T>
    constexpr T Clamp01(T value)
    {
        return Clamp(value, T{0}, T{1});
    }
    template<LessComparable T>
    requires requires(T a)
    {
        {a < 0} -> std::convertible_to<bool>;
    }
    constexpr T Abs(T value)
    {
        return (value < 0) ? -value : value;
    }
    template<LessComparable T>
    constexpr T Min(T a, T b)
    {
        return (a < b) ? a : b;
    }
    template<LessComparable T>
    constexpr T Max(T a, T b)
    {
        return (a < b) ? b : a;
    }
    constexpr float Repeat(float t, float length)
    {
        return Clamp(t - std::floor(t / length) * length, 0.0f, length);
    }
#pragma region 取整函数
    inline int CeilToInt(float f)
    {
        return static_cast<int>(std::ceil(f));
    }
    inline int FloorToInt(float f)
    {
        return static_cast<int>(std::floor(f));
    }
    inline int RoundToInt(float f)
    {
        return static_cast<int>(std::round(f));
    }
#pragma endregion
#pragma region 插值函数
    template<Arithmetic T>
    constexpr T Lerp(T a, T b, float t)
    {
        t = Clamp01(t);
        return a + (b - a) * t;
    }
    constexpr float InverseLerp(float a, float b, float value)
    {
        if (a != b)
            return Clamp01((value - a) / (b - a));
        return 0.0f;
    }
    constexpr float LerpAngle(float a, float b, float t)
    {
        float delta = Repeat((b - a), 360.0f);
        if (delta > 180.0f)
            delta -= 360.0f;
        return a + delta * Clamp01(t);
    }
#pragma endregion
#pragma region 三角函数
    inline float Sin(float f)          { return std::sin(f); }
    inline float Cos(float f)          { return std::cos(f); }
    inline float Tan(float f)          { return std::tan(f); }
    inline float Asin(float f)         { return std::asin(Clamp(f, -1.0f, 1.0f)); }
    inline float Acos(float f)         { return std::acos(Clamp(f, -1.0f, 1.0f)); }
    inline float Atan(float f)         { return std::atan(f); }
    inline float Atan2(float y, float x) { return std::atan2(y, x); }
#pragma endregion
#pragma region 指数函数
    inline float Exp(float f)          { return std::exp(f); }
    inline float Log(float f)          { return std::log(f); }
    inline float Log10(float f)        { return std::log10(f); }
    inline float Pow(float f, float p) { return std::pow(f, p); }
    inline float Sqrt(float f)         { return std::sqrt(f); }
#pragma endregion
}