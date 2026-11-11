#pragma once
#include "Core/Object/Container/PoolPtr.h"

namespace Core::Container
{
    template<typename T>
    PoolPtr<T> CreatePoolPtr(T* obj, ObjectPool<T>* pool)
    {
        return {obj, pool};
    }

    template<typename T>
    PoolPtr<T>::PoolPtr(PoolPtr<T>&& other) noexcept : _ptr(other._ptr), _pool(other._pool)
    {
        other.ReleaseOwn();
    }

    template<typename T>
    PoolPtr<T>::~PoolPtr() { Reset(); }

    template<typename T>
    PoolPtr<T>& PoolPtr<T>::operator=(PoolPtr<T>&& other) noexcept
    {
        if(&other == this)
            return *this;
        Reset();
        _ptr = other._ptr;
        _pool = other._pool;

        other.ReleaseOwn();
        return *this;
    }

    template<typename T>
    PoolPtr<T>::operator bool() const { return _ptr != nullptr; }

    template<typename T>
    T& PoolPtr<T>::operator*() const { return *_ptr; }

    template<typename T>
    T* PoolPtr<T>::operator->() const { return _ptr; }

    template<typename T>
    T* PoolPtr<T>::GetPtr() const { return _ptr; }

    template<typename T>
    void PoolPtr<T>::ReleaseOwn()
    {
        _ptr = nullptr;
        _pool = nullptr;
    }

    template<typename T>
    void PoolPtr<T>::Reset()
    {
        if(!_ptr)
            goto exit;

        if(!_pool)
            delete _ptr;
        else
            _pool->Put(_ptr);

    exit:
        ReleaseOwn();
    }

    template<typename T>
    bool operator==(const PoolPtr<T>& a, const PoolPtr<T>& b)
    {
        return a._ptr == b._ptr && a._pool == b._pool;
    }
    template<typename T>
    bool operator!=(const PoolPtr<T>& a, const PoolPtr<T>& b) { return !(a == b); }

    template<typename T>
    bool operator==(const PoolPtr<T>& ptr, decltype(nullptr) null) { return ptr._ptr == nullptr; }
    template<typename T>
    bool operator!=(const PoolPtr<T>& ptr, decltype(nullptr) null)  { return ptr._ptr != nullptr; }

    template<typename T>
    bool operator==(decltype(nullptr) null, const PoolPtr<T>& ptr)  { return ptr._ptr == nullptr; }
    template<typename T>
    bool operator!=(decltype(nullptr) null, const PoolPtr<T>& ptr)  { return ptr._ptr != nullptr; }
}