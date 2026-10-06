#pragma once
#include "ToString.h"
#include <string>

namespace Core::Math
{
    struct Vector3
    {
        float x;
        float y;
        float z;

        Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
        Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

        void operator+=(const Vector3& other);
        void operator-=(const Vector3& other);
        void operator*=(float scalar);
        void operator/=(float scalar);
        Vector3 operator-() const;
        float Length() const;
        float LengthSquared() const;
        Vector3 Normalized() const;

        static const Vector3 zero;
        static const Vector3 one;
        static const Vector3 up;
        static const Vector3 right;
        static const Vector3 forward;
    };
    bool operator==(const Vector3& a, const Vector3& b);
    bool operator!=(const Vector3& a, const Vector3& b);
    Vector3 operator+(const Vector3& a, const Vector3& b);
    Vector3 operator-(const Vector3& a, const Vector3& b);
    Vector3 operator*(const Vector3& v, float scalar);
    Vector3 operator*(float scalar, const Vector3& v);
    Vector3 operator/(const Vector3& v, float scalar);
    float Distance(const Vector3& a, const Vector3& b);
    float Dot(const Vector3& a, const Vector3& b);
    Vector3 Cross(const Vector3& a, const Vector3& b);
    Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector3>(const Math::Vector3& value);
}