#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include "Math/Transform.h"

namespace Core
{
    class Rigidbody
    {
    private:
        RigidbodyHandle _handle;

    public:
        float mass = 1.0f;
        float inertia = 1.0f;

        float angularVelocity = 0.0f;

        Math::Vector2 linearVelocity;

    private:
        Math::Transform _transform;

    public:
        Rigidbody(RigidbodyHandle handle) : _handle(handle) {}

        constexpr RigidbodyHandle Handle() const { return _handle; }

        inline Math::Vector2 Position() const { return _transform.WorldPosition(); }
        inline float Rotation() const { return _transform.WorldRotation(); }
        inline const Math::Transform& Transform() const { return _transform; }

        inline void Position(const Math::Vector2& pos) { _transform.SetWorldPosition(pos); }
        inline void Rotation(float rot) { _transform.SetWorldRotation(rot); }

        inline void Step(float dt)
        {
            auto dx = linearVelocity * dt;
            auto da = angularVelocity * dt;
            _transform.SetWorldPosition(_transform.WorldPosition() + dx);
            _transform.SetWorldRotation(_transform.WorldRotation() + da);
        }
    };
}
