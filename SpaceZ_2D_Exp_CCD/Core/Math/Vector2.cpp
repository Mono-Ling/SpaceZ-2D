#include "Math/Vector2.h"
using namespace Core::Math;
using namespace std;

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
            scale = Epsilon * (scale > 0 ? 1.0f: -1.0f);
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
        if(Abs(len) < Epsilon)
            return Vector2::zero;
        return *this / len;
    }
    Vector2 Vector2::ProjectionVector(const Vector2& axis) const
    {
        auto dir = axis.Normalized();
        return Dot(*this, dir) * dir;
    }
    Vector2 Vector2::ProjectionPoint(const Vector2& origin, const Vector2& axis) const
    {
        auto offset = *this - origin;
        return origin + offset.ProjectionVector(axis);
    }
    Vector2 Vector2::Perp() const
    {
        return { -y , x };
    }

    bool operator==(const Vector2& a, const Vector2& b)
    {
        return Near(a.x, b.x) && Near(a.y , b.y);
    }
    bool operator!=(const Vector2& a, const Vector2& b)
    {
        return !(a == b);
    }
    Vector2 operator+(const Vector2& a, const Vector2& b)
    {
        return { a.x + b.x, a.y + b.y };
    }
    Vector2 operator-(const Vector2& a, const Vector2& b)
    {
        return { a.x - b.x, a.y - b.y };
    }
    Vector2 operator*(const Vector2& v, float scale)
    {
        return { v.x * scale, v.y * scale };
    }
    Vector2 operator*(float scale, const Vector2& v)
    {
        return v * scale;
    }
    Vector2 operator/(const Vector2& v, float scale)
    {
        if(Near(scale, 0.0f))
            scale = Epsilon * (scale > 0 ? 1.0f: -1.0f);
        return v * (1 / scale);
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
        return Abs(Cross(b - a, c - a)) < Epsilon;
    }
    float PointToLineDistance(const Vector2& point, const Vector2& a, const Vector2& b)
    {
        return Abs(Cross(point - a, b - a)) / Max((b - a).Length(), Epsilon);
    }

    template<>
    Vector2 Lerp<Vector2>(const Vector2& a, const Vector2& b, float n)
    {
        return
        {
            Lerp(a.x, b.x, n),
            Lerp(a.y , b.y, n),
        };
    }
}
namespace Core
{
    template<>
    string ToString<Vector2>(const Vector2& message)
    {
        return "(" + ToString(message.x) + "," + ToString(message.y) + ")";
    }
}