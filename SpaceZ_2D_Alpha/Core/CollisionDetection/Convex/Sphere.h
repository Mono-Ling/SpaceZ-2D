#pragma once
#include "CollisionDetection/Convex/Convex.h"
#include "Math/Vector2.h"
#include "Math/Math.h"

namespace Core::Collision
{
    class Sphere : public Convex
    {
        float _radius = 1.0f;

    public:
        inline void SetSphere(float r) { _radius = Math::Abs(r); }
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) override
        {
            return dir.Normalized() * _radius;
        }
    };
}