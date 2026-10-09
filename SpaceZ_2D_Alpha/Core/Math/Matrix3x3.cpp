#include "Math/Matrix3x3.h"
#include "Math/Math.h"

namespace Core::Math
{
    const Matrix3x3 Matrix3x3::identity = Matrix3x3();

    Matrix3x3 Matrix3x3::Transpose() const
    {
        Matrix3x3 result;
        result.m00 = m00; result.m01 = m10; result.m02 = m20;
        result.m10 = m01; result.m11 = m11; result.m12 = m21;
        result.m20 = m02; result.m21 = m12; result.m22 = m22;
        return result;
    }
    void Matrix3x3::TransposeSelf()
    {
        float temp;
        temp = m01;
        m01 = m10;
        m10 = temp;

        temp = m02;
        m02 = m20;
        m20 = temp;

        temp = m12;
        m12 = m21;
        m21 = temp;
    }

    Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b)
    {
        Matrix3x3 result;
        result.m00 = a.m00 * b.m00 + a.m01 * b.m10 + a.m02 * b.m20;
        result.m01 = a.m00 * b.m01 + a.m01 * b.m11 + a.m02 * b.m21;
        result.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02 * b.m22;

        result.m10 = a.m10 * b.m00 + a.m11 * b.m10 + a.m12 * b.m20;
        result.m11 = a.m10 * b.m01 + a.m11 * b.m11 + a.m12 * b.m21;
        result.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12 * b.m22;

        result.m20 = a.m20 * b.m00 + a.m21 * b.m10 + a.m22 * b.m20;
        result.m21 = a.m20 * b.m01 + a.m21 * b.m11 + a.m22 * b.m21;
        result.m22 = a.m20 * b.m02 + a.m21 * b.m12 + a.m22 * b.m22;

        return result;
    }
    Vector3 operator*(const Matrix3x3& m, const Vector3& v)
    {
        return Vector3
        (
            m.m00 * v.x + m.m01 * v.y + m.m02 * v.z,
            m.m10 * v.x + m.m11 * v.y + m.m12 * v.z,
            m.m20 * v.x + m.m21 * v.y + m.m22 * v.z
        );
    }
    Vector2 MulPoint(const Matrix3x3& m, const Vector2& v)
    {
        return Vector2
        (
            m.m00 * v.x + m.m01 * v.y + m.m02,
            m.m10 * v.x + m.m11 * v.y + m.m12
        );
    }
    Vector2 MulVector(const Matrix3x3& m, const Vector2& v)
    {
        return Vector2
        (
            m.m00 * v.x + m.m01 * v.y,
            m.m10 * v.x + m.m11 * v.y
        );
    }

    Matrix3x3 GetTranslate(const Vector2& pos)
    {
        Matrix3x3 T;
        T.m02 = pos.x;
        T.m12 = pos.y;
        T.m22 = 1.0f;
        return T;
    }
    Matrix3x3 GetRotate(float rot)
    {
        Matrix3x3 R;
        R.m00 = Cos(rot); R.m01 = -Sin(rot);
        R.m10 = Sin(rot); R.m11 = Cos(rot);
        return R;
    }
    Matrix3x3 GetScale(const Vector2& scale)
    {
        Matrix3x3 S;
        S.m00 = scale.x;
        S.m11 = scale.y;
        return S;
    }
    Matrix3x3 TRS(const Vector2& pos, float rot, const Vector2& scale)
    {
        auto S = GetScale(scale);
        auto R = GetRotate(rot);
        auto T = GetTranslate(pos);
        return T * R * S;
    }
    Matrix3x3 InverseTRS(const Vector2& pos, float rot, const Vector2& scale)
    {
        auto S = GetScale(Vector2(1.0f / Max(Epsilon, scale.x), 1.0f / Max(Epsilon, scale.y)));
        auto R2x2 = Matrix2x2(rot);
        R2x2.TransposeSelf();
        auto R = Matrix3x3(R2x2);
        auto T = GetTranslate(-pos);
        return S * R * T;
    }
}
namespace Core
{
    template<>
    std::string ToString<Math::Matrix3x3>(const Math::Matrix3x3& value)
    {
        return "[" + ToString(value.m00) + ", " + ToString(value.m01) + ", " + ToString(value.m02) + "]\n"
               "[" + ToString(value.m10) + ", " + ToString(value.m11) + ", " + ToString(value.m12) + "]\n"
               "[" + ToString(value.m20) + ", " + ToString(value.m21) + ", " + ToString(value.m22) + "]";
    }
}