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
        float mass;
        float inertia;

        float angle;
        float angularVelocity;

        Math::Vector2 position;
        Math::Vector2 linearVelocity;

        Rigidbody(RigidbodyHandle handle) : _handle(handle) {}

        constexpr RigidbodyHandle Handle() const { return _handle; }
    };
}
