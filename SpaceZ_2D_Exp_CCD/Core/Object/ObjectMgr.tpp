#pragma once
#include "Object/ObjectMgr.h"
#include "Debug/Debug.h"

namespace Core::Object
{
    template<typename T>
    Handle<T> ObjectMgr<T>::PeekNextHandle() const
    {
        if(!_buffer.empty())
            return _buffer.top();
        return Handle<T>(static_cast<int>(_objs.size()));
    }

    template<typename T>
    bool ObjectMgr<T>::Contains(const Handle<T>& handle) const
    {
        if(!handle)
            return false;
        int id = handle.Id();
        if(id < 0 || id >= _objs.size())
            return false;
        return _objs[id].first == handle;
    }

    template<typename T>
    bool ObjectMgr<T>::TryAdd(const std::function<std::unique_ptr<T>(const Handle<T>&)>& creator)
    {
        Handle<T> handle = PeekNextHandle();
        std::unique_ptr<T> ptr = creator(handle);
        if(handle != PeekNextHandle())
        {
            Debug::LogError("【对象管理器】creator回调修改了管理器，添加已取消:" + ToString(handle));
            return false;
        }
        return TryAdd(handle, std::move(ptr));
    }
    template<typename T>
    bool ObjectMgr<T>::TryAdd(const Handle<T>& handle , std::unique_ptr<T>&& ptr)
    {
        if(!ptr)
        {
            Debug::LogError("【对象管理器】对象添加失败，无效对象");
            return false;
        }
        if(!IsFree(handle))
        {
            Debug::LogError("【对象管理器】对象添加失败，无效句柄或句柄已存在:" + ToString(handle));
            return false;
        }
        int id = handle.Id();
        if(id < _objs.size())
        {
            _objs[id].first = handle;
            _objs[id].second = std::move(ptr);
        }
        else
            _objs.emplace_back(std::pair<Handle<T>, std::unique_ptr<T>>(handle, std::move(ptr)));
        Carry();
        return true;
    }

    template<typename T>
    bool ObjectMgr<T>::TryRemove(const Handle<T>& handle)
    {
        if(!Contains(handle))
            return false;
        int id = handle.Id();
        _objs[id].second.reset();
        _objs[id].first = Handle<T>::null;

        _buffer.emplace(handle.Carry());
        return true;
    }

    template<typename T>
    bool ObjectMgr<T>::TryGet(const Handle<T>& handle, T*& ptr)
    {
        ptr = nullptr;
        if(!Contains(handle))
            return false;
        ptr = _objs[handle.Id()].second.get();
        return true;
    }

    template<typename T>
    void ObjectMgr<T>::ForEach(const std::function<void(const Handle<T>&, T&)>& func)
    {
        for(auto& p : _objs)
        {
            if(!p.first || !p.second)
                continue;
            func(p.first, *p.second);
        }
    }

    template<typename T>
    void ObjectMgr<T>::Carry()
    {
        if(!_buffer.empty())
            _buffer.pop();
    }

    template<typename T>
    bool ObjectMgr<T>::IsFree(const Handle<T>& handle) const
    {
        return static_cast<bool>(handle) && handle == PeekNextHandle();
    }
}