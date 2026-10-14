#pragma once
#include "CollisionDetection/NarrowPhase/NarrowPhaseDetectPair.h"
#include "CollisionDetection/NarrowPhase/ConvexDetectReq.h"
#include "CollisionDetection/Geometry/Simplex.h"
#include <vector>

namespace Core::Collision
{
    class NarrowPhase
    {
        static constexpr int MAX_ITERATION = 36;
        std::vector<CollisionPair> _pairs;

    public:
        void ClearCollisionPairs();
        void SetDetectPairs(std::vector<NarrowPhaseDetectPair>& pairs);
        const std::vector<CollisionPair>& GetCollisionPairs() const;
    
    private:
        void CollisionDetection(NarrowPhaseDetectPair& p);
        bool CompleteSimplex(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex);
        bool GJK(const ConvexDetectReq& first, const ConvexDetectReq& second, Simplex& simplex);
        void EPA(const ConvexDetectReq& first, const ConvexDetectReq& second, const Simplex& simplex, CollisionPair& p);
    };
}