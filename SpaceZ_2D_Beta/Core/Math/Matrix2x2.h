#pragma once
#include "Core/Math/Vector2.h"
#include "Core/Debug/ToString.h"

namespace Core::Math
{
    struct Matrix2x2
    {
        float m00, m01;
        float m10, m11;

        constexpr Matrix2x2()
        : m00(1.0f), m01(0.0f),
          m10(0.0f), m11(1.0f) {}
        constexpr Matrix2x2(const Vector2& colA, const Vector2& colB)
        : m00(colA.x), m01(colB.x),
          m10(colA.y), m11(colB.y) {}
        constexpr Matrix2x2(float m00, float m01, float m10, float m11)
        : m00(m00), m01(m01),
          m10(m10), m11(m11) {}

        explicit Matrix2x2(float angle);
        explicit Matrix2x2(const Vector2& scale)
        :   m00(scale.x), m01(0.0f),
            m10(0.0f), m11(scale.y) {}
        
        Matrix2x2 Transpose() const;
        Matrix2x2& TransposeSelf();
        float Det() const;

        template<int row>
        Vector2  Row() const = delete;
        template<int col>
        Vector2 Column() const = delete;
        
        template<>
        constexpr Vector2 Row<0>() const { return { m00, m01 }; }
        template<>
        constexpr Vector2 Row<1>() const { return { m10, m11 }; }

        template<>
        constexpr Vector2 Column<0>() const { return {m00, m10}; }
        template<>
        constexpr Vector2 Column<1>() const { return {m01, m11}; }

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