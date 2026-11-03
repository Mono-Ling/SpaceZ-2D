#pragma once
#include "Collision/BVH/BVHNodeObject.h"
#include "Collision/Geometry/Bound.h"
#include <vector>

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
        BVHNodeObject obj = BVHNodeObject::null;

        BVHNode(const Bound& bound) : bound(bound), height(2) {}
        BVHNode(const BVHNodeObject& obj)
        : bound(Bound(obj.bound.center, obj.bound.extents * MULTIPLE)), obj(obj), height(1) {}

        bool IsLeaf() const;
        void ClearPtr();
        
        void UpdateSelf();
        void UpdateAll();

        void GetSelfCollisionPairs(std::vector<CollisionPair>& ans) const;
    };
    bool IsIntersect(BVHNode* a, BVHNode* b);
    float DeltaArea(BVHNode* node, const Bound& bound);
    BVHNode* GetParent(BVHNode* node);
    BVHNode* GetBro(BVHNode* node);
    BVHNode* GetGrandParent(BVHNode* node);
}