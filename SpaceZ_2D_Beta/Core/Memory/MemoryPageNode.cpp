#include "Core/Memory/MemoryPageNode.h"
#include "Core/Math/Math.h"
using namespace Core::Math;
using namespace std;

namespace Core::Memory
{
    void MemoryPageNode::UpdateSelf()
    {
        height = Max(GetHeight(left), GetHeight(right)) + 1;
        maxMemorySize = Max(Max(GetMaxMemorySize(left), GetMaxMemorySize(right)), page.size);
    }
    void MemoryPageNode::UpdateAll()
    {
        auto curr = this;
        while(curr)
        {
            curr->UpdateSelf();
            curr = curr->parent;
        }
    }
    MemoryPageNode* MemoryPageNode::RotateLeft()
    {
        auto pivot = right;
        if(!pivot)
            return this;

        auto grandParent = parent;

        right = pivot->left;
        if(right)
            right->parent = this;

        pivot->left = this;
        this->parent = pivot;

        pivot->parent = grandParent;
        if(grandParent)
        {
            if(grandParent->left == this)
                grandParent->left = pivot;
            else if(grandParent->right == this)
                grandParent->right = pivot;
        }

        UpdateSelf();
        pivot->UpdateSelf();
        return pivot;
    }
    MemoryPageNode* MemoryPageNode::RotateRight()
    {
        auto pivot = left;
        if(!pivot)
            return this;

        auto grandParent = parent;

        left = pivot->right;
        if(left)
            left->parent = this;

        pivot->right = this;
        this->parent = pivot;

        pivot->parent = grandParent;
        if(grandParent)
        {
            if(grandParent->left == this)
                grandParent->left = pivot;
            else if(grandParent->right == this)
                grandParent->right = pivot;
        }

        UpdateSelf();
        pivot->UpdateSelf();
        return pivot;
    }
    MemoryPageNode* MemoryPageNode::RotateLeftRight()
    {
        if(!left || !left->right)
            return this;
        left->RotateLeft();
        return RotateRight();
    }
    MemoryPageNode* MemoryPageNode::RotateRightLeft()
    {
        if(!right || !right->left)
            return this;
        right->RotateRight();
        return RotateLeft();
    }

    int GetHeight(MemoryPageNode* node)
    {
        if(!node)
            return 0;
        return node->height;
    }
    size_t GetMaxMemorySize(MemoryPageNode* node)
    {
        if(!node)
            return 0;
        return node->maxMemorySize;
    }
    MemoryPageNode* GetParent(MemoryPageNode* node)
    {
        if(!node)
            return nullptr;
        return node->parent;
    }
}