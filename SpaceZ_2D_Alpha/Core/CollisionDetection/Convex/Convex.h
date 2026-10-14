#pragma once
#include "Math/Vector2.h"
#include "Math/Matrix3x3.h"

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

        virtual Math::Vector2 SupportLocal(const Math::Vector2& dir) const = 0;
        virtual Math::Vector2 SurfacePointLocal(const Math::Vector2& dir) const
        {
            return SupportLocal(dir);
        }
    };
}