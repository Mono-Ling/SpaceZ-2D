#pragma once
#include "Core/Collision/Convex/Convex.h"
#include "Core/Math/Math.h"

namespace Core::Collision
{
    class Line : public Convex
    {
    private:
        float _length = 1.0f;

    public:
        inline void SetLine(float len) { _length = len >= 0.0f ? len : -len; }
        inline Math::Vector2 GuardMaxDistancePoint(const Math::Matrix3x3& local2world) const override
        {
            return Math::MulPoint(local2world, Math::Vector2(_length / 2.0f, 0.0f));
        }

    protected:
        inline Math::Vector2 SupportLocal(const Math::Vector2& dir) const override
        {
            float half = _length / 2.0f;
            Math::Vector2 left(-half, 0.0f);
            Math::Vector2 right(half, 0.0f);
            return Math::Dot(left, dir) > Math::Dot(right, dir) ? left : right;
        }
        /// @brief 获取边
        /// @param normal 内法线
        /// @return 线段端点
        inline std::pair<Math::Vector2, Math::Vector2> EdgeLocal(const Math::Vector2& normal) const override
        {
            auto n = normal.Normalized();
            float half = _length / 2.0f;
            Math::Vector2 left(-half, 0.0f);
            Math::Vector2 right(half, 0.0f);
            if(Math::Near(Math::Dot(Math::Vector2::up, n), 1.0f) || Math::Near(Math::Dot(Math::Vector2::down, n), 1.0f))
                return {left, right};
            auto point = Math::Dot(-left, n) > Math::Dot(-right, n) ? left : right;
            return {point, point};
        }
    };
}