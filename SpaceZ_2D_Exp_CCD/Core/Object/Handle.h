#pragma once
#include "Debug/ToString.h"
#include <functional>
#include <cstddef>
#include <cstdint>

namespace Core
{
    template<typename T>
    struct Handle
    {
    private:
        int _id = -1;
        int _generation = 0;
    
    public:
        constexpr Handle() = default;
        explicit Handle(int id) : _id(id < 0 ? -1 : id), _generation(0) {}

        explicit operator bool() const { return *this != null; }
        constexpr int Id() const { return _id; }
        constexpr int Generation() const { return _generation; }
        constexpr bool Equals(const Handle<T>& other) const
        {
            return _id == other._id && _generation == other._generation;
        }
        constexpr std::size_t GetHashCode() const
        {
            uint64_t id = static_cast<uint64_t>(static_cast<uint32_t>(_id));
            uint64_t gen = static_cast<uint64_t>(_generation);
            uint64_t h = gen << 32 | id;
            return std::hash<uint64_t>{}(h);
        }
        constexpr void CarrySelf() { _generation++; }

        friend constexpr bool operator<(const Handle<T>& a, const Handle<T>& b)
        {
            if(a._generation != b._generation)
                return a._generation < b._generation;
            return a._id < b._id;
        }

        static const Handle<T> null;
    };

    template<typename T>
    constexpr Handle<T> Handle<T>::null {};

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

    template<typename T>
    std::string ToString(const Handle<T>& handle)
    {
        return "( id:" + ToString(handle.Id()) + ", generation:" + ToString(handle.Generation()) + ")";
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