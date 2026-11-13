#pragma once
#include "Core/Object/Handle.h"
#include <vector>
#include <utility>
#include <memory>
#include <functional>
#include <concepts>

namespace Core::Container
{
    template<typename T, std::default_initializable Ptr = std::unique_ptr<T>>
    struct ObjectBuffer
    {
    private:
        std::function<T*(const Ptr&)> _getter;
        std::vector<std::pair<Handle<T>, Ptr>> _buffer;

    public:
        ObjectBuffer(const std::function<T*(const Ptr&)>& getter) : _getter(getter) {}
        ObjectBuffer(const ObjectBuffer<T>& other) = delete;
        ObjectBuffer(ObjectBuffer<T>&& other) = delete;
        virtual ~ObjectBuffer() = default;

        ObjectBuffer& operator=(const ObjectBuffer<T>& other) = delete;
        ObjectBuffer& operator=(ObjectBuffer<T>&& other) = delete;

        int Size() const;
        void Clear();
        bool Contains(const Handle<T>& handle) const;
        virtual bool TryAdd(const Handle<T>& handle , Ptr&& ptr);
        virtual bool TryRemove(const Handle<T>& handle);
        bool TryGet(const Handle<T>& handle, T*& ptr) const;
        void ForEach(const std::function<void(const Handle<T>&, T&)>& func);

    private:
        bool IsFree(const Handle<T>& handle) const;
    };
}
#include "Core/Object/Container/ObjectBuffer.tpp"