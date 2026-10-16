#pragma once
#include "Object/Collider.h"
#include "CollisionDetection/Convex/Sphere.h"

namespace Core
{
    class SphereCollider : public Collider
    {
        Collision::Sphere _sphere;
        
    public:
        SphereCollider(const ColliderHandle& handle) : Collider(handle) {}

        inline Math::Vector2 Support(const Math::Vector2& dir) const override
        {
            return _sphere.Support(dir, _transform.GetWorldToLocalMatrix(), _transform.GetLocalToWorldMatrix());
        }
        inline Collision::ConvexDetectReq GetConvexDetectReq() const override
        {
            return Collision::ConvexDetectReq{&_sphere, _transform.GetWorldToLocalMatrix(), _transform.GetLocalToWorldMatrix()};
        }

        inline void SetSphere(float r)
        {
            _sphere.SetSphere(r);
            UpdateBound();
        }
    };
}