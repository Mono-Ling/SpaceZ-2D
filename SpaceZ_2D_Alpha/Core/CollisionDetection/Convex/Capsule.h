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
            Math::Vector2 up(0, halfH);
            Math::Vector2 down(0, -halfH);
            Math::Vector2 localDir = Math::Dot(up, dir) > Math::Dot(down, dir) ? up : down;
            return localDir + dir.Normalized() * _radius;
        }
    };
}