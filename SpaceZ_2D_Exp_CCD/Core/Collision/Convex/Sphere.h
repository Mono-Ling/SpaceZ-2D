#pragma once
#include "Collision/Convex/Convex.h"

namespace Core::Collision
{
    class Sphere : public Convex
    {
        float _radius = 0.5f;

    public:
        inline void SetSphere(float r) { _radius = r > 0 ? r : -r; }
        
    protected:
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) const override
        {
            return dir.Normalized() * _radius;
        }
        inline std::pair<Math::Vector2, Math::Vector2> EdgeLocal(const Math::Vector2& normal) const override
        {
            Math::Vector2 point((-normal).Normalized() * _radius);
            return {point, point};
        }
        inline Math::Vector2 GuardMaxDistancePoint(const Math::Matrix3x3& local2world) const override
        {
            auto scale = local2world.GetScale();
            auto& dir = scale.x > scale.y ? Math::Vector2::right : Math::Vector2::up;
            return Math::MulPoint(local2world, dir * _radius);
        }
    };
}