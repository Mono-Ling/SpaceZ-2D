#include "Core/Memory/MemoryPageTree.h"
#include "Core/Math/Math.h"
#include <stack>
#include <utility>
using namespace Core::Math;
using namespace std;

namespace Core::Memory
{
    namespace
    {

        // 自 from 沿 parent 链向上刷新，并在每个失衡节点处旋转；新根出现时同步 root
        void FixUp(MemoryPageNode* from, MemoryPageNode*& root)
        {
            auto curr = from;
            while (curr)
            {
                curr->UpdateSelf();

                const int balance = GetHeight(curr->left) - GetHeight(curr->right);
                auto newRoot = curr;
                if (balance > 1)
                {
                    newRoot = GetHeight(curr->left->left) >= GetHeight(curr->left->right)
                                  ? curr->RotateRight()
                                  : curr->RotateLeftRight();
                }
                else if (balance < -1)
                {
                    newRoot = GetHeight(curr->right->right) >= GetHeight(curr->right->left)
                                  ? curr->RotateLeft()
                                  : curr->RotateRightLeft();
                }

                if (!newRoot->parent)
                    root = newRoot;
                curr = newRoot->parent;
            }
        }
    }

    MemoryPageTree::~MemoryPageTree()
    {
        Clear();
    }
    void MemoryPageTree::Clear()
    {
        if (!_root)
            return;
        using Item = pair<MemoryPageNode*, bool>;
        stack<Item, deque<Item, MgrAllocator<Item>>> st;
        st.emplace(_root, false);
        while (!st.empty())
        {
            auto p = st.top();
            st.pop();

            if (p.second)
                DestroyNode(p.first);
            else
            {
                st.emplace(p.first, true);
                if (p.first->right)
                    st.emplace(p.first->right, false);
                if (p.first->left)
                    st.emplace(p.first->left, false);
            }
        }
        _root = nullptr;
    }
    void* MemoryPageTree::GetMemory(size_t size)
    {
        if (!_root)
            return nullptr;
        if (!size)
            size = 1;
        MemoryPageNode* target = nullptr;
        auto curr = _root;
        while (curr && curr->maxMemorySize >= size)
        {
            if(curr->page.size >= size)
            {
                target = curr;
                break;
            }
            if (curr->left && curr->left->maxMemorySize >= size)
                curr = curr->left;
            else
                curr = curr->right;
        }
        if (!target)
            return nullptr;

        auto& page = target->page;
        auto ptr = page.Cut(size);

        if (!page)
            RemoveNode(target);
        else
            target->UpdateAll();
        return ptr;
    }
    void MemoryPageTree::PutMemory(MemoryPage page)
    {
        if (!page)
            return;
        auto upper = PeekUpper(page);
        auto lower = PeekLower(page);

        if (upper && page.TryMerge(upper->page))
            RemoveNode(upper);

        if (lower && lower->page.TryMerge(page))
            lower->UpdateAll();
        else
            InsertNode(CreateNode(page));
    }

    void MemoryPageTree::InsertNode(MemoryPageNode* node)
    {
        if (!node)
            return;

        if (!_root)
        {
            _root = node;
            return;
        }

        auto curr = _root;
        while (true)
        {
            if (node->page < curr->page)
            {
                if (!curr->left)
                {
                    curr->left = node;
                    break;
                }
                curr = curr->left;
            }
            else
            {
                if (!curr->right)
                {
                    curr->right = node;
                    break;
                }
                curr = curr->right;
            }
        }
        node->parent = curr;

        FixUp(curr, _root);
    }
    void MemoryPageTree::RemoveNode(MemoryPageNode* node)
    {
        if (!node)
            return;

        // 两个孩子时用中序后继的内容顶替，真正摘除的节点至多一个孩子
        auto target = node;
        if (target->left && target->right)
        {
            auto succ = target->right;
            while (succ->left)
                succ = succ->left;

            target->page = succ->page;
            target->UpdateSelf();
            target = succ;
        }

        auto child = target->left ? target->left : target->right;
        auto parent = target->parent;

        if (child)
            child->parent = parent;

        if (!parent)
            _root = child;
        else if (parent->left == target)
            parent->left = child;
        else
            parent->right = child;

        target->left = nullptr;
        target->right = nullptr;
        target->parent = nullptr;
        DestroyNode(target);

        if (parent)
            FixUp(parent, _root);
    }
    MemoryPageNode* MemoryPageTree::PeekUpper(const MemoryPage& page)
    {
        MemoryPageNode* ans = nullptr;
        auto curr = _root;
        while (curr)
        {
            if (page < curr->page)
            {
                ans = curr;
                curr = curr->left;
            }
            else
                curr = curr->right;
        }
        return ans;
    }
    MemoryPageNode* MemoryPageTree::PeekLower(const MemoryPage& page)
    {
        MemoryPageNode* ans = nullptr;
        auto curr = _root;
        while (curr)
        {
            if (curr->page < page)
            {
                ans = curr;
                curr = curr->right;
            }
            else
                curr = curr->left;
        }
        return ans;
    }
    MemoryPageNode* MemoryPageTree::CreateNode(const MemoryPage& page)
    {
        auto allocPtr = _allocator.allocate(1);
        allocPtr = new (allocPtr) MemoryPageNode(page);
        return allocPtr;
    }
    void MemoryPageTree::DestroyNode(MemoryPageNode* node)
    {
        if (!node)
            return;
        node->~MemoryPageNode();
        _allocator.deallocate(node, 1);
    }
}