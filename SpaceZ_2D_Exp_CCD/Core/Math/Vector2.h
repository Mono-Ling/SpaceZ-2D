#pragma once
#include "Math/Math.h"
#include "Debug/ToString.h"

namespace Core::Math
{
    struct Vector2
    {
        float x = 0.0f;
        float y = 0.0f;

        constexpr Vector2() = default;
        constexpr Vector2(float x, float y) : x(x), y(y) {}

        void operator+=(const Vector2& other);
        void operator-=(const Vector2& other);
        void operator*=(float scale);
        void operator/=(float scale);
        Vector2 operator-() const;

        float Length() const;
        float SqrLength() const;
        Vector2 Normalized() const;
        Vector2 ProjectionVector(const Vector2& axis) const;
        Vector2 ProjectionPoint(const Vector2& origin, const Vector2& axis) const;
        Vector2 Perp() const;

        static const Vector2 zero;
        static const Vector2 one;
        static const Vector2 up;
        static const Vector2 down;
        static const Vector2 left;
        static const Vector2 right;
    };
    
    inline constexpr Vector2 Vector2::zero{0.0f, 0.0f};
    inline constexpr Vector2 Vector2::one{1.0f, 1.0f};
    inline constexpr Vector2 Vector2::up{0.0f, 1.0f};
    inline constexpr Vector2 Vector2::down{0.0f, -1.0f};
    inline constexpr Vector2 Vector2::left{-1.0f, 0.0f};
    inline constexpr Vector2 Vector2::right{1.0f, 0.0f};

    bool operator==(const Vector2& a, const Vector2& b);
    bool operator!=(const Vector2& a, const Vector2& b);
    Vector2 operator+(const Vector2& a, const Vector2& b);
    Vector2 operator-(const Vector2& a, const Vector2& b);
    Vector2 operator*(const Vector2& v, float scale);
    Vector2 operator*(float scale, const Vector2& v);
    Vector2 operator/(const Vector2& v, float scale);
    float Distance(const Vector2& a, const Vector2& b);
    float Dot(const Vector2& a, const Vector2& b);
    float Cross(const Vector2& a, const Vector2& b);
    bool IsCollinear(const Vector2& a, const Vector2& b, const Vector2& c);
    float PointToLineDistance(const Vector2& point, const Vector2& a, const Vector2& b);

    template<>
    Vector2 Lerp<Vector2>(const Vector2& a, const Vector2& b, float n);
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector2>(const Math::Vector2& message);
}