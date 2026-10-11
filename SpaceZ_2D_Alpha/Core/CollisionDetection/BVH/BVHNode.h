#pragma once
#include "CollisionDetection/Geometry/Bound.h"
#include "CollisionDetection/BVH/BVHNodeObj.h"
#include <cfloat>

namespace Core::Collision
{
    class BVHNode
    {
    public:
        static constexpr float MULTIPLE = 1.5f;
        BVHNode* left = nullptr;
        BVHNode* right = nullptr;
        BVHNode* parent = nullptr;
        Bound bound;
        BVHNodeObj obj = BVHNodeObj::null;

        BVHNode(const Bound& bound) : bound(bound) {}
        BVHNode(const BVHNodeObj& obj)
        : bound(Bound(obj.bound.center, obj.bound.extents * MULTIPLE)), obj(obj) {}

        inline bool IsLeaf() const
        {
            return obj != BVHNodeObj::null
                && !left && !right;
        }
        inline void UpdateSelfBound()
        {
            if(left && right)
                bound = left->bound + right->bound;
        }
        inline void UpdateBound()
        {
            auto curr = this;
            while (curr)
            {
                curr->UpdateSelfBound();
                curr = curr->parent;
            }
        }
        inline void ClearPtr()
        {
            left = nullptr;
            right = nullptr;
            parent = nullptr;
        }
    };
    inline bool IsIntersect(const BVHNode& a, const BVHNode& b)
    {
        return IsIntersect(a.bound, b.bound);
    }
    inline float MergeCost(BVHNode* node, const Bound& bound)
    {
        if(!node)
            return FLT_MAX;
        auto merge = node->bound + bound;
        return merge.Area() - node->bound.Area();
    }
    inline BVHNode* GetParent(BVHNode* node)
    {
        return node ? node->parent : nullptr;
    }
    inline BVHNode* GetBro(BVHNode* node)
    {
        if(!node || !node->parent)
            return nullptr;
        return node == node->parent->left ? node->parent->right : node->parent->left;
    }
    inline BVHNode* GetGrandParent(BVHNode* node)
    {
        if(!node || !node->parent)
            return nullptr;
        return node->parent->parent;
    }
}