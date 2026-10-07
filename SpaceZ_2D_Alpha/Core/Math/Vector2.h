#pragma once
#include "ToString.h"

namespace Core::Math
{
    struct Vector2
    {
        float x;
        float y;

        Vector2() : x(0.0f), y(0.0f) {}
        Vector2(float x, float y) : x(x), y(y) {}

        void operator+=(const Vector2& other);
        void operator-=(const Vector2& other);
        void operator*=(float scalar);
        void operator/=(float scalar);
        Vector2 operator-() const;
        float Length() const;
        float LengthSquared() const;
        Vector2 Normalized() const;
        Vector2 ProjectionVector(const Vector2& axis) const;
        Vector2 RejectionVector(const Vector2& axis) const;

        static const Vector2 zero;
        static const Vector2 one;
        static const Vector2 up;
        static const Vector2 down;
        static const Vector2 left;
        static const Vector2 right;
    };
    Vector2 operator+(const Vector2& a, const Vector2& b);
    Vector2 operator-(const Vector2& a, const Vector2& b);
    Vector2 operator*(const Vector2& v, float scalar);
    Vector2 operator*(float scalar, const Vector2& v);
    Vector2 operator/(const Vector2& v, float scalar);
    bool operator==(const Vector2& a,const Vector2& b);
    bool operator!=(const Vector2& a,const Vector2& b);

    float Distance(const Vector2& a, const Vector2& b);
    float Dot(const Vector2& a, const Vector2& b);
    float Cross(const Vector2& a, const Vector2& b);
    Vector2 Lerp(const Vector2& start, const Vector2& end, float t);
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector2>(const Math::Vector2& value);
}