#pragma once
#include "Object/Object.h"
#include <utility>

namespace Core::Collision
{
    struct CollisionPair
    {
        std::pair<ColliderHandle, ColliderHandle> collider;
        std::pair<RigidbodyHandle, RigidbodyHandle> body;

        inline explicit operator bool() const
        {
            return static_cast<bool>(collider.first) && static_cast<bool>(collider.second)
                && (static_cast<bool>(body.first) || static_cast<bool>(body.second));
        }
    };
}