#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include <utility>

namespace Core::Collision
{
    struct BroadPhaseCollisionPair
    {
        std::pair<ColliderHandle, ColliderHandle> colliders;
        std::pair<RigidbodyHandle, RigidbodyHandle> bodies;

        inline explicit operator bool() const
        {
            return colliders.first && colliders.second
                && (bodies.first || bodies.second);
        }
    };
}