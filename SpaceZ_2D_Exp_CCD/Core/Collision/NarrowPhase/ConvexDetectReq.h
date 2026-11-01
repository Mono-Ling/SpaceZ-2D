#pragma once
#include "Collision/Convex/Convex.h"
#include "Math/Matrix3x3.h"
#include <utility>

namespace Core::Collision
{
    struct ConvexDetectReq
    {
    private:
        Convex* _convex = nullptr;
        Math::Matrix3x3 _localToWorld;
        Math::Matrix3x3 _worldToLocal;

    public:
        ConvexDetectReq(Convex* convex, const Math::Matrix3x3& local2world, const Math::Matrix3x3& world2local)
        : _convex(convex), _localToWorld(local2world), _worldToLocal(world2local) {}

        explicit operator bool() const;
        Math::Vector2 Support(const Math::Vector2& dir) const;

        /// @brief 获取凸体边
        /// @param normal 内法线
        /// @return 线段端点
        std::pair<Math::Vector2, Math::Vector2> Edge(const Math::Vector2& normal) const;

        Math::Vector2 Position() const;
        Math::Vector2 GuardMaxDistancePoint() const;
        void Step(const Math::Vector2& dx, float da);
    };
    Math::Vector2 MinkowskiDiff(const Math::Vector2 dir, const ConvexDetectReq& a, const ConvexDetectReq& b);
}