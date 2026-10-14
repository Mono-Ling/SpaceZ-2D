#pragma once
#include "Collision/CollisionPair.h"
#include "CollisionDetection/NarrowPhase/ConvexDetectReq.h"

namespace Core::Collision
{
    struct NarrowPhaseDetectPair
    {
        bool isCollision = false;
        CollisionPair collisionPair;
        ConvexDetectReq first;
        ConvexDetectReq second;

        NarrowPhaseDetectPair(const ConvexDetectReq& first, const ConvexDetectReq& second, const CollisionPair& p)
        : first(first), second(second), collisionPair(p) {}
    };
}
