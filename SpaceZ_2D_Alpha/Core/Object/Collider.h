#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include "CollisionDetection/Geometry/Bound.h"

namespace Core
{
    class Collider
    {
    public:
        float staticFriction;
        float dynamicFriction;
        float elasticity;

        float angle;
        Math::Vector2 position;

    private:
        RigidbodyHandle _body;
        ColliderHandle _handle;

        Collision::Bound _bound;
        
    public:
        Collider(const ColliderHandle& handle) : _handle(handle) {}
        
        constexpr ColliderHandle Handle() const { return _handle; }
        constexpr RigidbodyHandle Body() const { return _body; }
        constexpr void SetBody(const RigidbodyHandle& body) { _body = body; }
    };
}