#pragma once
#include "Core/Debug/ToString.h"
#include <functional>

namespace Core
{
    template<typename T>
    struct Handle
    {
    private:
        int _id;
        unsigned int _generation;

    public:
        constexpr Handle() : _id(-1), _generation(0) {}
        constexpr explicit Handle(int id) : _id(id > -1 ? id : -1), _generation(0) {}

        explicit operator bool() const { return *this != null; }
        constexpr int Id() const { return _id; }
        constexpr unsigned int Generation() const { return _generation; }
        constexpr std::size_t GetHashCode() const
        {
            auto id = static_cast<unsigned long long>(static_cast<unsigned int>(_id));
            auto gen = static_cast<unsigned long long>(_generation);
            unsigned long long hashCode = id | gen << 32;
            return std::hash<unsigned long long>{}(hashCode);
        }
        constexpr Handle& CarrySelf()
        {
            _generation++;
            return *this;
        }
        constexpr Handle Carry() const
        {
            Handle handle(*this);
            handle.CarrySelf();
            return handle;
        }

        friend bool operator==(const Handle& a, const Handle& b)
        {
            return a._id == b._id && a._generation == b._generation;
        }
        friend bool operator!=(const Handle& a, const Handle& b) { return !(a == b); }

        static const Handle null;
    };

    template<typename T>
    constexpr Handle<T> Handle<T>::null {};

    template<typename T>
    inline std::string ToString(const Handle<T>& handle)
    {
        return StringFormat("[", handle.Id(), ", ", handle.Generation(), "]");
    }
}
namespace std
{
    template<typename T>
    struct hash<Core::Handle<T>>
    {
        size_t operator()(const Core::Handle<T>& handle) const
        {
            return handle.GetHashCode();
        }
    };
}