#pragma once
#include "Object/Pool/Pool.h"
#include "Object/Pool/ObjectPool.h"
#include <cstddef>

namespace Core::Object
{
    template<typename T>
    requires std::default_initializable<T>
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

#pragma region Equals
        template<typename U>
        requires std::default_initializable<U>
        inline friend bool operator==(const PoolPtr<U>& a, const PoolPtr<U>& b);

        template<typename U>
        requires std::default_initializable<U>
        inline friend bool operator!=(const PoolPtr<U>& a, const PoolPtr<U>& b);

        template<typename U>
        requires std::default_initializable<U>
        inline friend bool operator==(const PoolPtr<U>& a, std::nullptr_t b);

        template<typename U>
        requires std::default_initializable<U>
        inline friend bool operator==(std::nullptr_t a, const PoolPtr<U>& b);

        template<typename U>
        requires std::default_initializable<U>
        inline friend bool operator!=(const PoolPtr<U>& a, std::nullptr_t b);

        template<typename U>
        requires std::default_initializable<U>
        inline friend bool operator!=(std::nullptr_t a, const PoolPtr<U>& b);
#pragma endregion
    };
}
#include "Object/Pool/PoolPtr.tpp"