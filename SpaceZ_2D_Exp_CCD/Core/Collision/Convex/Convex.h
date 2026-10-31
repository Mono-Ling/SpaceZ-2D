#pragma once
#include "Math/Vector2.h"
#include "Math/Matrix3x3.h"
#include <utility>

namespace Core::Collision
{
    class Convex
    {
    public:
        virtual ~Convex() = default;

        inline Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local) const
        {
            auto localDir = Math::MulVector(world2local, dir);
            return Math::MulPoint(local2world, SupportLocal(localDir));
        }

        /// @brief 获取凸体边
        /// @param normal 内法线
        /// @return 线段端点
        inline std::pair<Math::Vector2, Math::Vector2> Edge(const Math::Vector2& normal, const Math::Matrix3x3& local2world) const
        {
            auto localN = Math::MulVector(local2world.Transpose(), normal);
            auto line = EdgeLocal(localN);
            return
            {
                Math::MulPoint(local2world, line.first),
                Math::MulPoint(local2world, line.second),
            };
        }

    protected:
        virtual Math::Vector2 SupportLocal(const Math::Vector2& dir) const = 0;

        /// @brief 获取本地空间凸体边
        /// @param normal 内法线
        /// @return 线段端点
        virtual std::pair<Math::Vector2, Math::Vector2> EdgeLocal(const Math::Vector2& normal) const = 0;
    };
}