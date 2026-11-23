#include "Core/Memory/MemoryPage.h"
#include <new>
using namespace std;

namespace Core::Memory
{
    void* MemoryPage::Cut(size_t len) const
    {
        if (!address || size < len)
            throw bad_alloc();
        size -= len;
        uintptr_t ptr = address + size;
        return reinterpret_cast<void*>(ptr);
    }
    bool MemoryPage::TryMerge(const MemoryPage& other) const
    {
        if (!address || !size || !other || address == other.address)
            return false;
        if (address < other.address)
        {
            if (address + size != other.address)
                return false;
            size += other.size;
            return true;
        }
        return false;
    }

    bool operator==(const MemoryPage& a, const MemoryPage& b)
    {
        return a.address == b.address;
    }
    bool operator<(const MemoryPage& a, const MemoryPage& b)
    {
        return a.address < b.address;
    }
}