#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"

namespace Core
{
    class Collider
    {
    private:
        ColliderHandle _handle;
        RigidbodyHandle _body = RigidbodyHandle::null;

    public:
        float staticFriction = 0;
        float dynamicFriction = 0;
        float elasticity = 0;

        float rotation = 0.0f;
        Math::Vector2 position;

        Collider(const ColliderHandle& handle) : _handle(handle) {}

        inline ColliderHandle Handle() const { return _handle; }
        inline RigidbodyHandle Body() const { return _body; }
        inline void Body(const RigidbodyHandle& body) { _body = body; }
        inline Math::Vector2 Position() const { return position; }
        inline float Rotation() const { return rotation; }
    };
}