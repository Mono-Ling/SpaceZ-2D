#pragma once
#include "Math/Vector2.h"
#include "Math/Matrix3x3.h"

namespace Core::Math
{
    struct Transform
    {
    private:
        mutable bool _dirty = false;

        float _rotation = 0.0f;
        Vector2 _position = Vector2::zero;
        Vector2 _scale = Vector2::one;

        mutable Matrix3x3 _localToWorld = Matrix3x3::identity;
        mutable Matrix3x3 _localToParent = Matrix3x3::identity;
        Matrix3x3 _parentToWorld = Matrix3x3::identity;

        mutable Matrix3x3 _worldToLocal = Matrix3x3::identity;
        mutable Matrix3x3 _parentToLocal = Matrix3x3::identity;
        Matrix3x3 _worldToParent = Matrix3x3::identity;
    
    public:
        Vector2 WorldPosition() const;
        float WorldRotation() const;
        Vector2 WorldScale() const;

        void SetLocalPosition(const Vector2& pos);
        void SetLocalRotation(float rot);
        void SetLocalScale(const Vector2& scale);

        void SetParentTransform(const Transform& parent);

        Matrix3x3 GetLocalToWorldMatrix() const;
        Matrix3x3 GetWorldToLocalMatrix() const;

        Vector2 PointLocalToWorld(const Vector2& point) const;
        Vector2 DirLocalToWorld(const Vector2& dir) const;

        Vector2 PointWorldToLocal(const Vector2& point) const;
        Vector2 DirWorldToLocal(const Vector2& dir) const;
        Vector2 NormalWorldToLocal(const Vector2& normal) const;

        Vector2 PointWorldToLocalRT(const Vector2& point) const;

    private:
        void UpdateMatrix() const;
    };
}