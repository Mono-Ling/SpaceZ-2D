#pragma once
#include "Collision/Geometry/Simplex.h"
#include "Collision/Geometry/Polygon.h"
#include "Collision/NarrowPhase/ConvexDetectReq.h"
#include "Collision/Info/CollisionInfo.h"
#include <array>
#include <vector>

namespace Core::Collision
{
    constexpr int MAX_ITERATION = 36;
    
    bool GJK(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex);
    CollisionInfo EPA(const ConvexDetectReq& first, const ConvexDetectReq& second, const std::array<Math::Vector2, 3>& points);
    std::vector<CollisionInfo> ContactManifold(const ConvexDetectReq& first, const ConvexDetectReq& second, const CollisionInfo& info);
}