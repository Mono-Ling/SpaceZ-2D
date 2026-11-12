#pragma once
#include "Core/Math/Vector2.h"
#include "Core/Math/Matrix3x3.h"
#include <memory>

namespace Core
{
    class Transform
    {
    private:
        mutable bool _dirty = true;

        Math::Vector2 _position;
        float _rotation = 0.0f;
        Math::Vector2 _scale = Math::Vector2::one;

        std::weak_ptr<Transform> _parent;

        mutable Math::Matrix3x3 _local2parent;
        mutable Math::Matrix3x3 _parent2local;
        mutable Math::Matrix3x3 _local2world;
        mutable Math::Matrix3x3 _world2local;

    public:
        Math::Matrix3x3 LocalToWorldMatrix() const;
        Math::Matrix3x3 WorldToLocalMatrix() const;

        Math::Vector2 Position() const;
        float Rotation() const;
        Math::Vector2 Scale() const;

        void LocalPosition(const Math::Vector2& pos);
        void LocalRotation(float rot);
        void LocalScale(const Math::Vector2& scale);

        void SetParent(const std::shared_ptr<Transform>& parent);

    private:
        void UpdateMatrix() const;
    };
}