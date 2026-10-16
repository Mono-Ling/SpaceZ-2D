#pragma once
#include "Object/Object.h"
#include "Math/Vector2.h"
#include <utility>

namespace Core::Collision
{
    struct CollisionObj
    {
        ColliderHandle collider = ColliderHandle::null;
        RigidbodyHandle body = RigidbodyHandle::null;
        
        CollisionObj(const ColliderHandle& collider, const RigidbodyHandle& body)
        : collider(collider), body(body) {}
    };
    
    struct CollisionPair
    {
        CollisionObj first;
        CollisionObj second;

        float depth = 0;
        Math::Vector2 normal = Math::Vector2::zero; // first -> second
        Math::Vector2 point = Math::Vector2::zero;
        std::pair<Math::Vector2, Math::Vector2> anchorPoint;

        CollisionPair(const CollisionObj& first, const CollisionObj& second)
        : first(first), second(second) {}

        inline void SetCollisionInfo(const Math::Vector2& n, const Math::Vector2& p)
        {
            normal = n;
            point = p;
        }
    };
}
