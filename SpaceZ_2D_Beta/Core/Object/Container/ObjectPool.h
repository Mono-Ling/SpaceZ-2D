#pragma once
#include "Core/Object/Container/Pool.h"
#include <stack>
#include <memory>
#include <functional>

namespace Core::Container
{
    template<typename T>
    class ObjectPool
    {
    private:
        std::function<T*()> _creator;
        std::stack<std::unique_ptr<T>> _buffer;

        friend struct PoolPtr<T>;

    public:
        ObjectPool(const std::function<T*()>& creator) : _creator(creator) {}

        PoolPtr<T> Get()
        {
            if(!_buffer.empty())
            {
                auto ptr = _buffer.top().release();
                _buffer.pop();
                return CreatePoolPtr(ptr, this);
            }
            return CreatePoolPtr(_creator(), this);
        }

    private:
        void Put(T* obj)
        {
            if(!obj)
                return;
            _buffer.emplace(obj);
        }
    };
}
#include "Core/Object/Container/PoolPtr.h"