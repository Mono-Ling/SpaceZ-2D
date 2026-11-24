#pragma once
#include "Core/Memory/MemoryPageNode.h"
#include "Core/Memory/MgrAllocator.h"

namespace Core::Memory
{
    class MemoryPageTree
    {
        MgrAllocator<MemoryPageNode> _allocator;
        MemoryPageNode* _root = nullptr;

    public:
        MemoryPageTree() = default;
        MemoryPageTree(const MemoryPageTree& other) = delete;
        MemoryPageTree(MemoryPageTree&& other) = delete;
        ~MemoryPageTree();

        MemoryPageTree& operator=(const MemoryPageTree& other) = delete;
        MemoryPageTree& operator=(MemoryPageTree&& other) = delete;

        void Clear();
        void* GetMemory(std::size_t size);
        void PutMemory(MemoryPage page);

    private:
        void InsertNode(MemoryPageNode* node);
        void RemoveNode(MemoryPageNode* node);

        // 查找树中地址严格大于 page 的最小节点（后继），不存在返回 nullptr
        MemoryPageNode* PeekUpper(const MemoryPage& page);
        // 查找树中地址严格小于 page 的最大节点（前驱），不存在返回 nullptr
        MemoryPageNode* PeekLower(const MemoryPage& page);

        MemoryPageNode* CreateNode(const MemoryPage& page);
        void DestroyNode(MemoryPageNode* node);
    };
}