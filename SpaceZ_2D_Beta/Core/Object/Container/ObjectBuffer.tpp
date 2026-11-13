#pragma once
#include "Core/Object/Container/ObjectBuffer.h"
#include "Core/Debug/Debug.h"

namespace Core::Container
{
    template<typename T, std::default_initializable Ptr>
    int ObjectBuffer<T, Ptr>::Size() const { return static_cast<int>(_buffer.size()); }

    template<typename T, std::default_initializable Ptr>
    void ObjectBuffer<T, Ptr>::Clear() { _buffer.clear(); }

    template<typename T, std::default_initializable Ptr>
    bool ObjectBuffer<T, Ptr>::Contains(const Handle<T>& handle) const
    {
        if(!handle)
            return false;
        int id = handle.Id();
        if(id < 0 || id >= _buffer.size())
            return false;
        if(!_buffer[id].first)
            return false;
        return _buffer[id].first == handle;
    }

    template<typename T, std::default_initializable Ptr>
    bool ObjectBuffer<T, Ptr>::TryAdd(const Handle<T>& handle , Ptr&& ptr)
    {
        if(!IsFree(handle))
            return false;
            
        int id = handle.Id();
        if(id >= _buffer.size())
            _buffer.resize(id + 1);
        
        _buffer[id].second = std::move(ptr);
        _buffer[id].first = handle;

        return true;
    }

    template<typename T, std::default_initializable Ptr>
    bool ObjectBuffer<T, Ptr>::TryRemove(const Handle<T>& handle)
    {
        if(!Contains(handle))
            return false;
        int id = handle.Id();
        _buffer[id].second = std::move(Ptr{});
        _buffer[id].first = Handle<T>::null;
        return true;
    }

    template<typename T, std::default_initializable Ptr>
    bool ObjectBuffer<T, Ptr>::TryGet(const Handle<T>& handle, T*& ptr) const
    {
        ptr = nullptr;
        if(!Contains(handle) )
            return false;
        if(!_getter)
        {
            Debug::LogError("【ObjectBuffer】_getter为空，获取对象失败");
            return false;
        }
        ptr = _getter(_buffer[handle.Id()].second);
        return true;
    }

    template<typename T, std::default_initializable Ptr>
    void ObjectBuffer<T, Ptr>::ForEach(const std::function<void(const Handle<T>&, T&)>& func)
    {
        if(!_getter)
        {
            Debug::LogError("【ObjectBuffer】_getter为空，获取对象失败");
            return;
        }
        for(auto& p : _buffer)
        {
            if(!p.first)
                continue;
            auto obj = _getter(p.second);

            if(obj)
                func(p.first, *obj);
        }
    }

    template<typename T, std::default_initializable Ptr>
    bool ObjectBuffer<T, Ptr>::IsFree(const Handle<T>& handle) const
    {
        int id = handle.Id();
        if(id < 0)
            return false;
        if(id >= _buffer.size())
            return true;
        return !_buffer[id].first;
    }
}