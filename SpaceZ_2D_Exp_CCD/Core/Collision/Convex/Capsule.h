#pragma once
#include "Collision/Convex/Convex.h"
#include "Math/Math.h"
#include "Math/Matrix2x2.h"

namespace Core::Collision
{
    class Capsule : public Convex
    {
        static const int SAMPLE_COUNT = 4;
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
        /// @brief 获取边
        /// @param normal 内法线
        /// @return 线段端点
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
        inline Math::Vector2 GuardMaxDistancePoint(const Math::Matrix3x3& local2world) const override
        {
            float halfH = _height / 2.0f;
            Math::Matrix2x2 S(local2world.GetScale());

            float maxDis = 0.0f;
            Math::Vector2 maxDisPoint;
            float step = Math::PI / (2.0f * SAMPLE_COUNT);
            float angle = 0.0f;
            for(int i = 0; i <= SAMPLE_COUNT; i++)
            {
                Math::Vector2 point(_radius * Math::Cos(angle), halfH + _radius * Math::Sin(angle));
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