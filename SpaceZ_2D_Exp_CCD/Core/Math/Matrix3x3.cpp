#include "Math/Matrix3x3.h"
#include <utility>
using namespace std;

namespace Core::Math
{
    Matrix3x3 Matrix3x3::Transpose() const
    {
        return Matrix3x3
        {
            Row<0>(),
            Row<1>(),
            Row<2>()
        };
    }
    void Matrix3x3::TransposeSelf()
    {
        swap(m01, m10);
        swap(m02, m20);
        swap(m12, m21);
    }
    Vector2 Matrix3x3::GetPosition() const
    {
        return Vector2(m02, m12);
    }
    float Matrix3x3::GetRotate() const
    {
        return Atan2(m10, m00);
    }
    Vector2 Matrix3x3::GetScale() const
    {
        return Vector2
        {
            Sqrt(m00 * m00 + m10 * m10),
            Sqrt(m01 * m01 + m11 * m11)
        };
    }
    Matrix2x2 Matrix3x3::ToMatrix2x2() const
    {
        Matrix2x2 m;
        m.m00 = m00; m.m01 = m01;
        m.m10 = m10; m.m11 = m11;
        return m;
    }

    Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b)
    {
        Vector3 colA
        {
            Dot(a.Row<0>(), b.Column<0>()),
            Dot(a.Row<1>(), b.Column<0>()),
            Dot(a.Row<2>(), b.Column<0>()),
        };
        Vector3 colB
        {
            Dot(a.Row<0>(), b.Column<1>()),
            Dot(a.Row<1>(), b.Column<1>()),
            Dot(a.Row<2>(), b.Column<1>()),
        };
        Vector3 colC
        {
            Dot(a.Row<0>(), b.Column<2>()),
            Dot(a.Row<1>(), b.Column<2>()),
            Dot(a.Row<2>(), b.Column<2>()),
        };
        return Matrix3x3(colA, colB, colC);
    }
    Vector3 operator*(const Matrix3x3& m, const Vector3& v)
    {
        return Vector3
        {
            Dot(m.Row<0>(), v),
            Dot(m.Row<1>(), v),
            Dot(m.Row<2>(), v),
        };
    }

    Matrix3x3 GetTranslateMatrix(const Vector2& pos)
    {
        Matrix3x3 m;
        m.m02 = pos.x;
        m.m12 = pos.y;
        m.m22 = 1.0f;
        return m;
    }
    Matrix3x3 GetRotateMatrix(float rot)
    {
        Matrix3x3 m;
        m.m00 = Cos(rot);
        m.m10 = Sin(rot);

        m.m01 = -m.m10;
        m.m11 = m.m00;
        return m;
    }
    Matrix3x3 GetScaleMatrix(const Vector2& scale)
    {
        Matrix3x3 m;
        m.m00 = scale.x;
        m.m11 = scale.y;
        return m;
    }
    Matrix3x3 TRS(const Vector2& pos, float rot, const Vector2& scale)
    {
        auto S = GetScaleMatrix(scale);
        auto R = GetRotateMatrix(rot);
        auto T = GetTranslateMatrix(pos);
        return T * R * S;
    }
    Matrix3x3 InverseTRS(const Vector2& pos, float rot, const Vector2& scale)
    {
        auto T = GetTranslateMatrix(-pos);

        Matrix2x2 R2x2(rot);
        R2x2.TransposeSelf();
        Matrix3x3 R(R2x2);

        auto S = GetScaleMatrix({1.0f / Max(scale.x, Epsilon), 1.0f / Max(scale.y, Epsilon)});

        return S * R * T;
    }
    Matrix3x3 TR(const Vector2& pos, float rot)
    {
        auto R = GetRotateMatrix(rot);
        auto T = GetTranslateMatrix(pos);
        return T * R;
    }
    Matrix3x3 InverseTR(const Vector2& pos, float rot)
    {
        auto T = GetTranslateMatrix(-pos);

        Matrix2x2 R2x2(rot);
        R2x2.TransposeSelf();
        Matrix3x3 R(R2x2);

        return R * T;
    }
}
namespace Core
{
    template<>
    string ToString<Math::Matrix3x3>(const Math::Matrix3x3& value)
    {
        return "[" + ToString(value.Row<0>()) + "]\n"
             + "[" + ToString(value.Row<1>()) + "]\n"
             + "[" + ToString(value.Row<2>()) + "]";
    }
}