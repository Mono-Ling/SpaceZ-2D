#include "Core/Collision/BroadPhase/BVHTree.h"
#include "Core/Math/Math.h"
#include "Debug/Debug.h"
#include <cfloat>
#include <stack>
#include <utility>
using namespace Core::Math;
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
            if(p.first || p.second->IsLeaf())
            {
                delete p.second;
                continue;
            }
            st.emplace(true, p.second);
            st.emplace(false, p.second->right);
            st.emplace(false, p.second->left);
        }
        _root = nullptr;
        _leafNodeMap.clear();
    }
    bool BVHTree::Insert(const ColliderHandle& handle, const RigidbodyHandle& body, const LayerMask& layer, const Bound& bound)
    {
        if(!handle || TryGetLeafNode(handle))
            return false;
        BVHNode* node = new BVHNode(BVHObject(handle, body, layer, bound));
        InsertLeafNode(node);
        _leafNodeMap[handle] = node;
        return true;
    }
    bool BVHTree::Remove(const ColliderHandle& handle)
    {
        if(auto node = TryGetLeafNode(handle))
        {
            RemoveLeafNode(node);
            _leafNodeMap.erase(handle);
            return true;
        }
        return false;
    }
    bool BVHTree::Update(const ColliderHandle& handle, const RigidbodyHandle& body, const LayerMask& layer, const Bound& bound)
    {
        auto node = TryGetLeafNode(handle);
        if(!node)
            return false;
        BVHObject obj(handle, body, layer, bound);
        if(IsContains(node->bound, bound))
        {
            LayerMask oldLayer = node->obj.layer;
            node->obj = obj;
            if(oldLayer != obj.layer)
                node->UpdateAll();
            return true;
        }
        RemoveLeafNode(node);
        node = new BVHNode(obj);
        InsertLeafNode(node);
        _leafNodeMap[handle] = node;
        return true;
    }

    BVHNode* BVHTree::TryGetLeafNode(const ColliderHandle& handle) const
    {
        if(!handle)
            return nullptr;
        auto it = _leafNodeMap.find(handle);
        return it != _leafNodeMap.end() ? it->second : nullptr;
    }
    void BVHTree::InsertLeafNode(BVHNode* node)
    {
        if(!node)
            return;
        auto bro = GetBestBro(node->bound);
        if(!bro)
        {
            _root = node;
            return;
        }
        MergeNode(bro, node);

        // 向上迭代更新
        node->UpdateAll();
    }
    void BVHTree::RemoveLeafNode(BVHNode* node)
    {
        if(!node)
            return;
        auto parent = GetParent(node);
        if(!parent)
        {
            _root = nullptr;
            delete node;
            return;
        }

        auto bro = GetBro(node);
        auto grandParent = GetGrandParent(node);

        // 理论上不存在此路径
        if(!bro)
        {
            Debug::LogError("【BVHTree】出现单叶子节点，树结构受损");
            goto exit;
        }
        
        bro->parent = grandParent;
        if(!grandParent)
        {
            _root = bro;
            goto exit;
        }
        if(parent == grandParent->left)
            grandParent->left = bro;
        else
            grandParent->right = bro;

        // 向上迭代更新
        bro->UpdateAll();
        
    exit:
        delete parent;
        delete node;
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

        newParent->parent = oldParent;
        if(!oldParent)
        {
            _root = newParent;
            return;
        }
        if(oldParent->left == before)
            oldParent->left = newParent;
        else
            oldParent->right = newParent;
    }
    BVHNode* BVHTree::GetBestBro(const Bound& bound) const
    {
        if(!_root)
            return nullptr;

        float totalArea = _root->bound.Area();
        auto costFunc = [&](BVHNode* node)->float
        {
            if(!node)
                return FLT_MAX;
            return DeltaArea(node, bound) / Max(totalArea, Epsilon) + node->height * HEIGHT_COST_WEIGHT;
        };

        BVHNode* curr = _root;
        float currCost = costFunc(_root);
        while (curr)
        {
            if(curr->IsLeaf())
                break;
            float leftCost = costFunc(curr->left);
            float rightCost = costFunc(curr->right);
            
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
}