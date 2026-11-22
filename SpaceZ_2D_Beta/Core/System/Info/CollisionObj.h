#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Tools/Hash.h"
#include <utility>

namespace Core
{
    struct CollisionObj
    {
        std::pair<ColliderHandle, ColliderHandle> colliders;
        std::pair<RigidbodyHandle, RigidbodyHandle> bodies;

        CollisionObj(const std::pair<ColliderHandle, ColliderHandle>& c, const std::pair<RigidbodyHandle, RigidbodyHandle>& b)
        : colliders(c), bodies(b)
        {
            if(colliders.second < colliders.first)
                std::swap(colliders.first, colliders.second);

            if(bodies.second < bodies.first)
                std::swap(bodies.first, bodies.second);
        }

        inline operator bool() const noexcept
        {
            return colliders.first && colliders.second
                && (bodies.first || bodies.second);
        }
        constexpr std::size_t GetHashCode() const
        {
            return HashCombine(colliders.first, colliders.second, bodies.first, bodies.second);
        }
    };
    inline bool operator==(const CollisionObj& a, const CollisionObj& b)
    {
        return a.colliders == b.colliders && a.bodies == b.bodies;
    }
    inline bool operator!=(const CollisionObj& a, const CollisionObj& b)
    {
        return !(a == b);
    }
}
namespace std
{
    template<>
    struct hash<Core::CollisionObj>
    {
        inline std::size_t operator()(const Core::CollisionObj& obj) const
        {
            return obj.GetHashCode();
        }
    };
}