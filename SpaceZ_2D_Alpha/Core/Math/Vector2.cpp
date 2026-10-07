#include "Math/Vector2.h"
#include "Math/Math.h"

namespace Core::Math
{
    const Vector2 Vector2::zero  = Vector2(0.0f, 0.0f);
    const Vector2 Vector2::one   = Vector2(1.0f, 1.0f);
    const Vector2 Vector2::up    = Vector2(0.0f, 1.0f);
    const Vector2 Vector2::down  = Vector2(0.0f, -1.0f);
    const Vector2 Vector2::left  = Vector2(-1.0f, 0.0f);
    const Vector2 Vector2::right = Vector2(1.0f, 0.0f);

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
    void Vector2::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
    }
    void Vector2::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
    }
    Vector2 Vector2::operator-() const
    {
        return Vector2(-x, -y);
    }
    float Vector2::Length() const
    {
        return Sqrt(x * x + y * y);
    }
    float Vector2::LengthSquared() const
    {
        return x * x + y * y;
    }
    Vector2 Vector2::Normalized() const
    {
        float len = Length();
        if (len < Epsilon)
            return Vector2::zero;
        return Vector2(x / len, y / len);
    }
    Vector2 Vector2::ProjectionVector(const Vector2& axis) const
    {
        auto dir = axis.Normalized();
        return Dot(*this, dir) * dir;
    }
    Vector2 Vector2::RejectionVector(const Vector2& axis) const
    {
        return *this - ProjectionVector(axis);
    }

    Vector2 operator+(const Vector2& a, const Vector2& b)
    {
        return Vector2(a.x + b.x, a.y + b.y);
    }
    Vector2 operator-(const Vector2& a, const Vector2& b)
    {
        return Vector2(a.x - b.x, a.y - b.y);
    }
    Vector2 operator*(const Vector2& v, float scalar)
    {
        return Vector2(v.x * scalar, v.y * scalar);
    }
    Vector2 operator*(float scalar, const Vector2& v)
    {
        return Vector2(v.x * scalar, v.y * scalar);
    }
    Vector2 operator/(const Vector2& v, float scalar)
    {
        return Vector2(v.x / scalar, v.y / scalar);
    }
    bool operator==(const Vector2& a,const Vector2& b)
    {
        return (Abs(a.x - b.x) < Epsilon) && (Abs(a.y - b.y) < Epsilon);
    }
    bool operator!=(const Vector2& a,const Vector2& b)
    {
        return !(a == b);
    }

    float Distance(const Vector2& a, const Vector2& b)
    {
        return (a - b).Length();
    }
    float Dot(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }
    float Cross(const Vector2& a, const Vector2& b)
    {
        return a.x * b.y - a.y * b.x;
    }
    Vector2 Lerp(const Vector2& start, const Vector2& end, float t)
    {
        t = Clamp01(t);
        return start + (end - start) * t;
    }
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector2>(const Math::Vector2& value)
    {
        return "(" + ToString(value.x) + ", " + ToString(value.y) + ")";
    }
}