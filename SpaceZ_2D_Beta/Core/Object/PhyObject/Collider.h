#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Object/PhyObject/Transform.h"

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

        Transform transform;

        Collider(const ColliderHandle& handle) : _handle(handle) {}
        virtual ~Collider() = default;

        inline ColliderHandle Handle() const { return _handle; }
        inline RigidbodyHandle Body() const { return _body; }
        inline void Body(const RigidbodyHandle& body) { _body = body; }
        inline Math::Vector2 Position() const { return transform.Position(); }
        inline float Rotation() const { return transform.Rotation(); }
    };
}