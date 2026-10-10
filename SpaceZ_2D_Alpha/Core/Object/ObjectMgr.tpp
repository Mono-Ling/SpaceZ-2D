#include "Object/ObjectMgr.h"

namespace Core
{
    template<typename T>
    Handle<T> ObjectMgr<T>::CreateHandle()
    {
        if(!_handleBuffer.empty())
        {
            auto handle = _handleBuffer.front();
            _handleBuffer.pop();
            return handle;
        }
        return Handle<T>::FromId(static_cast<int>(_objMap.size()));
    }
    template<typename T>
    bool ObjectMgr<T>::AddObject(T* obj, const Handle<T>& handle)
    {
        if(!obj || handle == Handle<T>::null || Contains(handle))
            return false;
        _objMap.insert({handle, obj});
        return true;
    }
    template<typename T>
    bool ObjectMgr<T>::TryRemove(Handle<T> handle)
    {
        if(!Contains(handle))
            return false;
        _objMap.erase(handle);
        handle.CarrySelf();
        _handleBuffer.push(handle);
        return true;
    }

    template<typename T>
    bool ObjectMgr<T>::TryGet(const Handle<T>& handle, T*& ptr) const
    {
        auto it = _objMap.find(handle);
        if(it == _objMap.end())
            return false;
        ptr = it->second;
        return true;
    }
    template<typename T>
    bool ObjectMgr<T>::Contains(const Handle<T>& handle) const
    {
        return _objMap.count(handle);
    }
    template<typename T>
    void ObjectMgr<T>::Foreach(const std::function<void(const Handle<T>&, const T*)>& func)
    {
        for(auto& p : _objMap)
            func(p.first, p.second);
    }
}