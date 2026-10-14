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
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) override
        {
            return Math::Vector2
            {
                dir.x > 0 ? _extents.x : -_extents.x,
                dir.y > 0 ? _extents.y : -_extents.y
            };
        }
    };
}