#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Collision/Info/CollisionInfo.h"
#include <utility>
#include <vector>

namespace Core
{
    struct CollisionPair
    {
        std::pair<ColliderHandle, ColliderHandle> colliders;
        std::pair<RigidbodyHandle, RigidbodyHandle> bodies;
        std::vector<Collision::CollisionInfo> infos;

        inline operator bool() const
        {
            return colliders.first && colliders.second
                && (bodies.first || bodies.second)
                && !infos.empty();
        }
    };
}