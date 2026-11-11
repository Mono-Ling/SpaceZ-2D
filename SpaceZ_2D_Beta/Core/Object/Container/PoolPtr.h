#pragma once
#include "Core/Object/Container/ObjectPool.h"

namespace Core::Container
{
    template<typename T>
    struct PoolPtr
    {
    private:
        T* _ptr = nullptr;
        ObjectPool<T>* _pool = nullptr;

    public:
        PoolPtr() = default;
        PoolPtr(T* ptr, ObjectPool<T>* pool) : _ptr(ptr), _pool(pool) {}
        PoolPtr(const PoolPtr<T>& other) = delete;
        PoolPtr(PoolPtr<T>&& other) noexcept;
        ~PoolPtr();

        PoolPtr<T>& operator=(const PoolPtr<T>& other) = delete;
        PoolPtr<T>& operator=(PoolPtr<T>&& other) noexcept;

        explicit operator bool() const;
        T& operator*() const;
        T* operator->() const;
        T* GetPtr() const;
        void ReleaseOwn();
        void Reset();

        template<typename U>
        friend bool operator==(const PoolPtr<U>& a, const PoolPtr<U>& b);
        template<typename U>
        friend bool operator!=(const PoolPtr<U>& a, const PoolPtr<U>& b);

        template<typename U>
        friend bool operator==(const PoolPtr<U>& ptr, decltype(nullptr) null);
        template<typename U>
        friend bool operator!=(const PoolPtr<U>& ptr, decltype(nullptr) null);

        template<typename U>
        friend bool operator==(decltype(nullptr) null, const PoolPtr<U>& ptr);
        template<typename U>
        friend bool operator!=(decltype(nullptr) null, const PoolPtr<U>& ptr);
    };
}
#include "Core/Object/Container/PoolPtr.tpp"