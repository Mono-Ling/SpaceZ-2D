#pragma once
#include "Collision/Convex/Convex.h"
#include "Math/Math.h"

namespace Core::Collision
{
    class Sphere : public Convex
    {
        static const int SAMPLE_COUNT = 4;
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
            Math::Matrix2x2 S(local2world.GetScale());

            float maxDis = 0.0f;
            Math::Vector2 maxDisPoint;
            float step = Math::PI / (2.0f * SAMPLE_COUNT);
            float angle = 0.0f;

            for(int i = 0; i <= SAMPLE_COUNT; i++)
            {
                Math::Vector2 point(Math::Cos(angle) * _radius, Math::Sin(angle) * _radius);
                angle += step;

                float len = (S * point).SqrLength();
                if(len > maxDis)
                {
                    maxDis = len;
                    maxDisPoint = point;
                }
            }
            return Math::MulPoint(local2world, maxDisPoint);
        }
    };
}