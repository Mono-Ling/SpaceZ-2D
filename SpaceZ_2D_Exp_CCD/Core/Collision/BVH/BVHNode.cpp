#include "Collision/BVH/BVHNode.h"
#include "Math/Math.h"
#include <cfloat>
#include <stack>
using namespace Core::Math;
using namespace std;

namespace Core::Collision
{
    bool BVHNode::IsLeaf() const
    {
        return obj != BVHNodeObject::null
            && !left && !right;
    }
    void BVHNode::ClearPtr()
    {
        left = nullptr;
        right = nullptr;
        parent = nullptr;
    }
    void BVHNode::UpdateSelf()
    {
        if(IsLeaf())
            return;
        bound = left->bound + right->bound;
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
    void BVHNode::GetSelfCollisionPairs(vector<CollisionPair>& ans) const
    {
        if(IsLeaf())
            return;
        stack<pair<BVHNode* , BVHNode*>> st;
        st.emplace(pair<BVHNode*, BVHNode*>(left, right));
        while (!st.empty())
        {
            auto p = st.top();
            st.pop();
            if(!IsIntersect(p.first, p.second))
                continue;
            if(p.first->IsLeaf() || p.second->IsLeaf())
            {
                if(p.first->IsLeaf() && p.second->IsLeaf())
                {
                    if(IsIntersect(p.first->obj, p.second->obj))
                        ans.push_back(CreateCollisionPair(p.first->obj, p.second->obj));
                }
                else if(p.first->IsLeaf())
                {
                    st.emplace(pair<BVHNode*, BVHNode*>(p.first, p.second->left));
                    st.emplace(pair<BVHNode*, BVHNode*>(p.first, p.second->right));
                }
                else
                {
                    st.emplace(pair<BVHNode*, BVHNode*>(p.first->left, p.second));
                    st.emplace(pair<BVHNode*, BVHNode*>(p.first->right, p.second));
                }
            }
            else
            {
                st.emplace(pair<BVHNode*, BVHNode*>(p.first->left, p.second->left));
                st.emplace(pair<BVHNode*, BVHNode*>(p.first->right, p.second->right));

                st.emplace(pair<BVHNode*, BVHNode*>(p.first->left, p.second->right));
                st.emplace(pair<BVHNode*, BVHNode*>(p.first->right, p.second->left));
            }
        }
    }

    bool IsIntersect(BVHNode* a, BVHNode* b)
    {
        if(!a || !b)
            return false;
        return IsIntersect(a->bound, b->bound);
    }
    float DeltaArea(BVHNode* node, const Bound& bound)
    {
        if(!node)
            return FLT_MAX;
        auto temp = node->bound + bound;
        return temp.Area() - node->bound.Area();
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
        auto parent = node->parent;
        return node == parent->left ? parent->right : parent->left;
    }
    BVHNode* GetGrandParent(BVHNode* node)
    {
        if(!node || !node->parent)
            return nullptr;
        return node->parent->parent;
    }
}