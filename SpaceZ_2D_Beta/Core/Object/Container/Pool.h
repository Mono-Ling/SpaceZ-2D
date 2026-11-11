#pragma once

namespace Core::Container
{
    template<typename T>
    class ObjectPool;

    template<typename T>
    struct PoolPtr;

    template<typename T>
    PoolPtr<T> CreatePoolPtr(T* obj, ObjectPool<T>* pool);
}