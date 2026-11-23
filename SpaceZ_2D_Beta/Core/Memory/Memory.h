#pragma once
#include <memory>

namespace Core::Memory
{
    typedef void*(*AllocFunc)(std::size_t size);
    typedef void (*FreeFunc)(void* ptr);

    inline void* DefaultAlloc(std::size_t size) { return std::malloc(size); }
    inline void DefaultFree(void* ptr) { std::free(ptr); }

    inline AllocFunc allocFunc = DefaultAlloc;
    inline FreeFunc freeFunc = DefaultFree;

    inline void* Alloc(std::size_t size)
    {
        if(!allocFunc)
            return DefaultAlloc(size);
        return allocFunc(size);
    }
    inline void Free(void* ptr)
    {
        if(!freeFunc)
        {
            DefaultFree(ptr);
            return;
        }
        freeFunc(ptr);
    }
}