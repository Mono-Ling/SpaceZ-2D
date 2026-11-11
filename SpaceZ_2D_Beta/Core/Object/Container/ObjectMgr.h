#pragma once
#include "Core/Object/Handle.h"
#include "Core/Object/Container/ObjectBuffer.h"
#include <functional>
#include <memory>
#include <stack>

namespace Core::Container
{
    template<typename T>
    class ObjectMgr : public ObjectBuffer<T>
    {
    private:
        std::stack<Handle<T>> _handles;

    public:
        ObjectMgr()
        : ObjectBuffer<T>([](const std::unique_ptr<T>& ptr)->T*{ return ptr.get(); }) {}

        inline Handle<T> PeekNextHandle() const
        {
            if(!_handles.empty())
                return _handles.top();
            return Handle<T>(static_cast<int>(ObjectBuffer<T>::Size()));
        }
        inline bool TryAdd(const Handle<T>& handle , std::unique_ptr<T>&& ptr) override
        {
            if(handle != PeekNextHandle() || !ptr)
                return false;
            if(!ObjectBuffer<T>::TryAdd(handle, std::move(ptr)))
                return false;

            if(!_handles.empty())
                _handles.pop();
            return true;
        }
        inline bool TryRemove(const Handle<T>& handle) override
        {
            if(!ObjectBuffer<T>::TryRemove(handle))
                return false;
            _handles.push(handle.Carry());
            return true;
        }
    };
}