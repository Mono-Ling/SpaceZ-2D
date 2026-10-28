#include "Object/Pool/PoolPtr.h"

namespace Core::Object
{
    template<typename T>
    requires std::default_initializable<T>
    PoolPtr<T> CreatePoolPtr(T* obj, ObjectPool<T>* pool)
    {
        return PoolPtr<T>(obj, pool);
    }

    template<typename T>
    requires std::default_initializable<T>
    PoolPtr<T>::PoolPtr(PoolPtr<T>&& other) noexcept : _ptr(other._ptr), _pool(other._pool)
    {
        other.ReleaseOwn();
    }

    template<typename T>
    requires std::default_initializable<T>
    PoolPtr<T>::~PoolPtr() { Reset(); }

    template<typename T>
    requires std::default_initializable<T>
    PoolPtr<T>& PoolPtr<T>::operator=(PoolPtr<T>&& other) noexcept
    {
        if(this == &other)
            return *this;
        Reset();
        _ptr = other._ptr;
        _pool = other._pool;
        other.ReleaseOwn();

        return *this;
    }

    template<typename T>
    requires std::default_initializable<T>
    PoolPtr<T>::operator bool() const { return _ptr; }

    template<typename T>
    requires std::default_initializable<T>
    T& PoolPtr<T>::operator*() const { return *_ptr; }

    template<typename T>
    requires std::default_initializable<T>
    T* PoolPtr<T>::operator->() const { return _ptr; }

    template<typename T>
    requires std::default_initializable<T>
    T* PoolPtr<T>::GetPtr() const { return _ptr; }

    template<typename T>
    requires std::default_initializable<T>
    void PoolPtr<T>::ReleaseOwn()
    {
        _ptr = nullptr;
        _pool = nullptr;
    }

    template<typename T>
    requires std::default_initializable<T>
    void PoolPtr<T>::Reset()
    {
        if(!_ptr)
            goto exit;
        if(!_pool)
        {
            delete _ptr;
            goto exit;
        }
        _pool->Put(_ptr);
    exit:
        ReleaseOwn();
    }

    template<typename T>
    requires std::default_initializable<T>
    bool operator==(const PoolPtr<T>& a, const PoolPtr<T>& b)
    {
        return a._ptr == b._ptr && a._pool == b._pool;
    }

    template<typename T>
    requires std::default_initializable<T>
    bool operator!=(const PoolPtr<T>& a, const PoolPtr<T>& b)
    {
        return !(a == b);
    }

    template<typename T>
    requires std::default_initializable<T>
    bool operator==(const PoolPtr<T>& a, std::nullptr_t b) { return !a._ptr; }

    template<typename T>
    requires std::default_initializable<T>
    bool operator==(std::nullptr_t a, const PoolPtr<T>& b) { return !b._ptr; }

    template<typename T>
    requires std::default_initializable<T>
    bool operator!=(const PoolPtr<T>& a, std::nullptr_t b) { return a._ptr; }
    
    template<typename T>
    requires std::default_initializable<T>
    bool operator!=(std::nullptr_t a, const PoolPtr<T>& b) { return b._ptr; };
}