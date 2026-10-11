#pragma once
#include "CollisionDetection/Geometry/Bound.h"
#include "CollisionDetection/BVH/BVHNodeObj.h"
#include "Math/Math.h"
#include <cfloat>

namespace Core::Collision
{
    class BVHNode
    {
    public:
        static constexpr float MULTIPLE = 1.5f;
        int height;
        BVHNode* left = nullptr;
        BVHNode* right = nullptr;
        BVHNode* parent = nullptr;
        Bound bound;
        BVHNodeObj obj = BVHNodeObj::null;

        BVHNode(const Bound& bound) : bound(bound), height(2) {}
        BVHNode(const BVHNodeObj& obj)
        : bound(Bound(obj.bound.center, obj.bound.extents * MULTIPLE)), obj(obj), height(1) {}

        inline bool IsLeaf() const
        {
            return obj != BVHNodeObj::null
                && !left && !right;
        }
        inline void UpdateSelf()
        {
            if(!left || !right)
                return;
            bound = left->bound + right->bound;
            height = Math::Max(left->height, right->height) + 1;
        }
        inline void Update()
        {
            auto curr = this;
            while (curr)
            {
                curr->UpdateSelf();
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
    inline float DeltaArea(BVHNode* node, const Bound& bound)
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