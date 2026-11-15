#pragma once
#include "Core/Collision/BroadPhase/BVHNode.h"
#include "Core/Collision/Info/BroadPhaseCollisionPair.h"
#include "Core/Object/PhyObject/PhyObject.h"
#include <unordered_map>
#include <vector>
#include <functional>

namespace Core::Collision
{
    class BVHTree
    {
    private:
        static constexpr float HEIGHT_COST_WEIGHT = 0.005f;

        BVHNode* _root = nullptr;
        std::unordered_map<ColliderHandle, BVHNode*> _leafNodeMap;

    public:
        BVHTree() = default;
        BVHTree(const BVHTree& other) = delete;
        BVHTree(BVHTree&& other) = delete;
        ~BVHTree();

        BVHTree& operator=(const BVHTree& other) = delete;
        BVHTree& operator=(BVHTree&& other) = delete;

        void Clear();
        bool Insert(const ColliderHandle& handle, const RigidbodyHandle& body, const LayerMask& layer, const Bound& bound);
        bool Remove(const ColliderHandle& handle);
        bool Update(const ColliderHandle& handle, const RigidbodyHandle& body, const LayerMask& layer, const Bound& bound);

        std::vector<BroadPhaseCollisionPair> CollisionPairs() const;
        std::vector<std::pair<ColliderHandle, RigidbodyHandle>> Overlap(const std::function<bool(const LayerMask&, const Bound&)>& func) const;
        std::vector<std::pair<ColliderHandle, RigidbodyHandle>> BoundOverlap(const Bound& bound, const LayerMask& layer) const;

    private:
        BVHNode* TryGetLeafNode(const ColliderHandle& handle) const;
        
        void InsertLeafNode(BVHNode* node);
        void RemoveLeafNode(BVHNode* node);

        void MergeNode(BVHNode* before, BVHNode* after);
        BVHNode* GetBestBro(const Bound& bound) const;
    };
}