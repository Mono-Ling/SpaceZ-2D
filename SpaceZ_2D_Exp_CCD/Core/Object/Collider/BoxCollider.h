#pragma once
#include "Object/Collider.h"
#include "Collision/Convex/Box.h"

namespace Core
{
    class BoxCollider : public Collider, private Collision::Box
    {
    public:
        BoxCollider(const ColliderHandle& handle) : Collider::Collider(handle) {}

        using Collision::Box::SetBox;
        inline Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const override
        {
            return Collision::Box::Support(dir, local2world, world2local);
        }
        inline const Collision::Convex* ConvexPtr() const override
        {
            return this;
        }
    };
}