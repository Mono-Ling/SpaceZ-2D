#pragma once
#include "Math/Vector2.h"

namespace Core::Math
{
    struct Matrix2x2
    {
        float m00, m01;
        float m10, m11;

        constexpr Matrix2x2() :
            m00(1.0f), m01(0),
            m10(0), m11(1.0f) {}
        constexpr Matrix2x2(const Vector2& colA, const Vector2& colB) :
            m00(colA.x), m01(colB.x),
            m10(colA.y), m11(colB.y) {}

        explicit Matrix2x2(float angle);
        
        Matrix2x2 Transpose() const;
        void TransposeSelf();

        template<int row>
        Vector2  Row() const = delete;
        
        template<>
        constexpr Vector2 Row<0>() const { return { m00, m01 }; }
        template<>
        constexpr Vector2 Row<1>() const { return { m10, m11 }; }

        static const Matrix2x2 identity;
    };

    inline constexpr Matrix2x2 Matrix2x2::identity = Matrix2x2();

    Matrix2x2 operator*(const Matrix2x2& a, const Matrix2x2& b);
    Vector2 operator*(const Matrix2x2& m, const Vector2& v);
}
namespace Core
{
    template<>
    std::string ToString<Math::Matrix2x2>(const Math::Matrix2x2& value);
}