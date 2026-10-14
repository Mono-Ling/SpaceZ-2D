#pragma once
#include "CollisionDetection/Convex/Convex.h"
#include "Math/Matrix3x3.h"
#include "Math/Vector2.h"

namespace Core::Collision
{
    struct ConvexDetectReq
    {
    private:
        const Convex* _convex = nullptr;
        Math::Matrix3x3 _worldToLocal;
        Math::Matrix3x3 _localToWorld;
    
    public:
        ConvexDetectReq(const Convex* ptr, const Math::Matrix3x3& world, const Math::Matrix3x3& local)
        : _convex(ptr), _worldToLocal(world), _localToWorld(local) {}

        inline bool IsEnable() const { return _convex; }
        inline Math::Vector2 Support(const Math::Vector2& dir) const
        {
            if(!_convex)
                return Math::Vector2::zero;
            return _convex->Support(dir, _worldToLocal, _localToWorld);
        }
        inline Math::Vector2 SurfacePoint(const Math::Vector2& dir) const
        {
            if(!_convex)
                return Math::Vector2::zero;
            return _convex->SurfacePoint(dir, _worldToLocal, _localToWorld);
        }
        inline Math::Vector2 WorldPosition() const
        {
            return _localToWorld.GetPosition();
        }
        inline Math::Vector2 PointWorldToLocalRT(const Math::Vector2& point) const
        {
            auto m = InverseTR(_localToWorld.GetPosition(), _localToWorld.GetRotate());
            return MulPoint(m, point);
        }
    };
    inline Math::Vector2 GetMinkowskiDiff(const Math::Vector2 dir, const ConvexDetectReq& a, const ConvexDetectReq& b)
    {
        if(!a.IsEnable() || !b.IsEnable())
            return Math::Vector2::zero;
        return a.Support(dir) - b.Support(-dir);
    }
}