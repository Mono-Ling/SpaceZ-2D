#pragma once
#include <algorithm>
#include <functional>

namespace Core
{
    template<typename T>
    struct Handle
    {
        int id = -1;
        int generation = 0;
        static constexpr Handle<T> FromId(int id)
        {
            if(id < 0)
                return null;
            Handle<T> handle;
            handle.id = id;
            handle.generation = 0;
            return handle;
        }
        constexpr bool Equals(const Handle<T>& other) const
        {
            return id == other.id && generation == other.generation;
        }
        constexpr std::size_t GetHashCode() const
        {
            std::size_t h1 = std::hash<int>{}(id);
            std::size_t h2 = std::hash<int>{}(generation);

            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
        inline void CarrySelf() { generation++; }

        friend constexpr bool operator<(const Handle<T>& a, const Handle<T>& b)
        {
            if(a.generation != b.generation)
                return a.generation < b.generation;
            return a.id < b.id;
        }

        static const Handle<T> null;
    };

    template<typename T>
    const Handle<T> Handle<T>::null = Handle<T>();

    template<typename T>
    constexpr bool operator==(const Handle<T>& a, const Handle<T>& b)
    {
        return a.Equals(b);
    }
    template<typename T>
    constexpr bool operator!=(const Handle<T>& a, const Handle<T>& b)
    {
        return !a.Equals(b);
    }
}
namespace std
{
    template<typename T>
    struct hash<Core::Handle<T>>
    {
        size_t operator()(const Core::Handle<T>& h) const
        {
            return h.GetHashCode();
        }
    };
}