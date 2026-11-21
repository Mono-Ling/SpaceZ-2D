#pragma once
#include "Core/Tools/GetHashCode.h"

namespace Core
{
    struct HashCode
    {
        std::size_t hashCode = 0;

        HashCode() = default;
        HashCode(std::size_t t) : hashCode(t) {}

        constexpr operator std::size_t() const noexcept { return hashCode; }

        template<Hashable T>
        constexpr void Combine(const T& v)
        {
            std::size_t magic;
            if constexpr (sizeof(std::size_t) == 8)
                magic = 0x9e3779b97f4a7c15ull;
            else
                magic = 0x9e3779b9u;

            hashCode ^= GetHashCode(v) + magic + (hashCode << 6) + (hashCode >> 2);
        }
    };

    template<Hashable... Args>
    requires ( sizeof...(Args) >= 1 )
    constexpr std::size_t HashCombine(const Args&... args)
    {
        HashCode h;
        (h.Combine(args), ...);
        return h;
    }
}