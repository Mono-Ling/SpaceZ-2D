#pragma once
#include <algorithm>
#include <functional>

namespace Core
{
    template<typename T>
    struct Handle
    {
    private:
        int _id = -1;
        int _generation = 0;
    public:
        static constexpr Handle<T> FromId(int id)
        {
            if(id < 0)
                return null;
            Handle<T> handle;
            handle._id = id;
            handle._generation = 0;
            return handle;
        }
        constexpr bool Equals(const Handle<T>& other) const
        {
            return _id == other._id && _generation == other._generation;
        }
        constexpr std::size_t GetHashCode() const
        {
            std::size_t h1 = std::hash<int>{}(_id);
            std::size_t h2 = std::hash<int>{}(_generation);

            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
        inline void CarrySelf() { _generation++; }

        friend constexpr bool operator<(const Handle<T>& a, const Handle<T>& b)
        {
            if(a._generation != b._generation)
                return a._generation < b._generation;
            return a._id < b._id;
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