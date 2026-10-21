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
        
        inline Math::Vector2 Support(const Math::Vector2& dir,
                                     const Math::Matrix3x3& worldToLocal,
                                     const Math::Matrix3x3& localToWorld) const
        {
            auto localDir = Math::MulVector(worldToLocal, dir);
            return Math::MulPoint(localToWorld, SupportLocal(localDir));
        }
        inline Math::Vector2 SurfacePoint(const Math::Vector2& dir,
                                          const Math::Matrix3x3& worldToLocal,
                                          const Math::Matrix3x3& localToWorld) const
        {
            auto localDir = Math::MulVector(worldToLocal, dir);
            return Math::MulPoint(localToWorld, SurfacePointLocal(localDir));
        }
        /// @brief 获取凸体边
        /// @param normal 法线（向内）
        /// @param localToWorld 
        /// @return 
        inline std::pair<Math::Vector2, Math::Vector2> GetEdge(const Math::Vector2& normal,
                                                               const Math::Matrix3x3& worldToLocal,
                                                               const Math::Matrix3x3& localToWorld) const
        {
            auto localN = Math::MulVector(worldToLocal, normal);
            auto localEdge = GetEdgeLocal(localN);
            return
            {
                Math::MulPoint(localToWorld, localEdge.first),
                Math::MulPoint(localToWorld, localEdge.second)
            };
        }

        virtual std::pair<Math::Vector2, Math::Vector2> GetEdgeLocal(const Math::Vector2& normal) const = 0;
        virtual Math::Vector2 SupportLocal(const Math::Vector2& dir) const = 0;
        virtual Math::Vector2 SurfacePointLocal(const Math::Vector2& dir) const
        {
            return SupportLocal(dir);
        }
    };
}