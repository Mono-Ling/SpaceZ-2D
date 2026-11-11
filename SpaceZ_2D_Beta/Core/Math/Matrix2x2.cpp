#include "Core/Math/Matrix2x2.h"
#include <utility>
using namespace std;

namespace Core::Math
{
    Matrix2x2::Matrix2x2(float angle)
    {
        m00 = Cos(angle);
        m10 = Sin(angle);

        m01 = -m10;
        m11 = m00;
    }
    Matrix2x2 Matrix2x2::Transpose() const
    {
        return {Row<0>(), Row<1>()};
    }
    Matrix2x2& Matrix2x2::TransposeSelf()
    {
        swap(m10, m01);
        return *this;
    }
    float Matrix2x2::Det() const
    {
        return m00 * m11 - m10 * m01;
    }

    Matrix2x2 operator*(const Matrix2x2& a, const Matrix2x2& b)
    {
        return Matrix2x2
        {
            {
                Dot(a.Row<0>(), b.Column<0>()),
                Dot(a.Row<1>(), b.Column<0>()),
            },
            {
                Dot(a.Row<0>(), b.Column<1>()),
                Dot(a.Row<1>(), b.Column<1>()),
            },
        };
    }
    Vector2 operator*(const Matrix2x2& m, const Vector2& v)
    {
        return Vector2
        {
            Dot(m.Row<0>(), v),
            Dot(m.Row<1>(), v),
        };
    }
}
namespace Core
{
    template<>
    std::string ToString<Math::Matrix2x2>(const Math::Matrix2x2& value)
    {
        return StringFormat
        (
            "[", value.Row<0>(), "]\n",
            "[", value.Row<1>(), "]"
        );
    }
}