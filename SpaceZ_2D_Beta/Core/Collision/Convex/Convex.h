#pragma once
#include "Core/Math/Vector2.h"
#include "Core/Math/Matrix3x3.h"
#include <utility>

namespace Core::Collision
{
    class Convex
    {
    public:
        inline Math::Vector2 Support(const Math::Vector2& dir, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2loacl) const
        {
            auto localDir = Math::MulVector(world2loacl, dir);
            return Math::MulPoint(local2world, SupportLocal(localDir));
        }
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

        virtual Math::Vector2 GuardMaxDistancePoint(const Math::Matrix3x3& local2world) const = 0;
        
    protected:
        virtual Math::Vector2 SupportLocal(const Math::Vector2& dir) const = 0;
        virtual std::pair<Math::Vector2, Math::Vector2> EdgeLocal(const Math::Vector2& normal) const = 0;
    };
}