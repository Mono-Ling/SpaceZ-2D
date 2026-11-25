#pragma once
#include <memory>
#include <new>
#include <limits>
#include <type_traits>

namespace UnityPlugin
{
    template<typename T>
    struct NativeAllocator
    {
        using value_type = T;

        NativeAllocator() noexcept = default;
        template<typename U>
        NativeAllocator(const NativeAllocator<U>&) noexcept {}

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
            using other = NativeAllocator<U>;
        };

        using is_always_equal = std::true_type;
    };

    template<typename T, typename U>
    constexpr bool operator==(const NativeAllocator<T>& a, const NativeAllocator<U>& b) noexcept
    {
        return true;
    }
}