#include "Core/Math/Matrix3x3.h"
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
            Row<2>(),
        };
    }
    Matrix3x3& Matrix3x3::TransposeSelf()
    {
        swap(m01, m10);
        swap(m02, m20);
        swap(m12, m21);
        return *this;
    }
    float Matrix3x3::Det() const
    {
        return m00 * Cofactor<0,0>().Det()
             - m10 * Cofactor<1,0>().Det()
             + m20 * Cofactor<2,0>().Det();
    }
    Matrix3x3 Matrix3x3::Adjugate() const
    {
        return Matrix3x3
        {
            {
                Cofactor<0,0>().Det(),
                -Cofactor<0,1>().Det(),
                Cofactor<0,2>().Det(),
            },
            {
                -Cofactor<1,0>().Det(),
                Cofactor<1,1>().Det(),
                -Cofactor<1,2>().Det(),
            },
            {
                Cofactor<2,0>().Det(),
                -Cofactor<2,1>().Det(),
                Cofactor<2,2>().Det(),
            },
        };
    }
    Matrix3x3 Matrix3x3::Inverse() const
    {
        float det = Det();
        if(Near(det, 0.0f, Epsilon))
            det = det >= 0.0f ? Epsilon : -Epsilon;
        return 1 / det * Adjugate();
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

    template<>
    Vector3 Matrix3x3::Row<0>() const { return {m00, m01, m02}; }
    template<>
    Vector3 Matrix3x3::Row<1>() const { return {m10, m11, m12}; }
    template<>
    Vector3 Matrix3x3::Row<2>() const { return {m20, m21, m22}; }

    template<>
    Vector3 Matrix3x3::Column<0>() const { return { m00, m10, m20}; }
    template<>
    Vector3 Matrix3x3::Column<1>() const { return { m01, m11, m21}; }
    template<>
    Vector3 Matrix3x3::Column<2>() const { return { m02, m12, m22}; }

#pragma region 余子式
    template<>
    Matrix2x2 Matrix3x3::Cofactor<0,0>() const
    {
        return Matrix2x2
        {
            m11, m12,
            m21, m22,
        };
    }
    template<>
    Matrix2x2 Matrix3x3::Cofactor<0,1>() const
    {
        return Matrix2x2
        {
            m10, m12,
            m20, m22,
        };
    }
    template<>
    Matrix2x2 Matrix3x3::Cofactor<0,2>() const
    {
        return Matrix2x2
        {
            m10, m11,
            m20, m21,
        };
    }

    template<>
    Matrix2x2 Matrix3x3::Cofactor<1,0>() const
    {
        return Matrix2x2
        {
            m01, m02,
            m21, m22,
        };
    }
    template<>
    Matrix2x2 Matrix3x3::Cofactor<1,1>() const
    {
        return Matrix2x2
        {
            m00, m02,
            m20, m22,
        };
    }
    template<>
    Matrix2x2 Matrix3x3::Cofactor<1,2>() const
    {
        return Matrix2x2
        {
            m00, m01,
            m20, m21,
        };
    }

    template<>
    Matrix2x2 Matrix3x3::Cofactor<2,0>() const
    {
        return Matrix2x2
        {
            m01, m02,
            m11, m12,
        };
    }
    template<>
    Matrix2x2 Matrix3x3::Cofactor<2,1>() const
    {
        return Matrix2x2
        {
            m00, m02,
            m10, m12,
        };
    }
    template<>
    Matrix2x2 Matrix3x3::Cofactor<2,2>() const
    {
        return Matrix2x2
        {
            m00, m01,
            m10, m11,
        };
    }
#pragma endregion

    Matrix3x3 operator*(float n, const Matrix3x3& m)
    {
        return Matrix3x3
        {
            n * m.Column<0>(),
            n * m.Column<1>(),
            n * m.Column<2>(),
        };
    }
    Matrix3x3 operator*(const Matrix3x3& m, float n)
    {
        return n * m;
    }
    Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b)
    {
        return Matrix3x3
        {
            {
                Dot(a.Row<0>(), b.Column<0>()),
                Dot(a.Row<1>(), b.Column<0>()),
                Dot(a.Row<2>(), b.Column<0>()),
            },
            {
                Dot(a.Row<0>(), b.Column<1>()),
                Dot(a.Row<1>(), b.Column<1>()),
                Dot(a.Row<2>(), b.Column<1>()),
            },
            {
                Dot(a.Row<0>(), b.Column<2>()),
                Dot(a.Row<1>(), b.Column<2>()),
                Dot(a.Row<2>(), b.Column<2>()),
            },
        };
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
    Vector2 MulPoint(const Matrix3x3& m, const Vector2& v)
    {
        Vector3 point(v.x, v.y, 1.0f);
        point = m * point;
        return {point.x, point.y};
    }
    Vector2 MulVector(const Matrix3x3& m, const Vector2& v)
    {
        Vector3 point(v.x, v.y, 0.0f);
        point = m * point;
        return {point.x, point.y};
    }

    Matrix3x3 GetTranslateMatrix(const Vector2& pos)
    {
        Matrix3x3 m;
        m.m02 = pos.x;
        m.m12 = pos.y;
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
    std::string ToString<Math::Matrix3x3>(const Math::Matrix3x3& value)
    {
        return StringFormat
        (
            "[", value.Row<0>(), "]\n",
            "[", value.Row<1>(), "]\n",
            "[", value.Row<2>(), "]"
        );
    }
}