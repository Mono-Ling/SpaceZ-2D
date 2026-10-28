#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"

namespace Core
{
    class Rigidbody
    {
    private:
        RigidbodyHandle _handle;

    public:
        float mass = 1.0f;
        float inertia = 1.0f;

        float angle;
        float angularVelocity;

        Math::Vector2 position;
        Math::Vector2 linearVelocity;

        Rigidbody(const RigidbodyHandle& handle) : _handle(handle) {}

        inline RigidbodyHandle Handle() const { return _handle; }
        inline Math::Vector2 Position() const { return position; }
        inline float Rotation() const { return angle; }
    };
}