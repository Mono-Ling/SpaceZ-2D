#pragma once
#include "Core/Object/PhyObject/PhyObject.h"
#include "Core/Collision/Geometry/Bound.h"
#include "Core/Collision/Layer/LayerMask.h"
#include "Core/Collision/Info/BroadPhaseCollisionPair.h"

namespace Core::Collision
{
    struct BVHObject
    {
        LayerMask layer;

        ColliderHandle collider;
        RigidbodyHandle body;

        Bound bound;

        BVHObject() = default;
        BVHObject(const ColliderHandle& coll, const RigidbodyHandle& body, const LayerMask& layer, const Bound& bound)
        : collider(coll), body(body), layer(layer), bound(bound) {}

        static const BVHObject null;
    };

    inline const BVHObject BVHObject::null{};

    inline bool operator==(const BVHObject& a, const BVHObject& b)
    {
        return a.collider == b.collider
            && a.body == b.body
            && a.layer == b.layer
            && a.bound == b.bound;
    }
    inline bool operator!=(const BVHObject& a, const BVHObject& b) { return !(a == b); }

    inline bool IsIntersect(const BVHObject& a, const BVHObject& b)
    {
        if(a.collider == b.collider || a.body == b.body)
            return false;
        return IsIntersect(a.layer, b.layer) && IsIntersect(a.bound, b.bound);
    }
    inline BroadPhaseCollisionPair CollisionPair(const BVHObject& a, const BVHObject& b)
    {
        BroadPhaseCollisionPair p;

        p.colliders.first = a.collider;
        p.colliders.second = b.collider;

        p.bodies.first = a.body;
        p.bodies.second = b.body;

        return p;
    }
}