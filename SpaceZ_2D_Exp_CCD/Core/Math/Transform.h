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
        Vector2 _position;
        Vector2 _scale = Vector2::one;

        mutable Matrix3x3 _localToWorld;
        mutable Matrix3x3 _worldToLocal;
        Matrix3x3 _parentToWorld;
        Matrix3x3 _worldToParent;

    public:
        Matrix3x3 LocalToWorldMatrix() const;
        Matrix3x3 WorldToLocalMatrix() const;

        Vector2 Position() const;
        float Rotation() const;
        Vector2 Scale() const;

        void Position(const Vector2& pos);
        void Rotation(float rot);

        void LocalPosition(const Vector2& pos);
        void LocalRotation(float rot);
        void LocalScale(const Vector2& scale);

        void SetParentTransform(const Transform& parent);

    private:
        void UpdateMatrix() const;
    };
}