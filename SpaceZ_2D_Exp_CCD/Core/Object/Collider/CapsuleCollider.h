#pragma once
#include "Object/Collider.h"
#include "Collision/Convex/Capsule.h"

namespace Core
{
    class CapsuleCollider : public Collider, private Collision::Capsule
    {
    public:
        CapsuleCollider(const ColliderHandle& handle) : Collider::Collider(handle) {}

        using Collision::Capsule::SetCapsule;
        inline Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const override
        {
            return Collision::Capsule::Support(dir, local2world, world2local);
        }
        inline const Collision::Convex* ConvexPtr() const override
        {
            return this;
        }
    };
}