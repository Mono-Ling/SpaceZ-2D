#pragma once
#include "Core/Object/Container/UnionFindSet.h"
#include "Core/Debug/Debug.h"

namespace Core::Container
{
    template<typename T>
    void UnionFindSet<T>::AddPair(const Handle<T>& a, const Handle<T>& b)
    {
        if(!a || !b)
        {
            Debug::LogWarning("【句柄并查集】存在无效句柄，插入失败");
            return;
        }
        auto rootA = FindRoot(a);
        auto rootB = FindRoot(b);
        if(rootA == rootB)
            return;
        _unionFind[rootA] = rootB;
    }

    template<typename T>
    void UnionFindSet<T>::Clear() { _unionFind.clear(); }

    template<typename T>
    Handle<T> UnionFindSet<T>::FindRoot(const Handle<T>& handle)
    {
        if(!handle)
            return Handle<T>::null;

        if(!_unionFind.count(handle))
        {
            _unionFind[handle] = handle;
            return handle;
        }
        auto curr = handle;
        while (true)
        {
            auto it = _unionFind.find(curr);
            if(it == _unionFind.end())
                return Handle<T>::null;

            auto& next = it->second;
            if(curr == next)
                break;
            _unionFind[curr] = _unionFind[next];
            curr = next;
        }
        _unionFind[handle] = curr;
        return curr;
    }

    template<typename T>
    bool UnionFindSet<T>::IsSameRoot(const Handle<T>& a, const Handle<T>& b)
    {
        if(!a || !b)
            return false;
        return FindRoot(a) == FindRoot(b);
    }

    template<typename T>
    Handle<T> UnionFindSet<T>::operator[](const Handle<T>& handle) const
    {
        if(!handle)
            return Handle<T>::null;
        auto curr = handle;
        while (true)
        {
            auto it = _unionFind.find(curr);
            if(it == _unionFind.end())
                return Handle<T>::null;

            auto& next = it->second;
            if(curr == next)
                break;
            curr = next;
        }
        return curr;
    }
}