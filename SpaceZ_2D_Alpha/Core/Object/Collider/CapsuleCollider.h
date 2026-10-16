#pragma once
#include "Object/Collider.h"
#include "CollisionDetection/Convex/Capsule.h"

namespace Core
{
    class CapsuleCollider : public Collider
    {
        Collision::Capsule _capsule;

    public:
        CapsuleCollider(const ColliderHandle& handle) : Collider(handle) {}

        inline Math::Vector2 Support(const Math::Vector2& dir) const override
        {
            return _capsule.Support(dir, _transform.GetWorldToLocalMatrix(), _transform.GetLocalToWorldMatrix());
        }
        inline Collision::ConvexDetectReq GetConvexDetectReq() const override
        {
            return Collision::ConvexDetectReq{&_capsule, _transform.GetWorldToLocalMatrix(), _transform.GetLocalToWorldMatrix()};
        }

        inline void SetCapsule(float h, float r)
        {
            _capsule.SetCapsule(h, r);
            UpdateBound();
        }
    };
}