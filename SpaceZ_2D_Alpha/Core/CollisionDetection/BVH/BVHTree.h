#pragma once
#include "CollisionDetection/BVH/BVHNode.h"
#include "Collision/CollisionPair.h"
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
        std::unordered_map<ColliderHandle, BVHNode*> _nodeMap;

    public:
        BVHTree() = default;
        BVHTree(const BVHTree& other) = delete;
        ~BVHTree();
    
        void Clear();
        bool TryGetObj(const ColliderHandle& collider, BVHNode*& node);
        void Insert(const ColliderHandle& collider, const RigidbodyHandle& body, const Bound& bound);
        void Remove(const ColliderHandle& collider);
        void UpdateCollider(const ColliderHandle& collider, const RigidbodyHandle& body, const Bound& bound);

        std::vector<const BVHNode*> PreOrder() const;
        std::vector<const BVHNode* > InOrder() const;

        std::vector<CollisionPair> GetCollisionPairs() const;
        std::vector<ColliderHandle> Overlap(std::function<bool(const Bound&)> func) const;
        std::vector<ColliderHandle> BoundOverlap(const Bound& bound) const;

    private:
        void InsertNode(BVHNode* node);
        void RemoveLeafNode(BVHNode* node);
        BVHNode* GetBestBro(const Bound& after);
        void MergeNode(BVHNode* before, BVHNode* after);
    };
}