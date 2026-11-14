#include "Core/Math/Vector2.h"

namespace Core::Math
{
    void Vector2::operator+=(const Vector2& other)
    {
        x += other.x;
        y += other.y;
    }
    void Vector2::operator-=(const Vector2& other)
    {
        x -= other.x;
        y -= other.y;
    }
    void Vector2::operator*=(float scale)
    {
        x *= scale;
        y *= scale;
    }
    void Vector2::operator/=(float scale)
    {
        if(Near(scale, 0.0f))
            scale = scale >= 0.0f ? Epsilon : -Epsilon;
        x /= scale;
        y /= scale;
    }
    Vector2 Vector2::operator-() const
    {
        return {-x, -y};
    }
    float Vector2::Length() const
    {
        return Sqrt(x * x + y * y);
    }
    float Vector2::SqrLength() const
    {
        return x * x + y * y;
    }
    Vector2 Vector2::Normalized() const
    {
        auto len = Length();
        if(len < Epsilon)
            return Vector2::zero;
        return *this / len;
    }
    Vector2 Vector2::ProjectionVector(const Vector2& axis) const
    {
        auto dir = axis.Normalized();
        return Dot(*this, dir) * dir;
    }
    float Vector2::ProjectOffsetOnAxis(const Vector2& origin, const Vector2& axis) const
    {
        return Dot(*this - origin, axis.Normalized());
    }
    Vector2 Vector2::Perp() const
    {
        return { -y , x };
    }

    bool operator==(const Vector2& a, const Vector2& b)
    {
        return Near(a.x, b.x) && Near(a.y, b.y);
    }
    bool operator!=(const Vector2& a, const Vector2& b)
    {
        return !(a == b);
    }
    Vector2 operator+(const Vector2& a, const Vector2& b)
    {
        return Vector2
        {
            a.x + b.x,
            a.y + b.y,
        };
    }
    Vector2 operator-(const Vector2& a, const Vector2& b)
    {
        return Vector2
        {
            a.x - b.x,
            a.y - b.y,
        };
    }
    Vector2 operator*(const Vector2& v, float scale)
    {
        return Vector2
        {
            v.x * scale,
            v.y * scale,
        };
    }
    Vector2 operator*(float scale, const Vector2& v)
    {
        return v * scale;
    }
    Vector2 operator/(const Vector2& v, float scale)
    {
        if(Near(scale, 0.0f))
            scale = scale >= 0.0f ? Epsilon : -Epsilon;
        return Vector2
        {
            v.x / scale,
            v.y / scale,
        };
    }
    float Distance(const Vector2& a, const Vector2& b)
    {
        return (b - a).Length();
    }
    float Dot(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }
    float Cross(const Vector2& a, const Vector2& b)
    {
        return a.x * b.y - a.y * b.x;
    }
    bool IsCollinear(const Vector2& a, const Vector2& b, const Vector2& c)
    {
        return Near(Cross(b - a, c - a), 0.0f);
    }
    float PointToLineDistance(const Vector2& point, const Vector2& a, const Vector2& b)
    {
        if(a == b)
            return Distance(point, a);
        auto AB = b - a;
        return Abs(Cross(point - a, AB)) / AB.Length();
    }
    float PointToLineSegDistance(const Vector2& point, const Vector2& a, const Vector2& b)
    {
        if(a == b)
            return Distance(point, a);

        auto AB = b - a;
        if(LessNear(Dot(point - a, AB), 0.0f))
            return Distance(point, a);
        if(LessNear(Dot(point - b, a - b), 0.0f))
            return Distance(point, b);
        return Abs(Cross(point - a, AB)) / AB.Length();
    }

    template<>
    Vector2 Min<Vector2>(const Vector2& a, const Vector2& b)
    {
        return Vector2
        {
            Min(a.x, b.x),
            Min(a.y, b.y),
        };
    }
    template<>
    Vector2 Max<Vector2>(const Vector2& a, const Vector2& b)
    {
        return Vector2
        {
            Max(a.x, b.x),
            Max(a.y, b.y),
        };
    }
    template<>
    Vector2 Lerp<Vector2>(const Vector2& a, const Vector2& b, float n)
    {
        return Vector2
        {
            Lerp(a.x, b.x, n),
            Lerp(a.y, b.y, n),
        };
    }
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector2>(const Math::Vector2& value)
    {
        return StringFormat("(", value.x, ", ", value.y, ")");
    }
}