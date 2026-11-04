#pragma once
#include "Object/Collider.h"
#include "Collision/Convex/Sphere.h"

namespace Core
{
    class SphereCollider : public Collider, private Collision::Sphere
    {
    public:
        SphereCollider(const ColliderHandle& handle) : Collider::Collider(handle) {}

        using Collision::Sphere::SetSphere;
        inline Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const override
        {
            return Collision::Sphere::Support(dir, local2world, world2local);
        }
        inline const Collision::Convex* ConvexPtr() const override
        {
            return this;
        }
    };
}