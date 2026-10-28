#pragma once
#include "Object/Pool/Pool.h"
#include <memory>
#include <stack>

namespace Core::Object
{
    template<typename T>
    requires std::default_initializable<T>
    class ObjectPool
    {
    private:
        std::stack<std::unique_ptr<T>> _buffer;
        friend struct PoolPtr<T>;

        inline void Put(T* ptr)
        {
            if(!ptr)
                return;
            _buffer.emplace(ptr);
        }
    public:
        ObjectPool() = default;
        ObjectPool(const ObjectPool& other) = delete;
        ObjectPool(ObjectPool&& other) = default;

        inline PoolPtr<T> Get()
        {
            T* ans = nullptr;
            if(!_buffer.empty())
            {
                ans = _buffer.top().release();
                _buffer.pop();
            }
            else
                ans = new T();
            return CreatePoolPtr(ans, this);
        }
    };
}
#include "Object/Pool/PoolPtr.h"