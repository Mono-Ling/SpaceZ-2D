#pragma once
#include "Core/Object/Handle.h"
#include <unordered_map>

namespace Core::Container
{
    template<typename T>
    class UnionFindSet
    {
        std::unordered_map<Handle<T>, Handle<T>> _unionFind;

    public:
        void AddPair(const Handle<T>& a, const Handle<T>& b);
        void Clear();
        Handle<T> FindRoot(const Handle<T>& handle);
        bool IsSameRoot(const Handle<T>& a, const Handle<T>& b);
        Handle<T> operator[](const Handle<T>& handle) const;
    };
}
#include "Core/Object/Container/UnionFindSet.tpp"