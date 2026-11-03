#pragma once
#include "Collision/Geometry/Bound.h"
#include "Object/Object.h"
#include "Collision/Info/CollisionPair.h"

namespace Core::Collision
{
    struct BVHNodeObject
    {
        Bound bound;
        ColliderHandle handle;
        RigidbodyHandle body;

        BVHNodeObject() = default;
        BVHNodeObject(const Bound& b, ColliderHandle h, RigidbodyHandle rb) : bound(b), handle(h), body(rb) {}

        static const BVHNodeObject null;
    };
    inline const BVHNodeObject BVHNodeObject::null{Bound(), ColliderHandle::null, RigidbodyHandle::null};

    inline bool operator==(const BVHNodeObject& a, const BVHNodeObject& b)
    {
        return a.body == b.body && a.handle == b.handle && a.bound == b.bound;
    }
    inline bool operator!=(const BVHNodeObject& a, const BVHNodeObject& b)
    {
        return !(a == b);
    }
    inline bool IsIntersect(const BVHNodeObject& a, const BVHNodeObject& b)
    {
        if(a.handle == b.handle || a.body == b.body)
            return false;
        return IsIntersect(a.bound, b.bound);
    }
    inline CollisionPair CreateCollisionPair(const BVHNodeObject& a, const BVHNodeObject& b)
    {
        CollisionPair ans;
        ans.collider = {a.handle, b.handle};
        ans.body = {a.body, b.body};
        return ans;
    }
}