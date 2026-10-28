#pragma once
#include <concepts>

namespace Core::Object
{
    template<typename T>
    requires std::default_initializable<T>
    struct PoolPtr;

    template<typename T>
    requires std::default_initializable<T>
    class ObjectPool;

    template<typename T>
    requires std::default_initializable<T>
    PoolPtr<T> CreatePoolPtr(T* obj, ObjectPool<T>* pool);
}