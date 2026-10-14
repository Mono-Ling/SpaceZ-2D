#pragma once
#include "CollisionDetection/Convex/Convex.h"
#include "Math/Vector2.h"
#include "Math/Math.h"

namespace Core::Collision
{
    class Box : public Convex
    {
        Math::Vector2 _extents = Math::Vector2::one;

    public:
        inline void SetBox(const Math::Vector2& extents)
        {
            _extents = Math::Vector2
            {
                Math::Abs(extents.x),
                Math::Abs(extents.y)
            };
        }
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) const override
        {
            return Math::Vector2
            {
                dir.x > 0 ? _extents.x : -_extents.x,
                dir.y > 0 ? _extents.y : -_extents.y
            };
        }
        inline Math::Vector2 SurfacePointLocal(const Math::Vector2& dir) const override
        {
            Math::Vector2 d = dir.Normalized();
            if (Math::Abs(d.x) < Math::Epsilon && Math::Abs(d.y) < Math::Epsilon)
                return Math::Vector2::zero;

            float tx = Math::Abs(d.x) > Math::Epsilon ? _extents.x / Math::Abs(d.x) : Math::Infinity;
            float ty = Math::Abs(d.y) > Math::Epsilon ? _extents.y / Math::Abs(d.y) : Math::Infinity;
            return d * Math::Min(tx, ty);
        }
    };
}