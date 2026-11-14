#include "Core/Collision/BroadPhase/BVHNode.h"
#include "Core/Math/Math.h"
#include <cfloat>
#include <stack>
#include <utility>
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    BVHNode::~BVHNode() { ClearPtr(); }

    bool BVHNode::IsLeaf() const
    {
        return !left && !right
            && obj != BVHObject::null;
    }
    void BVHNode::ClearPtr()
    {
        parent = nullptr;
        left = nullptr;
        right = nullptr;
    }
    void BVHNode::UpdateSelf()
    {
        if(IsLeaf())
            return;
        bound = left->bound + right->bound;
        layer = left->layer | right->layer;
        height = Max(left->height, right->height) + 1;
    }
    void BVHNode::UpdateAll()
    {
        auto curr = this;
        while (curr)
        {
            curr->UpdateSelf();
            curr = curr->parent;
        }
    }
    void BVHNode::GetSelfCollisionPairs(vector<BroadPhaseCollisionPair>& ans) const
    {
        if(IsLeaf())
            return;
        stack<pair<BVHNode*, BVHNode*>> st;
        st.emplace(pair<BVHNode*, BVHNode*>(left, right));
        while (!st.empty())
        {
            auto curr = st.top();
            st.pop();

            auto& first = curr.first;
            auto& second = curr.second;
            if(!IsIntersect(first, second))
                continue;

            if(!first->IsLeaf() && !second->IsLeaf())
            {
                st.emplace(pair<BVHNode*, BVHNode*>(first->left, second->left));
                st.emplace(pair<BVHNode*, BVHNode*>(first->right, second->right));
                st.emplace(pair<BVHNode*, BVHNode*>(first->left, second->right));
                st.emplace(pair<BVHNode*, BVHNode*>(first->right, second->left));
            }
            else
            {
                if(first->IsLeaf() && second->IsLeaf())
                {
                    if(IsIntersect(first->obj, second->obj))
                        ans.emplace_back(CollisionPair(first->obj, second->obj));
                }
                else if(first->IsLeaf())
                {
                    st.emplace(pair<BVHNode*, BVHNode*>(first, second->left));
                    st.emplace(pair<BVHNode*, BVHNode*>(first, second->right));
                }
                else
                {
                    st.emplace(pair<BVHNode*, BVHNode*>(first->left, second));
                    st.emplace(pair<BVHNode*, BVHNode*>(first->right, second));
                }
            }
        }
    }
    bool IsIntersect(BVHNode* a, BVHNode* b)
    {
        if(!a || !b)
            return false;
        return IsIntersect(a->layer, b->layer) && IsIntersect(a->bound, b->bound);
    }

    float DeltaArea(BVHNode* node, const Bound& bound)
    {
        if(!node)
            return FLT_MAX;
        auto& nodeBound = node->bound;
        return (nodeBound + bound).Area() - nodeBound.Area();
    }
    BVHNode* GetParent(BVHNode* node)
    {
        if(!node)
            return nullptr;
        return node->parent;
    }
    BVHNode* GetBro(BVHNode* node)
    {
        if(!node || !node->parent)
            return nullptr;
        auto& parent = node->parent;
        return node == parent->left ? parent->right : parent->left;
    }
    BVHNode* GetGrandParent(BVHNode* node)
    {
        if(!node || !node->parent)
            return nullptr;
        return node->parent->parent;
    }
}