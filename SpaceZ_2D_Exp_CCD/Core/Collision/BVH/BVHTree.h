#pragma once
#include "Collision/BVH/BVHNode.h"
#include <unordered_map>
#include <vector>
#include <functional>

namespace Core::Collision
{
    class BVHTree
    {
        static constexpr float HEIGHT_COST_WEIGHT = 0.005f;

        BVHNode* _root = nullptr;
        std::unordered_map<ColliderHandle, BVHNode*> _leafNodeMap;

    public:
        BVHTree() = default;
        BVHTree(const BVHTree& other) = delete;
        BVHTree(BVHNode&& other) = delete;
        ~BVHTree();

        BVHTree& operator=(const BVHTree& other) = delete;
        BVHTree& operator=(BVHTree&& other) = delete;

        void Clear();
        bool Insert(const ColliderHandle& handle, const RigidbodyHandle& body, const Bound& bound);
        bool Remove(const ColliderHandle& handle);
        bool Update(const ColliderHandle& handle, const RigidbodyHandle& body, const Bound& bound);

        std::vector<CollisionPair> GetCollisionPairs() const;
        std::vector<ColliderHandle> BoundOverlap(const Bound& bound) const;
        std::vector<ColliderHandle> Overlap(const std::function<bool(const Bound&)>& func) const;

    private:
        BVHNode* TryGetLeafNode(const ColliderHandle& handle) const;
        void InsertNode(BVHNode* node);
        void RemoveLeafNode(BVHNode* node);
        BVHNode* GetBestBro(const Bound& after) const;
        void MergeNode(BVHNode* before, BVHNode* after);
    };
}