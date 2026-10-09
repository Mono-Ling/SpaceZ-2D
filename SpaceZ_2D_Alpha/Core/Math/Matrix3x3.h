#pragma once
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Matrix2x2.h"
#include "ToString.h"
#include <string>

namespace Core::Math
{
    struct Matrix3x3
    {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;
        Matrix3x3() : m00(1.0f), m01(0.0f), m02(0.0f),
                       m10(0.0f), m11(1.0f), m12(0.0f),
                       m20(0.0f), m21(0.0f), m22(1.0f) {}
        Matrix3x3(const Vector3& a, const Vector3& b, const Vector3& c)
        : m00(a.x), m01(b.x), m02(c.x),
          m10(a.y), m11(b.y), m12(c.y),
          m20(a.z), m21(b.z), m22(c.z) {}
        Matrix3x3(const Matrix2x2& m)
        : m00(m.m00), m01(m.m01), m02(0),
          m10(m.m10), m11(m.m11), m12(0),
          m20(0), m21(0), m22(1) {}

        Matrix3x3 Transpose() const;
        void TransposeSelf();

        static const Matrix3x3 identity;
    };
    Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b);
    Vector3 operator*(const Matrix3x3& m, const Vector3& v);
    Vector2 MulPoint(const Matrix3x3& m, const Vector2& v);
    Vector2 MulVector(const Matrix3x3& m, const Vector2& v);

    Matrix3x3 GetTranslate(const Vector2& pos);
    Matrix3x3 GetRotate(float rot);
    Matrix3x3 GetScale(const Vector2& scale);
    Matrix3x3 TRS(const Vector2& pos, float rot, const Vector2& scale);
    Matrix3x3 InverseTRS(const Vector2& pos, float rot, const Vector2& scale);
}
namespace Core
{
    template<>
    std::string ToString<Math::Matrix3x3>(const Math::Matrix3x3& value);
}