#pragma once
#include "Core/Object/PhyObject/Collider.h"
#include "Core/Collision/Convex/Convex.h"

namespace Core
{
    template<typename T>
    requires std::derived_from<T, Collision::Convex>
    class ConvexCollider : public Collider, public T
    {
    public:
        using Collider::Collider;

    protected:
        inline Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const override
        {
            return T::Support(dir, local2world, world2local);
        }
        inline const Collision::Convex* ConvexPtr() const override
        {
            return this;
        }
    };
}
#include "Core/Object/PhyObject/Collider/ConvexCollider.tpp"