#pragma once
#include "Object/Collider.h"
#include "CollisionDetection/Convex/Box.h"

namespace Core
{
    class BoxCollider : public Collider
    {
        Collision::Box _box;

    public:
        BoxCollider(const ColliderHandle& handle) : Collider(handle) {}

        inline Math::Vector2 Support(const Math::Vector2& dir) const override
        {
            return _box.Support(dir, _transform.GetWorldToLocalMatrix(), _transform.GetLocalToWorldMatrix());
        }
        inline Collision::ConvexDetectReq GetConvexDetectReq() const override
        {
            return Collision::ConvexDetectReq{&_box, _transform.GetWorldToLocalMatrix(), _transform.GetLocalToWorldMatrix()};
        }

        inline void SetBox(const Math::Vector2& extents)
        {
            _box.SetBox(extents);
            UpdateBound();
        }
    };
}