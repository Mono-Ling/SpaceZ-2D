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
                                     const Math::Matrix3x3& localToWorld)
        {
            auto localDir = Math::MulVector(worldToLocal, dir);
            return Math::MulVector(localToWorld, SupportLocal(localDir));
        }
        inline Math::Vector2 SurfacePoint(const Math::Vector2& dir,
                                          const Math::Matrix3x3& worldToLocal,
                                          const Math::Matrix3x3& localToWorld)
        {
            auto localDir = Math::MulVector(worldToLocal, dir);
            return Math::MulVector(localToWorld, SurfacePointLocal(localDir));
        }
        
        virtual Math::Vector2 SupportLocal(const Math::Vector2& dir) = 0;
        virtual Math::Vector2 SurfacePointLocal(const Math::Vector2& dir)
        {
            return SupportLocal(dir);
        }
    };
}