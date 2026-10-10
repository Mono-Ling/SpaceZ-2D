#pragma once
#include "Object/Handle.h"
#include <unordered_map>
#include <queue>
#include <functional>

namespace Core
{
    template<typename T>
    class ObjectMgr
    {
    private:
        std::unordered_map<Handle<T>, T*> _objMap;
        std::queue<Handle<T>> _handleBuffer;

    public:
        Handle<T> CreateHandle();
        bool AddObject(T* obj, const Handle<T>& handle);
        bool TryRemove(Handle<T> handle);
        bool TryGet(const Handle<T>& handle, T*& ptr) const;
        bool Contains(const Handle<T>& handle) const;
        void Foreach(const std::function<void(const Handle<T>&, const T*)>& func);
    };
}
#include "Object/ObjectMgr.tpp"