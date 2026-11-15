#include "Core/Collision/BroadPhase/BVHTree.h"
#include <stack>
using namespace std;

namespace Core::Collision
{
    vector<BroadPhaseCollisionPair> BVHTree::CollisionPairs() const
    {
        vector<BroadPhaseCollisionPair> ans;
        if(!_root)
            return ans;
        stack<BVHNode*> st;
        st.emplace(_root);
        while (!st.empty())
        {
            auto node = st.top();
            st.pop();
            if(!node || node->IsLeaf())
                continue;
            node->GetSelfCollisionPairs(ans);
            st.emplace(node->left);
            st.emplace(node->right);
        }
        return ans;
    }
    vector<pair<ColliderHandle, RigidbodyHandle>> BVHTree::Overlap(const function<bool(const LayerMask&, const Bound&)>& func) const
    {
        vector<pair<ColliderHandle, RigidbodyHandle>> ans;
        if(!_root || !func)
            return ans;
        stack<BVHNode*> st;
        st.emplace(_root);
        while (!st.empty())
        {
            auto node = st.top();
            st.pop();
            if(!node || !func(node->layer, node->bound))
                continue;

            if(node->IsLeaf())
            {
                auto& obj = node->obj;
                if(func(obj.layer, obj.bound))
                    ans.emplace_back(obj.collider, obj.body);
            }
            else
            {
                st.emplace(node->left);
                st.emplace(node->right);
            }
        }
        return ans;
    }
    vector<pair<ColliderHandle, RigidbodyHandle>> BVHTree::BoundOverlap(const Bound& bound, const LayerMask& layer) const
    {
        return Overlap([&](const LayerMask& l, const Bound& b)->bool
        {
            return IsIntersect(l, layer) && IsIntersect(b, bound);
        });
    }
}