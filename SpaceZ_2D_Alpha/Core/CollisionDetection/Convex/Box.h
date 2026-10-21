#pragma once
#include "CollisionDetection/Convex/Convex.h"
#include "Math/Vector2.h"
#include "Math/Math.h"

namespace Core::Collision
{
    class Box : public Convex
    {
        // 接触边方向与法向夹角允许偏离 90° 的最大值（正弦，约 10°），超出则认为接触退化为角点
        static constexpr float EdgeTiltSin = 0.1736f;

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
        inline std::pair<Math::Vector2, Math::Vector2> GetEdgeLocal(const Math::Vector2& normal) const override
        {
            Math::Vector2 n = normal.Normalized();
            if (n == Math::Vector2::zero)
                return { Math::Vector2::zero, Math::Vector2::zero };

            Math::Vector2 d = -n;
            float tx = Math::Abs(d.x) > Math::Epsilon ? _extents.x / Math::Abs(d.x) : Math::Infinity;
            float ty = Math::Abs(d.y) > Math::Epsilon ? _extents.y / Math::Abs(d.y) : Math::Infinity;
            Math::Vector2 surface = d * Math::Min(tx, ty);

            // 仅当接触边近乎垂直于法向（边方向与法向夹角在 90°±10° 内）才输出完整边；
            if (tx < ty && Math::Abs(n.y) <= EdgeTiltSin)
            {
                float x = n.x > 0.0f ? -_extents.x : _extents.x;
                return
                {
                    { x, _extents.y },
                    { x, -_extents.y }
                };
            }
            if (ty < tx && Math::Abs(n.x) <= EdgeTiltSin)
            {
                float y = n.y > 0.0f ? -_extents.y : _extents.y;
                return
                {
                    { _extents.x, y },
                    { -_extents.x, y }
                };
            }
            return { surface, surface };
        }
    };
}