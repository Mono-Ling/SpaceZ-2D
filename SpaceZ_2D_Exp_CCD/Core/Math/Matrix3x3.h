#pragma once
#include "Math/Vector3.h"
#include "Math/Matrix2x2.h"
#include "Debug/ToString.h"

namespace Core::Math
{
    struct Matrix3x3
    {
        float m00, m01, m02;
        float m10, m11, m12;
        float m20, m21, m22;

        constexpr Matrix3x3() : 
            m00(1.0f), m01(0.0f), m02(0.0f),
            m10(0.0f), m11(1.0f), m12(0.0f),
            m20(0.0f), m21(0.0f), m22(1.0f) {}
        constexpr Matrix3x3(const Vector3& colA, const Vector3& colB, const Vector3& colC) :
            m00(colA.x), m01(colB.x), m02(colC.x),
            m10(colA.y), m11(colB.y), m12(colC.y),
            m20(colA.z), m21(colB.z), m22(colC.z) {}

        constexpr Matrix3x3(const Matrix2x2& m) :
            m00(m.m00), m01(m.m01), m02(0.0f),
            m10(m.m10), m11(m.m11), m12(0.0f),
            m20(0.0f), m21(0.0f), m22(1.0f) {}

        Matrix3x3 Transpose() const;
        void TransposeSelf();

        Vector2 GetPosition() const;
        float GetRotate() const;
        Vector2 GetScale() const;
        Matrix2x2 ToMatrix2x2() const;

        template<int row>
        Vector3 Row() const = delete;
        template<int col>
        Vector3 Column() const = delete;

        template<>
        constexpr Vector3 Row<0>() const { return Vector3(m00, m01, m02); }
        template<>
        constexpr Vector3 Row<1>() const { return Vector3(m10, m11, m12); }
        template<>
        constexpr Vector3 Row<2>() const { return Vector3(m20, m21, m22); }

        template<>
        constexpr Vector3 Column<0>() const { return Vector3(m00, m10, m20); }
        template<>
        constexpr Vector3 Column<1>() const { return Vector3(m01, m11, m21); }
        template<>
        constexpr Vector3 Column<2>() const { return Vector3(m02, m12, m22); }

        static const Matrix3x3 identity;
    };

    inline constexpr Matrix3x3 Matrix3x3::identity{};

    Matrix3x3 operator*(const Matrix3x3& a, const Matrix3x3& b);
    Vector3 operator*(const Matrix3x3& m, const Vector3& v);
    Vector2 MulPoint(const Matrix3x3& m, const Vector2& v);
    Vector2 MulVector(const Matrix3x3& m, const Vector2& v);

    Matrix3x3 GetTranslateMatrix(const Vector2& pos);
    Matrix3x3 GetRotateMatrix(float rot);
    Matrix3x3 GetScaleMatrix(const Vector2& scale);
    Matrix3x3 TRS(const Vector2& pos, float rot, const Vector2& scale);
    Matrix3x3 InverseTRS(const Vector2& pos, float rot, const Vector2& scale);
    Matrix3x3 TR(const Vector2& pos, float rot);
    Matrix3x3 InverseTR(const Vector2& pos, float rot);
}
namespace Core
{
    template <> std::string ToString<Math::Matrix3x3>(const Math::Matrix3x3& value);
}