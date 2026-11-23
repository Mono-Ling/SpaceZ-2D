#pragma once
#include <memory>
#include <new>
#include <limits>
#include <type_traits>

namespace Core::Memory
{
    template<typename T>
    struct MgrAllocator
    {
        using value_type = T;

        MgrAllocator() noexcept = default;
        template<typename U>
        MgrAllocator(const MgrAllocator<U>&) noexcept {}

        inline T* allocate(std::size_t n)
        {
            if(n > std::numeric_limits<std::size_t>::max() / sizeof(T))
                throw std::bad_alloc();
            void* ptr = std::malloc(n * sizeof(T));
            if(!ptr)
                throw std::bad_alloc();
            return static_cast<T*>(ptr);
        }
        inline void deallocate(T* ptr, std::size_t) noexcept
        {
            std::free(ptr);
        }

        template<typename U>
        struct rebind
        {
            using other = MgrAllocator<U>;
        };

        using is_always_equal = std::true_type;
    };

    template<typename T, typename U>
    constexpr bool operator==(const MgrAllocator<T>& a, const MgrAllocator<U>& b) noexcept
    {
        return true;
    }
    template<typename T>
    constexpr bool operator==(const MgrAllocator<T>& a, decltype(nullptr) null) noexcept
    {
        return false;
    }
    template<typename T>
    constexpr bool operator==(decltype(nullptr) null,const MgrAllocator<T>& a) noexcept
    {
        return false;
    }
}