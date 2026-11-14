#pragma once
#include "Core/Collision/BroadPhase/BVHObject.h"
#include "Core/Collision/Geometry/Bound.h"
#include "Core/Collision/Layer/LayerMask.h"

namespace Core::Collision
{
    class BVHNode
    {
    public:
        static constexpr float BOUND_EXPAND = 1.5f;

        int height = 0;
        BVHNode* parent = nullptr;
        BVHNode* left = nullptr;
        BVHNode* right = nullptr;

        LayerMask layer;
        Bound bound;

        BVHObject obj;

        BVHNode(const Bound& bound) : bound(bound) {}
        BVHNode(const BVHObject& obj)
        : bound(Bound(obj.bound.center, obj.bound.extents * BOUND_EXPAND)), layer(obj.layer), obj(obj), height(1) {}
        ~BVHNode();

        bool IsLeaf() const;
        void ClearPtr();
        
        void UpdateSelf();
        void UpdateAll();

        void GetSelfCollisionPairs(std::vector<BroadPhaseCollisionPair>& ans) const;
    };
    bool IsIntersect(BVHNode* a, BVHNode* b);
    float DeltaArea(BVHNode* node, const Bound& bound);
    BVHNode* GetParent(BVHNode* node);
    BVHNode* GetBro(BVHNode* node);
    BVHNode* GetGrandParent(BVHNode* node);
}