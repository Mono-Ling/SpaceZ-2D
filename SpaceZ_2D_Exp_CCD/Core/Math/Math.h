#pragma once
#include <cmath>
#include <concepts>

namespace Core::Math
{
    constexpr float PI         = 3.14159265358979323846f;
    constexpr float Deg2Rad    = PI / 180.0f;
    constexpr float Rad2Deg    = 180.0f / PI;
    constexpr float Epsilon    = 0.00001f;

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

    template<LessComparable T>
    constexpr T Min(const T& a, const T& b);
    template<GreaterComparable T>
    constexpr T Max(const T& a, const T& b);

    template<typename T>
    requires LessComparable<T> && GreaterComparable<T>
    constexpr T Clamp(const T& value, const T& minV, const T& maxV);

    template<typename T>
    requires LessComparable<T> && GreaterComparable<T> && requires(T type)
    {
        { T{0} } -> std::same_as<T>;
        { T{1} } -> std::same_as<T>;
    }
    constexpr T Clamp01(const T& value);

    template<LessComparable T>
    requires requires(T value)
    {
        { T{0} } -> std::same_as<T>;
        { -value } -> std::convertible_to<T>;
    }
    constexpr T Abs(const T& value);

    template<typename T>
    requires requires(T type)
    {
        { type + type } -> std::same_as<T>;
        { type - type } -> std::same_as<T>;
        { type * 0.0f } -> std::same_as<T>;
    }
    constexpr T Lerp(const T& a, const T& b, float n);

    template<typename T>
    requires std::floating_point<T>
    constexpr bool Near(const T& a, const T& b);

    constexpr float Repeat(float t, float length)
    {
        return Clamp(t - std::floor(t / length) * length, 0.0f, length);
    }
    constexpr float LerpAngle(float a, float b, float t)
    {
        float delta = Repeat((b - a), 360.0f);
        if (delta > 180.0f)
            delta -= 360.0f;
        return a + delta * Clamp01(t);
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
#include "Math/Math.tpp"