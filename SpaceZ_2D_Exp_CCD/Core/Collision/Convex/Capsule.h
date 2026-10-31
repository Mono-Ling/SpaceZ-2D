#pragma once
#include "Collision/Convex/Convex.h"
#include "Math/Math.h"

namespace Core::Collision
{
    class Capsule : public Convex
    {
        float _radius = 0.5f;
        float _height = 1.0f;

    public:
        inline void SetCapsule(float r, float h)
        {
            _radius = Math::Abs(r);
            _height = Math::Abs(h);
        }

    protected:
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) const override
        {
            float halfH = _height / 2.0f;
            float y = dir.y > 0.0f ? halfH : (dir.y < 0.0f ? -halfH : 0.0f);
            return Math::Vector2(0.0f, y) + dir.Normalized() * _radius;
        }
        inline std::pair<Math::Vector2, Math::Vector2> EdgeLocal(const Math::Vector2& normal) const override
        {
            auto n = normal.Normalized();
            float halfH = _height / 2.0f;

            if(Math::Near(Math::Dot(Math::Vector2::left, n), 1.0f))
                return {{_radius, halfH}, {_radius, -halfH}};
            if(Math::Near(Math::Dot(Math::Vector2::right, n), 1.0f))
                return {{-_radius, halfH}, {-_radius, -halfH}};
                
            auto sup = SupportLocal(-n);
            return {sup, sup};
        }
    };
}