#pragma once
#include "CollisionDetection/Convex/Convex.h"
#include "Math/Vector2.h"
#include "Math/Math.h"

namespace Core::Collision
{
    class Capsule : public Convex
    {
        float _height = 1.0f;
        float _radius = 1.0f;

    public:
        inline void SetCapsule(float h, float r)
        {
            _height = Math::Abs(h);
            _radius = Math::Abs(r);
        }
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) override
        {
            float halfH = _height / 2.0f;
            float y = dir.y > 0.0f ? halfH : (dir.y < 0.0f ? -halfH : 0.0f);
            return Math::Vector2(0.0f, y) + dir.Normalized() * _radius;
        }
        inline Math::Vector2 SurfacePointLocal(const Math::Vector2& dir) override
        {
            Math::Vector2 d = dir.Normalized();
            if (Math::Abs(d.x) < Math::Epsilon && Math::Abs(d.y) < Math::Epsilon)
                return Math::Vector2::zero;

            float halfH = _height / 2.0f;

            // 射线与侧壁 x = ±radius 相交，交点高度 y = radius * tan(倾角)
            if (Math::Abs(d.x) > Math::Epsilon)
            {
                float y = _radius * d.y / Math::Abs(d.x);
                if (Math::Abs(y) <= halfH)
                    return Math::Vector2(d.x > 0.0f ? _radius : -_radius, y);
            }

            // 射线从端半圆出射：解 |t * d - c|^2 = radius^2（c 为端半圆圆心）取远根
            float dy = Math::Abs(d.y);
            float disc = Math::Max(_radius * _radius - halfH * halfH * d.x * d.x, 0.0f);
            float t = dy * halfH + Math::Sqrt(disc);
            return d * t;
        }
    };
}