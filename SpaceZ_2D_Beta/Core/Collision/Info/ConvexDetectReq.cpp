#include "Core/Collision/Info/ConvexDetectReq.h"
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    ConvexDetectReq::operator bool() const { return _convex; }
    Vector2 ConvexDetectReq::Support(const Vector2& dir) const
    {
        if(!_convex)
            return Vector2::zero;
        return _convex->Support(dir, _localToWorld, _worldToLocal);
    }
    pair<Vector2, Vector2> ConvexDetectReq::Edge(const Vector2& normal) const
    {
        if(!_convex)
            return {Vector2::zero, Vector2::zero};
        return _convex->Edge(normal, _localToWorld);
    }
    Vector2 ConvexDetectReq::Position() const
    {
        if(!_convex)
            return Vector2::zero;
        return _localToWorld.GetPosition();
    }
    Vector2 ConvexDetectReq::GuardMaxDistancePoint() const
    {
        if(!_convex)
            return Vector2::zero;
        return _convex->GuardMaxDistancePoint(_localToWorld);
    }
    void ConvexDetectReq::Step(const Vector2& dx, float da)
    {
        // 局部轴对齐缩放下
        // deltaT * TRS * deltaR <=> deltaT * T * R * deltaR * S
        _localToWorld = GetTranslateMatrix(dx) * _localToWorld * GetRotateMatrix(da);
        _worldToLocal = GetRotateMatrix(-da) * _worldToLocal * GetTranslateMatrix(-dx);
    }

    Vector2 MinkowskiDiff(const Vector2 dir, const ConvexDetectReq& a, const ConvexDetectReq& b)
    {
        if(!a || !b)
            return Vector2::zero;
        return a.Support(dir) - b.Support(-dir);
    }
}