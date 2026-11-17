#pragma once
#include "Core/Collision/Info/ConvexDetectReq.h"
#include "Core/Collision/Geometry/Simplex.h"
#include "Core/Collision/Info/CollisionInfo.h"
#include "Core/Collision/Info/ContinueDetectReq.h"
#include <array>
#include <vector>

namespace Core::Collision
{
    constexpr int MAX_ITERATION = 36;

    bool GJK(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex);
    CollisionInfo EPA(const ConvexDetectReq& first, const ConvexDetectReq& second, const std::array<Math::Vector2, 3>& points);
    std::vector<CollisionInfo> ContactManifold(const ConvexDetectReq& first, const ConvexDetectReq& second, const CollisionInfo& info);

    bool ContinueCollision(ContinueDetectReq& req, float& time, std::vector<CollisionInfo>& infos);
}