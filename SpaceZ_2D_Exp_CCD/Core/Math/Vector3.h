#pragma once
#include "Math/Math.h"
#include "Debug/ToString.h"

namespace Core::Math
{
    struct Vector3
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        constexpr Vector3() = default;
        constexpr Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        void operator+=(const Vector3& other);
        void operator-=(const Vector3& other);
        void operator*=(float scalar);
        void operator/=(float scalar);
        Vector3 operator-() const;
        float Length() const;
        float SqrLength() const;
        Vector3 Normalized() const;

        static const Vector3 zero;
        static const Vector3 one;
        static const Vector3 up;
        static const Vector3 right;
        static const Vector3 forward;
    };

    inline constexpr Vector3 Vector3::zero    = Vector3(0.0f, 0.0f, 0.0f);
    inline constexpr Vector3 Vector3::one     = Vector3(1.0f, 1.0f, 1.0f);
    inline constexpr Vector3 Vector3::up      = Vector3(0.0f, 1.0f, 0.0f);
    inline constexpr Vector3 Vector3::right   = Vector3(1.0f, 0.0f, 0.0f);
    inline constexpr Vector3 Vector3::forward = Vector3(0.0f, 0.0f, 1.0f);

    constexpr float Dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    constexpr Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return Vector3
        (
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    bool operator==(const Vector3& a, const Vector3& b);
    bool operator!=(const Vector3& a, const Vector3& b);
    Vector3 operator+(const Vector3& a, const Vector3& b);
    Vector3 operator-(const Vector3& a, const Vector3& b);
    Vector3 operator*(const Vector3& v, float scalar);
    Vector3 operator*(float scalar, const Vector3& v);
    Vector3 operator/(const Vector3& v, float scalar);
    float Distance(const Vector3& a, const Vector3& b);

    template<>
    Vector3 Lerp<Vector3>(const Vector3& start, const Vector3& end, float t);
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector3>(const Math::Vector3& value);
}