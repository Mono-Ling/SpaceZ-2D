#include "CollisionDetection/BVH/BVHTree.h"
#include <stack>
#include <utility>
using namespace std;

namespace Core::Collision
{
    BVHTree::~BVHTree() { Clear(); }
    void BVHTree::Clear()
    {
        if(!_root)
            return;
        stack<pair<bool, BVHNode*>> st;
        st.push({false, _root});
        while (!st.empty())
        {
            auto node = st.top();
            st.pop();
            if(!node.second)
                continue;
            if(!node.first)
            {
                st.push({true, node.second});
                st.push({false, node.second->right});
                st.push({false, node.second->left});
            }
            else
                delete node.second;
        }
        _root = nullptr;
        _nodeMap.clear();
    }
    vector<BVHNode*> BVHTree::PreOrder() const
    {
        vector<BVHNode*> ans;
        stack<BVHNode*> st;
        auto curr = _root;
        while (!st.empty() || curr)
        {
            while (curr)
            {
                st.push(curr);
                ans.push_back(curr);
                curr = curr->left;
            }
            curr = st.top();
            st.pop();
            curr = curr->right;
        }
        return ans;
    }
    vector<BVHNode*> BVHTree::InOrder() const
    {
        vector<BVHNode*> ans;
        stack<BVHNode*> st;
        auto curr = _root;
        while (!st.empty() || curr)
        {
            while (curr)
            {
                st.push(curr);
                curr = curr->left;
            }
            curr = st.top();
            st.pop();
            ans.push_back(curr);
            curr = curr->right;
        }
        return ans;
    }
    bool BVHTree::TryGetObj(const ColliderHandle& collider, BVHNode*& node)
    {
        if(collider == ColliderHandle::null)
            return false;
        auto it = _nodeMap.find(collider);
        if(it == _nodeMap.end())
            return false;
        node = it->second;
        return true;
    }
    void BVHTree::Insert(const ColliderHandle& collider, const RigidbodyHandle& body, const Bound& bound)
    {
        if(_nodeMap.count(collider))
            return;
        BVHNodeObj nodeObj(collider, body, bound);
        auto node = new BVHNode(nodeObj);

        _nodeMap.insert({collider, node});
        InsertNode(node);
    }
    void BVHTree::Remove(const ColliderHandle& collider)
    {
        BVHNode* node;
        if(!TryGetObj(collider, node))
            return;
        _nodeMap.erase(collider);
        RemoveLeafNode(node);
    }
    void BVHTree::UpdateCollider(const ColliderHandle& collider, const RigidbodyHandle& body, const Bound& bound)
    {
        BVHNode* node;
        if(!TryGetObj(collider, node))
            return;
        BVHNodeObj obj(collider, body, bound);
        if(IsContains(node->bound, bound))
        {
            node->obj = obj;
            return;
        }
        RemoveLeafNode(node);
        node = new BVHNode(obj);
        InsertNode(node);
        _nodeMap[collider] = node;
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
        auto bro = GetBestBro(node->bound);
        MergeNode(bro, node);
        node->UpdateBound();
    }
    void BVHTree::RemoveLeafNode(BVHNode* node)
    {
        if(!node || !node->IsLeaf())
            return;
        auto parent = GetParent(node);
        if(!parent)
        {
            delete node;
            _root = nullptr;
            return;
        }
        auto bro = GetBro(node);
        // 理论上不存在此分支
        if(!bro)
        {
            node->ClearPtr();
            delete node;

            parent->ClearPtr();
            delete parent;
            return;
        }

        auto grandParent = GetGrandParent(node);
        if(!grandParent)
        {
            _root = bro;
            goto exit;
        }

        if(grandParent->left == parent)
            grandParent->left = bro;
        else
            grandParent->right = bro;
        
    exit:
        node->ClearPtr();
        parent->ClearPtr();
        delete node;
        delete parent;

        bro->parent = grandParent;
        bro->UpdateBound();
    }
    BVHNode* BVHTree::GetBestBro(const Bound& after)
    {
        if(!_root)
            return nullptr;
        auto node = _root;
        float currCost = MergeCost(_root, after);
        while (node)
        {
            float leftCost = MergeCost(node->left, after);
            float rightCost = MergeCost(node->right, after);
            if(currCost < leftCost && currCost < rightCost)
                break;
            if(leftCost < rightCost)
            {
                node = node->left;
                currCost = leftCost;
            }
            else
            {
                node = node->right;
                currCost = rightCost;
            }
        }
        return node;
    }
    void BVHTree::MergeNode(BVHNode* before, BVHNode* after)
    {
        if(!before || !after)
            return;
        auto oldParent = GetParent(before);
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
        if(before == oldParent->left)
            oldParent->left = newParent;
        else
            oldParent->right = newParent;
    }
}