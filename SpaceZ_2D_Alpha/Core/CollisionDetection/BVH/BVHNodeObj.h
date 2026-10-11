#pragma once
#include "Object/Object.h"
#include "CollisionDetection/Geometry/Bound.h"

namespace Core::Collision
{
    struct BVHNodeObj
    {
        ColliderHandle handle;
        RigidbodyHandle body;
        Bound bound;

        BVHNodeObj(const ColliderHandle& handle, const RigidbodyHandle& body, const Bound& bound)
        : handle(handle), body(body), bound(bound) {}
        
        static const BVHNodeObj null;
    };
    inline const BVHNodeObj BVHNodeObj::null{ColliderHandle::null, RigidbodyHandle::null, Bound()};

    inline bool operator==(const BVHNodeObj& a, const BVHNodeObj& b)
    {
        return a.handle == b.handle
            && a.body == b.body
            && a.bound == b.bound;
    }
    inline bool operator!=(const BVHNodeObj& a, const BVHNodeObj& b) { return !(a == b); }
    inline bool IsIntersect(const BVHNodeObj& a, const BVHNodeObj& b)
    {
        if(a.handle == b.handle || a.body == b.body)
            return false;
        return IsIntersect(a.bound, b.bound);
    }
}
