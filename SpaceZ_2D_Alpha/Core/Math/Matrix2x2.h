#pragma once
#include "Math/Vector2.h"
#include "ToString.h"

namespace Core::Math
{
    struct Matrix2x2
    {
        float m00, m01;
        float m10, m11;

        Matrix2x2() : m00(1), m01(0),
                      m10(0), m11(1) {}
        Matrix2x2(const Vector2& a, const Vector2& b) : m00(a.x), m01(b.x),
                                                        m10(a.y), m11(b.y) {}
        explicit Matrix2x2(float angle);
        
        Matrix2x2 Transpose() const;
        void TransposeSelf();

        static const Matrix2x2 identity;
    };
    Matrix2x2 operator*(const Matrix2x2& a, const Matrix2x2& b);
    Vector2 operator*(const Matrix2x2& m, const Vector2& v);
}
namespace Core
{
    template<>
    std::string ToString<Core::Math::Matrix2x2>(const Core::Math::Matrix2x2& value);
}