#pragma once
#include "Core/Memory/MemoryPage.h"

namespace Core::Memory
{
    class MemoryPageNode
    {
    public:
        int height = 1;
        std::size_t maxMemorySize;

        MemoryPageNode* left = nullptr;
        MemoryPageNode* right = nullptr;
        MemoryPageNode* parent = nullptr;

        MemoryPage page;

        MemoryPageNode(const MemoryPage& page) : maxMemorySize(page.size), page(page) {}

        void UpdateSelf();
        void UpdateAll();

        MemoryPageNode* RotateLeft();
        MemoryPageNode* RotateRight();
        MemoryPageNode* RotateLeftRight();
        MemoryPageNode* RotateRightLeft();
    };
    int GetHeight(MemoryPageNode* node);
    std::size_t GetMaxMemorySize(MemoryPageNode* node);
    MemoryPageNode* GetParent(MemoryPageNode* node);
}