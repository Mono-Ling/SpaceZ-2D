#pragma once
#include "Collision/Convex/Convex.h"
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
                Math::Abs(extents.y),
            };
        }

    protected:
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) const override
        {
            return Math::Vector2
            {
                dir.x > 0 ? _extents.x : -_extents.x,
                dir.y > 0 ? _extents.y : -_extents.y
            };
        }
        inline std::pair<Math::Vector2, Math::Vector2> EdgeLocal(const Math::Vector2& normal) const override
        {
            auto n = normal.Normalized();
            Math::Vector2 A(-_extents.x, _extents.y);
            Math::Vector2 B(_extents.x, _extents.y);
            Math::Vector2 C(_extents.x, -_extents.y);
            Math::Vector2 D(-_extents.x, -_extents.y);

            if(Math::Near(Math::Dot(Math::Vector2::down, n), 1.0f))
                return {A, B};
            if(Math::Near(Math::Dot(Math::Vector2::left, n), 1.0f))
                return {B, C};
            if(Math::Near(Math::Dot(Math::Vector2::up, n), 1.0f))
                return {C, D};
            if(Math::Near(Math::Dot(Math::Vector2::right, n), 1.0f))
                return {D, A};

            float dotA = Math::Dot(-A, n);
            float dotB = Math::Dot(-B, n);
            float dotC = Math::Dot(-C, n);
            float dotD = Math::Dot(-D, n);
            float dotMax = Math::Max(Math::Max(dotA, dotB), Math::Max(dotC, dotD));

            if(Math::Near(dotA, dotMax))
                return {A, A};
            if(Math::Near(dotB, dotMax))
                return {B, B};
            if(Math::Near(dotC, dotMax))
                return {C, C};
            return {D, D};
        }
    };
}