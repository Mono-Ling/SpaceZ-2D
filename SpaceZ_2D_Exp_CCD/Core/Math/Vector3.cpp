#include "Math/Vector3.h"
using namespace std;

namespace Core::Math
{
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
    float Vector3::SqrLength() const
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
        return Near(a.x, b.x) && Near(a.y, b.y) && Near(a.z, b.z);
    }
    bool operator!=(const Vector3& a, const Vector3& b)
    {
        return !(a == b);
    }
    float Distance(const Vector3& a, const Vector3& b)
    {
        return (a - b).Length();
    }

    template<>
    Vector3 Lerp<Vector3>(const Vector3& start, const Vector3& end, float t)
    {
        return
        {
            Lerp(start.x, end.x, t),
            Lerp(start.y, end.y, t),
            Lerp(start.z, end.z, t),
        };
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