#pragma once
#include "Core/System/Info/CollisionObj.h"
#include "Core/System/Info/CollisionPair.h"
#include <unordered_map>
#include <vector>
#include <functional>

namespace Core
{
    class CollisionPairBuffer
    {
        std::unordered_map<CollisionObj, bool> _visibleMap;

        std::unordered_map<CollisionObj, std::vector<Collision::CollisionInfo>> _enterMap;
        std::unordered_map<CollisionObj, std::vector<Collision::CollisionInfo>> _stayMap;
        std::vector<CollisionObj> _exitList;
    
    public:
        void ResetVisible();
        void CopyPair(const CollisionPair& p);
        void MovePair(CollisionPair& p);
        void DiffExitPairs();

        std::size_t EnterPairsCount() const;
        std::size_t StayPairsCount() const;
        std::size_t ExitPairsCount() const;
        void ForeachEnterPairs(const std::function<void(const CollisionObj&, const std::vector<Collision::CollisionInfo>&)>& func) const;
        void ForeachStayPairs(const std::function<void(const CollisionObj&, const std::vector<Collision::CollisionInfo>&)>& func) const;
        void ForeachExitPairs(const std::function<void(const CollisionObj&)>& func) const;
    };
}