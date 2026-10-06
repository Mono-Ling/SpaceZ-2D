#include "Math/Vector3.h"
#include "Math/Math.h"

namespace Core::Math
{
    const Vector3 Vector3::zero    = Vector3(0.0f, 0.0f, 0.0f);
    const Vector3 Vector3::one     = Vector3(1.0f, 1.0f, 1.0f);
    const Vector3 Vector3::up      = Vector3(0.0f, 1.0f, 0.0f);
    const Vector3 Vector3::right   = Vector3(1.0f, 0.0f, 0.0f);
    const Vector3 Vector3::forward = Vector3(0.0f, 0.0f, 1.0f);

    void Vector3::operator+=(const Vector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    void Vector3::operator-=(const Vector3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }
    void Vector3::operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
    }
    void Vector3::operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
    }
    Vector3 Vector3::operator-() const
    {
        return Vector3(-x, -y, -z);
    }
    float Vector3::Length() const
    {
        return Sqrt(x * x + y * y + z * z);
    }
    float Vector3::LengthSquared() const
    {
        return x * x + y * y + z * z;
    }
    Vector3 Vector3::Normalized() const
    {
        float len = Length();
        if (len < Epsilon)
            return Vector3::zero;
        return Vector3(x / len, y / len, z / len);
    }

    Vector3 operator+(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
    Vector3 operator-(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
    }
    Vector3 operator*(const Vector3& v, float scalar)
    {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
    Vector3 operator*(float scalar, const Vector3& v)
    {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }
    Vector3 operator/(const Vector3& v, float scalar)
    {
        return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
    }
    bool operator==(const Vector3& a, const Vector3& b)
    {
        return (Abs(a.x - b.x) < Epsilon)
            && (Abs(a.y - b.y) < Epsilon)
            && (Abs(a.z - b.z) < Epsilon);
    }
    bool operator!=(const Vector3& a, const Vector3& b)
    {
        return !(a == b);
    }
    float Distance(const Vector3& a, const Vector3& b)
    {
        return (a - b).Length();
    }
    float Dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }
    Vector3 Lerp(const Vector3& start, const Vector3& end, float t)
    {
        t = Clamp01(t);
        return start + (end - start) * t;
    }
}
namespace Core
{
    template<>
    std::string ToString<Math::Vector3>(const Math::Vector3& value)
    {
        return "(" + ToString(value.x) + ", " + ToString(value.y) + ", " + ToString(value.z) + ")";
    }
}