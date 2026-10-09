#include "Math/Matrix2x2.h"
#include "Math/Math.h"

namespace Core::Math
{
    const Matrix2x2 Matrix2x2::identity = Matrix2x2();

    Matrix2x2::Matrix2x2(float angle)
    {
        m00 = Cos(angle); m01 = -Sin(angle);
        m10 = Sin(angle); m11 = Cos(angle);
    }
    Matrix2x2 Matrix2x2::Transpose() const
    {
        Matrix2x2 m;
        m.m00 = m00; m.m01 = m10;
        m.m10 = m01; m.m11 = m11;
        return m;
    }
    void Matrix2x2::TransposeSelf()
    {
        float temp = m01;
        m01 = m10;
        m10 = temp;
    }

    Matrix2x2 operator*(const Matrix2x2& a, const Matrix2x2& b)
    {
        Matrix2x2 m;
        m.m00 = a.m00 * b.m00 + a.m01 * b.m10;
        m.m01 = a.m00 * b.m01 + a.m01 * b.m11;

        m.m10 = a.m10 * b.m00 + a.m11 * b.m10;
        m.m11 = a.m10 * b.m01 + a.m11 * b.m11;
        return m;
    }
    Vector2 operator*(const Matrix2x2& m, const Vector2& v)
    {
        Vector2 ans;
        ans.x = m.m00 * v.x + m.m01 * v.y;
        ans.y = m.m10 * v.x + m.m11 * v.y;
        return ans;
    }
}
namespace Core
{
    template<>
    std::string ToString<Core::Math::Matrix2x2>(const Core::Math::Matrix2x2& value)
    {
        return "[" + ToString(value.m00) + "," + ToString(value.m01) + "]\n"
             + "[" + ToString(value.m10) + "," + ToString(value.m11) + "]";
    }
}