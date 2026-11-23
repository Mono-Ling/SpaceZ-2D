#pragma once
#include <cstdint>
#include <cstddef>

namespace Core::Memory
{
    struct MemoryPage
    {
        std::uintptr_t address = 0;
        mutable std::size_t size = 0;

        MemoryPage() noexcept = default;
        MemoryPage(std::uintptr_t addr, std::size_t s) noexcept
        : address(addr), size(s) {}

        constexpr explicit operator bool() const
        {
            return address && size;
        }

        void* Cut(std::size_t len) const;
        bool TryMerge(const MemoryPage& other) const;
    };
    bool operator==(const MemoryPage& a, const MemoryPage& b);
    bool operator<(const MemoryPage& a, const MemoryPage& b);
}