#include "CollisionDetection/BVH/BVHNode.h"
#include <stack>
#include <utility>
using namespace std;

namespace Core::Collision
{
    void BVHNode::GetSelfCollisionPairs(std::vector<CollisionPair>& ans) const
    {
        if(IsLeaf())
            return;
        stack<pair<BVHNode*, BVHNode*>> st;
        st.push({left, right});
        while (!st.empty())
        {
            auto p = st.top();
            st.pop();
            if(!IsIntersect(*p.first, *p.second))
                continue;
            if(!p.first->IsLeaf() || !p.second->IsLeaf())
            {
                if(!p.first->IsLeaf() && !p.second->IsLeaf())
                {
                    st.push({p.first->left, p.second->left});
                    st.push({p.first->right, p.second->right});

                    st.push({p.first->left, p.second->right});
                    st.push({p.first->right, p.second->left});
                }
                else if(!p.first->IsLeaf())
                {
                    st.push({p.first->left, p.second});
                    st.push({p.first->right, p.second});
                }
                else
                {
                    st.push({p.first, p.second->left});
                    st.push({p.first, p.second->right});
                }
            }
            else
                if(IsIntersect(p.first->obj, p.second->obj))
                    ans.push_back(GetCollisionPair(p.first->obj, p.second->obj));
        }
    }
}
