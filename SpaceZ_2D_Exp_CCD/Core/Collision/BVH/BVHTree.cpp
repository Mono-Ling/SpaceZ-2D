#include "Collision/BVH/BVHTree.h"
#include "Debug/Debug.h"
#include <stack>
using namespace std;

namespace Core::Collision
{
    BVHTree::~BVHTree() { Clear(); }
    void BVHTree::Clear()
    {
        if(!_root)
            return;
        stack<pair<bool, BVHNode*>> st;
        st.emplace(false, _root);
        while (!st.empty())
        {
            auto p = st.top();
            st.pop();
            if(!p.second)
                continue;
            if(p.first)
            {
                delete p.second;
                continue;
            }
            st.emplace(true, p.second);
            st.emplace(false, p.second->right);
            st.emplace(false, p.second->left);
        }
        _leafNodeMap.clear();
        _root = nullptr;
    }
    bool BVHTree::Insert(const ColliderHandle& handle, const RigidbodyHandle& body, const Bound& bound)
    {
        if(TryGetLeafNode(handle))
            return false;
        BVHNodeObject obj(bound, handle, body);
        auto node = new BVHNode(obj);
        InsertNode(node);
        _leafNodeMap.emplace(handle, node);
        return true;
    }
    bool BVHTree::Remove(const ColliderHandle& handle)
    {
        auto node = TryGetLeafNode(handle);
        if(!node)
            return false;
        RemoveLeafNode(node);
        _leafNodeMap.erase(handle);
        return true;
    }
    bool BVHTree::Update(const ColliderHandle& handle, const RigidbodyHandle& body, const Bound& bound)
    {
        auto node = TryGetLeafNode(handle);
        if(!node)
            return false;
        if(IsContains(node->bound, bound))
        {
            node->obj.body = body;
            node->obj.bound = bound;
            return true;
        }
        RemoveLeafNode(node);
        node = new BVHNode(BVHNodeObject(bound, handle, body));
        InsertNode(node);
        _leafNodeMap[handle] = node;
        return true;
    }
    vector<CollisionPair> BVHTree::GetCollisionPairs() const
    {
        vector<CollisionPair> ans;
        if(!_root)
            return ans;
        stack<BVHNode*> st;
        st.push(_root);
        while (!st.empty())
        {
            auto node = st.top();
            st.pop();
            if(!node || node->IsLeaf())
                continue;
            node->GetSelfCollisionPairs(ans);
            st.push(node->left);
            st.push(node->right);
        }
        return ans;
    }
    vector<ColliderHandle> BVHTree::BoundOverlap(const Bound& bound) const
    {
        return Overlap([&](const Bound& node)->bool
        {
            return IsIntersect(node, bound);
        });
    }
    vector<ColliderHandle> BVHTree::Overlap(const function<bool(const Bound&)>& func) const
    {
        vector<ColliderHandle> ans;
        if(!_root)
            return ans;
        stack<BVHNode*> st;
        st.push(_root);
        while (!st.empty())
        {
            auto node = st.top();
            st.pop();
            if(!node || !func(node->bound))
                continue;
            if(!node->IsLeaf())
            {
                st.push(node->left);
                st.push(node->right);
            }
            else
                if(func(node->obj.bound))
                    ans.push_back(node->obj.handle);
        }
        return ans;
    }

    BVHNode* BVHTree::TryGetLeafNode(const ColliderHandle& handle) const
    {
        auto it = _leafNodeMap.find(handle);
        if(it == _leafNodeMap.end())
            return nullptr;
        return it->second;
    }
    void BVHTree::InsertNode(BVHNode* node)
    {
        if(!node)
            return;
        if(!_root)
        {
            _root = node;
            return;
        }
        auto best = GetBestBro(node->bound);
        MergeNode(best, node);
        node->UpdateAll();
    }
    void BVHTree::RemoveLeafNode(BVHNode* node)
    {
        if(!node)
            return;
        auto parent = GetParent(node);
        if(!parent)
        {
            delete node;
            _root = nullptr;
            return;
        }
        auto grandParent = GetGrandParent(node);
        auto bro = GetBro(node);
        if(!bro)
        {
            Debug::LogError("【BVH树】树结构受损，出现单子节点");
            goto exit;
        }

        if(grandParent)
        {
            
            if(grandParent->left == parent)
            grandParent->left = bro;
            else
                grandParent->right = bro;
        }
        else
            _root = bro;
        bro->parent = grandParent;
        bro->UpdateAll();

    exit:
        node->ClearPtr();
        parent->ClearPtr();
        delete node;
        delete parent;
    }
    BVHNode* BVHTree::GetBestBro(const Bound& after) const
    {
        if(!_root)
            return nullptr;
        BVHNode* curr = _root;
        float totalArea = _root->bound.Area();
        float currCost = DeltaArea(curr, after) / totalArea + curr->height * HEIGHT_COST_WEIGHT;
        while (curr)
        {
            if(curr->IsLeaf())
                break;
            float leftCost = DeltaArea(curr->left, after) / totalArea + curr->left->height * HEIGHT_COST_WEIGHT;
            float rightCost = DeltaArea(curr->right, after) / totalArea + curr->right->height * HEIGHT_COST_WEIGHT;
            if(leftCost > currCost && rightCost > currCost)
                break;
            if(leftCost < rightCost)
            {
                currCost = leftCost;
                curr = curr->left;
            }
            else
            {
                currCost = rightCost;
                curr = curr->right;
            }
        }
        return curr;
    }
    void BVHTree::MergeNode(BVHNode* before, BVHNode* after)
    {
        if(!before || !after)
            return;
        auto oldParent = before->parent;
        auto newParent = new BVHNode(before->bound + after->bound);
        newParent->left = before;
        newParent->right = after;
        before->parent = newParent;
        after->parent = newParent;

        if(!oldParent)
        {
            _root = newParent;
            return;
        }

        newParent->parent = oldParent;
        if(oldParent->left == before)
            oldParent->left = newParent;
        else
            oldParent->right = newParent;
    }
}